/**
 * Copyright © 2003-2006 by The Trustees of Columbia University in 
 * the City of New York.  All rights reserved.  This software is being
 * provided at no cost for educational and/or research purposes only.
 *
 * Author: Michel Galley, galley@cs.columbia.edu
 * $Id: TextToolkit.h 43 2006-11-14 00:22:30Z galley $
 */

#ifndef __TEXTTOOLKIT_H__
#define __TEXTTOOLKIT_H__

//#include <boost/regex.hpp>

#include <lcseg/hashutils.h>

namespace lcseg {

class TextToolkit {

public:
  //! Dynamically load a list of stop words, and store them 
  //! in a lexicon.
  static void load_commonword_list(const char* filename);
  //! Remove common words contained in a stop list
  //! (most closed-class words plus very common words):
  static bool is_commonword(char*);
  //! Debug-level:
  static int _DL;

private:
  //! Lexicon storing dynamically loaded list of stopwords.
  static Lexicon _lexicon;
  
};

}

#endif
