#######################################################
# Location of the Berkeley DB:
#BERKELEY=/home/galley/i386-linux
BERKELEY=/usr/lib/x86_64-linux-gnu/

#######################################################
# Location of boost library:
#BOOST=/proj/nlp/tools/languages/boost_1_33_1/include/boost-1_33_1
BOOST=/home/fj/Downloads/boost_1_69_0/stage/lib
#######################################################
# Choice of stemmer:
# (important: if you switch from one to the
#  other, don't forget to 'make clean' before
#  rebuilding the distribution)
STEMARG = -D KSTEM
#STEMARG = -D PAICE
