//start_simulation.C

#include <TSystem.h>
#include <TInterpreter.h>
#include <TROOT.h>
#include <AliLog.h>

#include <cstdio>
#include <iostream>

void start_simulation(TString name, Int_t pythiaEvents, TString procStr, TString gen, UInt_t seed, TString lhe,
    TString beamType, Double_t ebeam1, Double_t ebeam2, Bool_t rejectISR = kFALSE, Double_t minPtHard = -1, Double_t maxPtHard = -1,
    UInt_t debug_level = 0)
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

  std::cout << "Loading libraries for PYTHIA6" << std::endl;
  gSystem->Load("libpythia6_4_28.so");

  std::cout << "Loading libraries for PYTHIA8" << std::endl;
  gSystem->Load("libpythia8210dev.so");
  gSystem->Load("libAliPythia8.so");
  gSystem->Setenv("PYTHIA8DATA", gSystem->ExpandPathName("$ALICE_ROOT/PYTHIA8/pythia8/xmldoc"));
  gSystem->Setenv("LHAPDF",      gSystem->ExpandPathName("$ALICE_ROOT/LHAPDF"));
  gSystem->Setenv("LHAPATH",     gSystem->ExpandPathName("$ALICE_ROOT/LHAPDF/PDFsets"));

  if (gen.Contains("evtgen")) {
    std::cout << "Loading libraries for EvtGen" << std::endl;

    gSystem->Load("libPhotos");
    gSystem->Load("libEvtGen");
    gSystem->Load("libTEvtGen");
  }

  gROOT->ProcessLine(".L AliGenEvtGen_dev.cxx+g");

  gROOT->ProcessLine(".L OnTheFlySimulationGenerator.cxx+g");

  char command[500] = {0};

  sprintf(command, ".x runJetSimulation.C+g(\"%s\", %d, \"%s\", \"%s\", %d, \"%s\", \"%s\", %f, %f, %d, %f, %f, %d)",
      name.Data(), pythiaEvents, procStr.Data(), gen.Data(), seed, lhe.Data(),
      beamType.Data(), ebeam1, ebeam2, rejectISR, minPtHard, maxPtHard, debug_level);

  gROOT->ProcessLine(command);
}
