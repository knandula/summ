/**
 * Copyright © 2003-2006 by The Trustees of Columbia University in 
 * the City of New York.  All rights reserved.  This software is being
 * provided at no cost for educational and/or research purposes only.
 *
 * Author: Michel Galley, galley@cs.columbia.edu
 * $Id: FeatureVector.h 43 2006-11-14 00:22:30Z galley $
 */

#ifndef __FEATUREVECTOR_H__
#define __FEATUREVECTOR_H__

#include <string>

#include <lcseg/hashutils.h>
#include <lcseg/logging.h>

namespace lcseg {

/////////////////////////////////////////////////////
// FeatureVector class definition:

template <typename T>
class FeatureVector {
 
public:
  // Type definition of a hashtable whose keys are strings ("string" class)
  // and values are of type T (specified by the template definition)
  typedef hash_map<const std::string,T,hash_str> AttrVals;
  // Iterator type definitions (both constant and non-constant):
  typedef typename AttrVals::iterator iterator; 
  typedef typename AttrVals::const_iterator const_iterator;
  // Enumeration of the different types of similarity measures:
  enum SimType { sim_cos = 0, sim_jaccard };

public:
  //! Return constant reference to first feature.
  const_iterator begin() const { return _av.begin(); }
  //! Return constant reference to last feature past one (past-of-end).
  const_iterator end() const { return _av.end(); }
  //! Return reference to first feature.
  iterator begin() { return _av.begin(); }
  //! Return reference to last feature past one (past-of-end).
  iterator end() { return _av.end(); }
  //! Return iterator pointing to feature whose name is passed as parameter.
  iterator find(const std::string& e) { return _av.find(e); }
  //! Insert feature name and value into object.
  iterator insert(std::pair<const std::string,T>& p) { return _av.insert(p); }

