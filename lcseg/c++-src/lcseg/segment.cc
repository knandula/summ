/**
 * Copyright � 2003-2006 by The Trustees of Columbia University in 
 * the City of New York.  All rights reserved.  This software is being
 * provided at no cost for educational and/or research purposes only.
 *
 * Author: Michel Galley, galley@cs.columbia.edu
 * $Id: segment.cc 43 2006-11-14 00:22:30Z galley $
 */

#include <iostream>
#include <string>
#include <fstream>
#include <iosfwd>
#include <algorithm>
#include <cmath>

#include <lcseg/LCseg.h>
#include <lcseg/TextTiling.h>
#include <lcseg/BagOfWords.h>
#include <lcseg/TextToolkit.h>
#include <lcseg/defs.h>

using namespace lcseg;

void usage(const char *commandline) {
  std::cerr << "Do not run this program directly; use 'segment' instead."
            << std::endl;
  exit(1);
}

// Segmentation algorithm described in (Galley et al. '03):
int main(int argc, char* argv[]) {

  // Parameters of the algo:
  
  // Benchmarking:
#ifdef BENCHMARK
  clock_t time1 = clock();
#endif  
  
  // Set default parameters:
  int n = -1;
  int window_size = WINDOW_SIZE;
  int rhiatus = RHIATUS;
  float alpha = ALPHA;
  float plimit = PLIMIT;

  // Process command line parameters:
  int opt, dl=1;
  char *orig_file=NULL, *norm_file=NULL, *sw_list=NULL, *matlab_file=NULL;
  bool lc_func=false, texttiling=false;
  while ((opt = getopt(argc,argv,"o:f:m:lh:s:w:n:p:a:c:S:D:tR")) != -1)
    switch (opt) {  
      case 'o': orig_file=optarg;         break;
      case 'f': norm_file=optarg;         break;
      case 'm': matlab_file=optarg;       break;
      case 'l': lc_func=true;             break;
      case 'h': rhiatus=atoi(optarg);     break;
      case 'w': window_size=atoi(optarg); break;
      case 'n': n=atoi(optarg);           break;
      case 'p': plimit=atof(optarg);      break;
      case 'a': alpha=atof(optarg);       break;
      case 'S': sw_list=optarg;           break;
	  case 'D': dl = atoi(optarg);        break;
      case 't': texttiling=true;          break;
	  case 'R': LCseg::_ROTATE=true;      break;
   }

  // Change debug-level:
  if(dl != 1) {
	TextToolkit::_DL = dl;
  }
  
  // Check if a filename exists:
  if(!orig_file)
      usage(argv[0]);
  if(!norm_file) 
      norm_file = orig_file;

  // Load list of stopwords if one is provided: 
  if(sw_list != NULL)
    TextToolkit::load_commonword_list(sw_list);

  // Create segmenter object:
  Seg *seg;
  if(texttiling)
      seg = new TextTiling;
  else
      seg = new LCseg;
  
  // Open and read text file to be segmented:
  DLOG("Opening file: " << norm_file);
  std::ifstream norm_in(norm_file);
  // Check that the file could be opened:
  if(norm_in.fail()) {
    DERR("Error opening file : " << norm_file);
    exit(1);
  }

  // Read text and create context vectors:
  seg->read_text(norm_in);
  norm_in.close();

  // Segmentation:
  DLOG("Hiatus: " << rhiatus);
  DLOG("Nb of segments: " << n);
  if(texttiling)
      ((TextTiling*)seg)->
        texttiling_segment(n,window_size,FILTER_ORDER,CONS,alpha,plimit);
  else
      ((lcseg::LCseg*)seg)->
        lcseg_segment(n,rhiatus,window_size,FILTER_ORDER,CONS,alpha,plimit);

  // Three types of output: lexical cohesion function plot, 
  // matlab file, or segmented text:
  if(lc_func)
    seg->print_lc_function(std::cout);
  else if(matlab_file)
    seg->print_matlab_lc_function(matlab_file);
  else {
    // Seek the beginning of the input file, then print segmentation:
    std::ifstream orig_in(orig_file);
    // Check that the file could be opened:
    if(orig_in.fail()) {
      DERR("Error opening file : " << orig_file);
      exit(1);
    }
    seg->print_segmented_text(std::cout,orig_in);
    orig_in.close();
  }
  delete seg;

  // Benchmarking:
#ifdef BENCHMARK
  clock_t time2 = clock();
  std::cerr << "CPU time: " 
            << (double)(time2-time1)/(double)CLOCKS_PER_SEC 
            << "s.\n";
#endif
  return 0;
}
