include ../../Env.mak

CC=gcc
CXX=g++
LDFLAGS = -L$(BERKELEY)/lib -ldb
CCFLAGS=-g -O3 -DNDEBUG -DBASEDIR=\"$(BASEDIR)\" -I../../ -I$(BERKELEY)/include
CXXFLAGS=-g -O3 -DNDEBUG -DBASEDIR=\"$(BASEDIR)\" -I../../ -I$(BERKELEY)/include

STEMFILE = ../../../data/pandk_loose.stemclass
STEMDB   = ../../../data/pandk_loose.db

all: stem kstem.o $(STEMDB) 

kstem.o: kstem.c kstem.h
	$(CC) $(CCFLAGS) -c kstem.c

$(STEMDB): createstem $(STEMFILE)
	rm -f $(STEMDB)
	cat $(STEMFILE) | LD_LIBRARY_PATH=$(BASEDIR)/sharedlibs ./createstem

createstem: createstem.c
	$(CC) $(CCFLAGS) -o createstem createstem.c $(LIBS) $(LDFLAGS) 

stem: stem.cc kstem.o
	$(CXX) $(CXXFLAGS) -o $@ stem.cc kstem.o $(LIBS) $(LDFLAGS) 

.PHONY: clean

clean:
	rm -f kstem.o *~

realclean: clean
	rm -f $(STEMDB) createstem stem
