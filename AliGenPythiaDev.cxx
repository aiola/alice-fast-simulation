/*************************************************************************
 * Copyright(c) 1998-2016, ALICE Experiment at CERN, All rights reserved. *
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

#include <TParticle.h>
#include <TTree.h>
#include <TFile.h>

#include <AliPythiaRndm.h>
#include <AliDecayerPythia.h>
#include <AliLog.h>
#include <AliPythia.h>
#include <AliRun.h>
#include <AliGenPythiaEventHeader.h>

#include "AliGenPythiaDev.h"

/// \cond CLASSIMP
ClassImp(AliGenPythiaDev);
/// \endcond


/**
 *  Default Constructor
 */
AliGenPythiaDev::AliGenPythiaDev():
  AliGenMC(),
  fProcess(kPyCharm),
  fItune(-1),
  fStrucFunc(kCTEQ5L),
  fWeightPower(0.),
  fPtHardMin(0.),
  fPtHardMax(1.e4),
  fYHardMin(-1.e10),
  fYHardMax(1.e10),
  fGinit(1),
  fGfinal(1),
  fCRoff(0),
  fSetNuclei(kFALSE),
  fUseNuclearPDF(kFALSE),
  fUseLorentzBoost(kTRUE),
  fNewMIS(kFALSE),
  fNucPdf(0),
  fLHEFile(),
  fNpartons(0),
  fNev(0),
  fXsection(0.),
  fPythia(0),
  fTrials(0),
  fTrialsRun(0),
  fQ(0.),
  fX1(0.),
  fX2(0.),
  fDecayer(new AliDecayerPythia()),
  fDebugEventFirst(-1),
  fDebugEventLast(-1),
  fHeader(0),
  fNParent(),
  fNSelected(),
  fNTrackIt()
{
  if (!AliPythiaRndm::GetPythiaRandom()) AliPythiaRndm::SetPythiaRandom(GetRandom());
}

/**
 * Set a range of event numbers, for which a table
 * of generated particle will be printed
 * @param eventFirst First event number to be printed
 * @param eventLast  Last event number to be printed
 */
void AliGenPythiaDev::SetEventListRange(Int_t eventFirst, Int_t eventLast)
{
  fDebugEventFirst = eventFirst;
  fDebugEventLast  = eventLast;
  if (fDebugEventLast==-1) fDebugEventLast=fDebugEventFirst;
}

/**
 * Initialization
 */
void AliGenPythiaDev::Init()
{
  SetMC(AliPythia::Instance());
  fPythia = dynamic_cast<AliPythia*>(fMCEvGen);

  if (!fPythia) {
    AliErrorStream() << "No PYTHIA generator found! Initialization not done!" << std::endl;
    return;
  }

  if (fWeightPower != 0) fPythia->SetWeightPower(fWeightPower);
  fPythia->SetCKIN(3,fPtHardMin);
  fPythia->SetCKIN(4,fPtHardMax);
  fPythia->SetCKIN(7,fYHardMin);
  fPythia->SetCKIN(8,fYHardMax);

  // Nuclei
  if (fProjectile != "p" || fTarget != "p") fPythia->SetCollisionSystem(fProjectile, fTarget);
  if (fUseNuclearPDF) fPythia->SetNuclei(fAProjectile, fATarget, fNucPdf);

  //  initial state radiation
  fPythia->SetMSTP(61, fGinit);

  //  final state radiation
  fPythia->SetMSTP(71, fGfinal);

  //color reconnection strength
  if (fCRoff == 1) fPythia->SetMSTP(95, 0);
  fPythia->SetMSTP(91,0);

  if (!fLHEFile.IsNull()) {
    char* fname = new char[fLHEFile.Length() + 1];
    strcpy(fname, fLHEFile.Data());
    if (!fLHEFile.IsNull()) fPythia->OpenFortranFile(97, fname);
    delete[] fname;
  }

  //
  fPythia->ProcInit(fProcess, fEnergyCMS, fStrucFunc, fItune);
  //  Forward Paramters to the AliPythia object
  fDecayer->SetForceDecay(fForceDecay);

  fDecayer->Init();

  //  This counts the total number of calls to Pyevnt() per run.
  fTrialsRun = 0;
  fQ         = 0.;
  fX1        = 0.;
  fX2        = 0.;
  fNev       = 0 ;

  AliGenMC::Init();

  // Reset Lorentz boost if requested
  if(!fUseLorentzBoost) {
    fDyBoost = 0;
    AliWarningStream() << "Demand to discard Lorentz boost." << std::endl;
  }

  if (fSetNuclei) {
    fDyBoost = 0;
    AliWarningStream() << "Deprecated function SetNuclei() used (nPDFs + no boost). Use SetProjectile + SetTarget + SetUseNuclearPDF + SetUseLorentzBoost instead." << std::endl;
  }
  fPythia->SetPARJ(200, 0.0);
  fPythia->SetPARJ(199, 0.0);
  fPythia->SetPARJ(198, 0.0);
  fPythia->SetPARJ(197, 0.0);

  if (AliLog::GetDebugLevel("","AliGenPythiaDev") >= 1 ) {
    fPythia->Pystat(4);
    fPythia->Pystat(5);
  }
}

