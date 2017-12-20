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
#include <readFile.h>

int nLevels; //number of levels in the nucleus
int nCurLev; //current level being read in.
int nGamma; //total number of gammas

extern std::vector < std::vector <double> > dLevel; // [level serial #-1][energy, spin, or parity]
extern std::vector < std::vector <double> > dGamma; //[gamma index][energy, transition strength, starting state index , ending state index]

void readRIPL(char* cRIPL)
{
    nLevels = 0; //number of levels in the nucleus
    nCurLev = 0; //current level being read in.
    nGamma = 0; //total number of gammas

    std::ifstream fLevels(cRIPL,ios::in); //read only file

    string sBuffer; //read line in here
	char cBuffer[255]; //for putting lines into the output files

	while (!fLevels.eof()) //loop through the file
	{
		if(fLevels.eof()) break; //So it doesn't read the last line twice
		getline(fLevels,sBuffer); //read in line
		//check what kind of line it is
		//the first 14 lines are meta data/maintenance, and only line 4 is needed
		if(sBuffer.size() < 40 || !strncmp(sBuffer.substr(0,1).c_str(),"-",1)) continue; // skip blank or short lines and lines with "-" in it
		else if(!strncmp(sBuffer.substr(2,17).c_str(),"number of levels:",17)) //read the number of levels in. Returns 0 when there is a match.
		{
			for (int i=0; i<sBuffer.length(); i++) //go through characters in line
			{
				if(isdigit(sBuffer.at(i))) //is it a number?
				{
					nLevels = atoi(sBuffer.substr(i,3).c_str()); //read number of levels
					std::cout << "Number of levels in nucleus: " << nLevels << std::endl;
					break; //break out of loop
				}
			}
		}
		else if(isblank(sBuffer.at(3))) //getting into the lines of interest
		{
			//Check if it's a level or gamma line
			if(isdigit(sBuffer.at(2))) //level line
			{
				//read level information into array.
				std::vector <double> dInfo; //row for this level
				dInfo.push_back(atof(sBuffer.substr(6,8).c_str())*1000.0); //energy in keV
				dInfo.push_back(atof(sBuffer.substr(16,4).c_str())); //spin
				dInfo.push_back(atof(sBuffer.substr(21,2).c_str())); //parity as 1 or -1
				dLevel.push_back(dInfo); // add row to array
				nCurLev++;
			}
			else if(isdigit(sBuffer.at(42)))//gamma line
			{
				std::vector <double> dInfo; //row for this gamma
				dInfo.push_back(atof(sBuffer.substr(49,5).c_str())*1000.0); //energy of transition in keV
				dInfo.push_back(atof(sBuffer.substr(56,5).c_str())*pow(10.0,atof(sBuffer.substr(62,3).c_str()))); //transition strength
				dInfo.push_back(atof(sBuffer.substr(78,5).c_str())*pow(10.0,atof(sBuffer.substr(84,3).c_str()))); //conversion coefficient
				dInfo.push_back(nCurLev); // starting state index
				dInfo.push_back(atoi(sBuffer.substr(40,3).c_str())); // ending state index. 
				dGamma.push_back(dInfo); // add row to array
				nGamma++;
			}
		}
	}
	fLevels.close();
	std::cout << "Finished reading in the RIPL-3 file" << std::endl;
}