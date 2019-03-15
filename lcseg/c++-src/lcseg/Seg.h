/**
 * Copyright © 2003-2006 by The Trustees of Columbia University in 
 * the City of New York.  All rights reserved.  This software is being
 * provided at no cost for educational and/or research purposes only.
 *
 * Author: Michel Galley, galley@cs.columbia.edu
 * $Id: Seg.h 43 2006-11-14 00:22:30Z galley $
 */

#ifndef __SEG_H__
#define __SEG_H__

#include <list>
#include <vector>
#include <set>
#include <string>
#include <fstream>
#include <iosfwd>

#include <lcseg/BagOfWords.h>
#include <lcseg/logging.h>

namespace lcseg {

//! Structure defining decreasing ordering between features 
//! (based on number of occurrences).
struct pair_sort {
  bool operator()(const std::pair<int,float>& p1, 
                  const std::pair<int,float>& p2) const {
    return (p1.second >= p2.second);
  }
};

class Seg {

public:
  //! Bag of words (counts are integer).
  typedef BagOfWords<int> BOW;
  //! Sequence of bag of words.
  typedef std::vector<BOW*> BOWseq;
  //! Sequence of minima locations and associated scores.
  typedef std::vector< std::pair<int,float> > MinimaScores;
    
public:
  //! Constructor that creates a segmentation with no boundaries.
  Seg() {}
  //! Virtual destructor: deletes all bags of words.
  virtual ~Seg() { empty_bows(); }
  //! Get reference to sequence of bag of words.
  BOWseq& get_BOW_seq() { return _bows; }
  //! Get pointer to specific bag of word.
  BOW* get_bow(int i)   { return _bows[i]; }
  //! Add a new bag of word at the end of the sequence.
  void add_bow(BOW* b)  { _bows.push_back(b); } 
  //! Clear bag of words.
  void empty_bows();
  //! Get length of lexical cohesion function.
  size_t get_lc_function_size() const { return _lc_function.size(); } 
  //! Get length of segmentation score function.
  size_t get_segmentation_score_size() const 
  { return _segmentation_scores.size(); } 
  //! Get lexical cohesion score of a specific sentence.
  float get_lc_function(unsigned int i) const 
  { return _lc_function[i]; }
  //! Get segmentation score of a specific sentence.
  float get_segmentation_score(unsigned int i) const 
  { return _segmentation_scores[i]; }
  //! Function that finds all local minima (after smoothing), and assigns
  //! a score to each potential boundary located at a minimum. This function
  //! updates _minima and _segmentation_scores.
  void get_segmentation_scores_with_hillclimbing(int cons);
  //! Read text from a stream, and store selected tokens into bag of words
  //! (a bag of word for each sentence).
  void read_text(std::ifstream&); 
  //! Write segmented text to a stream. 
  void print_segmented_text(std::ostream&, std::istream&) const;
  //! Print lexical cohesion function to a stream in a format that
  //! can be read by matlab.
  void print_lc_function(std::ostream&) const;
  //! Print lexical cohesion function to a file in a format that
  //! can be read by matlab.
  void print_matlab_lc_function(const char* filename) const;
  //! Virtual segmentation algorithm. It is supposed to update
  //! _bnd.
  virtual void segment()=0;
  //! Class for handling I/O exceptions:
  class IOException { };
  
protected:
  //! Smooth the lexical cohesion function (moving average).
  void smooth_moving_average(int smooth_order=0);
  //! Partially re-order elements of the lexical cohesion plot, 
  //! e.g. a sequence [1 2 4 3 5 4 3] is transformed to [1 2 3 4 5 4 3].
  void smooth_reorder();
  //! Place boundaries (whose number, nb_seg+1, may be given) at the 
  //! "appropriate" place given parameters alpha and p_limit described in the 
  //! paper, and given a lexical cohesion function (one additional parameter
  //! is not described in the paper: cons, the minimum number of sentences
  //! between to consecutive boundaries).
  void get_boundaries_with_segmentation_scores 
    (int nb_seg, float alpha, float plimit);

private:
  //! Prevent accidental use of copy constructor.
  Seg(const Seg&);
  //! Prevent accidental use of assignment operator.
  void operator=(const Seg&);

protected:
  //! A text represented as a sequence of bag of words.
  BOWseq _bows;
  //! Lexical cohesion function (cosine similarity scores computed
  //! from lexical chains).
  std::vector<float> _lc_function;
  //! Segmentation scores used to determine the location of topic segments.
  std::vector<float> _segmentation_scores;
  //! List of minima and their associated segmentation scores.
  MinimaScores _minima;
  //! Binary vector deterimining boundary locations.
  std::set<int> _bnd;
};

} 

#endif
