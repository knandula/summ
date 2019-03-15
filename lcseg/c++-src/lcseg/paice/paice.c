
/*******************************   paice.c   **********************************

   Version:      1.1

   Purpose:      Implementation of the Paice/Husk stemming algorithm given in:
                    Paice, C. 1990. "Another Stemmer," 
                    ACM SIGIR Forum, 24(3), 56-61.

   Provenance:   Written by Chris Campbell, November 1993.
                 Modified by C. Fox, 7/30/98
                   - renamed stemmer function
                   - changed function declarations to C/C++ form
                   - simplified some conditional expressions
                   - changed flags to booleans and added boolean constants
                   - replaced constant values with descriptive macro names
                   - fixed bug caused by unset word_intact variable
                   - fixed several small bugs in the rules
                   - radically simplified and sped up ImplementRules
                   - increased indentation to make more readable
                 Modified by C. Fox, 12/19/00
                   - fixed subtle same length replacement bug

   Notes:        Written for CS5705 project.
**/

/**************************** Standard Include Files **************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/****************** Private Defines and Data Structures ***********************/

#define FALSE           0
#define TRUE            1
#define EOS           '\0'       /* end-of-string */
#define LAMBDA        "\0"       /* empty string */
/*#define TRACE                    /* for seeing what happens */

#define IsVowel(c)   ('a'==(c)||'e'==(c)||'i'==(c)||'o'==(c)||'u'==(c))

typedef struct 
{
    int old_offset;          /* from end of word to start of suffix */
    char *old_end;           /* suffix replaced */
    char *new_end;           /* suffix replacement */
    int replace_length;      /* length of old suffix to replace */
    int intact_flag;         /* use rule only if original word */
    int terminate_flag;      /* continue stemming or quit flag */
} RuleList;

static char *word_end;         /* pointer to the end of the word */
static int word_intact;        /* keep track if a word has been stemmed */

static RuleList a_rules[] =
{
      1,      "ia",      LAMBDA,    2,    TRUE,      TRUE,
      0,       "a",      LAMBDA,    1,    TRUE,      TRUE,
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};

static RuleList b_rules[] =
{
      1,      "bb",      LAMBDA,    1,    FALSE,     TRUE,
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};

static RuleList c_rules[] =
{
      3,      "ytic",   "s",        3,    FALSE,     TRUE,
      1,      "ic",     LAMBDA,     2,    FALSE,     FALSE,
      1,      "nc",     "t",        1,    FALSE,     FALSE,
      0,      "\0",     LAMBDA,     0,    FALSE,     TRUE
};

static RuleList d_rules[] =
{
      1,      "dd",     LAMBDA,     1,    FALSE,     TRUE,
      2,      "ied",    "y",        3,    FALSE,     FALSE,
      3,      "ceed",   "ss",       2,    FALSE,     TRUE,
      2,      "eed",    LAMBDA,     1,    FALSE,     TRUE,
      1,      "ed",     LAMBDA,     2,    FALSE,     FALSE,
      3,      "hood",   LAMBDA,     4,    FALSE,     FALSE,
      0,      "\0",     LAMBDA,     0,    FALSE,     TRUE
};

static RuleList e_rules[] =
{
      0,      "e",       LAMBDA,    1,    FALSE,     FALSE,
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};

static RuleList f_rules[] =
{
      3,      "lief",    "v",       1,    FALSE,     TRUE,
      1,      "if",      LAMBDA,    2,    FALSE,     FALSE,
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};

static RuleList g_rules[] =
{
      2,      "ing",     LAMBDA,    3,    FALSE,     FALSE,
      2,      "iag",     "y",       3,    FALSE,     TRUE,
      1,      "ag",      LAMBDA,    2,    FALSE,     FALSE,
      1,      "gg",      LAMBDA,    1,    FALSE,     TRUE,
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};

static RuleList h_rules[] =
{
      1,      "th",      LAMBDA,    2,    TRUE,      TRUE,
      4,      "guish",   "ct",      5,    FALSE,     TRUE,
      2,      "ish",     LAMBDA,    3,    FALSE,     FALSE,
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};

static RuleList i_rules[] =
{
      0,      "i",       LAMBDA,    1,    TRUE,      TRUE,
      0,      "i",       "y",       1,    FALSE,     FALSE,
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};

static RuleList j_rules[] =
{
      1,      "ij",      "d",       1,    FALSE,     TRUE,
      2,      "fuj",     "s",       1,    FALSE,     TRUE,
      1,      "uj",      "d",       1,    FALSE,     TRUE,
      1,      "oj",      "d",       1,    FALSE,     TRUE,
      2,      "hej",     "r",       1,    FALSE,     TRUE,
      3,      "verj",    "t",       1,    FALSE,     TRUE,
      3,      "misj",    "t",       2,    FALSE,     TRUE,
      1,      "nj",      "d",       1,    FALSE,     TRUE,
      0,      "j",       "s",       1,    FALSE,     TRUE,
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};

