/* 
Purpose of program: The purpose of this code is:
	-Take a RIPL-3 file of the user's choosing
	-Import the levels and transitions in
	-Search for transitions of interest as defined by:
		-Specific levels based on spin-parity
	-Compare the found transitions of interest to a list of gamma lines from a file
	-Output the lines of interest with relevant information:
	 	-outgoing/incoming levels
	 	-transition energy
	 	-transition strength

Created by: Sabrina Strauss (Date Format: DD-MM-YY)
Log:
22-01-16
Working on the skeleton of the code. Starting without the ability to put in separate inputs (hardcoding the inputs until we get it working.) Goal of the day: successfully read the RIPL-3 file into program. 
	-Successfully done. Reads RIPL-3 file into arrays. Need to implement vectors for dynamic size capabilities.

25-01-16
Changing to a vector system. Going to make it less necessary to read in the exact number of lines.
	-Successfully switched to a vector system.
	-Successfully adjusted the number of lines situation. No longer necessary for strict number of lines.
Need to write search program to pick out gammas leaving or entering transitions of interest.
	-Added spin and parity flags.

28-01-16
Adding in search feature.
	-Added a vector to store the flagged level indices
	-Search is up, not not fully functional

29-01-16
Fixed search function.
	-Prints out in a specific order

02-02-16
Adding in printing to file and the search against a gammas
	-Successfully added print to file
	-Fixed another issue with the search
	-Choose what spin to flag
	-Added prompts when it runs

05-02-16
Should add a few more options in the beginning. Do I still want to add a single gamma mode?
	-Fixed an issue with reading in spins >=  10. May need to be further adjusted for half-integer spins.
	-Adding a prompt for whether to include "non-existent" aka not before observed, transitions.

11-12-17
This won't compile on the CRC due to a bug in GCC. Currently changing the code so as not to use string comparison and conversion functions.
*/

//libraries to include
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

//set namespace
using namespace std;

