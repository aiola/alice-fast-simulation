//start_simulation.C

#include <TSystem.h>
#include <TInterpreter.h>
#include <TROOT.h>

#include <cstdio>

void start_simulation(TString name, Int_t pythiaEvents, TString procStr, TString gen, UInt_t seed, TString lhe,
    TString beamType, Double_t ebeam1, Double_t ebeam2, Bool_t rejectISR = kFALSE, Double_t minPtHard = -1, Double_t maxPtHard = -1)
{

  gInterpreter->AddIncludePath("$ALICE_ROOT/include");
  gInterpreter->AddIncludePath("$ALICE_PHYSICS/include");
  gInterpreter->AddIncludePath("$FASTJET/include");

  //load fastjet libraries 3.x
  gSystem->Load("libCGAL");

  gSystem->Load("libfastjet");
  gSystem->Load("libsiscone");
  gSystem->Load("libsiscone_spherical");
  gSystem->Load("libfastjetplugins");
  gSystem->Load("libfastjetcontribfragile");

  gSystem->Load("libpythia6_4_28.so");
  gSystem->Load("libpythia8210.so");
  gROOT->ProcessLine(".L OnTheFlySimulationGenerator.cxx+g");

  char command[500] = {0};

  sprintf(command, ".x runJetSimulation.C+g(\"%s\", %d, \"%s\", \"%s\", %d, \"%s\", \"%s\", %f, %f, %d, %f, %f)",
      name.Data(), pythiaEvents, procStr.Data(), gen.Data(), seed, lhe.Data(),
      beamType.Data(), ebeam1, ebeam2, rejectISR, minPtHard, maxPtHard);

  gROOT->ProcessLine(command);
}