static RuleList k_rules[] =
{
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};

static RuleList l_rules[] =
{
      5,      "ifiabl",  LAMBDA,    6,    FALSE,     TRUE,
      3,      "iabl",    "y",       4,    FALSE,     TRUE,
      2,      "abl",     LAMBDA,    3,    FALSE,     FALSE,
      2,      "ibl",     LAMBDA,    3,    FALSE,     TRUE,
      2,      "bil",     "l",       2,    FALSE,     FALSE,
      1,      "cl",      LAMBDA,    1,    FALSE,     TRUE,
      3,      "iful",    "y",       4,    FALSE,     TRUE,
      2,      "ful",     LAMBDA,    3,    FALSE,     FALSE,
      1,      "ul",      LAMBDA,    2,    FALSE,     TRUE,
      2,      "ial",     LAMBDA,    3,    FALSE,     FALSE,
      2,      "ual",     LAMBDA,    3,    FALSE,     FALSE,
      1,      "al",      LAMBDA,    2,    FALSE,     FALSE,
      1,      "ll",      LAMBDA,    1,    FALSE,     TRUE,
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};

static RuleList m_rules[] =
{
      2,      "ium",     LAMBDA,    3,    FALSE,     TRUE,
      1,      "um",      LAMBDA,    2,    TRUE,      TRUE,
      2,      "ism",     LAMBDA,    3,    FALSE,     FALSE,
      1,      "mm",      LAMBDA,    1,    FALSE,     TRUE,
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};

static RuleList n_rules[] =
{
      3,      "sion",    "j",       4,    FALSE,     FALSE,
      3,      "xion",    "ct",      4,    FALSE,     TRUE,
      2,      "ion",     LAMBDA,    3,    FALSE,     FALSE,
      2,      "ian",     LAMBDA,    3,    FALSE,     FALSE,
      1,      "an",      LAMBDA,    2,    FALSE,     FALSE,
      2,      "een",     LAMBDA,    0,    FALSE,     TRUE,
      1,      "en",      LAMBDA,    2,    FALSE,     FALSE,
      1,      "nn",      LAMBDA,    1,    FALSE,     TRUE,
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};

static RuleList o_rules[] =
{
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};

static RuleList p_rules[] =
{
      3,      "ship",    LAMBDA,    4,    FALSE,     FALSE,
      1,      "pp",      LAMBDA,    1,    FALSE,     TRUE,
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};

static RuleList q_rules[] =
{
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};

static RuleList r_rules[] =
{
      1,      "er",      LAMBDA,    2,    FALSE,     FALSE,
      2,      "ear",     LAMBDA,    0,    FALSE,     TRUE,
      1,      "ar",      LAMBDA,    2,    FALSE,     TRUE,
      1,      "or",      LAMBDA,    2,    FALSE,     FALSE,
      1,      "ur",      LAMBDA,    2,    FALSE,     FALSE,
      1,      "rr",      LAMBDA,    1,    FALSE,     TRUE,
      1,      "tr",      LAMBDA,    1,    FALSE,     FALSE,
      2,      "ier",     "y",       3,    FALSE,     FALSE,
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};

static RuleList s_rules[] =
{
      2,      "ies",     "y",       3,    FALSE,     FALSE,
      2,      "sis",     LAMBDA,    2,    FALSE,     TRUE,
      1,      "is",      LAMBDA,    2,    FALSE,     FALSE,
      3,      "ness",    LAMBDA,    4,    FALSE,     FALSE,
      1,      "ss",      LAMBDA,    0,    FALSE,     TRUE,
      2,      "ous",     LAMBDA,    3,    FALSE,     FALSE,
      1,      "us",      LAMBDA,    2,    TRUE,      TRUE,
      0,      "s",       LAMBDA,    1,    TRUE,      FALSE,
      0,      "s",       LAMBDA,    0,    FALSE,     TRUE,
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};

static RuleList t_rules[] =
{
      5,      "plicat",  "y",       4,    FALSE,     TRUE,
      1,      "at",      LAMBDA,    2,    FALSE,     FALSE,
      3,      "ment",    LAMBDA,    4,    FALSE,     FALSE,
      2,      "ent",     LAMBDA,    3,    FALSE,     FALSE,
      2,      "ant",     LAMBDA,    3,    FALSE,     FALSE,
      3,      "ript",    "b",       2,    FALSE,     TRUE,
      3,      "orpt",    "b",       2,    FALSE,     TRUE,
      3,      "duct",    LAMBDA,    1,    FALSE,     TRUE,
      4,      "sumpt",   LAMBDA,    2,    FALSE,     TRUE,
      3,      "cept",    "iv",      2,    FALSE,     TRUE,
      3,      "olut",    "v",       2,    FALSE,     TRUE,
      3,      "sist",    LAMBDA,    0,    FALSE,     TRUE,
      2,      "ist",     LAMBDA,    3,    FALSE,     FALSE,
      1,      "tt",      LAMBDA,    1,    FALSE,     TRUE,
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};