//main program
int main()
{
	//File names
	char cRIPL[255] = "RIPL-3_154Gd.dat"; //RIPL-3 File
	char cSearch[255] = "SearchGamma.dat"; //Gammas to search for
	char cGammas[255] = "154Gd_Transitions.dat"; //For flagged spin-parities
	char cCompare[255] = "154Gd_Comparison.dat"; //For comparing against gammas

	int nLevels = 0; //number of levels in the nucleus
	int nCurLev = 0; //current level being read in.
	int nGamma = 0; //total number of gammas

	double dSpin = 0; //spin to flag, double variable for non-even nuclei. If negative, don't use flags.
	int nParity = 1; //parity to flag, 1 for +, -1 for -
	double dMaxEnergy = 3000.0; //max energy in the lvel scheme to look to
	double dSearch; //Gamma to search for
	double dError = 0.5; //Error on gamma search, in keV
	bool bExist; //Whether the gamma has been seen
	bool bDNETransitions = false; //Whether to include any difference between levels, or known only. Default is known only

	vector < vector <double> > dLevel; // [level serial #-1][energy, spin, or parity]
	vector <int> nFlagged; //level indexes of the states with the flagged spin-parity combination.
	vector < vector <double> > dGamma; //[gamma index][energy, transition strength, starting state index , ending state index]
	vector < vector <double> > dGammaFlagged; //same as above, but for use with flagged information.
	vector < vector <double> > dCompareVector; //vector for comparisons

	//Time to get some user inputs!
	cout << "RIPL-3 file: ";
	cin >> cRIPL;
	cout << "File of gammas to search for: ";
	cin >> cSearch;
	cout << "Error in keV on gammas: ";
	cin >> dError;
	cout << "Maximum level energy in keV: ";
	cin >> dMaxEnergy;
	cout << endl << "Please enter the spin-parity for flagged states. If you do not want to flag a spin-parity, enter -10 0: ";
	cin >> dSpin >> nParity; 
	cout << endl << "Do you want to include any possible transitions (1) or only known transitions (0)? ";
	cin >> bDNETransitions; 

	/*READING IN THE RIPL-3 FILE HERE*/
	ifstream fLevels(cRIPL,ios::in); //read only file
	ifstream fSearch(cSearch,ios::in); //Gammas to search for file
	ofstream fGammas(cGammas,ios::out); //flagged spin-parity file
	ofstream fCompare(cCompare,ios::out); //Found levels for the gammas to search for
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
					cout << "Number of levels in nucleus: " << nLevels << endl;
					break; //break out of loop
				}
			}
		}
		else if(isblank(sBuffer.at(3))) //getting into the lines of interest
		{
			//cout << "line of interest" << endl;
			//Check if it's a level or gamma line
			if(isdigit(sBuffer.at(2))) //level line
			{
				//read level information into array.
				vector <double> dInfo; //row for this level
				dInfo.push_back(atof(sBuffer.substr(6,8).c_str())*1000.0); //energy in keV
				dInfo.push_back(atof(sBuffer.substr(16,4).c_str())); //spin
				dInfo.push_back(atof(sBuffer.substr(21,2).c_str())); //parity as 1 or -1
				dLevel.push_back(dInfo); // add row to array
				if(dInfo[1]==dSpin && dInfo[2]==nParity)
				{
					nFlagged.push_back(dLevel.size()); //add index of flagged level to array.
				}
				nCurLev++;
//				cout << dInfo[0] << endl;
			}
			else if(isdigit(sBuffer.at(42)))//gamma line
			{
				vector <double> dInfo; //row for this gamma
				dInfo.push_back(atof(sBuffer.substr(49,5).c_str())*1000.0); //energy of transition in keV
				dInfo.push_back(atof(sBuffer.substr(56,5).c_str())*pow(10.0,atof(sBuffer.substr(62,3).c_str()))); //transition strength
				dInfo.push_back(atof(sBuffer.substr(78,5).c_str())*pow(10.0,atof(sBuffer.substr(84,3).c_str()))); //conversion coefficient
				dInfo.push_back(nCurLev); // starting state index
				dInfo.push_back(atoi(sBuffer.substr(40,3).c_str())); // ending state index. 
				dGamma.push_back(dInfo); // add row to array
				//if(dGamma[nGamma][3]==4) cout << dGamma[nGamma][0] << "\t" << dGamma[nGamma][1] << "\t" << dGamma[nGamma][2] << "\t" << dGamma[nGamma][3] << "\t" << dGamma[nGamma][4] <<endl;
				nGamma++;
			}
		}
	}
	fLevels.close();
	cout << "Finished reading in the RIPL-3 file" << endl;

	//LOOP TO FIND AND PRINT THE TRANSITIONS OF INTEREST
	for(int i=0; i < nFlagged.size(); i++)
	{
		if(dLevel[nFlagged[i]-1][0]>dMaxEnergy) break; //don't write after this point, too high in level scheme
		//if ground state, skip
		if(dLevel[nFlagged[i]-1][0]==0) i++;
		//Print out header
		fGammas << "Outgoing transitions for state at " << dLevel[nFlagged[i]-1][0] << " keV" << endl;
		//check start -> Outgoing
		sprintf(cBuffer,"Energy\t\tIntensity\tConv. Coef.\tStart Energy\tStart Jp\t End Energy\tEnd Jp\n");
		fGammas << cBuffer;
		for(int j=0; j < dGamma.size(); j++)
		{
			if(nFlagged[i]==dGamma[j][3] && dLevel[dGamma[j][3]][0] < dMaxEnergy)
			{
				dGammaFlagged.push_back(dGamma[j]);
				sprintf(cBuffer,"%10.6f\t%10.6f\t%10.6f\t%10.6f\t%i  %i\t\t%10.6f\t%i  %i\n",dLevel[dGamma[j][3]-1][0]-dLevel[dGamma[j][4]-1][0],dGamma[j][1],dGamma[j][2],dLevel[dGamma[j][3]-1][0],(int)dLevel[dGamma[j][3]-1][1],(int)dLevel[dGamma[j][3]-1][2],dLevel[dGamma[j][4]-1][0],(int)dLevel[dGamma[j][4]-1][1],(int)dLevel[dGamma[j][4]-1][2]);
				fGammas << cBuffer;
			}
		}
		//Print out header
		fGammas << "Incoming transitions for state at " << dLevel[nFlagged[i]-1][0] << " keV" << endl;
		//check stop -> Incoming
		sprintf(cBuffer,"Energy\t\tIntensity\tConv. Coef.\tStart Energy\tStart Jp\t End Energy\tEnd Jp\n");
		fGammas << cBuffer;
		for(int j=0; j < dGamma.size(); j++)
		{
			if(nFlagged[i]==dGamma[j][4] && dLevel[dGamma[j][3]][0] < dMaxEnergy)
			{
				dGammaFlagged.push_back(dGamma[j]);
				sprintf(cBuffer,"%10.6f\t%10.6f\t%10.6f\t%10.6f\t%i  %i\t\t%10.6f\t%i  %i\n",dLevel[dGamma[j][3]-1][0]-dLevel[dGamma[j][4]-1][0],dGamma[j][1],dGamma[j][2],dLevel[dGamma[j][3]-1][0],(int)dLevel[dGamma[j][3]-1][1],(int)dLevel[dGamma[j][3]-1][2],dLevel[dGamma[j][4]-1][0],(int)dLevel[dGamma[j][4]-1][1],(int)dLevel[dGamma[j][4]-1][2]);
				fGammas << cBuffer;
			}
		}
	}
	cout << "Finished printing flagged gammas, if any were flagged" << endl;

	//Now, we'll search against the gammas
	if(dSpin < -1)
	{
		dCompareVector = dGamma;
	}
	else
	{
		dCompareVector = dGammaFlagged;
	}
	//search through the  gamma file
	while(!fSearch.eof())
	{
		if(fSearch.eof()) break;
		getline(fSearch,sBuffer);
		dSearch = atof(sBuffer.c_str());
		cout << "Searching for possible line matches for " << dSearch << " keV" << endl;
		fCompare << "\nPossible line matches for the gamma energy " << dSearch << " keV" << endl;
		sprintf(cBuffer,"Energy\t\tIntensity\tConv. Coef.\tStart Energy\tStart Jp\t End Energy\tEnd Jp\n");
		fCompare << cBuffer;
		for(int i=0; i < dLevel.size(); i++) //loop over the levels
		{
			if(dLevel[i][0] > dMaxEnergy) break;
			for(int j=i+1; j < dLevel.size(); j++) // loop over the remaining levels above it
			{
				if(dLevel[j][0] > dMaxEnergy) break;
				if( fabs((dLevel[j][0]-dLevel[i][0])-dSearch) <= dError) //Within the error
				{
					//search to see if a corresponding gamma exists
					bExist = false;
					//cout << i << "\t" << j << endl;
					for(int k=0; k < dCompareVector.size() ; k++)
					{
						//cout << dLevel[j][0]-dLevel[i][0] << "\t" << dLevel[dCompareVector[k][3]-1][0]-dLevel[dCompareVector[k][4]-1][0] << endl;
						if((dLevel[j][0]-dLevel[i][0])==dLevel[dCompareVector[k][3]-1][0]-dLevel[dCompareVector[k][4]-1][0])
						{
							sprintf(cBuffer,"%10.6f\t%10.6f\t%10.6f\t%10.6f\t%i  %i\t\t%10.6f\t%i  %i\n",dLevel[j][0]-dLevel[i][0],dCompareVector[k][1],dCompareVector[k][2],dLevel[j][0],(int)dLevel[j][1],(int)dLevel[j][2],dLevel[i][0],(int)dLevel[i][1],(int)dLevel[i][2]);
							fCompare << cBuffer;
							bExist = true;
						}
					}
					if (!bExist && bDNETransitions && (dLevel[j][1]-dLevel[i][1]< 5 && dLevel[j][1]-dLevel[i][1]> -5) && dLevel[j][1] != -1 && dLevel[i][1] != -1) //Only includ levels with known spins and transitions with a change in spin of 4 or less
					{

						sprintf(cBuffer,"%10.6f\t%s\t%s\t%10.6f\t%i  %i\t\t%10.6f\t%i  %i\n",dLevel[j][0]-dLevel[i][0],"Does Not","Exist",dLevel[j][0],(int)dLevel[j][1],(int)dLevel[j][2],dLevel[i][0],(int)dLevel[i][1],(int)dLevel[i][2]);
						fCompare << cBuffer;
					}
				}
			}
		}
	}
	fGammas.close();
	fCompare.close();
	return 0; //end the program
}