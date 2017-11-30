// OnTheFlySimulationGenerator

#ifndef ONTHEFLYSIMULATIONGENERATOR_H
#define ONTHEFLYSIMULATIONGENERATOR_H

#include <TString.h>
#include <PythiaProcesses.h>
#include "AliDecayer.h"

class AliGenPythia;
class AliAnalysisTaskSE;
class AliAnalysisManager;
class AliGenEvtGen;
class AliGenerator;
class AliGenCocktail;
class AliGenPythiaPlus;

class OnTheFlySimulationGenerator {
public:
  // PYTHIA6 tunes: https://arxiv.org/pdf/1005.3457v5.pdf
  enum EPythia6Tune_t {
    kPerugia0 = 320,
    kPerugio0NOCR = 324,
    kPerugia2010 = 327,
    kPerugia2011 = 350,
    kPerugia2011NOCR = 354,
    kPerugia2012 = 370,
    kPerugia2012NOCR = 375
  };

  enum EPythia8Tune_t {
    kMonash2013  = 14
  };

  enum EGenerator_t {
    kPowheg,
    kPythia6,
    kPythia8,
    kEvtGen
  };

  enum ESpecialParticle_t {
    kNoSpecialParticle = 0,
    kccbar = 1,
    kbbbar = 2
  };

  enum EBeamType_t {
    kpp = 1,
    kpPb = 2
  };

  OnTheFlySimulationGenerator();
  OnTheFlySimulationGenerator(TString taskname);
  OnTheFlySimulationGenerator(TString taskname, Int_t numevents, Process_t proc, ESpecialParticle_t specialPart, Bool_t forceHadDecay, Int_t seed, TString lhe);

  void SetName(TString taskname)                          { fName            = taskname      ; }
  void SetNumberOfEvents(Int_t numevents)                 { fEvents          = numevents     ; }
  void SetProcess(Process_t proc)                         { fProcess         = proc          ; }
  void SetSpecialParticle(ESpecialParticle_t specialPart) { fSpecialParticle = specialPart   ; }
  void SetForceHadronicDecay(Bool_t forceHadDecay)        { fForceHadDecay   = forceHadDecay ; }
  void SetSeed(Int_t seed)                                { fSeed            = seed          ; }
  void SetLHEFile(TString lhe)                            { fLHEFile         = lhe           ; }
  void SetEnergyBeam1(Float_t e)                          { fEnergyBeam1     = e             ; }
  void SetEnergyBeam2(Float_t e)                          { fEnergyBeam2     = e             ; }
  void SetPythia6Tune(EPythia6Tune_t tune)                { fPythia6Tune     = tune          ; }
  void SetPythia8Tune(EPythia8Tune_t tune)                { fPythia8Tune     = tune          ; }
  void SetPtHardRange(Double_t minPtHard, Double_t maxPtHard) { fMinPtHard   = minPtHard ; fMaxPtHard   = maxPtHard ; }
  void EnableJetQA(Bool_t b = kTRUE)                      { fJetQA           = b             ; }
  void EnableJetTree(Bool_t b = kTRUE)                    { fJetTree         = b             ; }
  void EnableDMesonJets(Bool_t b = kTRUE)                 { fDMesonJets      = b             ; }
  void SetBeamType(EBeamType_t b)                         { fBeamType        = b             ; }
  void SetRejectISR(Bool_t b)                             { fRejectISR       = b             ; }
  void SetPartonEventGenerator(EGenerator_t gen)          { fPartonEvent     = gen           ; }
  void SetHadronization(EGenerator_t gen)                 { fHadronization   = gen           ; }
  void SetDecayer(EGenerator_t gen)                       { fDecayer         = gen           ; }

  static AliGenPythia* CreatePythia6Gen(EBeamType_t beam, Float_t e_cms, EPythia6Tune_t tune, Process_t proc, ESpecialParticle_t specialPart, Double_t ptHardMin, Double_t ptHardMax, Decay_t forceDecay);
  static AliGenPythiaPlus* CreatePythia8Gen(EBeamType_t beam, Float_t e_cms, EPythia8Tune_t tune, Process_t proc, Double_t ptHardMin, Double_t ptHardMax, Decay_t forceDecay);
  static AliGenEvtGen* CreateEvtGen(Decay_t forceDecay);
  static AliGenCocktail* CreateCocktailGen(EBeamType_t beam, Float_t e_cms);

  AliGenerator* CreateGenerator();
  void PrepareAnalysisManager();
  void Start(UInt_t debug_level = 0);

  const TString&     GetName()               const { return fName           ; }
  Int_t              GetNumberOfEvents()     const { return fEvents         ; }
  Process_t          GetProcess()            const { return fProcess        ; }
  ESpecialParticle_t GetSpecialParticle()    const { return fSpecialParticle; }
  Bool_t             GetForceHadronicDecay() const { return fForceHadDecay  ; }
  Int_t              GetSeed()               const { return fSeed           ; }
  const TString&     GetLHEFile()            const { return fLHEFile        ; }
  Float_t            GetCMSEnergy()                { if (fCMSEnergy < 0) CalculateCMSEnergy(); return fCMSEnergy; }
  Float_t            GetEnergyBeam1()        const { return fEnergyBeam1    ; }
  Float_t            GetEnergyBeam2()        const { return fEnergyBeam2    ; }
  EPythia6Tune_t     GetPythia6Tune()        const { return fPythia6Tune    ; }
  Double_t           GetPtHardMin()          const { return fMinPtHard      ; }
  Double_t           GetPtHardMax()          const { return fMaxPtHard      ; }

protected:
  void               AddJetQA();
  void               AddJetTree();
  void               AddDJet();

  void               CalculateCMSEnergy();

  TString              fName             ;
  AliAnalysisManager*  fAnalysisManager  ;
  Int_t                fEvents           ;
  Process_t            fProcess          ;
  ESpecialParticle_t   fSpecialParticle  ;
  Bool_t               fForceHadDecay    ;
  Int_t                fSeed             ;
  TString              fLHEFile          ;
  Float_t              fCMSEnergy        ; // in TeV
  EPythia6Tune_t       fPythia6Tune      ;
  EPythia8Tune_t       fPythia8Tune      ;
  Double_t             fMinPtHard        ;
  Double_t             fMaxPtHard        ;
  EBeamType_t          fBeamType         ;
  Bool_t               fJetQA            ;
  Bool_t               fJetTree          ;
  Bool_t               fDMesonJets       ;
  Float_t              fEnergyBeam1      ; // in GeV
  Float_t              fEnergyBeam2      ; // in GeV
  Bool_t               fRejectISR        ;
  EGenerator_t         fPartonEvent      ;
  EGenerator_t         fHadronization    ;
  EGenerator_t         fDecayer          ;

private:
  OnTheFlySimulationGenerator(const OnTheFlySimulationGenerator&);//not implemented
  OnTheFlySimulationGenerator& operator=(const OnTheFlySimulationGenerator&); //not implemented
};

#endif
