/**
 * Copyright © 2003-2006 by The Trustees of Columbia University in 
 * the City of New York.  All rights reserved.  This software is being
 * provided at no cost for educational and/or research purposes only.
 *
 * Author: Michel Galley, galley@cs.columbia.edu
 * $Id: stoplist.h 43 2006-11-14 00:22:30Z galley $
 */

#ifndef __STOPLIST_H__
#define __STOPLIST_H__

namespace lcseg {

unsigned 
int sw_hash(register char *);

int /* actually boolean */ 
sw_in_word_set(register char *str);

}

#endif
