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

int main(int argc, char** argv)
{

  if(argc<2) {
    std::cout<<"You need to provide an directory (where your .root files, output by GATE, are) "<<std::endl ;
    return 1;
  }

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

  int nentries = (int)(Coincidences->GetEntries());
  std::cout << "Number of entries = " << nentries << std::endl;

  for (long int i = 0 ; i < nentries ; i++)
  {
    Coincidences->GetEntry(i);
    //
    if (eventID1 == eventID2)
	  {
	    if ((comptonPhantom1 == 0) && (comptonPhantom2 == 0)) Trues++;
	    else Scatters++;
	  }
    else Randoms++;
  }

  std::cout << "Trues    = " << Trues << std::endl;
  std::cout << "Scatters = " << Scatters << std::endl;
  std::cout << "Randoms  = " << Randoms << std::endl;

  return 0;
}
