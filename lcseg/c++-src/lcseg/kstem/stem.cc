/**
 * Copyright © 2003-2006 by The Trustees of Columbia University in 
 * the City of New York.  All rights reserved.  This software is being
 * provided at no cost for educational and/or research purposes only.
 *
 * Author: Michel Galley, galley@cs.columbia.edu
 * $Id: stem.cc 43 2006-11-14 00:22:30Z galley $
 */

#include <iostream>
#include <sstream>
#include <string>

extern "C" {
#include <lcseg/kstem/kstem.h>
}

using namespace std;

int main(int argc, char *argv[]) {
  // Init berkeley DB:
  initdb();
  // Read text file:
  std::string line;
  while(getline(cin,line)) {
    stringstream ss(line);  
    string tok;
    while(ss >> tok)
        cout << KStem(const_cast<char*>(tok.c_str())) << " ";
    cout << "\n";
  }
  // Close berkeley DB:
  closedb();
  return 0;  
}
