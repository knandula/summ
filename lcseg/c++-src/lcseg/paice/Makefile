#
# Makefile for a Paice stemmer program.
# Written and tested by Christopher Fox 7/30/98
#

CC=gcc
CFLAGS=-g -O3
LDFLAGS=
LIBS=

all: paice.o

demo:	demo.o paice.o
	$(CC) -o $@ demo.o paice.o

stemmer:	stemmer.o paice.o
	$(CC) -o $@ stemmer.o paice.o

demo.o:	demo.c paice.h
	$(CC) $(CFLAGS) -c demo.c

paice.o:	paice.c paice.h
	$(CC) $(CFLAGS) -c paice.c


.PHONY: clean

clean:
	rm -f paice.o demo.o demo *~ core
