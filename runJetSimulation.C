//runJetSimulationGrid.C

#include <iostream>

#include <TSystem.h>
#include <TInterpreter.h>
#include <TROOT.h>

#include "AliLog.h"

#include "OnTheFlySimulationGenerator.h"

void runJetSimulation(TString name, Int_t pythiaEvents, TString procStr, TString gen, UInt_t seed, TString lhe,
    TString beamType, Double_t ebeam1, Double_t ebeam2, Bool_t rejectISR = kFALSE, Double_t minPtHard = -1, Double_t maxPtHard = -1,
    UInt_t debug_level = 0)
{
  //gSystem->SetFPEMask(kInvalid | kDivByZero | kOverflow | kUnderflow);
  gSystem->SetFPEMask(kNoneMask);
  //gSystem->SetFPEMask(kDivByZero);

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
      AliWarningGeneralStream("") << "An LHE file was provided ("<< lhe.Data() << ") but PYTHIA was selected as generator. The LHE file will be ignored" << std::endl;
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
  if (gen == "pythia8") {
    partonEvent = OnTheFlySimulationGenerator::kPythia8;
    hadronization = OnTheFlySimulationGenerator::kPythia8;
    decayer = OnTheFlySimulationGenerator::kPythia8;
    if (!lhe.IsNull()) {
      AliWarningGeneralStream("") << "An LHE file was provided ("<< lhe.Data() << ") but PYTHIA was selected as generator. The LHE file will be ignored" << std::endl;
      lhe = "";
    }
    if (procStr == "dijet") {
      proc = kPyJets;
    }
    else if (procStr == "charm") {
      if (minPtHard == 0) {
        minPtHard = 2.75;
        if (maxPtHard <= minPtHard) maxPtHard = 99999.;
      }
      proc = kPyCharmppMNRwmi;
    }
    else if (procStr == "beauty") {
      if (minPtHard == 0) {
        minPtHard = 2.75;
        if (maxPtHard <= minPtHard) maxPtHard = 99999.;
      }
      proc = kPyBeautyppMNR;
    }
  }
  else if (gen == "powheg+pythia6") {
    partonEvent = OnTheFlySimulationGenerator::kPowheg;
    if (lhe.IsNull()) {
      AliErrorGeneralStream("") << "Must provide an LHE file if POWHEG is selected as event generator!" << std::endl;
      return;
    }
    if (minPtHard >= 0 && maxPtHard >= 0) {
      AliWarningGeneralStream("") << "Pt hard bins are ignored for POWHEG" << std::endl;
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
      AliErrorGeneralStream("") << "Must provide an LHE file if POWHEG is selected as event generator!" << std::endl;
      return;
    }
    if (minPtHard >= 0 && maxPtHard >= 0) {
      AliWarningGeneralStream("") << "Pt hard bins are ignored for POWHEG" << std::endl;
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
      AliWarningGeneralStream("") << "An LHE file was provided ("<< lhe.Data() << ") but PYTHIA was selected as generator. The LHE file will be ignored" << std::endl;
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
  else if (gen == "powheg+pythia8") {
    partonEvent = OnTheFlySimulationGenerator::kPowheg;
    hadronization = OnTheFlySimulationGenerator::kPythia8;
    decayer = OnTheFlySimulationGenerator::kPythia8;
    if (lhe.IsNull()) {
      AliErrorGeneralStream("") << "Must provide an LHE file if POWHEG is selected as event generator!" << std::endl;
      return;
    }
    if (minPtHard >= 0 && maxPtHard >= 0) {
      AliWarningGeneralStream("") << "Pt hard bins are ignored for POWHEG" << std::endl;
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
  else {
    AliErrorGeneralStream("") << "Generator '" << gen.Data() << "' not recognized!" << std::endl;
    return;
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
    AliErrorGeneralStream("") << "ERROR: Beam type " << beamType.Data() << " not recognized!! Not running..." << std::endl;
    return;
  }
  sim->Start(debug_level);
}
