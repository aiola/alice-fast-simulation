#ifndef ALIFASTSIMULATIONTASK_H
#define ALIFASTSIMULATIONTASK_H

#include "AliAnalysisTaskSE.h"

#include "PythiaProcesses.h"

class AliGenerator;
class TClonesArray;
class TProfile;
class TH1;
class TString;
class TList;
class AliStack;
class AliHeader;
class AliMCEvent;
class AliEmcalMCTrackSelector;
class AliGenPythia;

class AliFastSimulationTask : public AliAnalysisTaskSE {
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

  AliFastSimulationTask();
  AliFastSimulationTask(const char *name, Bool_t drawqa);
  virtual ~AliFastSimulationTask();

  void           UserCreateOutputObjects();
  void           UserExec(Option_t*);
  void           FillPythiaHistograms();

  void           SetGen(AliGenerator *gen)             { fGen             = gen; }
  void           SetMCParticlesName(const char *n)     { fMCParticlesName = n  ; }

  static AliGenPythia* CreatePythia6Gen(Float_t e_cms, EPythiaTune_t tune=kPerugia2012, Process_t proc=kPyMb,
      ESpecialParticle_t specialPart = kNoSpecialParticle, Int_t ptHardMin=0, Int_t ptHardMax=1,
      Bool_t forceHadronicDecay=kFALSE, Float_t ptWeight=0);

  static AliFastSimulationTask* AddTaskFastSimulation(AliGenerator* genGen, TString partName = "mcparticles", TString taskName = "FastSimulationTask", const Bool_t drawQA = kTRUE);

 protected:
  Bool_t         ExecOnce();
  void           Run();

  Bool_t         fQAhistos;               // draw qa plots
  AliGenerator  *fGen;                    // generator
  AliHeader     *fHeader;                 //!event header
  AliEmcalMCTrackSelector* fMCTrackSelector; //!MC track selector
  AliMCEvent    *fMCEvent;                //!MC event
  TString        fMCParticlesName;        // name of MC particle collection
  Bool_t         fIsInit;                 //!=true if initialized
  AliStack      *fStack;                  //!stack
  TClonesArray  *fMCParticles;            //!MC particles collection
  TH1           *fHistTrials;             //!trials from generator
  TProfile      *fHistXsection;           //!x-section from generator
  TH1           *fHistPtHard;             //!pt hard distribution
  TList         *fOutput;                 //!output list for QA histograms

 private:
  AliFastSimulationTask(const AliFastSimulationTask&);            // not implemented
  AliFastSimulationTask &operator=(const AliFastSimulationTask&); // not implemented

  ClassDef(AliFastSimulationTask, 1) // Fast simulation task
};
#endif
