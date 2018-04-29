//start_merging.C

#ifndef __CINT__
#include <TSystem.h>
#include <TInterpreter.h>
#include <TROOT.h>
#include <AliLog.h>
#endif

#include <cstdio>
#include <iostream>

void start_merging(const char* fileList, const char* skipList = "", const char* acceptList = "", Int_t n=2)
{
  gInterpreter->AddIncludePath("$ALICE_ROOT/include");
  gInterpreter->AddIncludePath("$ALICE_PHYSICS/include");

  gSystem->Load("libSTEER");
  gSystem->Load("libAOD");
  gSystem->Load("libPWGEMCALtasks");
  gSystem->Load("libPWGJEEMCALJetTasks");

  TString command = TString::Format(".x runJetSimulationMergingGrid.C+g(\"%s\", \"%s\", \"%s\", %d)",
      fileList, skipList, acceptList, n);

  gROOT->ProcessLine(command.Data());
}
