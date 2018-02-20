// OnTheFlySimulationGenerator

#ifndef ONTHEFLYSIMULATIONGENERATOR_H
#define ONTHEFLYSIMULATIONGENERATOR_H

#define EVTGEN_DEV 1

#if EVTGEN_DEV == 1
#include "AliGenEvtGen_dev.h"
#else
#include <AliGenEvtGen.h>
#endif

#include "AliGenPythia_dev.h"

#include <TString.h>
#include <PythiaProcesses.h>
#include <AliDecayer.h>

class AliAnalysisTaskSE;
class AliAnalysisManager;
class AliGenerator;
class AliGenCocktail;

class OnTheFlySimulationGenerator {
public:
  typedef AliGenPythia_dev::EPythiaTune_t EPythiaTune_t;

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
  void SetPythia6Tune(EPythiaTune_t tune)                 { fPythiaTune      = tune          ; }
  void SetPtHardRange(Double_t minPtHard, Double_t maxPtHard) { fMinPtHard   = minPtHard ; fMaxPtHard   = maxPtHard ; }
  void EnableJetQA(Bool_t b = kTRUE)                      { fJetQA           = b             ; }
  void EnableJetTree(Bool_t b = kTRUE)                    { fJetTree         = b             ; }
  void EnableDMesonJets(Bool_t b = kTRUE)                 { fDMesonJets      = b             ; }
  void SetBeamType(EBeamType_t b)                         { fBeamType        = b             ; }
  void SetRejectISR(Bool_t b)                             { fRejectISR       = b             ; }
  void SetPartonEventGenerator(EGenerator_t gen)          { fPartonEvent     = gen           ; }
  void SetHadronization(EGenerator_t gen)                 { fHadronization   = gen           ; }
  void SetDecayer(EGenerator_t gen)                       { fDecayer         = gen           ; }

  static AliGenPythia_dev* CreatePythia6Gen(EBeamType_t beam, Float_t e_cms, EGenerator_t partonEvent, TString lhe, EPythiaTune_t tune, Process_t proc, ESpecialParticle_t specialPart, Double_t ptHardMin, Double_t ptHardMax, Decay_t forceDecay);
  static AliGenPythia_dev* CreatePythia8Gen(EBeamType_t beam, Float_t e_cms, EGenerator_t partonEvent, TString lhe, EPythiaTune_t tune, Process_t proc, Double_t ptHardMin, Double_t ptHardMax, Decay_t forceDecay);

#if EVTGEN_DEV == 1
  static AliGenEvtGen_dev* CreateEvtGen(Decay_t forceDecay, AliGenEvtGen_dev::DecayOff_t decayOff);
#else
  static AliGenEvtGen* CreateEvtGen(Decay_t forceDecay, AliGenEvtGen::DecayOff_t decayOff);
#endif

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
  EPythiaTune_t      GetPythiaTune()         const { return fPythiaTune     ; }
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
  EPythiaTune_t        fPythiaTune      ;
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
  std::vector<std::string>
                       fDebugClassNames  ;

private:
  OnTheFlySimulationGenerator(const OnTheFlySimulationGenerator&);//not implemented
  OnTheFlySimulationGenerator& operator=(const OnTheFlySimulationGenerator&); //not implemented
};

#endif
