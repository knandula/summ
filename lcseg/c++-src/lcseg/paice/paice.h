
/********************************** paice.h **********************************

   Purpose:    Header for paice stemming module - paice.c

   Provenance: Written by Chris Campbell, November 1993.
               Modified by C. FOx 7/30/98--change stemmer name

   Notes:      Returns FALSE (0) if a word containing non-alphabetical 
               characters has been passed to it and therefore is not stemmed,
               otherwise TRUE (1).
**/

/**************************** Public Routines   *******************************/

#ifndef PAICE_H
#define PAICE_H

extern char* PaiceStem( char *word );

#endif
