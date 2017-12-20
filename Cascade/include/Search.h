#ifndef SEARCH_H
#define SEARCH_H


//C++ libraries
#include <math.h> //math header for pow
#include <cstring> //string class
#include <cstdlib> //for atoi -> convert string to integer
#include <stdlib.h> //for
#include <cstdio> //for fprintf, to print formatted string to file
#include <cctype> //check on character types
#include <vector>
#include <iostream> // terminal i/o
#include <fstream> // file i/o
#include <sstream> // string i/o

#ifndef _SEARCH_CXX
#define EXTERNAL extern
#else
#define EXTERNAL
#endif

void findTrans(double dSearch, std::ofstream fTransition);
void findIncoming();
void CascadeRecursion(std::vector<std::vector<double> > dCascade, std::ofstream fCascade, int nCounter);
void findCascade();

#endif