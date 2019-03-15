/**
 * Copyright © 2003-2006 by The Trustees of Columbia University in 
 * the City of New York.  All rights reserved.  This software is being
 * provided at no cost for educational and/or research purposes only.
 *
 * Author: Michel Galley, galley@cs.columbia.edu
 * $Id: LCseg.cc 43 2006-11-14 00:22:30Z galley $
 */

#include <iostream>
#include <string>
#include <fstream>
#include <iosfwd>
#include <algorithm>
#include <cmath>

#include <lcseg/BagOfWords.h>
#include <lcseg/LCseg.h>

namespace lcseg {

int Chain::_txtlen = 0;
bool LCseg::_ROTATE = false;

void LCseg::lcseg_segment
  (int nb_seg, int rhiatus, int window_size, int smooth,
   int cons, float alpha, float plimit) {
  // Set text length in the chain class:
  Chain::set_txtlen(_bows.size());
  // Identify strong chains:
  algo_strong_chains(rhiatus);
  // Compute lexical cohesion function:
  algo_lc_function(window_size);

#ifdef DEBUG
  // Dump lexical cohesion function if needed:
  write_lc_function(std::cerr);
#endif

  // Set boundaries:
  get_segmentation_scores_with_hillclimbing(cons);
  get_boundaries_with_segmentation_scores(nb_seg,alpha,plimit); 
}

void LCseg::algo_strong_chains(int rhiatus) {
  
  int size = _bows.size();
  int last = size-1;

  // "Hiatus" value relative to document length:
  int hiatus = int(size/rhiatus);

  // Three hashtables that keep track where a term first appeared,
  // where it was last seen, and the nb of occ:
  hashmap_str first_occ,last_occ,nb_occ; 
  hashmap_str::iterator it_first,it_last,it_nb;
  
  // For each bag of word:
  for(int i=0; i<=last; ++i) {
    // For each word in bow, process each word, and determine
    // if it is its first occurence, and when it was last seen:
    for(BagOfWords<int>::iterator it = _bows[i]->begin(), 
                                  it_end = _bows[i]->end();
        it != it_end; ++it) {
      it_first = first_occ.find(it->first);
      if(it_first == first_occ.end()) {
        // Never seen, so add it to first_occ:
        first_occ.insert(std::make_pair(it->first,i));
        last_occ.insert(make_pair(it->first,i));
        nb_occ.insert(make_pair(it->first,it->second));
        continue;
      }

      // Term already seen:
      // Determine the last occurence of the term:
      it_last = last_occ.find(it->first);
      it_nb = nb_occ.find(it->first);
      // Create a new chain if the current occurence is too far from
      // the last one, or if we reached the end of text:
      if(i - hiatus > it_last->second + 1) {
        if(it_first->second != it_last->second) {
          chains.insert(Chain(it_first->second,it_last->second,
                              it_nb->second,it->first));
        }
        it_first->second = i;
        it_last->second = i;
        it_nb->second = it->second;
      } else {
        it_last->second = i; // set when the term was last seen
        it_nb->second  += it->second; // increase count of occurences
      }
    } // end for each word in bow
  }

  // Create all term reps:
  hashmap_str::iterator it_first_end = first_occ.end();
  
  for(it_first = first_occ.begin(); it_first != it_first_end; ++it_first) {
    it_last = last_occ.find(it_first->first);
    it_nb = nb_occ.find(it_first->first);
    if(it_first->second != it_last->second) {
      chains.insert(Chain(it_first->second,it_last->second,
                          it_nb->second,it_first->first));
    }
  }

#ifdef DEBUG
  for(Chains::iterator it  = chains.begin(); 
                                it != chains.end(); ++it)
    it->write(std::cerr);
  DLOG("Nb of term reps: " << chains.size());
#endif
}
 
// Compute lc_function using strong chain repetitions:
void LCseg::algo_lc_function(int k) {

  // Init the lexical cohesion profile:
  int size = _bows.size();
  int last = size-1;
  _lc_function.resize(last,0.0);
  
  // Init distribution of reps_bows (on the stack):
  TREP_list reps_bows(size+1);

  // Process strong term reps, and place them in the term_reps_dist table:
  for(Chains::iterator it = chains.begin(), 
      it_end = chains.end();
      it != it_end; ++it) {
    int start = it->get_start(),
        end = it->get_end();
    float score = it->get_score();
    for(int i=start;i<=end;++i) {
      reps_bows[i].insert
          (std::pair<std::string,float>(it->get_word(),score));
    }
  }

  // Two sliding-window technique described in Hearst '94:
  for(int i = 0; i<last; ++i) {
    TREP l=reps_bows[i],
         r=reps_bows[i+1];
    //std::cerr << "i: " << i << std::endl;
    //for(int j1=i-1,j2=i+2; j1>i-k && j2<=i+k;--j1,++j2) {
    for(int j1=i,j2=i+1; j1>i-k && j2<=i+k;--j1,++j2) {
      if(j1<0 || j2>last) break;
	  //LOGT(_DL,4,"");
      l = FeatureVector<float>::max(l,reps_bows[j1]);
      r = FeatureVector<float>::max(r,reps_bows[j2]);
    }
    _lc_function[i] = FeatureVector<float>::similarity
                     (l,r,FeatureVector<float>::sim_cos);
  }
}

}
