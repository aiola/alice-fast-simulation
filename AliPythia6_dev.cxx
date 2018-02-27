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

#include <iostream>

#include <AliPythiaRndm.h>
#include <AliLog.h>

#include "AliPythia6_dev.h"

ClassImp(AliPythia6_dev)

#ifndef WIN32
# define pyclus pyclus_
# define pycell pycell_
# define pyshow pyshow_
# define pyrobo pyrobo_
# define pyquen pyquen_
# define pyevnw pyevnw_
# define pyshowq pyshowq_
# define qpygin0 qpygin0_
# define pytune  pytune_
# define py2ent  py2ent_
# define setpowwght setpowwght_
# define type_of_call
#else
# define pyclus PYCLUS
# define pycell PYCELL
# define pyrobo PYROBO
# define pyquen PYQUEN
# define pyevnw PYEVNW
# define pyshowq PYSHOWQ
# define qpygin0 QPYGIN0
# define pytune  PYTUNE
# define py2ent  PY2ENT
# define setpowwght SETPOWWGHT
# define type_of_call _stdcall
#endif

extern "C" void type_of_call pyclus(Int_t & );
extern "C" void type_of_call pycell(Int_t & );
extern "C" void type_of_call pyshow(Int_t &, Int_t &, Double_t &);
extern "C" void type_of_call pyrobo(Int_t &, Int_t &, Double_t &, Double_t &, Double_t &, Double_t &, Double_t &);
extern "C" void type_of_call pyquen(Double_t &, Int_t &, Double_t &);
extern "C" void type_of_call pyevnw();
extern "C" void type_of_call pyshowq(Int_t &, Int_t &, Double_t &);
extern "C" void type_of_call pytune(Int_t &);
extern "C" void type_of_call py2ent(Int_t &, Int_t&, Int_t&, Double_t&);
extern "C" void type_of_call qpygin0();
extern "C" void type_of_call setpowwght(Double_t &);

AliPythia6_dev::AliPythia6_dev():
  fProcess(kPyMbDefault),
  fItune(-1),
  fEcms(0.),
  fStrucFunc(kCTEQ5L),
  fLHEFile(),
  fNewMIS(kTRUE)
{
  // Default Constructor
  //
  //  Set random number
  if (!AliPythiaRndm::GetPythiaRandom()) AliPythiaRndm::SetPythiaRandom(GetRandom());
}

void AliPythia6_dev::SetPtHardRange(Float_t ptmin, Float_t ptmax)
{
  SetCKIN(3, ptmin);
  SetCKIN(4, ptmax);

}
void AliPythia6_dev::SetYHardRange(Float_t ymin, Float_t ymax)
{
  SetCKIN(7, ymin);
  SetCKIN(8, ymax);
}

void AliPythia6_dev::SetInitialAndFinalStateRadiation(Int_t flag1, Int_t flag2)
{
  //  initial state radiation
  SetMSTP(61, flag1);

  //  final state radiation
  SetMSTP(71, flag2);
}

void AliPythia6_dev::ProcInit(Process_t process, Float_t energy, StrucFunc_t strucfunc, Int_t itune)
{
  // Initialise the process to generate
  if (!AliPythiaRndm::GetPythiaRandom()) AliPythiaRndm::SetPythiaRandom(GetRandom());

  fProcess = process;
  fEcms = energy;
  fStrucFunc = strucfunc;
  fItune = itune;

  //  Select the tune
  if (itune > -1) {
    Pytune(itune);
    if (GetMSTP(192) > 1 || GetMSTP(193) > 1) {
      AliWarningStream() << "Structure function for tune " << itune << " set to " << AliStructFuncType::PDFsetName(strucfunc).Data() << std::endl;
      SetMSTP(52,2);
      SetMSTP(51, AliStructFuncType::PDFsetIndex(strucfunc));
    }
  }


  //...Switch off decay of pi0, K0S, Lambda, Sigma+-, Xi0-, Omega-.
  SetMDCY(Pycomp(111) ,1,0); // pi0
  SetMDCY(Pycomp(310) ,1,0); // K0S
  SetMDCY(Pycomp(3122),1,0); // kLambda
  SetMDCY(Pycomp(3112),1,0); // sigma -
  SetMDCY(Pycomp(3222),1,0); // sigma +
  SetMDCY(Pycomp(3312),1,0); // xi -
  SetMDCY(Pycomp(3322),1,0); // xi 0
  SetMDCY(Pycomp(3334),1,0); // omega-
  // Select structure function
  SetMSTP(52,2);
  SetMSTP(51, AliStructFuncType::PDFsetIndex(strucfunc));
  // Particles produced in string fragmentation point directly to either of the two endpoints
  // of the string (depending in the side they were generated from).
  SetMSTU(16,2);

  //
  // Pythia initialisation for selected processes//
  //
  // Make MSEL clean
  //
  for (Int_t i=1; i<= 200; i++) {
    SetMSUB(i,0);
  }
  //  select charm production
  switch (process) {
  case kPyMbDefault:
    // All soft QCD processes
    SetMSEL(0);
    SetMSUB(92, 1);             // single diffraction AB-->XB
    SetMSUB(93, 1);             // single diffraction AB-->AX
    SetMSUB(94, 1);             // double diffraction
    SetMSUB(95, 1);             // low pt production
    break;

  case kPyMbNonDiffr:
    // Inelastic non-diffractive collisions -> should correspond to experimental minimum-bias
    SetMSEL(0);
    SetMSUB(95, 1); // low pt production
    break;

  case kPyJets:
    //  QCD Jets
    SetMSEL(1);
    break;

  case kPyCharm:
    SetMSEL(4);
    // heavy quark mass
    SetPMAS(4, 1, 1.5);
    break;

  case kPyBeauty:
    SetMSEL(5);
    // heavy quark mass
    SetPMAS(5, 1, 4.75);
    break;

  case kPyJetsPWHG:
    //    N.B.
    //    ====
    //    For the case of jet production the following parameter setting
    //    limits the transverse momentum of secondary scatterings, due
    //    to multiple parton interactions, to be less than that of the
    //    primary interaction (see POWHEG Dijet paper arXiv:1012.3380
    //    [hep-ph] sec. 4.1 and also the PYTHIA Manual).
    SetMSTP(86,1);

    //    maximum number of errors before pythia aborts (def=10)
    SetMSTU(22,10);
    //    number of warnings printed on the shell
    SetMSTU(26,20);
    break;

  case kPyCharmPWHG:
  case kPyBeautyPWHG:
    //    number of warnings printed on the shell
    SetMSTU(26,20);
    break;

  default:
    AliWarningStream() << "Process '" << process << "' not implemented!!" << std::endl;
    break;
  }

  //  Initialize PYTHIA
  if (AliLog::GetDebugLevel("","AliPythia6_dev") >= 1 ) {
    Pystat(4);
    Pystat(5);
  }

  // all resonance decays switched on
  SetMSTP(41,1);

  if (!fLHEFile.IsNull() && (process == kPyJetsPWHG || process == kPyCharmPWHG || process == kPyBeautyPWHG || process == kPyWPWHG)) {
    char* fname = new char[fLHEFile.Length() + 1];
    strcpy(fname, fLHEFile.Data());
    if (!fLHEFile.IsNull()) OpenFortranFile(97, fname);
    delete[] fname;
    Initialize("USER","","",0.);
  }
  else {
    Initialize("CMS", "p", "p", fEcms);
  }
}

