/**
 * Copyright © 2003-2006 by The Trustees of Columbia University in 
 * the City of New York.  All rights reserved.  This software is being
 * provided at no cost for educational and/or research purposes only.
 *
 * Author: Michel Galley, galley@cs.columbia.edu
 * $Id: BagOfWords.h 43 2006-11-14 00:22:30Z galley $
 */

#ifndef __BAGOFWORDS_H__
#define __BAGOFWORDS_H__

#include <string>
#include <fstream>
#include <iosfwd>
#include <boost/tokenizer.hpp>

#include <lcseg/FeatureVector.h>
#include <lcseg/TextToolkit.h>
#include <lcseg/logging.h>

extern "C" {
#ifdef KSTEM
#include <lcseg/kstem/kstem.h>
#elif PAICE
#include <lcseg/paice/paice.h>
#endif
}

namespace lcseg {

template<typename T> class BagOfWords;

typedef FeatureVector<int>   int_FV;
typedef FeatureVector<long>  long_FV;
typedef FeatureVector<float> float_FV;
typedef BagOfWords<int>      int_BW;
typedef BagOfWords<long>     long_BW;
typedef BagOfWords<float>    float_BW;

template<typename T> class 
BagOfWords : public FeatureVector<T> {

public:
     //! Base constructor that creates an empty bag of words.
     BagOfWords() : FeatureVector<T>(),_nbOfWords(0) { }
     //! Copy constructor performing a deep copy of its argument.
     BagOfWords(const BagOfWords&);
     //! Copy constructor performing a deep copy of its argument.
     BagOfWords(const FeatureVector<T>&);
     //! Fill object with tokens read from a stream.
     void read_tokens(std::ifstream&); 
     //! Fill object with tokens read from a string.
     void read_tokens(const std::string&); 
     //! Dump all tokens of current object to a stream.
     void write(std::ostream&) const;
     //! Perform a deep copy of the argument. 
     BagOfWords& operator=(const BagOfWords&);
     //! Add tokens contained in argument to current bag of word.
     BagOfWords& operator+=(const BagOfWords&);
     //! Subtract tokens contained in argument to current bag of word.
     BagOfWords& operator-=(const BagOfWords&);
     //! Multiply number of occurrences of each token by a given factor.
     BagOfWords& operator*=(T);
     //! Class for handling I/O exceptions.
     class IOException { };

protected:
     // The "tokenizer" type specifies the kind of separator 
     // used for tokenization. It is rather simplistic, it strips 
     // out all punctuation, then separates tokens and provides
     // an iterator to process them one by one:
     typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
     // Nb. of words in the bag of words:
     T _nbOfWords;
};

/////////////////////////////////////////////////////
// Helper functions:

// Standard way of adding text to a bag of words:
template<typename T>
std::istream& operator>>(std::istream&, BagOfWords<T>&);

// Output counts of a bag of words into a file stream:
template<typename T>
std::ostream& operator<<(std::ostream&, const BagOfWords<T>&);

template<typename T>
BagOfWords<T>& operator+
  (const BagOfWords<T>&, const BagOfWords<T>&);

template<typename T>
BagOfWords<T>& operator-
  (const BagOfWords<T>&, const BagOfWords<T>&);

template<typename T>
BagOfWords<T>& operator*
  (T val, const BagOfWords<T>&);

/////////////////////////////////////////////////////
// Member function definitions:

template<typename T>
inline void BagOfWords<T>::read_tokens(std::ifstream& in) {

  std::string line;
 
  // Fill the feature vector with tokens:
  while(std::getline(in,line,'\n'))
     read_tokens(line);
}

template<typename T>
void BagOfWords<T>::read_tokens(const std::string& str) {
  tokenizer tok(str);
  int n = 0;
  char buf[100];
  char *stem;
  typename FeatureVector<T>::iterator it; 
  for (tokenizer::iterator cur=tok.begin(); cur!=tok.end(); ++cur, ++n) {
     strcpy(buf,cur->c_str());
     if(TextToolkit::is_commonword(buf))
        continue;
     char *scan = buf;
     bool alphanum = false;
     while(*scan != '\0') {
        if(isalnum(*scan)) {
          alphanum = true;
          break;
        }
        scan++;
     }
     if(!alphanum)
        continue;
#ifdef KSTEM
     stem = KStem(buf);
#elif  PAICE
     stem = PaiceStem(buf);
#else
     stem = buf;
#endif
     const std::string str(stem);
     it = this->_av.find(str);
     if (it != this->_av.end())
        it->second++;
     else 
        this->_av.insert(make_pair(str,1));
  }
  _nbOfWords += n;
}    

template<typename T>
void BagOfWords<T>::write(std::ostream& out) const {
  out << "Contents of bow (" << reinterpret_cast<int>(this) << "):" 
        << std::endl; 
  FeatureVector<T>::write(out);
  out << "Number of words: " << _nbOfWords << std::endl;
}

template<typename T>
std::istream& operator>>(std::ifstream& in, BagOfWords<T>& bow) {
  in.read(bow);
  return in;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const BagOfWords<T>& bow) {
  out.write(bow);
  return out;
}

template<typename T>
BagOfWords<T>::BagOfWords(const BagOfWords& bow) : 
  FeatureVector<T>(bow) {
  //DLOG("Calling BagOfWords<T>::BagOfWords(BW)");
  _nbOfWords = bow._nbOfWords;
}

template<typename T>
BagOfWords<T>::BagOfWords(const FeatureVector<T>& bow) :
  FeatureVector<T>(bow) {
  //DLOG("Calling BagOfWords<T>::BagOfWords(FV)");
}
 
template<typename T>
BagOfWords<T>& BagOfWords<T>::operator=(const BagOfWords<T>& bow) {
  //DLOG("Calling BagOfWords<T>::operator=(BW)");
  FeatureVector<T>::operator=(bow);
  _nbOfWords = bow._nbOfWords;
}

template<typename T>
inline BagOfWords<T>& BagOfWords<T>::operator+=(const BagOfWords& bow) {
  //DLOG("Calling BagOfWords<T>::operator+=");
  FeatureVector<T>::operator+=(bow);
  _nbOfWords += bow._nbOfWords;
  return *this;
}

template<typename T>
inline BagOfWords<T>& BagOfWords<T>::operator-=(const BagOfWords& bow) {
  //DLOG("Calling BagOfWords<T>::operator-=");
  FeatureVector<T>::operator-=(bow);
  _nbOfWords -= bow._nbOfWords;
  return *this;
}

template<typename T>
inline BagOfWords<T>& BagOfWords<T>::operator*=(T val) {
  //DLOG("Calling BagOfWords<T>::operator*=");
  FeatureVector<T>::operator*=(val);
  _nbOfWords *= val;
  return *this;
}

template<typename T>
inline BagOfWords<T>& operator+(const BagOfWords<T>& bow1, 
                                const BagOfWords<T>& bow2) {
  //DLOG("Calling BagOfWords<T>::operator+");
  BagOfWords<T> *newBow = new BagOfWords<T>(bow1);
  *newBow += bow2;
  return *newBow;
}

template<typename T>
inline BagOfWords<T>& operator-(const BagOfWords<T>& bow1, 
                                const BagOfWords<T>& bow2) {
  //DLOG("Calling BagOfWords<T>::operator-");
  BagOfWords<T> *newBow = new BagOfWords<T>(bow1);
  *newBow -= bow2;
  return *newBow;
}

template<typename T>
inline BagOfWords<T>& operator*(T val, const BagOfWords<T>& bow) {
  //DLOG("Calling BagOfWords<T>::operator*");
  BagOfWords<T> *newBow = new BagOfWords<T>(bow);
  *newBow *= val;
  return *newBow;
}

template<typename T>
inline BagOfWords<T>& operator*(const BagOfWords<T>& bow, T val) {
  //DLOG("Calling BagOfWords<T>::operator*(reversed)");
  return val*bow;
}

}

#endif
