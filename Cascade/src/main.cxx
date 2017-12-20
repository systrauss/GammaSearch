/*
Please check the README for more information about this code and it's purpose.
*/

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

//User files
#include <Search.h>
#include <readFile.h>

extern int nLevels; //number of levels in the nucleus
extern int nCurLev; //current level being read in.
extern int nGamma; //total number of gammas

std::vector < std::vector <double> > dLevel; // [level serial #-1][energy, spin, or parity]
std::vector < std::vector <double> > dGamma; //[gamma index][energy, transition strength, starting state index , ending state index]
std::vector < std::vector <double> > dTrans; //[Transition index][energy, transition strength, starting state index , ending state index]

double dMaxEnergy; //max energy in the level scheme to look to
double dError; //Error on gamma search, in keV

int main(int argc, char* argv[]) //Order of arguments: RIPl-3, Search File, Max Energy, Error Bar
{
	//File names
	char cBuffer[255];
	std::cout << "IF YOU DO NOT ENTER A RIPL-3 FILE, IT MAY CRASH THE CODE" << std::endl;
	char* cRIPL3 = argv[1]; //RIPL-3 File
	if(strcmp(cRIPL3,"") == 0 || strcmp(cRIPL3,"0") == 0)
	{
		while(strcmp(cRIPL3,"") == 0 || strcmp(cRIPL3,"0") == 0)
		{
			std::cout << "Please enter a valid RIPL-3 file name: ";
			std::cin >> cRIPL3;
		}
	}
	char* cSearch = argv[2]; //Gammas to search for
	double dSearch = 0;
	if(strcmp(cSearch,"") == 0 || strcmp(cSearch,"0") == 0)
	{
		while(dSearch <= 0)
		{
			std::cout << "You have chosen not to provide a list of gammas to search for. Please enter a single gamma-ray in keV: ";
			std::cin >> cSearch;
			dSearch = atof(cSearch); //If a non-number is given, atof will return 0.
		}
	}
	dMaxEnergy =atof(argv[3]); 
	while(dMaxEnergy <= 0)
	{
		std::cout << "Please give a valid maximum energy within the level scheme (in keV): ";
		std::cin >> cBuffer;
		dMaxEnergy = atof(cBuffer); //If a non-number is given, atof will return 0.
	}
	dError =atof(argv[4]); 
	while(dError <= 0)
	{
		std::cout << "Please give a valid error bar search parameter (in keV): ";
		std::cin >> cBuffer;
		dError = atof(cBuffer); //If a non-number is given, atof will return 0.
	}


	//All the heavy lifting
	std::ofstream fTransition("output/Transitions.dat",ios::out); //Transitions file
	if(dSearch==0) //actually read a file in, as opposed to a single gamma
	{
		while(!fSearch.eof())
		{
			if(fSearch.eof()) break;
			getline(fSearch,sBuffer);
			dSearch = atof(sBuffer.c_str());
			findTrans(dSearch, fTransition);
		}
	}
	else
	{
		findTrans(dSearch, fTransition);
	}
	fTransition.close();

	//Now, let's find any gammas going into the transitions of interest.

	findIncoming();
	findCascade();

	return 0; //end the program
}