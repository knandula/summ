/**
 * Copyright © 2006 by The Trustees of Columbia University in 
 * the City of New York.  All rights reserved.  This software is being
 * provided at no cost for educational and/or research purposes only.
 *
 * Author: Michel Galley, galley@cs.columbia.edu
 * $Id: TextTiling.h 43 2006-11-14 00:22:30Z galley $
 */

#ifndef __TEXTTILING_H__
#define __TEXTTILING_H__

#include <vector>
#include <set>
#include <string>
#include <fstream>
#include <iosfwd>

#include <lcseg/Seg.h>
#include <lcseg/BagOfWords.h>
#include <lcseg/hashutils.h>
#include <lcseg/logging.h>
#include <lcseg/defs.h>

namespace lcseg {

class TextTiling : public Seg {

private:
  // Make these member functions inaccessible:
  TextTiling(const TextTiling&);
  void operator=(const TextTiling&);
  void operator&(const TextTiling&);
  
public:
  // Default constructor.
  TextTiling() : Seg() {}
  
  // Run segmenter with default parameters;
  virtual void segment() { 
    texttiling_segment(-1,WINDOW_SIZE,FILTER_ORDER,CONS,ALPHA,PLIMIT); 
  }
  // Run segmenter with custom parameters:
  void texttiling_segment
    (int nb_seg, int window_size, int smooth, int cons, float alpha, float plimit);
  
  // Compute lexical cohesion function:
  void algo_lc_function(int=WINDOW_SIZE);
};

} 

#endif
