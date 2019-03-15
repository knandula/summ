#####################################################################
## Copyright � 2003-2006 by The Trustees of Columbia University in 
## the City of New York.  All rights reserved.  This software is being
## provided at no cost for educational and/or research purposes only.
#####################################################################

#####################################################################
# Build options 
#####################################################################

# Other options may be machine/platform specific:
include Env.mak

COMMONARGS = -g -O2 -DNDEBUG -Wall -fpermissive -Wno-deprecated -Wno-write-strings
CXXARGS = $(COMMONARGS) -I$(BOOST) -I.
CCARGS = $(COMMONARGS)
LDARGS = -L. -llcseg -L$(BERKELEY) -l db 

#####################################################################
# Directories
#####################################################################

BASEDIR=$(PWD)/../
BIN_DIR = $(BASEDIR)/bin
SRC_DIR = lcseg
JAVA_SRC_DIR = $(BASEDIR)/java-src

#####################################################################
# Directories
#####################################################################

CXX  = g++
CC   = gcc
AR   = ar cr
RM   = /bin/rm -f
JAVA = java

#####################################################################
# Targets
#####################################################################

TARGET = $(BIN_DIR)/segment.i686.out

LIB    = liblcseg.a

SEG_OBJ     = $(SRC_DIR)/Seg.o
LCSEG_OBJ   = $(SRC_DIR)/LCseg.o
TTSEG_OBJ   = $(SRC_DIR)/TextTiling.o
TTK_OBJ     = $(SRC_DIR)/TextToolkit.o
#STOP_OBJ    = $(SRC_DIR)/stoplist.o
KSTEM_OBJ   = $(SRC_DIR)/kstem/kstem.o
PAICE_OBJ   = $(SRC_DIR)/paice/paice.o
COMMON_OBJS = $(SEG_OBJ) $(LCSEG_OBJ) $(TTSEG_OBJ) $(TTK_OBJ)   $(KSTEM_OBJ)
#COMMON_OBJS = $(SEG_OBJ) $(LCSEG_OBJ) $(TTSEG_OBJ) $(TTK_OBJ) $(STOP_OBJ)  $(KSTEM_OBJ)
# Uncomment the following if you want to use Paice's stemmer
# intead of the K stemmer (and comment out the above two lines):
#COMMON_OBJS = $(SEG_OBJ) $(LCSEG_OBJ) $(TTK_OBJ) $(STOP_OBJ) $(PAICE_OBJ)

#####################################################################
# Build directives 
#####################################################################

all: $(TARGET)

$(TARGET): $(LIB) $(SRC_DIR)/segment.cc
	$(CXX) $(CXXARGS) $(SRC_DIR)/segment.cc  $(LDARGS) -o $(TARGET)

$(LIB): $(COMMON_OBJS)
	$(AR) $@ $^

$(SRC_DIR)/segment.o: $(SRC_DIR)/segment.cc
	$(CXX) $(CXXARGS) $(STEMARG) -c $<

$(LCSEG_OBJ): $(SRC_DIR)/LCseg.cc $(SRC_DIR)/LCseg.h $(SRC_DIR)/defs.h
	$(CXX) $(CXXARGS) $(STEMARG) -c -o $@ $(SRC_DIR)/LCseg.cc

$(TTSEG_OBJ): $(SRC_DIR)/TextTiling.cc $(SRC_DIR)/TextTiling.h $(SRC_DIR)/defs.h
	$(CXX) $(CXXARGS) $(STEMARG) -c -o $@ $(SRC_DIR)/TextTiling.cc

$(SEG_OBJ): $(SRC_DIR)/Seg.cc $(SRC_DIR)/Seg.h $(SRC_DIR)/FeatureVector.h \
            $(SRC_DIR)/BagOfWords.h $(SRC_DIR)/defs.h \
            $(SRC_DIR)/kstem/Makefile
	cd $(SRC_DIR)/kstem; BASEDIR=$(BASEDIR) make
	$(CXX) $(CXXARGS) $(STEMARG) -c -o $@ $(SRC_DIR)/Seg.cc 

$(TTK_OBJ): $(SRC_DIR)/TextToolkit.cc $(SRC_DIR)/TextToolkit.h \
            $(SRC_DIR)/stoplist.h
	$(CXX) $(CXXARGS) -c -o $@ $(SRC_DIR)/TextToolkit.cc

#$(STOP_OBJ): $(SRC_DIR)/stoplist.c $(SRC_DIR)/stoplist.h
#	$(CC) $(CCARGS) -c -o $@ $(SRC_DIR)/stoplist.c

$(SRC_DIR)/stoplist.c: ../data/commonwords.list \
                       $(JAVA_SRC_DIR)/perfect_hash/Makefile
	cd $(JAVA_SRC_DIR)/perfect_hash && make
	$(JAVA) -cp $(JAVA_SRC_DIR) perfect_hash/GGPerf -gnuc -returnboolean \
	     -hash_name=sw_hash -lookup_name=sw_in_word_set \
	     $< $(SRC_DIR)/stoplist.c

#####################################################################
# Segmentation on a toy example...
#####################################################################

.PHONY: test

test:
	cat ../samples/dummy_plain.txt | $(BIN_DIR)/segment

#####################################################################
# Cleanup
#####################################################################

.PHONY: clean fullclean

clean: 
	cd $(SRC_DIR)/kstem && make clean
	cd $(SRC_DIR)/paice && make clean
	$(RM) $(SRC_DIR)/*.o $(SRC_DIR)/stoplist.c $(COMMON_OBJS)

realclean: clean
	cd $(SRC_DIR)/kstem && make realclean
	$(RM) $(TARGET) $(LIB)
