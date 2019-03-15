/**
 * Copyright © 2006 by The Trustees of Columbia University in 
 * the City of New York.  All rights reserved.  This software is being
 * provided at no cost for educational and/or research purposes only.
 *
 * Author: Michel Galley, galley@cs.columbia.edu
 * $Id: TextTiling.cc 43 2006-11-14 00:22:30Z galley $
 */

#include <iostream>
#include <string>
#include <fstream>
#include <iosfwd>
#include <algorithm>
#include <cmath>

#include <lcseg/BagOfWords.h>
#include <lcseg/TextTiling.h>

namespace lcseg {

void TextTiling::texttiling_segment
  (int nb_seg, int window_size, int smooth, int cons, float alpha, float plimit) {
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

// Compute lc_function using cosine metric between two 
// adjacent windows:
void TextTiling::algo_lc_function(int k) {

  // Init the lexical cohesion profile:
  int size = _bows.size();
  int last = size-1;
  _lc_function.resize(last,0.0);
  
  // Two sliding-window technique described in Hearst '94:
  for(int i = 0; i<last; ++i) {
      FeatureVector<int> l,r;
    for(int j1=i,j2=i+1; j1>i-k && j2<=i+k;--j1,++j2) {
      if(j1<0 || j2>last) break;
      l += *_bows[j1];
      r += *_bows[j2];
    }
    _lc_function[i] = FeatureVector<int>::similarity
                     (l,r,FeatureVector<int>::sim_cos);
  }
}

}
