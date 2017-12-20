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


extern std::vector < std::vector <double> > dLevel; // [level serial #-1][energy, spin, or parity]
extern std::vector < std::vector <double> > dGamma; //[gamma index][energy, transition strength, starting state index , ending state index]
extern std::vector < std::vector <double> > dTrans; //[Transition index][energy, transition strength, starting state index , ending state index]

extern double dMaxEnergy; 
extern double dError; //Error on gamma search, in keV

void CascadeRecursion(std::vector<std::vector<double> > dCascade, std::ofstream fCascade, int nCounter);

void findTrans(double dSearch, std::ofstream fTransition)
{
    string sBuffer; //read line in here
	char cBuffer[255]; //for putting lines into the output files

	std::cout << "Searching for possible line matches for " << dSearch << " keV" << std::endl;
	fTransition << "\nPossible line matches for the gamma energy " << dSearch << " keV" << std::endl;
	sprintf(cBuffer,"Energy\t\tIntensity\tConv. Coef.\tStart Energy\tStart Jp\t End Energy\tEnd Jp\n");
	fTransition << cBuffer;
	for(int i=0; i < dLevel.size(); i++) //loop over the levels
	{
		if(dLevel[i][0] > dMaxEnergy) break;
		for(int j=i+1; j < dLevel.size(); j++) // loop over the remaining levels above it
		{
			if(dLevel[j][0] > dMaxEnergy) break;
			if( fabs((dLevel[j][0]-dLevel[i][0])-dSearch) <= dError) //Within the error
			{
				//search to see if a corresponding gamma exists
				for(int k=0; k < dGamma.size() ; k++)
				{
					if((dLevel[j][0]-dLevel[i][0])==dLevel[dGamma[k][3]-1][0]-dLevel[dGamma[k][4]-1][0])
					{
						sprintf(cBuffer,"%10.6f\t%10.6f\t%10.6f\t%10.6f\t%i  %i\t\t%10.6f\t%i  %i\n",dLevel[j][0]-dLevel[i][0],dGamma[k][1]dGamma[k][2],dLevel[j][0],(int)dLevel[j][1],(int)dLevel[j][2],dLevel[i][0],(int)dLevel[i][1],(int)dLevel[i][2]);
						fTransition << cBuffer; //write to formatted file
						
				        dTrans.push_back(dGamma[k]); // add row to transition array
					}
				}
			}
		}
	}
}

void findIncoming()
{
    std::ofstream fIncoming("output/Incoming.dat",std::ios::out); //Incoming gammas file
    string sBuffer; //read line in here
	char cBuffer[255]; //for putting lines into the output files

    for(int i=0; i<dTrans.size(); i++)
    {
        fIncoming << "Incoming transitions for gamma of energy " << dTrans[i][0] << " keV" << std::endl;
	    sprintf(cBuffer,"Energy\t\tIntensity\tConv. Coef.\tStart Energy\tStart Jp\t End Energy\tEnd Jp\n");
	    fIncoming << cBuffer;
	    for(int j=0; j < dGamma.size(); j++)
	    {
	    	if(dTrans[i][3]==dGamma[j][4] && dLevel[dGamma[j][4]][0] < dMaxEnergy)
	    	{
	    		sprintf(cBuffer,"%10.6f\t%10.6f\t%10.6f\t%10.6f\t%i  %i\t\t%10.6f\t%i  %i\n",dLevel[dGamma[j][3]-1][0]-dLevel[dGamma[j][4]-1][0],dGamma[j][1],dGamma[j][2],dLevel[dGamma[j][3]-1][0],(int)dLevel[dGamma[j][3]-1][1],(int)dLevel[dGamma[j][3]-1][2],dLevel[dGamma[j][4]-1][0],(int)dLevel[dGamma[j][4]-1][1],(int)dLevel[dGamma[j][4]-1][2]);
	    		fIncoming << cBuffer;
	    	}
	    }
    }
    fIncoming.close();
}

void CascadeRecursion(std::vector<std::vector<double> > dCascade, std::ofstream fCascade, int nCounter)
{
    //nCounter keeps track of the length of this cascade, including the original transition
    if(dCascade[nCounter-1][4]==1) //End state energy
    {
        for(int i=1; i<nCounter;i++)
        {
            sprintf(cBuffer,"%10.6f\t%10.6f\t%10.6f\t%10.6f\t%i  %i\t\t%10.6f\t%i  %i\n",dLevel[dCascade[i][3]-1][0]-dLevel[dCascade[i][4]-1][0],dCascade[i][1],dCascade[i][2],dLevel[dCascade[i][3]-1][0],(int)dLevel[dCascade[i][3]-1][1],(int)dLevel[dCascade[i][3]-1][2],dLevel[dCascade[i][4]-1][0],(int)dLevel[dCascade[i][4]-1][1],(int)dLevel[dCascade[i][4]-1][2]);
	    	fCascade << cBuffer;
        }
    }
    else 
    {
        if(dCascade.size()>=nCounter) //cascade is longer than it should be at this level.
        {
            for(int i=0; i<dGamma.size();i++)
            {
                if(dCascade[nCounter-1][4]==dGamma[i][3]) //looking at this level, not the farthest down
                {
                    dCascade[nCounter][0]=dGamma[i][0];
                    dCascade[nCounter][1]=dGamma[i][1];
                    dCascade[nCounter][2]=dGamma[i][2];
                    dCascade[nCounter][3]=dGamma[i][3];
                    dCascade[nCounter][4]=dGamma[i][4];
                    CascadeRecursion(dCascade,fCascade,nCounter+1);
                }
            }
        }
        else
        {
            for(int i=0; i<dGamma.size();i++)
            {
                if(dCascade[nCounter-1][4]==dGamma[i][3]) //looking at this level, not the farthest down
                {
                    dCascade.push_back(dGamma[i]);
                    CascadeRecursion(dCascade,fCascade,nCounter+1);
                }
            }
        }
    }
}

void findCascade()
{
    std::ofstream fCascade("output/Cascade.dat",std::ios::out); //Cascade gammas file
	char cBuffer[255]; //for putting lines into the output files

    for(int i=0; i<dTrans.size(); i++)
    {
        fCascade << "Cascades for gamma of energy " << dTrans[i][0] << " keV" << std::endl;
	    sprintf(cBuffer,"Energy\t\tIntensity\tConv. Coef.\tStart Energy\tStart Jp\t End Energy\tEnd Jp\n");
	    fCascade << cBuffer;
	    for(int i=0; i < dTrans.size(); i++)
	    {
            std::vector<std::vector<double> > dCascade;
            dCascade.push_back(dTrans[i]);
            CascadeRecursion(dCascade,fCascade,1);
	    }
    }
    fCascade.close();
}