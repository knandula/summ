
/********************************   demo.c   *********************************
 *
 *  Program to demonstrate and test the Paice stemming function.  This 
 *  program takes a single filename on the command line and lists the terms
 *  in the file and their stems on stdout.
 *
 *   7/10/95 Written from stemmer by C. Fox
 *  11/25/97 Comments and names updated by C. Fox
 *   7/30/98 Adapted from Porter to Paice stemmer by C. Fox
 *   8/30/00 Timing capability added by C. Fox
**/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "paice.h"

#define  USAGE = "usage: demo [word file]\n"

/*#define __TIMING__       /* define this if you want stem timing output */

/******************************************************************************/
/********************   Private Defines and Data Structures   *****************/

#define EOS                           '\0'

/******************************************************************************/
/************************   Private Function Definitions   ********************/

#ifdef __STDC__

static char * GetNextTerm( FILE *stream, int size, char *term );

#else

static char * GetNextTerm();

#endif

/*FN***************************************************************************

        GetNextTerm( stream, size, term )

   Returns: char * -- buffer with the next input term, NULL at EOF

   Purpose: Grab the next token from an input stream

   Plan:    Part 1: Return NULL immediately if there is no input
            Part 2: Initialize the local variables
            Part 3: Main Loop: Put the next word into the term buffer
            Part 4: Return the output buffer

   Notes:   None.
**/

static char *
GetNextTerm( stream, size, term )
   FILE *stream;  /* in: source of input characters */
   int size;      /* in: bytes in the output buffer */
   char *term;    /* in/out: where the next term in placed */
   {
   char *ptr;  /* for scanning through the term buffer */
   int ch;     /* current character during input scan */

           /* Part 1: Return NULL immediately if there is no input */
   if ( EOF == (ch = getc(stream)) ) return( NULL );

                  /* Part 2: Initialize the local variables */
   *term = EOS;
   ptr = term;

        /* Part 3: Main Loop: Put the next word into the term buffer */
   do
      {
         /* scan past any leading non-alphabetic characters */
      while ( (EOF != ch ) && !isalpha(ch) ) ch = getc( stream );

         /* copy input to output while reading alphabetic characters */
      while ( (EOF != ch ) && isalpha(ch) )
         {
         if ( ptr == (term+size-1) ) ptr = term;
         *ptr++ = ch;
         ch = getc( stream );
         }

         /* terminate the output buffer */
      *ptr = EOS;
      }
   while ( (EOF != ch) && !*term );

                    /* Part 4: Return the output buffer */
   return( term );

   } /* GetNextTerm */

/******************************************************************************/
/*FN***************************************************************************

        main( argc, argv )

   Returns: int -- 0 on success, 1 on failure

   Purpose: Program main function

   Plan:    Part 1: Open the input file
            Part 2: Process each word in the file
            Part 3: Close the input file and return

   Notes:   
**/

int
main( argc, argv )
   int argc;     /* in: how many arguments */
   char *argv[]; /* in: text of the arguments */
   {
#ifdef __TIMING__
   clock_t startTime;   /* when stemming starts */
   double duration;     /* how long it took */
#endif

   char term[64];   /* for the next term from the input line */
   char stem[64];   /* for the stem of the term */
   FILE *stream;    /* where to read characters from */

                       /* Part 1: Open the input file */
   if ( !(stream = fopen(argv[1],"r")) ) exit(1);

                  /* Part 2: Process each word in the file */
#ifdef __TIMING__
   startTime=clock();
   while( GetNextTerm(stream,64,term) ) PaiceStem( term );
   duration = (double)(clock()-startTime)/CLOCKS_PER_SEC;
   printf( "Stemming duration in seconds: %lf \n", duration );
#else
   while( GetNextTerm(stream,64,term) )
      {
      (void)strcpy(stem,term);
      PaiceStem( stem );
      (void)printf( "%20s %s\n", term, stem );
      }
#endif

                 /* Part 3: Close the input file and return */
   (void)fclose( stream );
   return(0);

   } /* main */
