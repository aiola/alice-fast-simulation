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
  void SetPythiaTune(EPythiaTune_t tune)                  { fTune            = tune          ; }
  void SetPtHardBin(Int_t ptHard)                         { fPtHardBin       = ptHard        ; }
  void EnableJetQA(Bool_t b = kTRUE)                      { fJetQA           = b             ; }
  void SetBeamType(EBeamType_t b)                         { fBeamType        = b             ; }
  void EnableJetTree(Bool_t b = kTRUE)                    { fJetTree         = b             ; }

  static AliGenPythia* CreatePythia6Gen(EBeamType_t beam, Float_t e_cms, EPythiaTune_t tune, Process_t proc, ESpecialParticle_t specialPart, Int_t ptHardMin, Int_t ptHardMax, Bool_t forceHadronicDecay);

  void PrepareAnalysisManager();
  void Start();

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
  EPythiaTune_t      GetPythiaTune()         const { return fTune           ; }
  Int_t              GetPtHardMin()          const { return fPtHardBins[fPtHardBin]    ; }
  Int_t              GetPtHardMax()          const { return fPtHardBins[fPtHardBin+1]  ; }

protected:
  void               AddJetQA();
  void               AddJetTree();
  void               AddDJet_pp();
  void               AddDJet_pPb();

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
  EPythiaTune_t        fTune             ;
  Int_t                fPtHardBin        ;
  TArrayD              fPtHardBins       ;
  Bool_t               fJetQA            ;
  EBeamType_t          fBeamType         ;
  Bool_t               fJetTree          ;
  Float_t              fEnergyBeam1      ; // in GeV
  Float_t              fEnergyBeam2      ; // in GeV

private:
  OnTheFlySimulationGenerator(const OnTheFlySimulationGenerator&);//not implemented
  OnTheFlySimulationGenerator& operator=(const OnTheFlySimulationGenerator&); //not implemented
};
