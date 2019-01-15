// OnTheFlySimulationGenerator

#ifndef ONTHEFLYSIMULATIONGENERATOR_H
#define ONTHEFLYSIMULATIONGENERATOR_H


#include "AliGenEvtGen_dev.h"
#include "AliGenPythia_dev.h"
#include "AliGenExtFile_dev.h"

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
    kHerwig7,
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
  OnTheFlySimulationGenerator(TString taskname, Int_t numevents, Process_t proc, ESpecialParticle_t specialPart, Bool_t forceHadDecay, Int_t seed, TString lhe, TString hep);

  void SetName(TString taskname)                          { fName            = taskname      ; }
  void SetNumberOfEvents(Int_t numevents)                 { fEvents          = numevents     ; }
  void SetProcess(Process_t proc)                         { fProcess         = proc          ; }
  void SetSpecialParticle(ESpecialParticle_t specialPart) { fSpecialParticle = specialPart   ; }
  void SetSeed(Int_t seed)                                { fSeed            = seed          ; }
  void SetLHEFile(TString lhe)                            { fLHEFile         = lhe           ; }
  void SetHepMCFile(TString hep)                          { fHepMCFile       = hep           ; }
  void SetEnergyBeam1(Float_t e)                          { fEnergyBeam1     = e             ; }
  void SetEnergyBeam2(Float_t e)                          { fEnergyBeam2     = e             ; }
  void SetPythia6Tune(EPythiaTune_t tune)                 { fPythia6Tune      = tune         ; }
  void SetPythia8Tune(EPythiaTune_t tune)                 { fPythia8Tune      = tune         ; }
  void SetPtHardRange(Double_t minPtHard, Double_t maxPtHard)
                                                          { fMinPtHard       = minPtHard     ; fMaxPtHard        = maxPtHard    ; }
  void EnableJetQA(Bool_t b = kTRUE)                      { fJetQA           = b             ; }
  void EnableJetTree(Bool_t b = kTRUE)                    { fJetTree         = b             ; }
  void EnableDMesonJets(Bool_t b = kTRUE)                 { fDMesonJets      = b             ; }
  void EnableDMesonHadrCorr(Bool_t b = kTRUE)             { fDMesonHadrCorr  = b             ; }
  void SetBeamType(EBeamType_t b)                         { fBeamType        = b             ; }
  void SetRejectISR(Bool_t b)                             { fRejectISR       = b             ; }
  void SetPartonEventGenerator(EGenerator_t gen)          { fPartonEvent     = gen           ; }
  void SetHadronization(EGenerator_t gen)                 { fHadronization   = gen           ; }
  void SetDecayer(EGenerator_t gen)                       { fDecayer         = gen           ; }
  void SetExtendedEventInfo(Bool_t b)                     { fExtendedEventInfo = b           ; }

  static AliGenPythia_dev* CreatePythia6Gen(EBeamType_t beam, Float_t e_cms, EGenerator_t partonEvent, TString lhe, EPythiaTune_t tune, Process_t proc, ESpecialParticle_t specialPart, Double_t ptHardMin, Double_t ptHardMax);
  static AliGenPythia_dev* CreatePythia8Gen(EBeamType_t beam, Float_t e_cms, EGenerator_t partonEvent, TString lhe, EPythiaTune_t tune, Process_t proc, ESpecialParticle_t specialPart, Double_t ptHardMin, Double_t ptHardMax);
  static AliGenExtFile_dev* CreateHerwig7Gen(EBeamType_t beam, Float_t e_cms, TString hep, ESpecialParticle_t specialPart);
  static AliGenEvtGen_dev* CreateEvtGen(AliGenEvtGen_dev::DecayOff_t decayOff);

  static AliGenCocktail* CreateCocktailGen(EBeamType_t beam, Float_t e_cms);

  AliGenerator* CreateGenerator();
  void PrepareAnalysisManager();
  void Start(UInt_t debug_level = 0);

  const TString&     GetName()               const { return fName           ; }
  Int_t              GetNumberOfEvents()     const { return fEvents         ; }
  Process_t          GetProcess()            const { return fProcess        ; }
  ESpecialParticle_t GetSpecialParticle()    const { return fSpecialParticle; }
  Int_t              GetSeed()               const { return fSeed           ; }
  const TString&     GetLHEFile()            const { return fLHEFile        ; }
  const TString&     GetHepMCFile()          const { return fHepMCFile      ; }
  Float_t            GetCMSEnergy()                { if (fCMSEnergy < 0) CalculateCMSEnergy(); return fCMSEnergy; }
  Float_t            GetEnergyBeam1()        const { return fEnergyBeam1    ; }
  Float_t            GetEnergyBeam2()        const { return fEnergyBeam2    ; }
  EPythiaTune_t      GetPythia6Tune()        const { return fPythia6Tune    ; }
  EPythiaTune_t      GetPythia8Tune()        const { return fPythia8Tune    ; }
  Double_t           GetPtHardMin()          const { return fMinPtHard      ; }
  Double_t           GetPtHardMax()          const { return fMaxPtHard      ; }

protected:
  void               AddJetQA(const char* file_name = "");
  void               AddJetTree(const char* file_name = "");
  void               AddDJet(const char* file_name = "");
  void               AddDhcorr(const char* file_name = "");

  void               CalculateCMSEnergy();

  TString              fName             ;
  AliAnalysisManager*  fAnalysisManager  ;
  Int_t                fEvents           ;
  Process_t            fProcess          ;
  ESpecialParticle_t   fSpecialParticle  ;
  Int_t                fSeed             ;
  TString              fLHEFile          ;
  TString              fHepMCFile        ;
  Float_t              fCMSEnergy        ; // in TeV
  EPythiaTune_t        fPythia6Tune      ;
  EPythiaTune_t        fPythia8Tune      ;
  Double_t             fMinPtHard        ;
  Double_t             fMaxPtHard        ;
  EBeamType_t          fBeamType         ;
  Bool_t               fJetQA            ;
  Bool_t               fJetTree          ;
  Bool_t               fDMesonJets       ;
  Bool_t               fDMesonHadrCorr   ;
  Float_t              fEnergyBeam1      ; // in GeV
  Float_t              fEnergyBeam2      ; // in GeV
  Bool_t               fRejectISR        ;
  EGenerator_t         fPartonEvent      ;
  EGenerator_t         fHadronization    ;
  EGenerator_t         fDecayer          ;
  Bool_t               fExtendedEventInfo;
  std::vector<std::string>
                       fDebugClassNames  ;

private:
  OnTheFlySimulationGenerator(const OnTheFlySimulationGenerator&);//not implemented
  OnTheFlySimulationGenerator& operator=(const OnTheFlySimulationGenerator&); //not implemented
};

#endif
