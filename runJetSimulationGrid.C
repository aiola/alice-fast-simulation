//runJetSimulationGrid.C

#include <TSystem.h>
#include <TInterpreter.h>

void runJetSimulationGrid(TString name, Int_t pythiaEvents, TString procStr, TString gen, UInt_t seed, TString lhe, TString beamType, Double_t ebeam1, Double_t ebeam2, Int_t ptHard = -1)
{
  //gSystem->SetFPEMask(TSystem::kInvalid | TSystem::kDivByZero | TSystem::kOverflow | TSystem::kUnderflow);
  gSystem->SetFPEMask(TSystem::kNoneMask);

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
    if (ptHard < 0) {
      trainName = Form("FastSim_%s_%s", gen.Data(), procStr.Data());
    }
    else {
      trainName = Form("FastSim_%s_%s_%d", gen.Data(), procStr.Data(), ptHard);
    }
  }

  Process_t proc = kPyMb;
  Bool_t forceDecay = kFALSE;
  OnTheFlySimulationGenerator::ESpecialParticle_t specialPart = OnTheFlySimulationGenerator::kNoSpecialParticle;
  if (gen == "pythia") {
    if (!lhe.IsNull()) {
      Printf("An LHE file was provided (%s) but PYTHIA was selected as generator. The LHE file will be ignored",lhe);
      lhe = "";
    }
    if (procStr == "dijet") {
      proc = kPyJets;
      specialPart = OnTheFlySimulationGenerator::kNoSpecialParticle;
    }
    else if (procStr == "charm") {
      if (ptHard >= 0) {
        proc = kPyJets;
      }
      else {
        proc = kPyCharmppMNRwmi;
      }
      specialPart = OnTheFlySimulationGenerator::kccbar;
    }
    else if (procStr == "beauty") {
      if (ptHard >= 0) {
        proc = kPyJets;
      }
      else {
        proc = kPyBeautyppMNRwmi;
      }
      specialPart = OnTheFlySimulationGenerator::kbbbar;
    }
  }
  else if (gen == "powheg") {
    if (lhe.IsNull()) {
      Printf("Must provide an LHE file if POWHEG is selected as event generator!");
      return;
    }
    if (ptHard >= 0) {
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

  OnTheFlySimulationGenerator* sim = new OnTheFlySimulationGenerator(trainName);
  sim->SetNumberOfEvents(pythiaEvents);
  sim->SetProcess(proc);
  sim->SetSpecialParticle(specialPart);
  sim->SetForceHadronicDecay(forceDecay);
  sim->SetSeed(seed);
  sim->SetLHEFile(lhe);
  sim->SetEnergyBeam1(ebeam1);
  sim->SetEnergyBeam2(ebeam2);
  sim->SetPtHardBin(ptHard);
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
