/**
 * Copyright © 2003-2006 by The Trustees of Columbia University in 
 * the City of New York.  All rights reserved.  This software is being
 * provided at no cost for educational and/or research purposes only.
 *
 * Author: Michel Galley, galley@cs.columbia.edu
 * $Id: LCseg.h 43 2006-11-14 00:22:30Z galley $
 */

#ifndef __LCSEG_H__
#define __LCSEG_H__

#include <vector>
#include <set>
#include <string>
#include <fstream>
#include <iosfwd>

#include <lcseg/Chain.h>
#include <lcseg/Seg.h>
#include <lcseg/BagOfWords.h>
#include <lcseg/hashutils.h>
#include <lcseg/logging.h>
#include <lcseg/defs.h>

namespace lcseg {

class LCseg : public Seg {

private:
  typedef FeatureVector<float> TREP;
  typedef std::vector<TREP> TREP_list;
  
  // Type definition of a hashtable whose keys are strings ("string" class)
  // and values are of type T (specified by the template definition)
#ifdef __GNU_CXX_NAMESPACE
  typedef __gnu_cxx::hash_map< const std::string, int, hash_str >
    hashmap_str;
#endif
#ifdef STD_NAMESPACE
  typedef std::hash_map< const std::string, int, hash_str >
    hashmap_str;
#endif

  // Make these member functions inaccessible:
  LCseg(const LCseg&);
  void operator=(const LCseg&);
  void operator&(const LCseg&);
  
public:
  // Default constructor.
  LCseg() : Seg() {}
  
  // Run segmenter with default parameters;
  virtual void segment() { 
    lcseg_segment(-1,RHIATUS,WINDOW_SIZE,FILTER_ORDER,CONS,ALPHA,PLIMIT); 
  }
  // Run segmenter with custom parameters:
  void lcseg_segment
    (int nb_seg, int rhiatus, int window_size, int smooth,
     int cons, float alpha, float plimit);
  
  // Find strong term repetitions:
  void algo_strong_chains(int rhiatus);

  // Compute lexical cohesion function:
  void algo_lc_function(int=WINDOW_SIZE);

  // Two sliding windows rotate:
  static bool _ROTATE;

protected:
  // Definition of set of unordered chains:
  typedef std::set<Chain> Chains;
  // Chains ordered by their starting
  // and ending positions:
  Chains chains;
};

} 

#endif
