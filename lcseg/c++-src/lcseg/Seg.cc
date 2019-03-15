/**
 * Copyright © 2003-2006 by The Trustees of Columbia University in 
 * the City of New York.  All rights reserved.  This software is being
 * provided at no cost for educational and/or research purposes only.
 *
 * Author: Michel Galley, galley@cs.columbia.edu
 * $Id: Seg.cc 43 2006-11-14 00:22:30Z galley $
 */

#include <iostream>
#include <string>
#include <fstream>
#include <iosfwd>
#include <algorithm>
#include <cmath>

#include <lcseg/Seg.h>
#include <lcseg/TextToolkit.h>
#include <lcseg/defs.h>

#ifdef KSTEM
#include <lcseg/kstem/kstem.h>
#endif

namespace lcseg {

void 
Seg::empty_bows() {
  // Delete all bag of words created in read_text:
  BOWseq::iterator it = _bows.begin(), it_end = _bows.end();
  for(; it != it_end; ++it)
    delete *it;
  // Clear vector.
  _bows.clear(); 
}

// Moving average filter:
void Seg::smooth_moving_average(int smooth_order) {

  // Make a temporary version of lc_function:
  std::vector<float> lc_function_tmp = _lc_function;
  
  int order = FILTER_ORDER;
  int k     = 2*order+1;
  for(int i=0, lasti = _lc_function.size()-1; i<=lasti; ++i) {
    for(int j=i-order, lastj=i+order; j<=lastj; ++j)
      lc_function_tmp[i] += _lc_function[j];
    lc_function_tmp[i] /= k;
  }

  // Make a deep copy and erase the original lc_function:
  _lc_function = lc_function_tmp;
}

void Seg::get_segmentation_scores_with_hillclimbing(int cons) {
  
  // Make sure it is the first time we run the function:
  assert(_segmentation_scores.empty());
  assert(_minima.empty());
  
  // Find local minima, compute the mean and std dev, 
  // and select below threshold minima as bnd:
  MinimaScores minima_tmp;

  // (Note: don't process first and last element, since they only 
  // have one neighbor)
  for(int i=1, lasti = _lc_function.size()-1; i<lasti; ++i) {
    if(_lc_function[i-1] < _lc_function[i]) continue;
    if(_lc_function[i] > _lc_function[i+1]) continue;
    
    // Hill climbing:
    float ref = _lc_function[i];
    float lmax = ref, rmax = ref;
    for(int j=i-1; j>=0; --j) {
      float sc = _lc_function[j];
      if(sc < ref) break;
      if(sc > lmax) lmax = sc;
    }
    for(int j=i+1; j<=lasti; ++j) {
      float sc = _lc_function[j];
      if(sc < ref) break;
      if(sc > rmax) rmax = sc;
    }

    // Store score (hyp probability):
    float p = (lmax + rmax - 2*ref)/2;
    minima_tmp.push_back(std::make_pair(i,p));
  }

  // Sort these minima by descending values:
  stable_sort(minima_tmp.begin(),minima_tmp.end(),pair_sort());

  // Use cons parameter to rule out boundaries
  // that are too close to one another:
  for(MinimaScores::iterator it = minima_tmp.begin(); 
      it != minima_tmp.end(); ++it) {
    bool near = false;
    // Compare current boundary with start and end: do not 
    // allow a boundary to be too close:
    if(it->first < cons || it->first >= 
       static_cast<int>(_lc_function.size()) - cons) 
      continue;

    // Compare current boundary with previous ones:
    for(MinimaScores::iterator it2 = _minima.begin(); 
        it2 != _minima.end(); ++it2) {
      if(abs(it->first - it2->first) <= cons) {
        near = true;
        break;
      }
    }
    if(!near) {
      _minima.push_back(std::make_pair(it->first,it->second));
    }
  }
  
  // Create segmentation scores:
  _segmentation_scores.resize(_lc_function.size());
  for(MinimaScores::iterator it = _minima.begin(); 
      it != _minima.end(); ++it)
     _segmentation_scores[it->first] = it->second;
}
 
void
Seg::get_boundaries_with_segmentation_scores
  (int nb_seg, float alpha, float plimit) {

  if(nb_seg > 0) {
    // The number of boundaries is already determined. Just pick the
    // n lowest values in the LC profile:
    int nb_bnd = nb_seg-1, size = _minima.size();
    int i = (size < nb_bnd) ? size-1 : nb_bnd-1;
    for(; i>=0; --i) {
      _bnd.insert(_minima[i].first);
      DLOG("Bnd location: " << _minima[i].first);
    }
  } else {
    // The number of boundaries is unknown. We have to estimate it 
    // using the mean and variance:
    float sum1=0.0; int count=0;
    for(int i=0, lasti = _minima.size()-1; i<=lasti; ++i) { //mean
      float sc = _minima[i].second;
      if(sc > plimit) {
        sum1 += sc; 
        ++count;
      }
    }
    float mean = sum1/count;

    float sum2=0.0, tmp;
    for(int i=0, lasti = _minima.size()-1; i<=lasti; ++i) { //std dev
      float sc = _minima[i].second;
      if(sc > plimit) {
        tmp = sc-mean;
        sum2 += tmp*tmp;
      }
    }
    float stddev;
    if(count>1) 
      stddev = sqrt(sum2/(count-1));
    else
      stddev = 0;
      
    // Determine threshold:
    float thr = mean - alpha*stddev;

    // Select boundaries:
    for(int i=0, lasti = _minima.size()-1;
        i <= lasti && _minima[i].second >= thr;
        ++i) {
      _bnd.insert(_minima[i].first);
      DLOG("Adding boundary at: " << _minima[i].first
                << " Score: " << _minima[i].second);
    }
  }
}

void Seg::read_text(std::ifstream& in) {

  // Init berkeley DB if needed:
#ifdef KSTEM
  initdb();
#endif

  // Read text file:
  std::string line;
  while (std::getline(in,line,'\n')) {
#ifdef IGNORE_SEPARATORS
    if(line.find_first_of(SEPARATOR) == 0)
        continue;
#endif
    // The BagOfWords objects are deleted in ~Seg:
    BagOfWords<int> *bow = new BagOfWords<int>();
    bow->read_tokens(line);
    _bows.push_back(bow);
  }

  // Close berkeley DB if needed:
#ifdef KSTEM
  closedb();
#endif
}

void Seg::print_segmented_text(std::ostream& out, std::istream& in) const {

  // Output text with boundaries:
  std::string line;
  std::set<int>::iterator it = _bnd.begin(), it_end = _bnd.end();
  int next_bnd = (it != it_end) ? *it : -1;
  
  int cur_line = -1;
  out << SEPARATOR << std::endl;
  while(std::getline(in,line,'\n')) {
    out << line << std::endl;
    if(++cur_line == next_bnd) {
      if(++it != it_end)
        next_bnd = *it;
      out << SEPARATOR << std::endl;
    }
  }
  out << SEPARATOR << std::endl;
}

void Seg::print_lc_function(std::ostream& out) const {
  int last = _bows.size()-1;
  for(int i = 0; i<last; ++i)
    out << _lc_function[i] << "\t" 
        << _segmentation_scores[i] << std::endl;
}

void Seg::print_matlab_lc_function(const char* filename) const {
  std::ofstream out(filename);
  int last = _bows.size()-1;
  out << "A = [ ";
  for(int i = 0; i<last; ++i)
    out << _lc_function[i] << " ";
  out << "];" << std::endl;
  out << "hold;" << std::endl;
  out << "plot(0:" << last-1 << ",A);" << std::endl;
  out << "axis([0 " << last << " 0 1]); " << std::endl;
  out << "hyp = [ ";
  for(std::set<int>::iterator it = _bnd.begin(), it_end = _bnd.end();
    it != it_end; ++it) {
    out << *it << " ";
  }
  out << "];" << std::endl;
  out << "vline(hyp, \'k:\');" << std::endl;
  out << "axis tight;" << std::endl;
  out << "set(gca,\'XTick\',20*[1:" << ceil(last/20) << "]);" << std::endl;
}

}
