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

  // Parsing generator string into parton event, hadronizer, decayer
  TString partonEvent_str, hadronization_str, decayer_str;
  TObjArray* generators = gen.Tokenize("+");
  UInt_t n = generators->GetEntries();
  if (n == 1) {
    partonEvent_str = generators[0].GetName();
    hadronization_str = generators[0].GetName();
    decayer_str = generators[0].GetName();
  }
  else if (n == 2) {
    partonEvent_str = generators[0].GetName();
    decayer_str = generators[1].GetName();
    if (decayer_str == "pythia6" || decayer_str == "pythia8") {
      hadronization_str = decayer_str;
    }
    else {
      hadronization_str = partonEvent_str;
    }
  }
  else if (n == 3) {
    partonEvent_str = generators[0].GetName();
    hadronization_str = generators[1].GetName();
    decayer_str = generators[2].GetName();
  }

  // Parton event generator selection
  if (partonEvent_str == "powheg") {
    partonEvent = OnTheFlySimulationGenerator::kPowheg;

    // POWHEG selection needs an LHE file (POWHEG runs standalone before the execution of this program)
    if (lhe.IsNull()) {
      AliErrorGeneralStream("") << "Must provide an LHE file if POWHEG is selected as event generator! Aborting." << std::endl;
      return;
    }

    // Select the PYTHIA process
    if (procStr == "dijet") {
      proc = kPyJetsPWHG;
    }
    else if (procStr == "charm") {
      proc = kPyCharmPWHG;
    }
    else if (procStr == "beauty") {
      proc = kPyBeautyPWHG;
    }
    else {
      AliWarningGeneralStream("") << "You choose '" << procStr.Data() << "'. Not clear what process you want to simualte. Aborting." << std::endl;
      return;
    }

    // No need to trigger on special particles
    specialPart = OnTheFlySimulationGenerator::kNoSpecialParticle;
  }
  else if (partonEvent_str == "pythia6") {
    partonEvent = OnTheFlySimulationGenerator::kPythia6;

    // No LHE file needed in this case
    if (!lhe.IsNull()) {
      AliWarningGeneralStream("") << "An LHE file was provided ("<< lhe.Data() << ") but PYTHIA6 was selected as generator. The LHE file will be ignored" << std::endl;
      lhe = "";
    }

    // Always use jet-jet mode for PYTHIA6
    proc = kPyJets;

    // Trigger on HF particle if required
    if (procStr == "dijet") {
      specialPart = OnTheFlySimulationGenerator::kNoSpecialParticle;
    }
    else if (procStr == "charm") {
      specialPart = OnTheFlySimulationGenerator::kccbar;
    }
    else if (procStr == "beauty") {
      specialPart = OnTheFlySimulationGenerator::kbbbar;
    }
  }
  else if (partonEvent_str == "pythia8") {
    partonEvent = OnTheFlySimulationGenerator::kPythia8;

    // No LHE file needed in this case
    if (!lhe.IsNull()) {
      AliWarningGeneralStream("") << "An LHE file was provided ("<< lhe.Data() << ") but PYTHIA8 was selected as generator. The LHE file will be ignored" << std::endl;
      lhe = "";
    }

    // Select the PYTHIA process
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

    // No need to trigger on special particles
    specialPart = OnTheFlySimulationGenerator::kNoSpecialParticle;
  }
  else {
    AliErrorGeneralStream("") << "Parton event generator selection '" << partonEvent_str.Data() << "' is not valid. Aborting." << std::endl;
    return;
  }

  // Hadronization must be either PYTHIA6 or PYTHIA8
  if (hadronization_str == "pythia6") {
    hadronization = OnTheFlySimulationGenerator::kPythia6;

    if (partonEvent != OnTheFlySimulationGenerator::kPythia6 && partonEvent != OnTheFlySimulationGenerator::kPowheg) {
      AliErrorGeneralStream("") << "Hadronization '" << hadronization_str.Data() << "' not compatible with parton event from '" << partonEvent_str.Data() << "'. Aborting." << std::endl;
      return;
    }
  }
  else if (hadronization_str == "pythia8") {
    hadronization = OnTheFlySimulationGenerator::kPythia8;

    // Current implementation of PYTHIA8 wrapper in AliRoot cannot read LHE files
    if (partonEvent != OnTheFlySimulationGenerator::kPythia8) {
      AliErrorGeneralStream("") << "Hadronization '" << hadronization_str.Data() << "' not compatible with parton event from '" << partonEvent_str.Data() << "'. Aborting." << std::endl;
      return;
    }
  }
  else {
    AliErrorGeneralStream("") << "Hadronization generator selection '" << hadronization_str.Data() << "' is not valid. Aborting." << std::endl;
    return;
  }

  // Decayer selection
  if (decayer_str == "pythia6") {
    decayer = OnTheFlySimulationGenerator::kPythia6;

    if (hadronization != OnTheFlySimulationGenerator::kPythia6) {
      AliErrorGeneralStream("") << "Decayer '" << decayer_str.Data() << "' not compatible with hadronization from '" << hadronization_str.Data() << "'. Aborting." << std::endl;
      return;
    }
  }
  else if (decayer_str == "pythia8") {
    decayer = OnTheFlySimulationGenerator::kPythia8;

    if (hadronization != OnTheFlySimulationGenerator::kPythia8) {
      AliErrorGeneralStream("") << "Decayer '" << decayer_str.Data() << "' not compatible with hadronization from '" << hadronization_str.Data() << "'. Aborting." << std::endl;
      return;
    }
  }
  else if (decayer_str == "evtgen") {
    decayer = OnTheFlySimulationGenerator::kEvtGen;
  }
  else {
    AliErrorGeneralStream("") << "Decayer generator selection '" << decayer_str.Data() << "' is not valid. Aborting." << std::endl;
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
