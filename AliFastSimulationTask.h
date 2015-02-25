#ifndef ALIFASTSIMULATIONTASK_H
#define ALIFASTSIMULATIONTASK_H

#include "AliAnalysisTaskSE.h"

class AliGenerator;
class TClonesArray;
class TProfile;
class TH1;
class TString;
class TList;
class AliStack;

class AliFastSimulationTask : public AliAnalysisTaskSE {
 public:
  AliFastSimulationTask();
  AliFastSimulationTask(const char *name, Bool_t drawqa);
  virtual ~AliFastSimulationTask();

  void           UserCreateOutputObjects();
  void           UserExec(Option_t*);
  void           FillPythiaHistograms();

  void           SetGen(AliGenerator *gen)             { fGen             = gen; }
  void           SetMCParticlesName(const char *n)     { fMCParticlesName = n  ; }

 protected:
  Bool_t         ExecOnce();
  void           Run();

  Bool_t         fQAhistos;               // draw qa plots
  AliGenerator  *fGen;                    // generator
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
