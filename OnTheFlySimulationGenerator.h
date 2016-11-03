// OnTheFlySimulationGenerator

#include <TString.h>
#include <PythiaProcesses.h>

class AliGenPythia;
class AliAnalysisTaskSE;
class AliAnalysisManager;

class OnTheFlySimulationGenerator {
public:
  // PYTHIA6 tunes: https://arxiv.org/pdf/1005.3457v5.pdf
  enum EPythiaTune_t {
    kPerugia0 = 320,
    kPerugio0NOCR = 324,
    kPerugia2010 = 327,
    kPerugia2011 = 350,
    kPerugia2011NOCR = 354,
    kPerugia2012 = 370,
    kPerugia2012NOCR = 375
  };

  enum ESpecialParticle_t {
    kNoSpecialParticle = 0,
    kccbar = 1,
    kbbbar = 2
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
  void SetCMSEnergy(Float_t e_cms)                        { fCMSEnergy       = e_cms         ; }
  void SetPythiaTune(EPythiaTune_t tune)                  { fTune            = tune          ; }
  void SetPtHardRange(Int_t ptHardMin, Int_t ptHardMax)   { fPtHardMin       = ptHardMin     ; fPtHardMax       = ptHardMax; }

  static AliGenPythia* CreatePythia6Gen(Float_t e_cms, EPythiaTune_t tune, Process_t proc, ESpecialParticle_t specialPart, Int_t ptHardMin, Int_t ptHardMax, Bool_t forceHadronicDecay);

  void PrepareAnalysisManager();
  void Start();

  const TString&     GetName()               const { return fName           ; }
  Int_t              GetNumberOfEvents()     const { return fEvents         ; }
  Process_t          GetProcess()            const { return fProcess        ; }
  ESpecialParticle_t GetSpecialParticle()    const { return fSpecialParticle; }
  Bool_t             GetForceHadronicDecay() const { return fForceHadDecay  ; }
  Int_t              GetSeed()               const { return fSeed           ; }
  const TString&     GetLHEFile()            const { return fLHEFile        ; }
  Float_t            GetCMSEnergy()          const { return fCMSEnergy      ; }
  EPythiaTune_t      GetPythiaTune()         const { return fTune           ; }
  Int_t              GetPtHardMin()          const { return fPtHardMin      ; }
  Int_t              GetPtHardMax()          const { return fPtHardMax      ; }

protected:
  TString              fName             ;
  AliAnalysisManager*  fAnalysisManager  ;
  Int_t                fEvents           ;
  Process_t            fProcess          ;
  ESpecialParticle_t   fSpecialParticle  ;
  Bool_t               fForceHadDecay    ;
  Int_t                fSeed             ;
  TString              fLHEFile          ;
  Float_t              fCMSEnergy        ;
  EPythiaTune_t        fTune             ;
  Int_t                fPtHardMin        ;
  Int_t                fPtHardMax        ;
};
