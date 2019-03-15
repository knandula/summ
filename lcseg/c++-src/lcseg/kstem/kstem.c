/**
 * Copyright © 2003-2006 by The Trustees of Columbia University in 
 * the City of New York.  All rights reserved.  This software is being
 * provided at no cost for educational and/or research purposes only.
 *
 * Author: Michel Galley, galley@cs.columbia.edu
 * $Id: kstem.c 43 2006-11-14 00:22:30Z galley $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <db.h>

#define  DATABASE "/data/pandk_loose.db"
#define MAX_LINE_LEN 50000
/* Make the cache large enough to contain the entire DB: */
#define CACHE_SIZE  (2048*1024)
#define CONCAT(x,y) (x y)
#define LCSEGDIR    (getenv("LCSEGDIR"))

DB *dbp=NULL;

void
initdb() {

	/* Determine database name: */
	char dbname[1024];
	if(LCSEGDIR == NULL) {
		printf("Error: environment variable LCSEGDIR is undefined.\n");
		exit(1);
	}
	strcpy(dbname,LCSEGDIR);
	strcpy(dbname+strlen(dbname),DATABASE);
	
    /* Open DB: */
    if(dbp != NULL)
      return;
    int ret;

    if ((ret = db_create(&dbp, NULL, 0)) != 0) {
        fprintf(stderr, "db_create: %s\n", db_strerror(ret));
        exit(1);
    }

    /* Set cache size: */
    dbp->set_cachesize(dbp,0,CACHE_SIZE,0);
    /* Open the database in read-only mode: */
    if ((ret = dbp->open(dbp,
        NULL,
        dbname, NULL, DB_HASH, DB_RDONLY, 0664)) != 0) {
        dbp->err(dbp, ret, "%s", dbname);
        dbp->close(dbp, 0);
        exit (1);
    }
}

void
closedb() {
    if(dbp != NULL)
        dbp->close(dbp, 0);
    dbp=NULL;
}

char*
KStem(char* word)
{
    DBT key, data;
    int ret;
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));
    key.data = word;
    key.size = strlen(word)+1;

    if ((ret = dbp->get(dbp, NULL, &key, &data, 0)) == 0) {
        /* printf("db: %s: key retrieved: data was %s.\n",
                 (char *)key.data, (char *)data.data); */
        return (char *)data.data;
    }
    return word;
}
