//runJetSimulationGrid.C

void runJetSimulationGrid(Int_t pythiaEvents, TString procStr, TString gen, UInt_t seed, TString lhe, TString name)
{
  //gSystem->SetFPEMask(TSystem::kInvalid | TSystem::kDivByZero | TSystem::kOverflow | TSystem::kUnderflow);
  gSystem->SetFPEMask(TSystem::kNoneMask);

  gInterpreter->AddIncludePath("$ALICE_ROOT/include");
  gInterpreter->AddIncludePath("$ALICE_PHYSICS/include");
  gInterpreter->AddIncludePath("$FASTJET/include");
  gInterpreter->AddIncludePath("/opt/alicesw/RooUnfold/src");

  //load fastjet libraries 3.x
  gSystem->Load("libCGAL");

  gSystem->Load("libfastjet");
  gSystem->Load("libsiscone");
  gSystem->Load("libsiscone_spherical");
  gSystem->Load("libfastjetplugins");
  gSystem->Load("libfastjetcontribfragile");

  gSystem->Load("libpythia6_4_28.so");
  gROOT->ProcessLine(".L AliAnalysisTaskSEhfcjMCanalysis.cxx+g");
  gROOT->ProcessLine(".L runJetSimulation.C+g");

  TString trainName;
  if (!name.IsNull()) {
    trainName = Form("FastSim_%s", name.Data());
  }
  else {
    trainName = Form("FastSim_%s_%s", gen.Data(), procStr.Data());
  }

  Process_t proc = kPyMb;
  Bool_t forceDecay = kFALSE;
  ESpecialParticle_t specialPart = kNoSpecialParticle;
  if (gen == "pythia") {
    if (!lhe.IsNull()) {
      Printf("An LHE file was provided (%s) but PYTHIA was selected as generator. The LHE file will be ignored",lhe);
      lhe = "";
    }
    if (procStr == "dijet") {
      proc = kPyJets;
      forceDecay = kFALSE;
      specialPart = kNoSpecialParticle;
    }
    else if (procStr == "charm") {
      proc = kPyCharm;
      forceDecay = kFALSE;
      specialPart = kccbar;
    }
    else if (procStr == "beauty") {
      proc = kPyBeauty;
      forceDecay = kFALSE;
      specialPart = kbbbar;
    }
  }
  else if (gen == "powheg") {
    if (lhe.IsNull()) {
      Printf("Must provide an LHE file if POWHEG is selected as event generator!");
      return;
    }
    if (procStr == "dijet") {
      proc = kPyJetsPWHG;
      forceDecay = kFALSE;
      specialPart = kNoSpecialParticle;
    }
    else if (procStr == "charm") {
      proc = kPyCharmPWHG;
      forceDecay = kFALSE;
      specialPart = kNoSpecialParticle;
    }
    else if (procStr == "beauty") {
      proc = kPyBeautyPWHG;
      forceDecay = kFALSE;
      specialPart = kNoSpecialParticle;
    }
  }
  runJetSimulation(pythiaEvents, proc, specialPart, forceDecay, trainName, seed, lhe);
}