static RuleList u_rules[] =
{
      2,      "iqu",     LAMBDA,    3,    FALSE,     TRUE,
      2,      "ogu",     LAMBDA,    1,    FALSE,     TRUE,
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};

static RuleList v_rules[] =
{
      2,      "siv",     "j",       3,    FALSE,     FALSE,
      2,      "eiv",     LAMBDA,    0,    FALSE,     TRUE,
      1,      "iv",      LAMBDA,    2,    FALSE,     FALSE,
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};

static RuleList w_rules[] =
{
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};

static RuleList x_rules[] =
{
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};

static RuleList y_rules[] =
{
      2,      "bly",     LAMBDA,    1,    FALSE,     FALSE,
      2,      "ily",     "y",       3,    FALSE,     FALSE,
      2,      "ply",     LAMBDA,    0,    FALSE,     TRUE,
      1,      "ly",      LAMBDA,    2,    FALSE,     FALSE,
      2,      "ogy",     LAMBDA,    1,    FALSE,     TRUE,
      2,      "phy",     LAMBDA,    1,    FALSE,     TRUE,
      2,      "omy",     LAMBDA,    1,    FALSE,     TRUE,
      2,      "opy",     LAMBDA,    1,    FALSE,     TRUE,
      2,      "ity",     LAMBDA,    3,    FALSE,     FALSE,
      2,      "ety",     LAMBDA,    3,    FALSE,     FALSE,
      2,      "lty",     LAMBDA,    2,    FALSE,     TRUE,
      4,      "istry",   LAMBDA,    5,    FALSE,     TRUE,
      2,      "ary",     LAMBDA,    3,    FALSE,     FALSE,
      2,      "ory",     LAMBDA,    3,    FALSE,     FALSE,
      2,      "ify",     LAMBDA,    3,    FALSE,     TRUE,
      2,      "ncy",     "t",       2,    FALSE,     FALSE,
      2,      "acy",     LAMBDA,    3,    FALSE,     FALSE,
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};

static RuleList z_rules[] =
{
      1,      "iz",      LAMBDA,    2,    FALSE,     FALSE,
      1,      "yz",      "s",       1,    FALSE,     TRUE,
      0,      "\0",      LAMBDA,    0,    FALSE,     TRUE
};


/************************ Private Function Declarations ***********************/

static int Implement_Rules( char *word, RuleList *rule );
static int Acceptance_Test( char *word );

/******************************************************************************/
/*********************** Private Function Routines ****************************/

/*FN****************************************************************************

       Acceptance_Test( word )

    Returns: int -- TRUE (1) if the stemmed word passes test , FALSE (0) to 
             restore the word to form prior to most recent stemming.

    Purpose: Test a stemmed word to make sure it is not wrongly reduced too far.

    Plan:    Test first character for a vowel or consonant. Depending on which
             one is found check the appropriate acceptability conditions. If
             the conditions are met return TRUE else return FALSE. 

    Notes:   None.
**/

static int
Acceptance_Test(
   char *word )     /* in: buffer with word to check */
{
  char *start;      /* used to trip through word testing chars */

  if( IsVowel(*word) )               /* test 1st character of word */
  {
    if( 1 < strlen(word) )           /* word must be at least 2 char. long */
      return(TRUE);
  }
  else                               /* must be a character */
  {
    if( 2 < strlen(word) )           /* word must be at least 3 char. long */
    {
      for(start = word; *start != EOS; start++)  /* test each char. in word */
      {
         if(IsVowel( *start ) || 'y' == *start)  /* does a vowel or 'y' exist */
           return(TRUE);
      }
    }
  }
  return(FALSE);

}  /* Acceptance_Test */

/*FN****************************************************************************

       Implement_Rules( word, rule )

    Returns: int -- TRUE (1) if stemming should terminate, FALSE (0) otherwise

    Purpose: Apply a set of rules to replace the suffix of a word.

    Plan:    Loop through rule set until a suffix match match is found. Check
             all appropriate conditions, if they are met implement rule on 
             the word. If the stemming process should continue return TRUE else
             return FALSE.

    Notes:   This is the main routine driving the stemmer. It goes through a 
             set of suffix replacement rules looking for a match to the current
             suffix. When it finds one and all other conditions are met, then
             the suffix is either replaced, dropped or protected, and the
             function will return.   
**/

