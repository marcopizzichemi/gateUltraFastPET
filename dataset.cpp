// compile with
// g++ -o dataset dataset.cpp `root-config --cflags --libs`

// This program takes and output of gateToString, a root file
// then produces a list of points in variuos text files
// x1 y1 z1 x2 y2 z2
// with different strategies:
// 1: only events where energy deposition was in 2 and only 2 crystals
// 2: events where energy dep was in 3 and only 3 crystals. this is split in more subcases
//    2a. the scatter events are averaged in one average event
//    2b. the first hit of the scatter is saved (100% efficient compton reconstruction)
//    2c. the first hit of the scatter is recognized with a given efficiency (parameter compton_eff below)
// data is saved in 4 different files


#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>    // std::sort
#include "TROOT.h"
#include "TSystem.h"
#include "TChain.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TH1.h"
#include "TAxis.h"
#include "TDirectory.h"
#include "TList.h"
#include "Rtypes.h"
#include "TChainElement.h"
#include "TTree.h"
#include "TFile.h"
#include "TStyle.h"
#include "TH2.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TRandom.h"

struct point
{
  Int_t eventID;
  Int_t parentID;
  Int_t trackID;
  float x;
  float y;
  float z;
  float energy;
  float time;
};

int main(int argc, char** argv)
{

  //----------------------------------------------------------------------//
  //                                                                      //
  //                        ROOT STUFF                                    //
  //                                                                      //
  //----------------------------------------------------------------------//
  gROOT->ProcessLine("#include <vector>"); //needed by ROOT to deal with standard vectors
  //HACK to use the dictionary easily
  // Code taken from: http://www.gamedev.net/community/forums/topic.asp?topic_id=459511
  std::string fullFileName = "";
  std::string path = "";
  pid_t pid = getpid();
  char buf[20] = {0};
  sprintf(buf,"%d",pid);
  std::string _link = "/proc/";
  _link.append( buf );
  _link.append( "/exe");
  char proc[512];
  int ch = readlink(_link.c_str(),proc,512);
  if (ch != -1) {
    proc[ch] = 0;
    path = proc;
    std::string::size_type t = path.find_last_of("/");
    path = path.substr(0,t);
  }
  fullFileName = path + std::string("/");
  std::string command = ".L " + fullFileName + "structDictionary.C+";
  gROOT->ProcessLine(command.c_str());

  //INPUT FILE AND TTREE
  std::string inputfilename ;
  inputfilename = argv[1] ;
  std::cout << "Input file name is " << inputfilename << std::endl;
  TFile *inputFile = TFile::Open(inputfilename.c_str());
  TTree *tree = (TTree*) inputFile->Get("test");
  std::vector<point> *points = 0;
  tree->SetBranchAddress("points",&points);

  std::string::size_type t_point = inputfilename.find_last_of(".");
  // path = path.substr(0,t);

  std::string baseName = inputfilename.substr(0,t_point);
  //OUTPUT STREAMS
  std::string ofs2cryName                = "./out/" + baseName + "2cry.root";
  std::string ofs3cry_avgName            = "./out/" + baseName + "3cry-avg.root";
  std::string ofs3cry_magicalComptonName = "./out/" + baseName + "3cry-magicalCompton.root";
  std::string ofs3cry_effComptonName     = "./out/" + baseName + "3cry-effCompton.root";

  std::ofstream ofs2cry;
  ofs2cry.open (ofs2cryName.c_str(), std::ofstream::out);
  std::ofstream ofs3cry_avg;
  ofs3cry_avg.open (ofs3cry_avgName.c_str(), std::ofstream::out);
  std::ofstream ofs3cry_magicalCompton;
  ofs3cry_magicalCompton.open (ofs3cry_magicalComptonName.c_str(), std::ofstream::out);
  std::ofstream ofs3cry_effCompton;
  ofs3cry_effCompton.open (ofs3cry_effComptonName.c_str(), std::ofstream::out);

  //COUNTERS
  Int_t nsamples = tree->GetEntries();
  std::cout << nsamples << std::endl;
  Int_t onlyOneCrystals = 0;
  Int_t onlyTwoCrystals = 0;
  Int_t onlyThreeCrystals = 0;
  Int_t onlyFourCrystals = 0;
  Int_t moreThanFourCrystals = 0;
  Int_t onlyTwoCrystalsInEnergyWindow = 0;
  Int_t onlyThreeCrystalsInEnergyWindow = 0;
  Int_t onlyFourCrystalsInEnergyWindow = 0;
  Int_t moreThanFourCrystalsInEnergyWindow = 0;

  //COMPTON efficiency parameter
  Float_t compton_eff = 0.65; // fraction efficiency of compton assignment. e.g. 0.7 means 70%

  for(int i = 0 ; i < nsamples ; i ++)
  {
    tree->GetEntry(i);

    //--------------------------//
    // 1 crystal hit            //
    //--------------------------//
    if(points->size() < 2)  //single crystal hit, count and discard
    {
      onlyOneCrystals++;
    }
    //--------------------------//


    //selection of dataset. First, only when 2 and only 2 crystals are hit, and with the energy in the correct window -> more or less standard clearpem strategy
    //--------------------------//
    // 2 crystals hit           //
    //--------------------------//
    if(points->size() == 2)
    {
      if(points->at(0).energy > 0.450 && points->at(0).energy < 0.650 && points->at(1).energy > 0.450 && points->at(1).energy < 0.650) //energy limits
      {
        ofs2cry   << points->at(0).x << " "
                  << points->at(0).y << " "
                  << points->at(0).z << " "
                  << points->at(1).x << " "
                  << points->at(1).y << " "
                  << points->at(1).z
                  << std::endl;
        onlyTwoCrystalsInEnergyWindow++; //baseline sensitivity
      }
      onlyTwoCrystals++;
    }
    //--------------------------//


    //--------------------------//
    // 3 crystals hit           //
    //--------------------------//
    if(points->size() == 3)
    {
      //indentification of the pair. at least one must be in the "511 window", i.e. should have experience photoelectric deposition by one of the 2 gammas
      bool foundSingle511 = false;
      int SingleID = -1;
      for(Int_t pCount = 0 ; pCount < points->size() ; pCount++)
      {
        if(points->at(pCount).energy > 0.450 && points->at(pCount).energy < 0.650)
        {
          foundSingle511 = true;
          SingleID = pCount;    //take the ID of the single 511 deposition
        }
      }
      if(foundSingle511) //now take the other two
      {
        float sum_energy = 0;
        for(Int_t pCount = 0 ; pCount < points->size() ; pCount++)
        {
          if(pCount != SingleID) //only the others
          {
             //sum the energies
             sum_energy += points->at(pCount).energy;
          }
        }
        //check if sum is in window
        if(sum_energy > 0.450 && sum_energy < 0.650)
        {
          //count this event as 3 crystals only, in energy window
          onlyThreeCrystalsInEnergyWindow++;
          //---------------------------------------------//
          //strategy a: calculate an average event
          //---------------------------------------------//
          float temp_x = 0;
          float temp_y = 0;
          float temp_z = 0;
          for(Int_t pCount = 0 ; pCount < points->size() ; pCount++)
          {
            if(pCount != SingleID)
            {
              //average x,y,z
              temp_x += (points->at(pCount).x * points->at(pCount).energy) / sum_energy;
              temp_y += (points->at(pCount).y * points->at(pCount).energy) / sum_energy;
              temp_z += (points->at(pCount).z * points->at(pCount).energy) / sum_energy;
            }
          }
          //save in another file
          ofs3cry_avg << points->at(SingleID).x << " "
                      << points->at(SingleID).y << " "
                      << points->at(SingleID).z << " "
                      << temp_x << " "
                      << temp_y << " "
                      << temp_z
                      << std::endl;

          //---------------------------------------------//
          //strategy b: magical compton
          //---------------------------------------------//
          //check which crystal was hit first
          Int_t firstCrystalID;
          float timeOfFirst = INFINITY;
          for(Int_t pCount = 0 ; pCount < points->size() ; pCount++)
          {
            if(pCount != SingleID)
            {
              if(points->at(pCount).time <  timeOfFirst)
              {
                timeOfFirst = points->at(pCount).time;
                firstCrystalID = pCount;
              }
            }
          }
          //now save only the single 511 and the position of first between the two comptons (100% efficient compton discrimination)
          ofs3cry_magicalCompton << points->at(SingleID).x << " "
                                 << points->at(SingleID).y << " "
                                 << points->at(SingleID).z << " "
                                 << points->at(firstCrystalID).x << " "
                                 << points->at(firstCrystalID).y << " "
                                 << points->at(firstCrystalID).z
                                 << std::endl;

          //---------------------------------------------//
          //strategy c: compton with efficiency
          //---------------------------------------------//
          //get the remaing id among 0,1,2
          Int_t otherID = -1;
          for(int fastI = 0 ; fastI < 3 ; fastI++)
          {
            if(fastI != SingleID && fastI != firstCrystalID)
              otherID = fastI;
          }
          if(otherID == -1) std::cout << "ERROR!!!!!!!!!!!!" << std::endl;
          // choose the right one with an efficiency
          double dice = (double)rand() / (double)RAND_MAX;
          Int_t chosenCryID;
          if(dice <= compton_eff)
            chosenCryID = firstCrystalID;
          else
            chosenCryID = otherID;
          ofs3cry_effCompton << points->at(SingleID).x << " "
                            << points->at(SingleID).y << " "
                            << points->at(SingleID).z << " "
                            << points->at(chosenCryID).x << " "
                            << points->at(chosenCryID).y << " "
                            << points->at(chosenCryID).z
                            << std::endl;
        }
        else // the two scatter don't sum up to enter 511 window
        {
          //don't save the event for "reconstruction"
        }
      }
      else // no crystal among the 3 has a deposition that enters the 511 window
      {
        //don't save the event for "reconstruction"
      }
      onlyThreeCrystals++;
    }
    //--------------------------//


    //--------------------------//
    // 4 crystals hit           //
    //--------------------------//
    if(points->size() == 4)
    {
      onlyFourCrystals++;
      float sum_energy = 0;
      for(Int_t pCount = 0 ; pCount < points->size() ; pCount++)
      {
        sum_energy += points->at(pCount).energy;
      }
      if(sum_energy > 0.450 && sum_energy < 0.650)
      {
        onlyFourCrystalsInEnergyWindow++;
      }

    }
    //--------------------------//


    //--------------------------//
    // more than 4 crystals hit //
    //--------------------------//
    if(points->size() > 4)
    {
      moreThanFourCrystals++;
      float sum_energy = 0;
      for(Int_t pCount = 0 ; pCount < points->size() ; pCount++)
      {
        sum_energy += points->at(pCount).energy;
      }
      if(sum_energy > 0.450 && sum_energy < 0.650)
      {
        moreThanFourCrystalsInEnergyWindow++;
      }
    }
    //--------------------------//


    //FEEDBACK TO USER
    int perc = ((100*i)/nsamples); //should strictly have not decimal part, written like this...
    if( (perc % 10) == 0 )
    {
      std::cout << "\r";
      std::cout << perc << "% done... ";
      // std::cout << statuscounter << std::endl;
    }

  }
  std::cout << std::endl;

  //SENSITIVITY COUNTS
  std::cout << "Total dataset                       = " << nsamples << std::endl;
  std::cout << "Only 1 crystal                      = " << onlyOneCrystals << std::endl;
  std::cout << "Only 2 crystals                     = " << onlyTwoCrystals << "\t , @511 = " << onlyTwoCrystalsInEnergyWindow<< std::endl;
  std::cout << "Only 3 crystals                     = " << onlyThreeCrystals << "\t , @511 = "<< onlyThreeCrystalsInEnergyWindow << std::endl;
  std::cout << "Only 4 crystals                     = " << onlyFourCrystals << "\t , @511 = "<< onlyFourCrystalsInEnergyWindow <<std::endl;
  std::cout << "More than 4 crystals                = " << moreThanFourCrystals << "\t , @511 = "<< moreThanFourCrystalsInEnergyWindow <<std::endl;

  //CLOSE FILES
  inputFile->Close();
  ofs2cry.close();
  ofs3cry_avg.close();
  ofs3cry_magicalCompton.close();
  ofs3cry_effCompton.close();
  return 0;
}
