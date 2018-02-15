/**
 * \class AliGenPythiaDev
 * \brief Generator using the TPythia interface (via AliPythia) to generate pp collisions.
 * Using SetNuclei() also nuclear modifications to the structure functions
 * can be taken into account. This makes, of course, only sense for the
 * generation of the products of hard processes (heavy flavor, jets ...)
 *
 * \author Andreas Morsch <andreas.morsch@cern.ch>, CERN
 * \author Salvatore Aiola <salvatore.aiola@cern.ch>, Yale University
 * \date Feb 15, 2018
 */

#ifndef ALIGENPYTHIADEV_H
#define ALIGENPYTHIADEV_H

/**************************************************************************
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

#include <vector>

#include "AliGenMC.h"
#include "AliPythia.h"

class AliGenPythiaEventHeader;

class AliGenPythiaDev : public AliGenMC
{
public:
  enum EPythiaTune_t {
    // PYTHIA6 tunes: https://arxiv.org/pdf/1005.3457v5.pdf
    kPerugia0 = 320,
    kPerugio0NOCR = 324,
    kPerugia2010 = 327,
    kPerugia2011 = 350,
    kPerugia2011NOCR = 354,
    kPerugia2012 = 370,
    kPerugia2012NOCR = 375,
    // PYTHIA 8 tunes:
    kMonash2013  = 14
  };
  
  
  AliGenPythiaDev();
  virtual ~AliGenPythiaDev();

  // Interface of the generator (Used by AliMCGenHandler)
  virtual void    Generate();
  virtual void    Init();
  virtual void    SetSeed(UInt_t seed)                                    { GetRandom()->SetSeed(seed); }


  virtual void    SetProcess(Process_t proc)                             { fProcess      = proc    ; }
  virtual void    SetTune(EPythiaTune_t itune)                           { fItune        = itune   ; }
  virtual void    SetPDF(StrucFunc_t func)                               { fStrucFunc    = func    ; }
  
  
  // Rewieght pt, hard spectrum with pT/p0^n, set power n
  virtual void    SetWeightPower(Float_t power)                          { fWeightPower  = power   ; }
  virtual void    SetPtHard(Float_t ptmin, Float_t ptmax)                { fPtHardMin    = ptmin   ; fPtHardMax = ptmax; }
  virtual void    SetYHard(Float_t ymin, Float_t ymax)                   { fYHardMin     = ymin    ; fYHardMax  = ymax ; }

  // Set initial and final state gluon radiation
  virtual void    SetGluonRadiation(Int_t iIn, Int_t iFin)               { fGinit        = iIn     ; fGfinal = iFin; }
  virtual void    SetColorReconnectionOff(Int_t iflag)                   { fCRoff        = iflag   ; }
  virtual void    UseNewMultipleInteractionsScenario(Bool_t b)           { fNewMIS       = b       ; }

  // Treat protons as inside nuclei with mass numbers a1 and a2
  virtual void    SetNuclei(Int_t a1, Int_t a2, Int_t pdfset);
  virtual void    SetCollisionSystem(TString projectile, TString target) { fProjectile      = projectile; fTarget = target; }
  virtual void    SetNuclearPDF(Int_t pdf)                               { fNucPdf          = pdf       ; }
  virtual void    SetUseNuclearPDF(Bool_t val)                           { fUseNuclearPDF   = val       ; }
  virtual void    SetUseLorentzBoost(Bool_t val)                         { fUseLorentzBoost = val       ; }


  // Range of events to be printed
  virtual void    SetEventListRange(Int_t eventFirst=-1, Int_t eventLast=-1);

  virtual void FinishRun();
  void         WriteXsection(const Char_t *fname = "pyxsec.root");

protected:
  void     MakeHeader();

  Process_t                fProcess          ; ///<  Process type
  EPythiaTune_t            fItune            ; ///<  Pythia tune > 6.4
  StrucFunc_t              fStrucFunc        ; ///<  Structure Function
  Float_t                  fWeightPower      ; ///<  Power for cross section weights; 0 means no reweighting
  Float_t                  fPtHardMin        ; ///<  Lower pT-hard cut
  Float_t                  fPtHardMax        ; ///<  Higher pT-hard cut
  Float_t                  fYHardMin         ; ///<  Lower  y-hard cut
  Float_t                  fYHardMax         ; ///<  Higher y-hard cut
  Int_t                    fGinit            ; ///<  Initial state gluon radiation
  Int_t                    fGfinal           ; ///<  Final state gluon radiation
  Int_t                    fCRoff            ; ///<  Color reconnection off in the pythia6 annealying model
  Bool_t                   fSetNuclei        ; ///<  Flag indicating that SetNuclei has been called
  Bool_t                   fUseNuclearPDF    ; ///<  Flag if nuclear pdf should be applied
  Bool_t                   fUseLorentzBoost  ; ///<  Flag if lorentz boost should be applied
  Bool_t                   fNewMIS           ; ///<  Flag for the new multipple interactions scenario
  Int_t                    fNucPdf           ; ///<  Nuclear PDF
  TString                  fLHEFile          ; ///<  LEH file
  Int_t                    fNpartons         ; //!<! Number of partons before hadronisation
  Int_t                    fNev              ; //!<! Number of events
  Float_t                  fXsection         ; //!<! Cross-section
  AliPythia               *fPythia           ; //!<! Pythia
  Int_t                    fTrials           ; //!<! Number of trials for current event
  Int_t                    fTrialsRun        ; //!<! Number of trials for run
  Float_t                  fQ                ; //!<! Mean Q
  Float_t                  fX1               ; //!<! Mean x1
  Float_t                  fX2               ; //!<! Mean x2
  AliDecayer              *fDecayer          ; //!<! Pointer to the decayer instance
  Int_t                    fDebugEventFirst  ; //!<! First event to debug
  Int_t                    fDebugEventLast   ; //!<! Last  event to debug
  AliGenPythiaEventHeader *fHeader           ; //!<! Event header
  std::vector<int>         fNParent          ; //!<!
  std::vector<int>         fNSelected        ; //!<!
  std::vector<int>         fNTrackIt         ; //!<!

private:
  AliGenPythiaDev(const AliGenPythiaDev &Pythia);
  AliGenPythiaDev & operator=(const AliGenPythiaDev & rhs);

  /// \cond CLASSIMP
  ClassDef(AliGenPythiaDev, 1);
  /// \endcond
};
#endif
