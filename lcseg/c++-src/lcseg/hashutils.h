/**
 * Copyright © 2003-2006 by The Trustees of Columbia University in 
 * the City of New York.  All rights reserved.  This software is being
 * provided at no cost for educational and/or research purposes only.
 *
 * Author: Michel Galley, galley@cs.columbia.edu
 * $Id: hashutils.h 43 2006-11-14 00:22:30Z galley $
 */

#ifndef __HASHUTILS_H__
#define __HASHUTILS_H__

#include <string>
#include <cstring>
#if (__GNUC__ >= 3)
#include <ext/hash_map>
#include <ext/hash_set>
#if (__GNUC__ >= 4 || __GNUC_MINOR__ >= 1)
#define __GNU_CXX_NAMESPACE
#else
#define STD_NAMESPACE
#endif
#else
#include <hash_map>
#include <hash_set>
#define STD_NAMESPACE
#endif

#ifdef STD_NAMESPACE
using std::hash;
using std::hash_map;
using std::hash_set;
using std::hash_multimap;
using std::hash_multiset;
#endif
#ifdef __GNU_CXX_NAMESPACE
using __gnu_cxx::hash;
using __gnu_cxx::hash_map;
using __gnu_cxx::hash_set;
using __gnu_cxx::hash_multimap;
using __gnu_cxx::hash_multiset;
#endif

// Hashing "string"'s using the "char*" function defined in the 
// SGI implementation of the STL:
struct hash_str {
  size_t operator()(const std::string& s) const {
     return hash<char const *>()(s.c_str());
  }
};

// Comparison operator for c-strings (if none is provided, 
// hashed containers by default perform pointer comparisons, 
// which is probably not the behavior you want).
struct cmp_cstr {
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) == 0;
  }
};

// Comparison operator for strings:
struct cmp_str {
  bool operator()(const std::string& s1, const std::string& s2) const { return s1 == s2; }
};

typedef hash_set<std::string,hash_str,cmp_str> Lexicon;
//typedef hash_set<const char*,hash<const char*>,cmp_cstr> Lexicon;
//typedef hash_map<const char*,int,hash<const char*>,cmp_cstr> Lexicon;

#endif
