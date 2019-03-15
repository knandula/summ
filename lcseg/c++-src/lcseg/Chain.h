/**
 * Copyright © 2003-2006 by The Trustees of Columbia University in 
 * the City of New York.  All rights reserved.  This software is being
 * provided at no cost for educational and/or research purposes only.
 *
 * Author: Michel Galley, galley@cs.columbia.edu
 * $Id: Chain.h 43 2006-11-14 00:22:30Z galley $
 */

#ifndef __CHAIN_H__
#define __CHAIN_H__

namespace lcseg {

// Structure used to represent term repetitions:
class Chain {

public:
  // Ordering function:
  bool operator<(const Chain& r) const {
     if(_start != r._start) return (_start < r._start);
     if(_end   != r._end)   return (_end   < r._end);
     if(_count != r._count) return (_count > r._count);
     return (_word < r._word);
  }

  // Default constructor: chain of weight c spans b-e
  // (word 's' is being repreated 'c' times)
  Chain(int s, int e, int c, const std::string& str) : 
          _start(s), _end(e), _count(c), _word(str), _score(0) {
             compute_score();
  }

  // Chain scoring function:
  void compute_score() {
     int len = _end-_start+1;
     float div = (float)Chain::_txtlen/(float)len;
     _score = _count * log(div)/log(Chain::_txtlen);
  }

  // Write chain information to a stream:
  void write(std::ostream& out) const {
     if(_start < _end)
        out << _score << "\t[" << _start << ":" << _end << "]\t" 
             << _word << std::endl;
  }

  // Set the total length of the text (in # of sentences).
  static void set_txtlen(int txtlen) { Chain::_txtlen = txtlen; } 

  // Accessors:
  int get_start()        const { return _start; }
  int get_end()          const { return _end; }
  int get_count()        const { return _count; }
  std::string get_word() const { return _word; }
  float get_score()      const { return _score; }

protected:
  int _start;         //!< index of 1st sentence of the chain
  int _end;           //!< index of last sentence of the chain
  int _count;         //!< number of occurrences of _word
  std::string _word;  //!< repeated word
  float _score;       //!< chain score
  static int _txtlen; //!< TODO: remove this
};

}

#endif