/**
 * Generate one event
 */
void AliGenPythiaDev::Generate()
{
  if (!fPythia) {
    AliErrorStream() << "No PYTHIA generator found! No event generated!" << std::endl;
    return;
  }

  fDecayer->ForceDecay();

  Double_t polar[3]   = {0};
  Double_t origin[3]  = {0};
  Double_t p[4]       = {0};

  //  converts from mm/c to s
  const Float_t kconv = 0.001 / TMath::C();
  //
  Int_t nt = 0;
  Int_t jev = 0;
  Int_t j = 0, kf = 0;
  fTrials = 0;

  //  Set collision vertex position
  if (fVertexSmear == kPerEvent) Vertex();

  //  event loop
  do
  {
    // Produce event

    // Switch hadronisation off
    fPythia->SetMSTJ(1, 0);

    // Produce new event
    if (!fNewMIS) {
      fPythia->Pyevnt();
    }
    else {
      fPythia->Pyevnw();
    }
    fNpartons = fPythia->GetN();

    fPythia->SetMSTJ(1, 1);
    // perform hadronisation
    AliInfoStream() << "Calling hadronisation " << fPythia->GetN() << std::endl;

    fPythia->Pyexec();

    fTrials++;
    fPythia->ImportParticles(&fParticles,"All");

    if (TMath::Abs(fDyBoost) > 1.e-4) Boost();
    if (TMath::Abs(fXingAngleY) > 1.e-10) BeamCrossAngle();

    fNprimaries = 0;
    Int_t np = fParticles.GetEntriesFast();

    if (np == 0) continue;

    fNParent.clear();
    fNParent.resize(np, 0);
    fNSelected.clear();
    fNSelected.resize(np, 0);
    fNTrackIt.clear();
    fNTrackIt.resize(np, 0);

    Int_t nc = 0;        // Total n. of selected particles
    Int_t nTkbles = 0;   // Trackable particles

    for (int i = 0; i < np; i++) {
      TParticle* iparticle = static_cast<TParticle*>(fParticles.At(i));
      Int_t ks = iparticle->GetStatusCode();
      kf = CheckPDGCode(iparticle->GetPdgCode());

      // No initial state partons
      if (ks == 21) continue;

      nc++;
      // Decision on tracking
      fNTrackIt[i] = 0;

      // Track final state particle
      if (ks == 1) fNTrackIt[i] = 1;

      // Track particles selected by process if undecayed.
      if (fForceDecay == kNoDecay) {
        if (ParentSelected(kf)) fNTrackIt[i] = 1;
      } else {
        if (ParentSelected(kf)) fNTrackIt[i] = 0;
      }
      if (fNTrackIt[i] == 1) ++nTkbles; // Update trackable counter
    } // particle selection loop

    if (nc == 0)  continue;

    for (int i = 0; i<np; i++) {
      TParticle *  iparticle = static_cast<TParticle*>(fParticles.At(i));
      kf = CheckPDGCode(iparticle->GetPdgCode());
      Int_t ks = iparticle->GetStatusCode();
      p[0] = iparticle->Px();
      p[1] = iparticle->Py();
      p[2] = iparticle->Pz();
      p[3] = iparticle->Energy();

      origin[0] = fVertex[0] + iparticle->Vx() / 10; // [cm]
      origin[1] = fVertex[1] + iparticle->Vy() / 10; // [cm]
      origin[2] = fVertex[2] + iparticle->Vz() / 10; // [cm]

      Float_t tof     = fTime + kconv * iparticle->T();
      Int_t   ipa     = iparticle->GetFirstMother()-1;
      Int_t   iparent = (ipa > -1) ? fNParent[ipa] : -1;

      PushTrack(fTrackIt * fNTrackIt[i], iparent, kf,
          p[0], p[1], p[2], p[3],
          origin[0], origin[1], origin[2], tof,
          polar[0], polar[1], polar[2],
          kPPrimary, nt, 1., ks);
      fNParent[i] = nt;
      KeepTrack(nt);
      fNprimaries++;
    } //  PushTrack loop

    fQ  += fPythia->GetVINT(51);
    fX1 += fPythia->GetVINT(41);
    fX2 += fPythia->GetVINT(42);
    fTrialsRun += fTrials;
    fNev++;
    MakeHeader();

    break;
  } while (true); // event loop

  SetHighWaterMark(nt);

  //  get cross-section
  fXsection = fPythia->GetPARI(1);
}

