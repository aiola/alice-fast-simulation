//runJetSimulationGrid.C

//#include <TSystem.h>
//#include <TInterpreter.h>
//#include <TRoot.h>

//#include "OnTheFlySimulationGenerator.h"

void runJetSimulation(TString name, Int_t pythiaEvents, TString procStr, TString gen, UInt_t seed, TString lhe,
    TString beamType, Double_t ebeam1, Double_t ebeam2, Bool_t rejectISR = kFALSE, Double_t minPtHard = -1, Double_t maxPtHard = -1)
{
  //gSystem->SetFPEMask(kInvalid | kDivByZero | kOverflow | kUnderflow);
  gSystem->SetFPEMask(kNoneMask);

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
  gROOT->ProcessLine(".L OnTheFlySimulationGenerator.cxx+g");

  TString trainName;
  if (!name.IsNull()) {
    trainName = Form("FastSim_%s", name.Data());
  }
  else {
    if (minPtHard >= 0 && maxPtHard >= 0) {
      trainName = Form("FastSim_%s_%s", gen.Data(), procStr.Data());
    }
    else {
      trainName = Form("FastSim_%s_%s_%.0f_%.0f", gen.Data(), procStr.Data(), minPtHard, maxPtHard);
    }
  }

  Process_t proc = kPyMb;
  Bool_t forceDecay = kFALSE;
  OnTheFlySimulationGenerator::ESpecialParticle_t specialPart = OnTheFlySimulationGenerator::kNoSpecialParticle;
  OnTheFlySimulationGenerator::EGenerator_t partonEvent = OnTheFlySimulationGenerator::kPythia6;
  OnTheFlySimulationGenerator::EGenerator_t hadronization = OnTheFlySimulationGenerator::kPythia6;
  OnTheFlySimulationGenerator::EGenerator_t decayer = OnTheFlySimulationGenerator::kPythia6;
  if (gen == "pythia6") {
    if (!lhe.IsNull()) {
      Printf("An LHE file was provided (%s) but PYTHIA was selected as generator. The LHE file will be ignored",lhe);
      lhe = "";
    }
    if (procStr == "dijet") {
      proc = kPyJets;
      specialPart = OnTheFlySimulationGenerator::kNoSpecialParticle;
    }
    else if (procStr == "charm") {
      if (minPtHard >= 0 && maxPtHard >= 0) {
        proc = kPyJets;
      }
      else {
        proc = kPyJets;
        //proc = kPyCharmppMNRwmi;
      }
      specialPart = OnTheFlySimulationGenerator::kccbar;
    }
    else if (procStr == "beauty") {
      if (minPtHard >= 0 && maxPtHard >= 0) {
        proc = kPyJets;
      }
      else {
        proc = kPyJets;
        //proc = kPyBeautyppMNRwmi;
      }
      specialPart = OnTheFlySimulationGenerator::kbbbar;
    }
  }
  else if (gen == "powheg+pythia6") {
    partonEvent = OnTheFlySimulationGenerator::kPowheg;
    if (lhe.IsNull()) {
      Printf("Must provide an LHE file if POWHEG is selected as event generator!");
      return;
    }
    if (minPtHard >= 0 && maxPtHard >= 0) {
      Printf("Pt hard bins are ignored for POWHEG");
    }
    specialPart = OnTheFlySimulationGenerator::kNoSpecialParticle;
    if (procStr == "dijet") {
      proc = kPyJetsPWHG;
    }
    else if (procStr == "charm") {
      proc = kPyCharmPWHG;
    }
    else if (procStr == "beauty") {
      proc = kPyBeautyPWHG;
    }
  }
  else if (gen == "powheg+pythia6+evtgen") {
    partonEvent = OnTheFlySimulationGenerator::kPowheg;
    decayer = OnTheFlySimulationGenerator::kEvtGen;
    if (lhe.IsNull()) {
      Printf("Must provide an LHE file if POWHEG is selected as event generator!");
      return;
    }
    if (minPtHard >= 0 && maxPtHard >= 0) {
      Printf("Pt hard bins are ignored for POWHEG");
    }
    specialPart = OnTheFlySimulationGenerator::kNoSpecialParticle;
    if (procStr == "dijet") {
      proc = kPyJetsPWHG;
    }
    else if (procStr == "charm") {
      proc = kPyCharmPWHG;
    }
    else if (procStr == "beauty") {
      proc = kPyBeautyPWHG;
    }
  }
  else if (gen == "pythia6+evtgen") {
    decayer = OnTheFlySimulationGenerator::kEvtGen;
    if (!lhe.IsNull()) {
      Printf("An LHE file was provided (%s) but PYTHIA was selected as generator. The LHE file will be ignored",lhe);
      lhe = "";
    }
    if (procStr == "dijet") {
      proc = kPyJets;
      specialPart = OnTheFlySimulationGenerator::kNoSpecialParticle;
    }
    else if (procStr == "charm") {
      if (minPtHard >= 0 && maxPtHard >= 0) {
        proc = kPyJets;
      }
      else {
        proc = kPyJets;
        //proc = kPyCharmppMNRwmi;
      }
      specialPart = OnTheFlySimulationGenerator::kccbar;
    }
    else if (procStr == "beauty") {
      if (minPtHard >= 0 && maxPtHard >= 0) {
        proc = kPyJets;
      }
      else {
        proc = kPyJets;
        //proc = kPyBeautyppMNRwmi;
      }
      specialPart = OnTheFlySimulationGenerator::kbbbar;
    }
  }

  OnTheFlySimulationGenerator* sim = new OnTheFlySimulationGenerator(trainName);
  sim->SetNumberOfEvents(pythiaEvents);
  sim->SetProcess(proc);
  sim->SetSpecialParticle(specialPart);
  sim->SetForceHadronicDecay(forceDecay);
  sim->SetPartonEventGenerator(partonEvent);
  sim->SetHadronization(hadronization);
  sim->SetDecayer(decayer);
  sim->SetSeed(seed);
  sim->SetLHEFile(lhe);
  sim->SetEnergyBeam1(ebeam1);
  sim->SetEnergyBeam2(ebeam2);
  sim->SetPtHardRange(minPtHard, maxPtHard);
  sim->SetRejectISR(rejectISR);
  if (procStr == "dijet") {
    sim->EnableJetTree();
  }
  else if (procStr == "charm" || procStr == "beauty") {
    sim->EnableDMesonJets();
  }
  if (beamType == "pPb") {
    sim->SetBeamType(OnTheFlySimulationGenerator::kpPb);
  }
  else if (beamType == "pp") {
    sim->SetBeamType(OnTheFlySimulationGenerator::kpp);
  }
  else {
    Printf("ERROR: Beam type %s not recognized!! Not running...", beamType.Data());
    return;
  }
  sim->Start();
}
