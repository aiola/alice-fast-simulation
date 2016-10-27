//
// Fast simulation task.
//
// Author: S.Aiola

#include "AliFastSimulationTask.h"

#include <TClonesArray.h>
#include <TFolder.h>
#include <TLorentzVector.h>
#include <TParticle.h>
#include <TParticlePDG.h>
#include <TRandom3.h>
#include <TProfile.h>
#include <TH1F.h>
#include <TList.h>
#include <Riostream.h>

#include "AliAnalysisManager.h"
#include "AliGenerator.h"
#include "AliGenPythia.h"
#include "AliHeader.h"
#include "AliLog.h"
#include "AliMCEvent.h"
#include "AliHeader.h"
#include "AliMCParticle.h"
#include "AliRun.h"
#include "AliRunLoader.h"
#include "AliStack.h"
#include "AliGenPythiaEventHeader.h"
#include "AliEmcalMCTrackSelector.h"

ClassImp(AliFastSimulationTask)

//________________________________________________________________________
AliFastSimulationTask::AliFastSimulationTask() : 
AliAnalysisTaskSE("AliFastSimulationTask"),
  fQAhistos(kFALSE),
  fGen(0),
  fHeader(0),
  fMCTrackSelector(0),
  fMCEvent(0),
  fMCParticlesName("mcparticles"),
  fIsInit(kFALSE),
  fStack(0),
  fMCParticles(0),
  fHistTrials(0),
  fHistXsection(0),
  fHistPtHard(0),
  fOutput(0)
{
  // Default constructor.

}

//________________________________________________________________________
AliFastSimulationTask::AliFastSimulationTask(const char *name, Bool_t drawqa) :
  AliAnalysisTaskSE(name),
  fQAhistos(drawqa),
  fGen(0),
  fHeader(0),
  fMCTrackSelector(0),
  fMCEvent(0),
  fMCParticlesName("mcparticles"),
  fIsInit(kFALSE),
  fStack(0),
  fMCParticles(0),
  fHistTrials(0),
  fHistXsection(0),
  fHistPtHard(0),
  fOutput(0)
{
  // Standard constructor.

  if (fQAhistos) DefineOutput(1, TList::Class()); 
}

//________________________________________________________________________
AliFastSimulationTask::~AliFastSimulationTask()
{
  // Destructor
}

//________________________________________________________________________
void AliFastSimulationTask::UserCreateOutputObjects()
{
  // Create user output.

  if (!fQAhistos) return;

  OpenFile(1);
  fOutput = new TList();
  fOutput->SetOwner();

  fHistTrials = new TH1F("fHistTrials", "fHistTrials", 1, 0, 1);
  fHistTrials->GetYaxis()->SetTitle("trials");
  fOutput->Add(fHistTrials);

  fHistXsection = new TProfile("fHistXsection", "fHistXsection", 1, 0, 1);
  fHistXsection->GetYaxis()->SetTitle("xsection");
  fOutput->Add(fHistXsection);

  fHistPtHard = new TH1F("fHistPtHard", "fHistPtHard", 500, 0., 500.);
  fHistPtHard->GetXaxis()->SetTitle("p_{T,hard} (GeV/c)");
  fHistPtHard->GetYaxis()->SetTitle("counts");
  fOutput->Add(fHistPtHard);

  PostData(1, fOutput);
}

//________________________________________________________________________
void AliFastSimulationTask::UserExec(Option_t *) 
{
  // Execute per event.

  if (!fIsInit) fIsInit = ExecOnce();

  if (!fIsInit) return;

  Run();
}

//________________________________________________________________________
Bool_t AliFastSimulationTask::ExecOnce() 
{
  // Exec only once.
  AliInfo("Initializing...");

  if (!gAlice) {
    new AliRun("gAlice","The ALICE Off-line Simulation Framework");
    delete gRandom;
    gRandom = new TRandom3(0);
  }
  
  fGen->SetRandom(gRandom);

  AliGenPythia *genPythia = dynamic_cast<AliGenPythia*>(fGen);

  TFolder *folder = new TFolder(GetName(),GetName());
  AliRunLoader *rl = new AliRunLoader(folder);
  gAlice->SetRunLoader(rl);
  rl->MakeHeader();
  rl->MakeStack();
  fHeader = rl->GetHeader();
  fStack = rl->Stack();
  fHeader->SetStack(fStack);
  fGen->SetStack(fStack);
  fGen->Init();

  fMCEvent = new AliMCEvent();

  fMCTrackSelector = new AliEmcalMCTrackSelector();
  fMCTrackSelector->SetOnlyPhysPrim(kFALSE);
  fMCTrackSelector->SetEtaMax(-1);

  if (!(InputEvent()->FindListObject(fMCParticlesName))) {
    fMCParticles = new TClonesArray("AliAODMCParticle", 1000);
    fMCParticles->SetName(fMCParticlesName);
    InputEvent()->AddObject(fMCParticles);
  }

  if (!(InputEvent()->FindListObject(fStack->GetName()))) {
    InputEvent()->AddObject(fStack);
  }
  
  if (!(InputEvent()->FindListObject(fHeader->GetName()))) {
    InputEvent()->AddObject(fHeader);
  }

  AliInfo("Initialization succeeded!");

  return kTRUE;
}

