/**
 * Copyright © 2003-2006 by The Trustees of Columbia University in 
 * the City of New York.  All rights reserved.  This software is being
 * provided at no cost for educational and/or research purposes only.
 *
 * Author: Michel Galley, galley@cs.columbia.edu
 * $Id: logging.h 43 2006-11-14 00:22:30Z galley $
 */

#ifndef __LOGGING_H__
#define __LOGGING_H__

#include <iostream>
#include <cassert>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <unistd.h>

//! Set this to 2 to print only unexpected bahaviours, 
//! and to 1 to also print various logging information.
#define DEFDEBUGLEVEL 1

//! Print debug information. 
//! Do not remove {'s and }'s !!!
//! Otherwise, you will get a bad surprise if you run:
//! if(false) 
//!   OS_MSG(os,"when false seems to be true...",0)
#define _OS_MSG(OSTREAM,MESSAGE,DEBUGLEVEL,THRESHOLD) \
  { \
    if(DEBUGLEVEL >= THRESHOLD) { \
		assert(OSTREAM != NULL); \
        *OSTREAM << __BASE_FILE__ << ":" \
		         << __LINE__ << ": " \
		         << MESSAGE << std::endl; \
    } \
  }

#define OS_MSG(OSTREAM,MESSAGE,THRESHOLD) \
  _OS_MSG(OSTREAM,MESSAGE,DEFDEBUGLEVEL,THRESHOLD)

#define OS_LOG(OS,TXT)  OS_MSG(OS,TXT,2)
#define OS_ERR(OS,TXT)  OS_MSG(OS,TXT,1)
#define DLOG(TXT)       OS_LOG(&std::clog,TXT)
#define DERR(TXT)       OS_ERR(&std::cerr,TXT)

#define LOGT(D,T,TXT)     _OS_MSG(&std::clog,TXT,D,T)

//! Stop program execution. (do not remove { }'s !!)
#define OS_EXIT(OS) \
  { \
	assert(OS != NULL); \
    OS_ERR(OS,"aborting program execution.") \
    exit(1); \
  }

#define DEXIT()       OS_EXIT(&std::cerr)

#define OS_FLUSH(OSTREAM) \
  { \
	assert(OSTREAM != NULL); \
	OSTREAM->flush(); \
  } 

#endif
