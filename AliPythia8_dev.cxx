
/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

#include <AliLog.h>
#include <AliStack.h>
#include <AliPythiaRndm.h>
#include <Pythia8/Analysis.h>

#include "AliPythia8_dev.h"

ClassImp(AliPythia8_dev)

AliPythia8_dev::AliPythia8_dev():
  AliTPythia8(),
  AliPythiaBase_dev(),
  fProcess(kPyMbDefault),
  fItune(-1),
  fEcms(0.),
  fStrucFunc(-1),
  fLHEFile()
{
  // Default Constructor
  //
  //  Set random number
  if (!AliPythiaRndm::GetPythiaRandom()) AliPythiaRndm::SetPythiaRandom(GetRandom());
}

void AliPythia8_dev::ProcInit(Process_t process, Float_t energy, Int_t strucfunc, Int_t tune)
{
  // Initialise the process to generate
  if (!AliPythiaRndm::GetPythiaRandom())
    AliPythiaRndm::SetPythiaRandom(GetRandom());

  fProcess   = process;
  fEcms      = energy;
  fStrucFunc = strucfunc;
  fItune     = tune;

  if (tune > -1) ReadString(Form("Tune:pp = %3d", tune));

  // Pythia initialisation for selected processes
  switch (process) {
  case kPyMbDefault:
    // All soft QCD processes
    ReadString("SoftQCD:inelastic = on");
    break;

  case kPyMbNonDiffr:
    // Inelastic non-diffractive collisions -> should correspond to experimental minimum-bias
    ReadString("SoftQCD:nonDiffractive = on");
    break;

  case kPyJets:
    //  QCD Jets
    ReadString("HardQCD:all = on");
    break;

  case kPyCharm:
    ReadString("HardQCD:gg2ccbar = on");
    ReadString("HardQCD:qqbar2ccbar = on");
    //  heavy quark mass
    ReadString("ParticleData:mcRun = 1.5");
    break;

  case kPyBeauty:
    ReadString("HardQCD:gg2bbbar = on");
    ReadString("HardQCD:qqbar2bbbar = on");
    //  heavy quark mass
    ReadString("ParticleData:mbRun = 4.75");
    break;

  case kPyJetsPWHG:
  case kPyCharmPWHG:
  case kPyBeautyPWHG:
    // POWHEG
    if (!fLHEFile.IsNull()) {
      ReadConfigFile("powheg_pythia8_conf.cmnd");
      ReadString(TString::Format("Beams:LHEF = %s", fLHEFile.Data()));
    }
    break;

  default:
    AliWarningStream() << "Process '" << process << "' not implemented!!" << std::endl;
    break;
  }

  // Select structure function
  if (fStrucFunc >= 0) {
    // In order to use LHAPDF PYTHIA8 must be configured and compiled with the correct settings
    // This is not the case in AliRoot
    //ReadString("PDF:useLHAPDF = on");
    //ReadString(TString::Format("PDF:LHAPDFset = %s", AliStructFuncType::PDFsetName((StrucFunc_t)fStrucFunc).Data()));
    Int_t pdf_code = PYTHIA8PDFsetName((StrucFunc_t)fStrucFunc);
    if (pdf_code == 0) {
      AliErrorStream() << "PDF '" << AliStructFuncType::PDFsetName((StrucFunc_t)fStrucFunc).Data() << "(" << fStrucFunc << ") not available for PYTHIA8" << std::endl;
    }
    else {
      ReadString(TString::Format("PDF:pSet = %d", pdf_code));
    }
  }

  //  Initialize PYTHIA
  Initialize(2212, 2212, fEcms);
}

Int_t AliPythia8_dev::PYTHIA8PDFsetName(StrucFunc_t s)
{
  switch(s) {
  case kCTEQ5L:
    return 2;
    break;
  case kCTEQ6l:
    return 7;
    break;
  default:
    return 0;
    break;
  }
}

void AliPythia8_dev::SetDecayOff(const std::set<int>& pdg_codes)
{
  for (auto pdg : pdg_codes) ReadString(Form("%d:onMode = off",pdg));
}


void AliPythia8_dev::SetSeed(UInt_t seed)
{
  // set seed in PYTHIA8
  // NB. 900000000 is the maximum seed (0 is not allowed)
  SetPythiaSeed(seed);
}

void AliPythia8_dev::SetNuclei(Int_t /*a1*/, Int_t /*a2*/, Int_t /*pdg*/)
{
  AliErrorStream() << "AliPythia8_dev::SetNuclei not implemented!" << std::endl;
}


void AliPythia8_dev::GenerateEvent()
{
  // Generate one event
  AliTPythia8::GenerateEvent();
}

void AliPythia8_dev::PrintStatistics()
{
  // End of run statistics
  AliTPythia8::PrintStatistics();
}

void AliPythia8_dev::EventListing()
{
  // End of run statistics
  AliTPythia8::EventListing();
}

Int_t AliPythia8_dev::ProcessCode()
{
  // Returns the subprocess code for the current event
  return Pythia8()->info.code();
}

void AliPythia8_dev::SetPtHardRange(Float_t ptmin, Float_t ptmax)
{
  // Set the pt hard range
  ReadString(Form("PhaseSpace:pTHatMin = %13.3f", ptmin));
  ReadString(Form("PhaseSpace:pTHatMax = %13.3f", ptmax));
}

void AliPythia8_dev::SetYHardRange(Float_t /*ymin*/, Float_t /*ymax*/)
{
  // Set the y hard range
  AliWarningStream() << "YHardRange not implemented in PYTHIA 8." << std::endl;
}

void AliPythia8_dev::SetInitialAndFinalStateRadiation(Int_t flag1, Int_t flag2)
{
  //  initial state radiation
  if (flag1) {
    ReadString("PartonLevel:ISR = on");
  } else {
    ReadString("PartonLevel:ISR = off");
  }
  // final state radiation
  if (flag2) {
    ReadString("PartonLevel:FSR = on");
  } else {
    ReadString("PartonLevel:FSR = off");
  }
}

void AliPythia8_dev::GetXandQ(Float_t& x1, Float_t& x2, Float_t& q)
{
  // Get x1, x2 and Q for this event

  q  = Pythia8()->info.QFac();
  x1 = Pythia8()->info.x1();
  x2 = Pythia8()->info.x2();
}

Float_t AliPythia8_dev::GetXSection()
{
  // Get the total cross-section
  return Pythia8()->info.sigmaGen();
}

Float_t AliPythia8_dev::GetPtHard()
{
  // Get the pT hard for this event
  return Pythia8()->info.pTHat();
}

void AliPythia8_dev::PrintParticles()
{
  // Print list of particl properties
  ReadString("Main:showAllParticleData");
}
