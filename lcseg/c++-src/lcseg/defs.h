/**
 * Copyright © 2003-2006 by The Trustees of Columbia University in 
 * the City of New York.  All rights reserved.  This software is being
 * provided at no cost for educational and/or research purposes only.
 *
 * Author: Michel Galley, galley@cs.columbia.edu
 * $Id: defs.h 43 2006-11-14 00:22:30Z galley $
 */

#ifndef __LCSEG_DEFS_H__
#define __LCSEG_DEFS_H__

// Uncomment the following line if you want the running time:
#define BENCHMARK

// By default, remove isolated digits, isolated alphabetic characters,
// and words listed in the stopword list:
#define REMOVE_DIGITS
#define REMOVE_ALPHA
#define REMOVE_STOPWORDS

// Default parameter of LCseg:
#define WINDOW_SIZE    2
#define FILTER_ORDER   0
#define RHIATUS       11
#define CONS           1
#define ALPHA        0.5
#define PLIMIT       0.1

// Discards separators in input (this is convenient if you 
// want to segment goldstandard texts):
#define IGNORE_SEPARATORS

// String used to separate topic segments:
#define SEPARATOR "=========="

#endif