//________________________________________________________________________
void AliFastSimulationTask::Run() 
{
  // Run simulation.

  AliDebug(3,"Resetting event...");
  fMCParticles->Clear("C");
  fStack->Reset();
  AliDebug(3,"Generating new event...");
  fGen->Generate();
  AliDebug(3,"Setting MC event object...");
  fMCEvent->ConnectHeaderAndStack(fHeader);
  AliDebug(3,"Converting MC particles to AOD MC particles...");
  fMCTrackSelector->ConvertMCParticles(fMCEvent, fMCParticles);
  /*
  const Int_t nprim = fStack->GetNprimary();
  Int_t nParticles = 0;
  for (Int_t i=0; i < nprim; i++) {
    TParticle *part = fStack->Particle(i);
    if (!part) continue;

    AliMCParticle *mcpart = new ((*fMCParticles)[nParticles]) AliMCParticle(part, 0, i);
    mcpart->SetMother(part->GetFirstMother());

    nParticles++;
  }
  */
  AliDebug(3,"Plotting some general histograms...");
  FillPythiaHistograms();
}

//________________________________________________________________________
void AliFastSimulationTask::FillPythiaHistograms() 
{
  //Get PYTHIA info: pt-hard, x-section, trials

  if (!fQAhistos)
    return;

  AliRunLoader *rl = AliRunLoader::Instance();
  AliGenPythiaEventHeader *genPH = dynamic_cast<AliGenPythiaEventHeader*>(rl->GetHeader()->GenEventHeader());
  if(genPH) {
    Float_t xsec = genPH->GetXsection();
    Int_t trials = genPH->Trials();
    Float_t pthard = genPH->GetPtHard();

    fHistXsection->Fill(0.5,xsec);
    fHistTrials->Fill(0.5,trials);
    fHistPtHard->Fill(pthard);
  }
}

//________________________________________________________________________
AliGenPythia* AliFastSimulationTask::CreatePythia6Gen(Float_t e_cms, EPythiaTune_t tune, Process_t proc,
    ESpecialParticle_t specialPart, Int_t ptHardMin, Int_t ptHardMax,
    Bool_t forceHadronicDecay, Float_t ptWeight)
{
  AliGenPythia* genP = new AliGenPythia(1);

  // vertex position and smearing
  genP->SetVertexSmear(kPerEvent);
  genP->SetProcess(proc);

  if (ptHardMin > 0.) {
    genP->SetPtHard(ptHardMin, ptHardMax);
    if (ptWeight > 0) genP->SetWeightPower(ptWeight);
  }

  if (specialPart == kccbar) {
    genP->SetHeavyQuarkYRange(-5, 5);
    Float_t randcharge = gRandom->Rndm();
    if (randcharge > 0.5) {
      genP->SetTriggerParticle(4, 3., -1., 1000);
    }
    else {
      genP->SetTriggerParticle(-4, 3., -1., 1000);
    }
  }
  else if (specialPart == kbbbar) {
    genP->SetHeavyQuarkYRange(-5, 5);
    Float_t randcharge = gRandom->Rndm();
    if (randcharge > 0.5) {
      genP->SetTriggerParticle(5, 3., -1., 1000);
    }
    else {
      genP->SetTriggerParticle(-5, 3., -1., 1000);
    }
  }

  if (forceHadronicDecay) genP->SetForceDecay(kHadronicDWithout4BodiesWithV0);

  //   Center of mass energy
  genP->SetEnergyCMS(e_cms); // in GeV

  genP->UseNewMultipleInteractionsScenario(); // for all Pythia versions >= 6.3
  genP->SetTune(tune);

  genP->Print();
  return genP;
}

//________________________________________________________________________
AliFastSimulationTask* AliFastSimulationTask::AddTaskFastSimulation(AliGenerator* genGen, TString partName, TString taskName, const Bool_t drawQA)
{
  // Get the pointer to the existing analysis manager via the static access method.
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) {
    ::Error("AddTaskFastSimulation", "No analysis manager to connect to.");
    return 0;
  }

  // Check the analysis type using the event handlers connected to the analysis manager.
  if (!mgr->GetInputEventHandler()) {
    ::Error("AddTaskFastSimulation", "This task requires an input event handler");
    return 0;
  }

  // Init the task and do settings
  AliFastSimulationTask *fastSimTask = new AliFastSimulationTask(taskName,drawQA);
  fastSimTask->SetGen(genGen);
  fastSimTask->SetMCParticlesName(partName);

  // Final settings, pass to manager and set the containers
  mgr->AddTask(fastSimTask);

  // Create containers for input/output
  mgr->ConnectInput(fastSimTask, 0, mgr->GetCommonInputContainer());

  if (drawQA) {
    TString contName = taskName;
    contName += "_histos";
    AliAnalysisDataContainer *outc = mgr->CreateContainer(contName,
                                                          TList::Class(),
                                                          AliAnalysisManager::kOutputContainer,
                                                          AliAnalysisManager::GetCommonFileName());
    mgr->ConnectOutput(fastSimTask, 1, outc);
  }

  return fastSimTask;
}
