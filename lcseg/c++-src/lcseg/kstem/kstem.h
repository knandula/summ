/**
 * Copyright © 2003-2006 by The Trustees of Columbia University in 
 * the City of New York.  All rights reserved.  This software is being
 * provided at no cost for educational and/or research purposes only.
 *
 * Author: Michel Galley, galley@cs.columbia.edu
 * $Id: kstem.h 43 2006-11-14 00:22:30Z galley $
 */

#ifndef __KSTEM_H__
#define __KSTEM_H__

extern void initdb();
extern void closedb();
extern char* KStem( char *word );

#endif

