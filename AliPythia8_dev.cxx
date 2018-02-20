
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

#include "AliLog.h"
#include "AliStack.h"
#include "AliPythiaRndm.h"
#include "Pythia8/Analysis.h"

#include "AliPythia8_dev.h"

ClassImp(AliPythia8_dev)

// Particles produced in string fragmentation point directly to either of the two endpoints
// of the string (depending in the side they were generated from).
//    SetMSTU(16,2); // ????
//  String drawing almost completely minimizes string length.
//  Probability that an additional interaction gives two gluons
//  ... with color connection to nearest neighbours
//    SetPARP(85,0.9);
//  ... as closed gluon loop
//    SetPARP(86,0.95);
// Lambda_FSR scale.
//	SetPARJ(81, 0.29);
// Baryon production model
//	SetMSTJ(12,3); 
// String fragmentation
//	SetMSTJ(1,1);  
// sea quarks can be used for baryon formation
//      SetMSTP(88,2); 
// choice of max. virtuality for ISR
//	SetMSTP(68,1);
// regularisation scheme of ISR 
//	SetMSTP(70,2);
// all resonance decays switched on
//    SetMSTP(41,1);   

AliPythia8_dev::AliPythia8_dev():
  AliTPythia8(),
  AliPythiaBase_dev(),
  fProcess(kPyMb),
  fEcms(0.),
  fStrucFunc(kCTEQ5L),
  fLHEFile()
{
  // Default Constructor
  //
  //  Set random number
  if (!AliPythiaRndm::GetPythiaRandom()) AliPythiaRndm::SetPythiaRandom(GetRandom());
}

void AliPythia8_dev::ProcInit(Process_t process, Float_t energy, StrucFunc_t strucfunc, Int_t tune)
{
  // Initialise the process to generate
  if (!AliPythiaRndm::GetPythiaRandom())
    AliPythiaRndm::SetPythiaRandom(GetRandom());

  fProcess   = process;
  fEcms      = energy;
  fStrucFunc = strucfunc;
  ReadString("111:mayDecay  = on");

  // Select structure function
  //          ReadString("PDF:useLHAPDF = on");
  //	  ReadString(Form("PDF:LHAPDFset = %s", AliStructFuncType::PDFsetName(fStrucFunc).Data()));
  // Particles produced in string fragmentation point directly to either of the two endpoints
  // of the string (depending in the side they were generated from).

  //    SetMSTU(16,2); // ????

  //
  // Pythia initialisation for selected processes//
  //
  switch (process)
  {
  case kPyCharm:
    ReadString("HardQCD:gg2ccbar = on");
    ReadString("HardQCD:qqbar2ccbar = on");
    //  heavy quark masses
    ReadString("ParticleData:mcRun = 1.2");
    //
    //    primordial pT
    ReadString("BeamRemnants:primordialKT = on");
    ReadString("BeamRemnants:primordialKTsoft = 0.");
    ReadString("BeamRemnants:primordialKThard = 1.");
    ReadString("BeamRemnants:halfScaleForKT = 0.");
    ReadString("BeamRemnants:halfMassForKT = 0.");
    break;

  case kPyBeauty:
    ReadString("HardQCD:gg2bbbar = on");
    ReadString("HardQCD:qqbar2bbbar = on");
    ReadString("ParticleData:mbRun = 4.75");
    break;

  case kPyCharmUnforced:
  case kPyBeautyUnforced:
    // gq->qg
    ReadString("HardQCD:gq2qg = on");
    // gg->qq
    ReadString("HardQCD:gg2qq = on");
    // gg->gg
    ReadString("HardQCD:gg2gg = on");
    break;

  case kPyMbDefault:
    // Minimum Bias pp-Collisions
    // select Pythia min. bias model
    ReadString("SoftQCD:inelastic = on");
    if (tune > -1) ReadString(Form("Tune:pp = %3d", tune));
    break;

  case kPyJets:
    //  QCD Jets
    ReadString("HardQCD:all = on");
    break;

  default:
    AliWarningStream() << "Process '" << process << "' not implemented!!" << std::endl;
    break;
  }

  //  Initialize PYTHIA
  Initialize(2212, 2212, fEcms);
}

void AliPythia8_dev::SetSeed(UInt_t seed)
{
  //
  // set seed in PYTHIA8
  // NB. 900000000 is the maximum seed (0 is not allowed)
  //

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