Int_t AliPythia6_dev::CheckedLuComp(Int_t kf)
{
  // Check Lund particle code (for debugging)
  Int_t kc = Pycomp(kf);
  AliInfoStream() << "Lucomp kf = " << kf << ", kc = " << kc << std::endl;
  return kc;
}

void AliPythia6_dev::SetNuclei(Int_t a1, Int_t a2, Int_t pdf)
{
  // Treat protons as inside nuclei with mass numbers a1 and a2
  //    The MSTP array in the PYPARS common block is used to enable and
  //    select the nuclear structure functions.
  //    MSTP(52)  : (D=1) choice of proton and nuclear structure-function library
  //            =1: internal PYTHIA acording to MSTP(51)
  //            =2: PDFLIB proton  s.f., with MSTP(51)  = 1000xNGROUP+NSET
  //    If the following mass number both not equal zero, nuclear corrections of the stf are used.
  //    MSTP(192) : Mass number of nucleus side 1
  //    MSTP(193) : Mass number of nucleus side 2
  //    MSTP(194) : Nuclear structure function: 0: EKS98 8:EPS08 9:EPS09LO 19:EPS09NLO
  SetMSTP(52,2);
  SetMSTP(192, a1);
  SetMSTP(193, a2);
  SetMSTP(194, pdf);
}

void AliPythia6_dev::EventListing()
{
  return Pylist(2);
}

void AliPythia6_dev::PrintParticles()
{ 
  // Print list of particl properties
  Int_t np = 0;
  char*   name = new char[16];
  for (Int_t kf=0; kf<1000000; kf++) {
    for (Int_t c = 1;  c > -2; c-=2) {
      Int_t kc = Pycomp(c*kf);
      if (kc) {
        Float_t mass  = GetPMAS(kc,1);
        Float_t width = GetPMAS(kc,2);
        Float_t tau   = GetPMAS(kc,4);

        Pyname(kf,name);

        np++;

        AliInfoStream() << c*kf << " " << name << ", mass " << mass << ", width " << width << ", tau " << tau << std::endl;
      }
    }
  }
  AliInfoStream() << "Number of particles " << np << std::endl;
}

void AliPythia6_dev::Pyshow(Int_t ip1, Int_t ip2, Double_t qmax)
{
  //  Call Pythia jet reconstruction algorithm
  //
  pyshow(ip1, ip2, qmax);
}

Float_t AliPythia6_dev::GetXSection()
{
  return GetPARI(1);
}

Int_t AliPythia6_dev::ProcessCode()
{
  return GetMSTI(1);
}

void  AliPythia6_dev::PrintStatistics()
{
  Pystat(1);
}

Float_t AliPythia6_dev::GetEventWeight()
{
  // Not sure what this is
  //return GetVINT(97);

  return GetPARI(7) * GetPARI(10);
  // PARI(7) is 1 or -1, for weighted generation with accept/reject, e.g. POWHEG
  // PARI(10) is a weight associated with reweighted generation, using Pyevwt
}

void AliPythia6_dev::SetWeightPower(Double_t pow)
{
  setpowwght(pow);
  SetMSTP(142, 1); // Tell Pythia to use pyevwt to calculate event wghts
  if (GetCKIN(3) <= 0) AliWarningStream()  << "Need to set minimum p_T,hard to nonzero value for weighted event generation" << std::endl;
}

Float_t AliPythia6_dev::GetPtHard()
{
  return GetVINT(47);
}

void AliPythia6_dev::Pyevnw()
{
  // New multiple interaction scenario
  pyevnw();
}

Int_t AliPythia6_dev::GetNMPI()
{
  // returns the number of parton-parton interactions
  return (GetMSTI(31));
}