static int 
Implement_Rules(
   register char *word,     /* in/out: buffer with the word to stem */
   RuleList *rule )         /* in: data structure with stemming rule */
{
  int word_length = strlen(word);  /* length of incoming word */

  for ( /* rules passed in */ ; *(rule->old_end); rule++ )
  {
     if ( rule->old_offset < word_length )
     {
        char *ending = word_end - rule->old_offset;
        if ( 0 == strcmp(ending,rule->old_end) )
           if ( !rule->intact_flag || (rule->intact_flag && word_intact) )
           {
              char stemmed_word[word_length+1];
              (void)strcpy( stemmed_word, word );
              (void)strcpy( stemmed_word + word_length - rule->replace_length,
                            rule->new_end );

              if ( Acceptance_Test(stemmed_word) )
              {
                 #ifdef TRACE
                    printf( "Convert %s to %s\n", word, stemmed_word );
                 #endif
                 (void)strcpy( word, stemmed_word );
                 word_end = word + strlen(word) - 1;
                 word_intact = FALSE;
                 return ( rule->terminate_flag );
              }
           }
     }
  }

  return( TRUE );  /* stop stemming if no rules apply */

}  /* Implement_Rules */

/******************************************************************************/
/*********************** Public Function Declarations *************************/

/*FN****************************************************************************

       PaiceStem( word )

    Returns: int -- FALSE (0) if the word contains non-alphabetical characters
             and therefore is not stemmed, TRUE (1) otherwise.

    Purpose: Stem a word.

    Plan:    Part 1: Test the word to make sure it is all alphabetic. 
             Part 2: Implement the Paice/Husk stemming and rules. 
             Part 3: Return an indication of a successful stemming.

    Notes:   This function implements the Paice/Husk stemming algorithm. See:
                  Paice, C. 1990. "Another Stemmer," 
                  ACM SIGIR Forum, 24(3), 56-61.
**/

char*
PaiceStem(
   char *word )      /* in/out: the word stemmed */
{
  int stop = TRUE;   /* loop until it is time to stop */
      
      /* Part 1:   Check incoming word to ensure all alpha. characters */
  for( word_end = word; *word_end != EOS; word_end++)
    if( !isalpha( *word_end ) ) return( word );
  word_end--;                     /* now points at last char. in word */

      /* Part 2:   Implement the Paice/Husk stemming algorithm */
  word_intact = TRUE;
  do                  /* search for rules section */
    switch( *word_end )
    {
       case 'a': stop = Implement_Rules( word, a_rules );
                 break;
       case 'b': stop = Implement_Rules( word, b_rules );
                 break;
       case 'c': stop = Implement_Rules( word, c_rules );
                 break;
       case 'd': stop = Implement_Rules( word, d_rules );
                 break;
       case 'e': stop = Implement_Rules( word, e_rules );
                 break;
       case 'f': stop = Implement_Rules( word, f_rules );
                 break;
       case 'g': stop = Implement_Rules( word, g_rules );
                 break;
       case 'h': stop = Implement_Rules( word, h_rules );
                 break;
       case 'i': stop = Implement_Rules( word, i_rules );
                 break;
       case 'j': stop = Implement_Rules( word, j_rules );
                 break;
       case 'k': stop = Implement_Rules( word, k_rules );
                 break;
       case 'l': stop = Implement_Rules( word, l_rules );
                 break;
       case 'm': stop = Implement_Rules( word, m_rules );
                 break;
       case 'n': stop = Implement_Rules( word, n_rules );
                 break;
       case 'o': stop = Implement_Rules( word, o_rules );
                 break;
       case 'p': stop = Implement_Rules( word, p_rules );
                 break;
       case 'q': stop = Implement_Rules( word, q_rules );
                 break;
       case 'r': stop = Implement_Rules( word, r_rules );
                 break;
       case 's': stop = Implement_Rules( word, s_rules );
                 break;
       case 't': stop = Implement_Rules( word, t_rules );
                 break;
       case 'u': stop = Implement_Rules( word, u_rules );
                 break;
       case 'v': stop = Implement_Rules( word, v_rules );
                 break;
       case 'w': stop = Implement_Rules( word, w_rules );
                 break;
       case 'x': stop = Implement_Rules( word, x_rules );
                 break;
       case 'y': stop = Implement_Rules( word, y_rules );
                 break;
       case 'z': stop = Implement_Rules( word, z_rules );
                 break;
       default : stop = TRUE;   /* no section found */
                 break;
    }  /* switch */
  while ( !stop );     /* check for another rule section */
 
      /* Part 3: Return indication of successful completion of stemmer */
  return( word ); 

} /* PaiceStem */
