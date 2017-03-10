// compile with
// g++ -o gateToString gateToString.cpp `root-config --cflags --libs`

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




//struct of energy depositions events.
// where (crystal id and position in the crystal) was the energy deposited, how much and when
// local coordinates: remember that in GATE simulation,
struct enDep
{
  Float_t  localPosX;
  Float_t  localPosY;
  Float_t  localPosZ;
  Int_t    parentID;
  Int_t    trackID;
  Int_t    globalCryID;
  Int_t    eventID;
  Int_t    gantryID;
  Int_t    rsectorID;
  Int_t    moduleID;
  Int_t    submoduleID;
  Int_t    crystalID;
  Int_t    layerID;
  Double_t time;
  Float_t  edep;
};

struct avgCryEnergyDep
{
  Int_t eventID;
  Int_t parentID;
  Int_t trackID;
  int   globalCryID;
  float x;
  float y;
  float z;
  float sx;
  float sy;
  float sz;
  float energy;
  float time;
};

//function to compare deposition event struct vectors using the field time
bool compareByTime(const enDep &a,const enDep  &b)
{
  return a.time < b.time;
}

int main(int argc, char** argv)
{

  if(argc<2) {
    std::cout<<"You need to provide an directory (where your .root files, output by GATE, are) "<<std::endl ;
    return 1;
  }

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

  std::string filedir, inputfilename ;
  std::string filename, Moutputfilename, Poutputfilename ;

  filedir = argv[1] ;
  inputfilename = filedir + "*.root" ;

  std::cout << "Input file name is " << inputfilename << std::endl;
  TChain *Hits = new TChain("Hits") ;
  TChain *Singles = new TChain("Singles") ;
  TChain *Coincidences = new TChain("Coincidences") ;
  Hits->Add(inputfilename.c_str());
  Singles->Add(inputfilename.c_str());
  Coincidences->Add(inputfilename.c_str());
  long int Trues = 0;
  long int Scatters = 0;
  long int Randoms = 0;
  // Int_t   nbytes = 0;

  TFile* outputFile = new TFile("../output.root","recreate");

  TTree* outputTree = new TTree("test","test");
  // avgCryEnergyDep outAvg;
  std::vector<avgCryEnergyDep> averageDepEvents;
  outputTree->Branch("avgCryEnergyDep","std::vector<avgCryEnergyDep>",&averageDepEvents);


  //######################################################################################
  //#                        Variables                                                   #
  //######################################################################################

  //HITS
  Int_t HITSPDGEncoding;
  Int_t HITStrackID;
  Int_t HITSparentID;
  Double_t HITStime;
  Float_t HITSedep;
  Float_t HITSstepLength;
  Float_t HITSposX;
  Float_t HITSposY;
  Float_t HITSposZ;
  Float_t HITSlocalPosX;
  Float_t HITSlocalPosY;
  Float_t HITSlocalPosZ;
  Int_t HITSgantryID;
  Int_t HITSrsectorID;
  Int_t HITSmoduleID;
  Int_t HITSsubmoduleID;
  Int_t HITScrystalID;
  Int_t HITSlayerID;
  Int_t HITSphotonID;
  Int_t HITSnPhantomCompton;
  Int_t HITSnCrystalCompton;
  Int_t HITSnPhantomRayleigh;
  Int_t HITSnCrystalRayleigh;
  Int_t HITSprimaryID;
  Float_t HITSsourcePosX;
  Float_t HITSsourcePosY;
  Float_t HITSsourcePosZ;
  Int_t HITSsourceID;
  Int_t HITSeventID;
  Int_t HITSrunID;
  Float_t HITSaxialPos;
  Float_t HITSrotationAngle;
  Int_t HITSvolumeID;
  Char_t HITSprocessName[40];
  Char_t HITScomptVolName[40];
  Char_t HITSRayleighVolName[40];

  //SINGLES
  Int_t SINGLESrunID;
  Int_t SINGLESeventID;
  Int_t SINGLESsourceID;
  Float_t SINGLESsourcePosX;
  Float_t SINGLESsourcePosY;
  Float_t SINGLESsourcePosZ;
  Float_t SINGLEStime;
  Float_t SINGLESenergy;
  Float_t SINGLESglobalPosX;
  Float_t SINGLESglobalPosY;
  Float_t SINGLESglobalPosZ;
  Int_t SINGLESgantryID;
  Int_t SINGLESrsectorID;
  Int_t SINGLESmoduleID;
  Int_t SINGLESsubmoduleID;
  Int_t SINGLEScrystalID;
  Int_t SINGLESlayerID;
  Int_t SINGLEScomptonPhantom;
  Int_t SINGLEScomptonCrystal;
  Int_t SINGLESRayleighPhantom;
  Int_t SINGLESRayleighCrystal;
  Float_t SINGLESaxialPos;
  Float_t SINGLESrotationAngle;
  Char_t SINGLEScomptVolName[40];
  Char_t SINGLESRayleighVolName[40];

  //COINCIDENCES
  Float_t         CoincAxialPos, CoincRotationAngle, sinogramS, sinogramTheta;
  Char_t          comptVolName1[40], comptVolName2[40];
  Char_t          RayleighVolName1[40], RayleighVolName2[40];
  Int_t           comptonPhantom1, comptonPhantom2;
  Int_t           comptonCrystal1, comptonCrystal2;
  Int_t           RayleighPhantom1, RayleighPhantom2;
  Int_t           RayleighCrystal1, RayleighCrystal2;
  Int_t           CoincRunID, sourceID1, sourceID2, eventID1, eventID2;
  Int_t           layerID1, layerID2, crystalID1, crystalID2;
  Int_t           submoduleID1, submoduleID2, moduleID1, moduleID2, rsectorID1, rsectorID2, gantryID1, gantryID2;
  Float_t         energy1, energy2;
  Float_t         globalPosX1, globalPosX2, globalPosY1, globalPosY2, globalPosZ1, globalPosZ2;
  Float_t         sourcePosX1, sourcePosX2, sourcePosY1, sourcePosY2, sourcePosZ1, sourcePosZ2;
  Double_t        time1, time2;

  //######################################################################################
  //#                        Set branch addresses                                        #
  //######################################################################################

  //TTree Hits
  Hits->SetBranchStatus("*",0); //WHY?
  Hits->SetBranchAddress("PDGEncoding",&HITSPDGEncoding);
  Hits->SetBranchAddress("trackID",&HITStrackID);
  Hits->SetBranchAddress("parentID",&HITSparentID);
  Hits->SetBranchAddress("time",&HITStime);
  Hits->SetBranchAddress("edep",&HITSedep);
  Hits->SetBranchAddress("stepLength",&HITSstepLength);
  Hits->SetBranchAddress("posX",&HITSposX);
  Hits->SetBranchAddress("posY",&HITSposY);
  Hits->SetBranchAddress("posZ",&HITSposZ);
  Hits->SetBranchAddress("localPosX",&HITSlocalPosX);
  Hits->SetBranchAddress("localPosY",&HITSlocalPosY);
  Hits->SetBranchAddress("localPosZ",&HITSlocalPosZ);
  Hits->SetBranchAddress("gantryID",&HITSgantryID);
  Hits->SetBranchAddress("rsectorID",&HITSrsectorID);
  Hits->SetBranchAddress("moduleID",&HITSmoduleID);
  Hits->SetBranchAddress("submoduleID",&HITSsubmoduleID);
  Hits->SetBranchAddress("crystalID",&HITScrystalID);
  Hits->SetBranchAddress("layerID",&HITSlayerID);
  Hits->SetBranchAddress("photonID",&HITSphotonID);
  Hits->SetBranchAddress("nPhantomCompton",&HITSnPhantomCompton);
  Hits->SetBranchAddress("nCrystalCompton",&HITSnCrystalCompton);
  Hits->SetBranchAddress("nPhantomRayleigh",&HITSnPhantomRayleigh);
  Hits->SetBranchAddress("nCrystalRayleigh",&HITSnCrystalRayleigh);
  Hits->SetBranchAddress("primaryID",&HITSprimaryID);
  Hits->SetBranchAddress("sourcePosX",&HITSsourcePosX);
  Hits->SetBranchAddress("sourcePosY",&HITSsourcePosY);
  Hits->SetBranchAddress("sourcePosZ",&HITSsourcePosZ);
  Hits->SetBranchAddress("sourceID",&HITSsourceID);
  Hits->SetBranchAddress("eventID",&HITSeventID);
  Hits->SetBranchAddress("runID",&HITSrunID);
  Hits->SetBranchAddress("axialPos",&HITSaxialPos);
  Hits->SetBranchAddress("rotationAngle",&HITSrotationAngle);
  Hits->SetBranchAddress("volumeID",&HITSvolumeID);
  Hits->SetBranchAddress("processName",&HITSprocessName);
  Hits->SetBranchAddress("comptVolName",&HITScomptVolName);
  Hits->SetBranchAddress("RayleighVolName",&HITSRayleighVolName);

  //TTree Singles
  Singles->SetBranchStatus("*",0); //WHY?
  Singles->SetBranchAddress("runID",&SINGLESrunID);
  Singles->SetBranchAddress("eventID",&SINGLESeventID);
  Singles->SetBranchAddress("sourceID",&SINGLESsourceID);
  Singles->SetBranchAddress("sourcePosX",&SINGLESsourcePosX);
  Singles->SetBranchAddress("sourcePosY",&SINGLESsourcePosY);
  Singles->SetBranchAddress("sourcePosZ",&SINGLESsourcePosZ);
  Singles->SetBranchAddress("time",&SINGLEStime);
  Singles->SetBranchAddress("energy",&SINGLESenergy);
  Singles->SetBranchAddress("globalPosX",&SINGLESglobalPosX);
  Singles->SetBranchAddress("globalPosY",&SINGLESglobalPosY);
  Singles->SetBranchAddress("globalPosZ",&SINGLESglobalPosZ);
  Singles->SetBranchAddress("gantryID",&SINGLESgantryID);
  Singles->SetBranchAddress("rsectorID",&SINGLESrsectorID);
  Singles->SetBranchAddress("moduleID",&SINGLESmoduleID);
  Singles->SetBranchAddress("submoduleID",&SINGLESsubmoduleID);
  Singles->SetBranchAddress("crystalID",&SINGLEScrystalID);
  Singles->SetBranchAddress("layerID",&SINGLESlayerID);
  Singles->SetBranchAddress("comptonPhantom",&SINGLEScomptonPhantom);
  Singles->SetBranchAddress("comptonCrystal",&SINGLEScomptonCrystal);
  Singles->SetBranchAddress("RayleighPhantom",&SINGLESRayleighPhantom);
  Singles->SetBranchAddress("RayleighCrystal",&SINGLESRayleighCrystal);
  Singles->SetBranchAddress("axialPos",&SINGLESaxialPos);
  Singles->SetBranchAddress("rotationAngle",&SINGLESrotationAngle);
  Singles->SetBranchAddress("comptVolName",&SINGLEScomptVolName);
  Singles->SetBranchAddress("RayleighVolName",&SINGLESRayleighVolName);

  //TTree Coincidences
  Coincidences->SetBranchStatus("*",0); //WHY?
  Coincidences->SetBranchAddress("axialPos",&CoincAxialPos);
  Coincidences->SetBranchAddress("comptVolName1",&comptVolName1);
  Coincidences->SetBranchAddress("comptVolName2",&comptVolName2);
  Coincidences->SetBranchAddress("comptonCrystal1",&comptonCrystal1);
  Coincidences->SetBranchAddress("comptonCrystal2",&comptonCrystal2);
  Coincidences->SetBranchAddress("crystalID1",&crystalID1);
  Coincidences->SetBranchAddress("crystalID2",&crystalID2);
  Coincidences->SetBranchAddress("comptonPhantom1",&comptonPhantom1);
  Coincidences->SetBranchAddress("comptonPhantom2",&comptonPhantom2);
  Coincidences->SetBranchAddress("RayleighVolName1",&RayleighVolName1);
  Coincidences->SetBranchAddress("RayleighVolName2",&RayleighVolName2);
  Coincidences->SetBranchAddress("RayleighPhantom1",&RayleighPhantom1);
  Coincidences->SetBranchAddress("RayleighPhantom2",&RayleighPhantom2);
  Coincidences->SetBranchAddress("RayleighCrystal1",&RayleighCrystal1);
  Coincidences->SetBranchAddress("RayleighCrystal2",&RayleighCrystal2);
  Coincidences->SetBranchAddress("energy1",&energy1);
  Coincidences->SetBranchAddress("energy2",&energy2);
  Coincidences->SetBranchAddress("eventID1",&eventID1);
  Coincidences->SetBranchAddress("eventID2",&eventID2);
  Coincidences->SetBranchAddress("globalPosX1",&globalPosX1);
  Coincidences->SetBranchAddress("globalPosX2",&globalPosX2);
  Coincidences->SetBranchAddress("globalPosY1",&globalPosY1);
  Coincidences->SetBranchAddress("globalPosY2",&globalPosY2);
  Coincidences->SetBranchAddress("globalPosZ1",&globalPosZ1);
  Coincidences->SetBranchAddress("globalPosZ2",&globalPosZ2);
  Coincidences->SetBranchAddress("gantryID1",&gantryID1);
  Coincidences->SetBranchAddress("gantryID2",&gantryID2);
  Coincidences->SetBranchAddress("layerID1",&layerID1);
  Coincidences->SetBranchAddress("layerID2",&layerID2);
  Coincidences->SetBranchAddress("moduleID1",&moduleID1);
  Coincidences->SetBranchAddress("moduleID2",&moduleID2);
  Coincidences->SetBranchAddress("rotationAngle",&CoincRotationAngle);
  Coincidences->SetBranchAddress("rsectorID1",&rsectorID1);
  Coincidences->SetBranchAddress("rsectorID2",&rsectorID2);
  Coincidences->SetBranchAddress("runID",&CoincRunID);
  Coincidences->SetBranchAddress("sinogramS",&sinogramS);
  Coincidences->SetBranchAddress("sinogramTheta",&sinogramTheta);
  Coincidences->SetBranchAddress("sourceID1",&sourceID1);
  Coincidences->SetBranchAddress("sourceID2",&sourceID2);
  Coincidences->SetBranchAddress("sourcePosX1",&sourcePosX1);
  Coincidences->SetBranchAddress("sourcePosX2",&sourcePosX2);
  Coincidences->SetBranchAddress("sourcePosY1",&sourcePosY1);
  Coincidences->SetBranchAddress("sourcePosY2",&sourcePosY2);
  Coincidences->SetBranchAddress("sourcePosZ1",&sourcePosZ1);
  Coincidences->SetBranchAddress("sourcePosZ2",&sourcePosZ2);
  Coincidences->SetBranchAddress("submoduleID1",&submoduleID1);
  Coincidences->SetBranchAddress("submoduleID2",&submoduleID2);
  Coincidences->SetBranchAddress("time1",&time1);
  Coincidences->SetBranchAddress("time2",&time2);

  Int_t HITSnentries = (Int_t)(Hits->GetEntries());
  Int_t SINGLESCnentries = (Int_t)(Singles->GetEntries());
  Int_t COINCnentries = (Int_t)(Coincidences->GetEntries());
  std::cout << "Number of hits         = " << HITSnentries << std::endl;
  std::cout << "Number of singles      = " << SINGLESCnentries << std::endl;
  std::cout << "Number of coincidences = " << COINCnentries << std::endl;

  Int_t eventsCheck = -1;
  std::vector<enDep> energyDeposition;

  Int_t nRSectors          = 62;
  Int_t nModulesXRSector   = 1;
  Int_t nSubmodulesXModule = 16;
  Int_t nCrystalXSubmodule  = 4;
  Int_t nLayersXCrystal    = 1;

  // for (Int_t i = 0 ; i < (Int_t)(Hits->GetEntries()) ; i++)
  // {
  //   Hits->GetEntry(i); // each i is a energyDeposition
  //   if(HITStrackID > 3) std::cout << i  <<  " "  << HITStrackID  << " " << HITSparentID <<  std::endl;
  // }
  // std::cout << std::endl;

  // for (Int_t i = 0 ; i < (Int_t)(Hits->GetEntries()) ; i++)
  Int_t eventCounter = 0;
  for (Int_t i = 0 ; i < 40 ; i++)
  {
    Hits->GetEntry(i); // each i is a energyDeposition
    // std::cout<< eventsCheck << " " << HITSeventID << " " << HITStrackID <<  std::endl;

    if(eventsCheck != HITSeventID)
    {
      int CurrentID = -1;
      // "average" what happened into each crystal
      // stackingaction in g4 is last-in-first-out so we need to sort energyDeposition before we check what crystal was hit first (needed in gATE?)
      // check crystal sequence, check for returning gammas, calculate an average xyz per crystal

      std::vector < enDep > CrystalEnDepCollection;
      std::vector<std::vector < enDep > > separatedEnDep;
      if(energyDeposition.size() != 0)
      {
        std::sort(energyDeposition.begin(), energyDeposition.end(), compareByTime);
        for(int eEvent = 0; eEvent < energyDeposition.size(); eEvent++) //run on energy depositions and find in how many crystals energy was deposited
        {
          // std::cout << energyDeposition[eEvent].eventID << " " << energyDeposition[eEvent].globalCryID << " " <<  energyDeposition[eEvent].edep << " " << energyDeposition[eEvent].localPosX << " " << energyDeposition[eEvent].time << std::endl;
          //this for cycles on all the endep events. it stores them (each one is a struct) in a std::vector of structs, until the gamma changes crystal
          //when we enter a new crystal, the std::vector of that crystal is pushed_back into (guess what?) a std::vector of std::vector and another std::vector is created
          // so to sumarize:
          // energy deposition event -> struct
          // all energy deposition events in a given crystal -> std::vector<struct> -> i call this a "collection" below
          // all crystals touched by the gamma -> std::vector< std:: vector <struct> > this would be the std::vector of "collections"


          //read the crystal where energy was deposited
          //discard if there was no energy deposition (if edep == 0)
          // if(energyDeposition[eEvent].edep!=0)
          // {
            int cry = energyDeposition[eEvent].globalCryID;
            if(eEvent == 0) CurrentID = cry; //needed for the first step
            //create temp enDep variable and copy this eEvent into it
            enDep tempCrystalEnDep;
            tempCrystalEnDep = energyDeposition[eEvent];
            if(cry != CurrentID) // if this endep event is happening in a new crystal wrt the one before
            {
              separatedEnDep.push_back(CrystalEnDepCollection); // save the collection of this crystal into the std::vector of collections
              CrystalEnDepCollection.clear(); //clear this collection
              CurrentID = cry; //change the current id
            }

            CrystalEnDepCollection.push_back(tempCrystalEnDep); // save this enDep event into the collection of this crystal
            if(eEvent == energyDeposition.size() -1)
            {
              separatedEnDep.push_back(CrystalEnDepCollection);
            }
          // }
        }


        // now for each crystal average what happened inside
        // what happened in a crystal cannot be seen in split details by the detector so we need some useful
        // variables to gain the chance to filter the simulation dataset, if we want to understand
        // what's going on
        // std::vector<avgCryEnergyDep> averageDepEvents;
        averageDepEvents.clear();
        // now the en dep events are collected by crystal
        // run on each collection and find average
        for(int iColl = 0 ; iColl < separatedEnDep.size(); iColl++)
        {
          avgCryEnergyDep tempAvgEnDep;
          // std::cout << separatedEnDep.at(iColl).size() << std::endl;

          //initialize
          tempAvgEnDep.eventID = separatedEnDep.at(iColl).at(0).eventID;
          tempAvgEnDep.parentID = separatedEnDep.at(iColl).at(0).parentID;
          tempAvgEnDep.trackID = separatedEnDep.at(iColl).at(0).trackID;
          tempAvgEnDep.globalCryID = separatedEnDep.at(iColl).at(0).globalCryID;
          tempAvgEnDep.x = 0;
          tempAvgEnDep.y = 0;
          tempAvgEnDep.z = 0;
          tempAvgEnDep.time = 0;
          tempAvgEnDep.energy = 0;
          tempAvgEnDep.sx = 0;
          tempAvgEnDep.sy = 0;
          tempAvgEnDep.sz = 0;
          //now run on the energy deposition events in this crystal and calculate the averages
          // std::cout << "aaaaaaaaaaaaaa" << std::endl;

          for(int iEndep = 0; iEndep < separatedEnDep.at(iColl).size(); iEndep++)
          {
            tempAvgEnDep.energy += separatedEnDep.at(iColl).at(iEndep).edep;
          }
          if(tempAvgEnDep.energy != 0)
          {
            for(int iEndep = 0; iEndep < separatedEnDep.at(iColl).size(); iEndep++)
            {
              if(separatedEnDep.at(iColl).at(iEndep).edep != 0)
              {
                tempAvgEnDep.x += separatedEnDep.at(iColl).at(iEndep).localPosX * separatedEnDep.at(iColl).at(iEndep).edep;
                tempAvgEnDep.y += separatedEnDep.at(iColl).at(iEndep).localPosY * separatedEnDep.at(iColl).at(iEndep).edep;
                tempAvgEnDep.z += separatedEnDep.at(iColl).at(iEndep).localPosZ * separatedEnDep.at(iColl).at(iEndep).edep;
                tempAvgEnDep.time += separatedEnDep.at(iColl).at(iEndep).time * separatedEnDep.at(iColl).at(iEndep).edep;
              }
            }
            tempAvgEnDep.x = tempAvgEnDep.x / tempAvgEnDep.energy;
            tempAvgEnDep.y = tempAvgEnDep.y / tempAvgEnDep.energy;
            tempAvgEnDep.z = tempAvgEnDep.z / tempAvgEnDep.energy;
            tempAvgEnDep.time = tempAvgEnDep.time / tempAvgEnDep.energy;
            float varx = 0.0; // variance (needed for stdev afterwards)
            float vary = 0.0;
            float varz = 0.0;
            for(int iEndep = 0; iEndep < separatedEnDep.at(iColl).size(); iEndep++)
            {
              varx += (separatedEnDep.at(iColl).at(iEndep).edep * pow(separatedEnDep.at(iColl).at(iEndep).localPosX  - tempAvgEnDep.x,2)) / tempAvgEnDep.energy;
              vary += (separatedEnDep.at(iColl).at(iEndep).edep * pow(separatedEnDep.at(iColl).at(iEndep).localPosY  - tempAvgEnDep.y,2)) / tempAvgEnDep.energy;
              varz += (separatedEnDep.at(iColl).at(iEndep).edep * pow(separatedEnDep.at(iColl).at(iEndep).localPosZ  - tempAvgEnDep.z,2)) / tempAvgEnDep.energy;
            }
            tempAvgEnDep.sx = sqrt(varx);
            tempAvgEnDep.sy = sqrt(vary);
            tempAvgEnDep.sz = sqrt(varz);

            //save into the std::vector of averages

            averageDepEvents.push_back(tempAvgEnDep);
          }
        }

        // now output the reading of this event, like this
        // eventID (same as )
        if(averageDepEvents.size()>1) //single crystal pet makes no sense...
        {
          // outAvg = averageDepEvents;
          outputTree->Fill();
          //
          // eventCounter++;
          // for(int iAvg = 0; iAvg < averageDepEvents.size() ; iAvg++)
          // {
          //   std::cout << eventCounter << " " << averageDepEvents[iAvg].globalCryID << " " << averageDepEvents[iAvg].time << " " << averageDepEvents[iAvg].energy << " " << averageDepEvents[iAvg].x << " " << averageDepEvents[iAvg].y << " " << averageDepEvents[iAvg].z << " " << averageDepEvents[iAvg].parentID << " " << averageDepEvents[iAvg].trackID <<   std::endl;
          // }
        }

      }
      // std::cout<< "----------------------" << std::endl;


      //move to next pair of gammas, reset
      eventsCheck = HITSeventID;
      energyDeposition.clear();
    }
    //continue (or start) adding to the event

    //first thing first, go back to the original parent. It can be

    enDep hitDeposition;       //create a new endep struct
    hitDeposition.localPosX    = HITSlocalPosX;
    hitDeposition.localPosY    = HITSlocalPosY;
    hitDeposition.localPosZ    = HITSlocalPosZ;
    hitDeposition.eventID      = HITSeventID;
    hitDeposition.parentID     = HITSparentID;
    hitDeposition.trackID      = HITStrackID;
    hitDeposition.globalCryID  = HITSrsectorID * (nModulesXRSector * nSubmodulesXModule * nCrystalXSubmodule * nLayersXCrystal) +
                                 HITSmoduleID  * (nSubmodulesXModule * nCrystalXSubmodule * nLayersXCrystal) +
                                 HITSsubmoduleID * (nCrystalXSubmodule * nLayersXCrystal) +
                                 HITScrystalID * (nLayersXCrystal) +
                                 HITSlayerID;
    hitDeposition.gantryID     = HITSgantryID;
    hitDeposition.rsectorID    = HITSrsectorID;
    hitDeposition.moduleID     = HITSmoduleID;
    hitDeposition.submoduleID  = HITSsubmoduleID;
    hitDeposition.crystalID    = HITScrystalID;
    hitDeposition.layerID      = HITSlayerID;
    hitDeposition.time         = HITStime;
    hitDeposition.edep         = HITSedep;
    energyDeposition.push_back(hitDeposition); //push it to the vector
    // if(HITStrackID > 2)

  }


  // for (Int_t i = 0 ; i < COINCnentries ; i++)
  // {
  //   Coincidences->GetEntry(i);
  //   //
  //   if (eventID1 == eventID2)
	//   {
	//     if ((comptonPhantom1 == 0) && (comptonPhantom2 == 0)) Trues++;
	//     else Scatters++;
	//   }
  //   else Randoms++;
  //
  //
  // }
  outputFile->cd();
  outputTree->Write();
  outputFile->Close();
  // std::cout << "Trues    = " << Trues << std::endl;
  // std::cout << "Scatters = " << Scatters << std::endl;
  // std::cout << "Randoms  = " << Randoms << std::endl;

  return 0;
}
