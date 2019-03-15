#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <db.h>

#define  DATABASE "../../../data/pandk_loose.db"
#define  MAX_LINE_LEN 50000
#define  MAX_WORD_LEN 50
/* Change the following value if changing stem classes: */
#define  NB_KEYS 53904
/* Parameters to optimize the DB (you can safely ignore this): */
#define  PAGE_SIZE 8192
#define  AVG_KEY_SIZE 8.26
#define  AVG_DATA_SIZE 9.0
#define  FFACTOR ((int)((PAGE_SIZE-32)/(AVG_KEY_SIZE+AVG_DATA_SIZE+8)))

DB *dbp;

void
initdb() {
   FILE *f;
   int ret, t_ret;
   if ((ret = db_create(&dbp, NULL, 0)) != 0) {
      fprintf(stderr, "db_create: %s\n", db_strerror(ret));
      exit (1);
   }
   
	
   // Make sure the database doesn't already exist:
   if((f = fopen(DATABASE,"r")) != NULL ) {
     printf("File exists: %s\n",DATABASE);
     exit(1);
   }

   
   
   // Set the number of keys:
   dbp->set_h_nelem(dbp,NB_KEYS);
   dbp->set_h_ffactor(dbp,FFACTOR);
   fprintf(stderr,"Number of keys: %d Fill factor: %d\n",NB_KEYS,FFACTOR);

   // Open the database:
   if ((ret = dbp->open(dbp, NULL,
      DATABASE, NULL, DB_HASH, DB_CREATE, 0664)) != 0) {
      dbp->err(dbp, ret, "%s", DATABASE);
      dbp->close(dbp, 0);
      exit (1);
   }
}

void
closedb() {
  dbp->close(dbp, 0);
}

void
add_key(char* k, char* d)
{
   DBT key, data;
   int ret, t_ret;
   memset(&key, 0, sizeof(key));
   memset(&data, 0, sizeof(data));
   key.data = k;
   key.size = strlen(k)+1;
   data.data = d;
   data.size = strlen(d)+1;

   if ((ret = dbp->put(dbp, NULL, &key, &data, 0)) != 0) {
      dbp->err(dbp, ret, "DB->put");
      dbp->close(dbp, 0);
      exit(1);
   }
}

int main() {
  char line[MAX_LINE_LEN];
  char stem_copy[MAX_WORD_LEN];
  char *stem, *word;
  initdb();
  /* Read input: */
  while(fgets(line,MAX_LINE_LEN, stdin) != NULL) {
    stem = strtok(line, " ");
    while((word = strtok(NULL, " \n")) != NULL) {
      /* fprintf(stdout,"word: %s\tstem: %s\n",word,stem); */
      add_key(word,stem);
    }
  }
  closedb();
  return 0;  
}