/**
 * Print x-section summary
 */
void AliGenPythiaDev::FinishRun()
{
  fPythia->Pystat(1);

  if (fNev > 0.) {
    fQ  /= fNev;
    fX1 /= fNev;
    fX2 /= fNev;
  }

  AliInfoStream() << "Total number of Pyevnt() calls " << fTrialsRun << std::endl;
  AliInfoStream() << "Mean Q, x1, x2: " << fQ << fX1 << fX2 << std::endl;

  WriteXsection();
}

/**
 * Treat protons as inside nuclei with mass numbers a1 and a2
 */
void AliGenPythiaDev::SetNuclei(Int_t a1, Int_t a2, Int_t pdfset)
{
  fAProjectile   = a1;
  fATarget       = a2;
  fNucPdf        = pdfset;  // 0 EKS98 9 EPS09LO 19 EPS09NLO
  fUseNuclearPDF = kTRUE;
  fSetNuclei     = kTRUE;
}

/**
 * Make header for the simulated event
 */
void AliGenPythiaDev::MakeHeader()
{
  if (gAlice) {
    if (gAlice->GetEvNumber()>=fDebugEventFirst && gAlice->GetEvNumber()<=fDebugEventLast) fPythia->Pylist(2);
  }

  // Builds the event header, to be called after each event
  if (fHeader) delete fHeader;
  AliGenPythiaEventHeader* pythiaHeader = new AliGenPythiaEventHeader("Pythia");
  fHeader = pythiaHeader;
  fHeader->SetTitle(GetTitle());

  pythiaHeader->SetProcessType(fPythia->GetMSTI(1));

  // Number of trials
  pythiaHeader->SetTrials(fTrials);

  // Event Vertex
  fHeader->SetPrimaryVertex(fVertex);
  fHeader->SetInteractionTime(fTime);

  // Number of primaries
  fHeader->SetNProduced(fNprimaries);

  // Event weight
  fHeader->SetEventWeight(fPythia->GetVINT(97));

  // Number of MPI
  fHeader->SetNMPI(fPythia->GetNMPI());

  // Store pt^hard and cross-section
  pythiaHeader->SetPtHard(fPythia->GetVINT(47));
  pythiaHeader->SetXsection(fPythia->GetPARI(1));

  // Store Event Weight
  pythiaHeader->SetEventWeight(fPythia->GetPARI(7) * fPythia->GetPARI(10));
  // PARI(7) is 1 or -1, for weighted generation with accept/reject, e.g. POWHEG
  // PARI(10) is a weight associated with reweighted generation, using Pyevwt

  //  Pass header
  AddHeader(fHeader);
  fHeader = nullptr;
}

/**
 *  Write cross section and Ntrials to a tree in a file
 *  Used for pt-hard bin productions
 */
void AliGenPythiaDev::WriteXsection(const Char_t *fname)
{
  TFile fout(fname,"recreate");
  TTree tree("Xsection","Pythia cross section");
  // Convert to expected types for backwards compatibility
  Double_t xsec = fXsection;
  UInt_t trials = fTrialsRun;
  tree.Branch("xsection", &xsec, "X/D");
  tree.Branch("ntrials" , &trials , "X/i");
  tree.Fill();
  tree.Write();
  fout.Close();
}