  //! Default constructor: initializes an empty FeatureVector.
  FeatureVector() : _av() {}
  //! Copy constructor: creates a "deep" copy of a feature vector.
  FeatureVector(const FeatureVector&);
  //! Assignment operator that performs a deep copy of the argument.
  FeatureVector& operator=(const FeatureVector&);
  //! Compute and return the L2-norm of the vector.
  float l2_norm() const;
  //! Normalize feature vector, so that the sum of all its elements is 1.
  void normalize();
  //! Addition of two vectors through the += operator.
  //! (beware, this function isn't virtual)
  FeatureVector& operator+=(const FeatureVector&);
  //! Subtraction of a vector from another through the -= operator:
  //! (beware, this function isn't virtual)
  FeatureVector& operator-=(const FeatureVector&);
  //! Multiply a vector by a constant.
  //! (beware, this function isn't virtual)
  FeatureVector& operator*=(T);
  //! Compute and return the dot product between two vectors.
  T operator*(const FeatureVector&) const;
  //! Compute some similarity measure between two feature vectors
  //! (e.g. cosine or the extended Jaccard similarity).
  static float similarity 
     (const FeatureVector&, const FeatureVector&, SimType);
  //! Dump the feature vector to a stream.
  void write(std::ostream&) const;
  //! Insert a feature name and value pair.
  void insert(const std::pair<std::string,T>& p) { _av.insert(p); }
  //! Element-wise maximum of v1 and v2.
  static FeatureVector max(const FeatureVector& v1, const FeatureVector& v2);

protected:
  // Unique member variable, a hash table as defined above:
  AttrVals _av;
};

/////////////////////////////////////////////////////
// Helper functions:

template<typename T>
FeatureVector<T>& operator+
  (const FeatureVector<T>&, const FeatureVector<T>&);
template<typename T>
FeatureVector<T>& operator-
  (const FeatureVector<T>&, const FeatureVector<T>&);
template<typename T>
FeatureVector<T>& operator*(T, const FeatureVector<T>&);
template<typename T>
FeatureVector<T>& operator*(const FeatureVector<T>&, float);
template<typename T>
FeatureVector<T> normalized(const FeatureVector<T>&);

///////////////////////////////////////////////////////
// Member function definitions:

template<typename T>
FeatureVector<T>::FeatureVector(const FeatureVector& v) {
  //DLOG("Calling FeatureVector<T>::FeatureVector(FV)");
  _av = v._av;
}

template<typename T>
FeatureVector<T>& FeatureVector<T>::operator=(const FeatureVector& v) {
  //DLOG("Calling FeatureVector<T>::operator=(FV)");
  _av = v._av;
  return *this;
}

template<typename T> 
float FeatureVector<T>::l2_norm() const {
  const_iterator it,it_end;
  float l2norm = 0, val;
  for(it = _av.begin(), it_end = _av.end(); it != it_end; ++it) {
     val = it->second;
     l2norm += val * val;
  }
  return sqrt(l2norm);
}

template<typename T>
void FeatureVector<T>::normalize() {
  T sum = 0;
  for(const_iterator it=_av.begin(), it_end=_av.end(); it!=it_end; ++it) {
     sum = it->second;
  }
  if(sum)
     for(iterator it=_av.begin(), it_end=_av.end(); it!=it_end; ++it) {
        it->second /= sum;
     }
}

// inefficient: use normalize() instead:
template<typename T>
FeatureVector<T> normalized(const FeatureVector<T>& v) {
  FeatureVector<T> newV(v);
  newV.normalize();
  return newV;
}

template<typename T>
void FeatureVector<T>::write(std::ostream& out) const {
  for(const_iterator it = _av.begin(), it_end = _av.end(); it != it_end; ++it)
     out << "(k: " << it->first << " v: " << it->second << ") ";
  out << std::endl;
}

template<typename T> 
T FeatureVector<T>::operator*(const FeatureVector& v) const {

  // Swap the two vectors, so that v_sh is the shortest one:
  // (and v_lg the longest)
  const FeatureVector *v_sh, *v_lg;
  if(_av.size() > this->_av.size()) {
     v_sh = this; v_lg = &v;
  } else {
     v_sh = &v; v_lg = this;
  }

  T dot = 0;
  const_iterator it1,it2,
     v_sh_end = v_sh->end(),
     v_lg_end = v_lg->end();
  for(it1 = v_sh->begin(); it1 != v_sh_end; ++it1) {
     it2 = v_lg->_av.find(it1->first);
     if(it2 != v_lg_end)
        dot += it1->second * it2->second;
  }
  return dot;
}

template<typename T> 
float FeatureVector<T>::similarity
  (const FeatureVector<T>& v1, const FeatureVector<T>& v2, SimType sim)  {

  // Swap the two vectors, so that v_sh is the shortest one:
  // (and v_lg the longest)
  const FeatureVector<T> *v_sh, *v_lg;
  if(v1._av.size() > v2._av.size()) {
     v_sh = &v2; v_lg = &v1;
  } else {
     v_sh = &v1; v_lg = &v2;
  }

  // Process the first vector: compute its l2-norm, and the dot product
  // of the two vectors:
  const_iterator it1, it2,
     v_sh_end = v_sh->end(),
     v_lg_end = v_lg->end();
  T val1, val2, dot = 0, l2norm1 = 0, l2norm2 = 0;
  for(it1 = v_sh->_av.begin(); it1 != v_sh_end; ++it1) {
     val1 = it1->second;
     l2norm1 += val1*val1;
     it2 = v_lg->_av.find(it1->first);
     if(it2 != v_lg_end) {
        val2 = it2->second;
        dot += val1*val2;
     }
  }

  // Process the second vector: compute its l2-norm:
  for(it2 = v_lg->_av.begin(); it2 != v_lg_end; ++it2) {
     val2 = it2->second;
     l2norm2 += val2*val2;
  }

  // Compute similarity:
  float den;
  switch(sim) {
  case sim_cos:
     den = sqrt(l2norm1)*sqrt(l2norm2);
     if(den <= 0.0)
        return 0.0;
     DLOG("dot:" << dot << " den: " << den);
     return (float)(dot/den); // cast to "float" is needed,
                                      // since T can be "double"
  case sim_jaccard:
     den = l2norm1+l2norm2-dot;
     if(den <= 0.0)
        return 0.0;
     return (float)(dot/den); // cast to "float" is needed,
                                      // since T can be "double"
  default:
     throw;
  }
}

// Addition of two vectors (with the += operator)
template<typename T>
inline FeatureVector<T>& FeatureVector<T>::operator+=
                                                         (const FeatureVector& v) { 
  T val; 
  iterator it2; //this_end = _av.end();
  for(const_iterator it1 = v._av.begin(), v_end = v._av.end();
        it1 != v_end; ++it1) {
     val = it1->second;
     it2 = _av.find(it1->first);
     if(it2 != _av.end()) {
        it2->second += val;
     } else
        _av.insert(make_pair(it1->first,val));
  }
  return *this;
}

// Subtraction of a vector by another (with the -= operator)
template<typename T> 
inline FeatureVector<T>& FeatureVector<T>::operator-=
                                                         (const FeatureVector<T>& v) { 
  T val, newval;
  iterator it2;
  for(const_iterator it1 = v._av.begin(), v_end = v._av.end();
        it1 != v_end; ++it1) {
     val = it1->second;
     it2 = _av.find(it1->first);
     if(it2 != _av.end()) {
        newval = it2->second - val;
        if(newval != 0.0)
          it2->second = newval;
        else
          _av.erase(it1->first);
     } else
        _av.insert(make_pair(it1->first,val));
  }
  return *this;
}

template<typename T>
inline FeatureVector<T>& FeatureVector<T>::operator*=(T val)
{ 
  for(iterator it = _av.begin(), this_end = _av.end();
        it != this_end; ++it)
     it->second *= val;
  return *this;
}

template<typename T>
FeatureVector<T> FeatureVector<T>::max
  (const FeatureVector<T>& v1, const FeatureVector<T>& v2) {
  FeatureVector<T> v(v1);
  iterator it1;
  T val;
  for(const_iterator it2 = v2._av.begin(), it2_end = v2._av.end();
        it2 != it2_end; ++it2) {
     it1 = v._av.find(it2->first);
     val = it2->second;
     if(it1 != v._av.end()) {
        if(it1->second < val)
          it1->second = val;
     } else
        v._av.insert(make_pair(it2->first,val));
  }
  return v;
}

template<typename T>
inline FeatureVector<T>& operator+
  (const FeatureVector<T>& v1, const FeatureVector<T>& v2) {
  FeatureVector<T> newV(v1);
  newV += v2;
  return newV;  
}

template<typename T>
inline FeatureVector<T>& operator-
  (const FeatureVector<T>& v1, const FeatureVector<T>& v2) {
  FeatureVector<T> newV(v1);
  newV -= v2;
  return newV;  
}

template<typename T>
inline FeatureVector<T>& operator*
  (T c, const FeatureVector<T>& v) {
  FeatureVector<T> newV(v);
  newV *= c;
  return newV;
}

template<typename T>
inline FeatureVector<T>& operator*
  (const FeatureVector<T>& v, T c) {
  return c*v;
}

}

#endif
