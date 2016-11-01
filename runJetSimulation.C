// runJetSimulation.C

// std library
#include <iostream>

// Root classes
#include <TSystem.h>
#include <TString.h>
#include <TArrayI.h>
#include <TChain.h>
#include <TFile.h>

// AliRoot classes
#include <PythiaProcesses.h>
#include <AliGenPythia.h>
#include <AliESDInputHandler.h>
#include <AliAnalysisAlien.h>
#include <AliAnalysisManager.h>
#include <AliDummyHandler.h>
#include <AliMCGenHandler.h>
#include <AliRun.h>

// AliPhysics classes
#include <AliAnalysisTaskDmesonJets.h>
#include <AliEmcalJetTask.h>
#include <AliAnalysisTaskEmcalJetTree.h>
#include <AliAnalysisTaskEmcalJetQA.h>
#include <AliEmcalMCTrackSelector.h>

#include "AliAnalysisTaskSEhfcjMCanalysis.h"

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

AliGenPythia* CreatePythia6Gen(Float_t e_cms, EPythiaTune_t tune=kPerugia2011, Process_t proc=kPyMb,
    ESpecialParticle_t specialPart = kNoSpecialParticle, Int_t ptHardMin=0, Int_t ptHardMax=1,
    Bool_t forceHadronicDecay=kFALSE, Float_t ptWeight=0);

//______________________________________________________________________________
void runJetSimulation(
    const Int_t   numevents    = 50000,                       // number of events
    Process_t     proc         = kPyMb,
    ESpecialParticle_t specialPart = kNoSpecialParticle,
    Bool_t        forceHadDecay=kFALSE,
    const char   *taskname     = "FastSim",                    // sets name of grid generated macros
    Int_t         seed         = 0,
    const char   *lhe          = ""
  )
{
  // analysis manager
  AliAnalysisManager* mgr = new AliAnalysisManager(taskname);

  AliAnalysisManager::SetCommonFileName(Form("AnalysisResults_%s.root",taskname));

  // Dummy ESD event and ESD handler
  AliESDEvent *esdE = new AliESDEvent();
  esdE->CreateStdContent();
  AliESDVertex *vtx = new AliESDVertex(0.,0.,100);
  vtx->SetName("VertexTracks");
  vtx->SetTitle("VertexTracks");
  esdE->SetPrimaryVertexTracks(vtx);
  AliDummyHandler* dumH = new AliDummyHandler();
  dumH->SetEvent(esdE);
  mgr->SetInputEventHandler(dumH);

  /*
  kPyCharm, kPyBeauty, kPyCharmUnforced, kPyBeautyUnforced,
  kPyJpsi, kPyJpsiChi, kPyMb, kPyMbWithDirectPhoton, kPyMbNonDiffr, kPyJets, kPyDirectGamma,
  kPyCharmPbPbMNR, kPyD0PbPbMNR, kPyDPlusPbPbMNR, kPyDPlusStrangePbPbMNR, kPyBeautyPbPbMNR,
  kPyCharmpPbMNR, kPyD0pPbMNR, kPyDPluspPbMNR, kPyDPlusStrangepPbMNR, kPyBeautypPbMNR,
  kPyCharmppMNR, kPyCharmppMNRwmi, kPyD0ppMNR, kPyDPlusppMNR, kPyDPlusStrangeppMNR,
  kPyBeautyppMNR, kPyBeautyppMNRwmi, kPyBeautyJets, kPyW, kPyZ, kPyLambdacppMNR, kPyMbMSEL1,
  kPyOldUEQ2ordered, kPyOldUEQ2ordered2, kPyOldPopcorn,
  kPyLhwgMb, kPyMbDefault, kPyMbAtlasTuneMC09, kPyMBRSingleDiffraction, kPyMBRDoubleDiffraction,
  kPyMBRCentralDiffraction, kPyJetsPWHG, kPyCharmPWHG, kPyBeautyPWHG, kPyWPWHG, kPyZgamma
  */

  // Generator and generator handler
  AliGenPythia* gen = CreatePythia6Gen(7000., kPerugia2011, proc, specialPart, 0, 1, forceHadDecay);
  TString LHEfile(lhe);
  if (!LHEfile.IsNull()) gen->SetReadLHEF(LHEfile);

  AliMCGenHandler* mcInputHandler = new AliMCGenHandler();
  mcInputHandler->SetGenerator(gen);
  mcInputHandler->SetSeed(seed);
  mcInputHandler->SetSeedMode(1);
  mgr->SetMCtruthEventHandler(mcInputHandler);

  AliAnalysisTaskSEhfcjMCanalysis::AddTaskSEhfcjMCanalysis();

  AliEmcalMCTrackSelector* pMCTrackSel = AliEmcalMCTrackSelector::AddTaskMCTrackSelector("mcparticles",kFALSE,kFALSE,-1,kFALSE);

  UInt_t rejectOrigin = 0;
  if (proc ==  kPyCharmPWHG || proc ==  kPyCharm) {
    rejectOrigin = AliAnalysisTaskDmesonJets::EMesonOrigin_t::kFromBottom;
  }
  else if (proc ==  kPyBeautyPWHG || proc ==  kPyBeauty) {
    rejectOrigin = AliAnalysisTaskDmesonJets::EMesonOrigin_t::kAnyOrigin & ~AliAnalysisTaskDmesonJets::EMesonOrigin_t::kFromBottom;
  }

  rejectOrigin |= AliAnalysisTaskDmesonJets::EMesonOrigin_t::kUnknownQuark;

  AliAnalysisTaskEmcalJetQA* pJetQA = AliAnalysisTaskEmcalJetQA::AddTaskEmcalJetQA("mcparticles","","");
  pJetQA->SetForceBeamType(AliAnalysisTaskEmcalLight::kpp);
  pJetQA->SetMC(kTRUE);
  pJetQA->SetParticleLevel(kTRUE);
  pJetQA->SetIsPythia(kTRUE);
  pJetQA->SetVzRange(-999,999);

  if (proc == kPyCharmPWHG || proc == kPyBeautyPWHG || proc == kPyCharm || proc == kPyBeauty) {
    AliAnalysisTaskDmesonJets* pDMesonJetsTask = AliAnalysisTaskDmesonJets::AddTaskDmesonJets("", "", "usedefault", 2);
    pDMesonJetsTask->SetVzRange(-999,999);
    pDMesonJetsTask->SetIsPythia(kTRUE);
    pDMesonJetsTask->SetNeedEmcalGeom(kFALSE);
    pDMesonJetsTask->SetForceBeamType(AliAnalysisTaskEmcalLight::kpp);
    pDMesonJetsTask->SetOutputType(AliAnalysisTaskDmesonJets::kTreeOutput);
    AliAnalysisTaskDmesonJets::AnalysisEngine* eng = 0;
    eng = pDMesonJetsTask->AddAnalysisEngine(AliAnalysisTaskDmesonJets::kD0toKpi, AliAnalysisTaskDmesonJets::kMCTruth, AliJetContainer::kChargedJet, 0.4);
    eng->SetAcceptedDecayMap(AliAnalysisTaskDmesonJets::EMesonDecayChannel_t::kAnyDecay);
    eng->SetRejectedOriginMap(rejectOrigin);
    eng = pDMesonJetsTask->AddAnalysisEngine(AliAnalysisTaskDmesonJets::kD0toKpi, AliAnalysisTaskDmesonJets::kMCTruth, AliJetContainer::kChargedJet, 0.6);
    eng->SetAcceptedDecayMap(AliAnalysisTaskDmesonJets::EMesonDecayChannel_t::kAnyDecay);
    eng->SetRejectedOriginMap(rejectOrigin);
    eng = pDMesonJetsTask->AddAnalysisEngine(AliAnalysisTaskDmesonJets::kD0toKpi, AliAnalysisTaskDmesonJets::kMCTruth, AliJetContainer::kFullJet, 0.4);
    eng->SetAcceptedDecayMap(AliAnalysisTaskDmesonJets::EMesonDecayChannel_t::kAnyDecay);
    eng->SetRejectedOriginMap(rejectOrigin);
    eng = pDMesonJetsTask->AddAnalysisEngine(AliAnalysisTaskDmesonJets::kDstartoKpipi, AliAnalysisTaskDmesonJets::kMCTruth, AliJetContainer::kChargedJet, 0.4);
    eng->SetAcceptedDecayMap(AliAnalysisTaskDmesonJets::EMesonDecayChannel_t::kAnyDecay);
    eng->SetRejectedOriginMap(rejectOrigin);
    eng = pDMesonJetsTask->AddAnalysisEngine(AliAnalysisTaskDmesonJets::kDstartoKpipi, AliAnalysisTaskDmesonJets::kMCTruth, AliJetContainer::kChargedJet, 0.6);
    eng->SetAcceptedDecayMap(AliAnalysisTaskDmesonJets::EMesonDecayChannel_t::kAnyDecay);
    eng->SetRejectedOriginMap(rejectOrigin);
    eng = pDMesonJetsTask->AddAnalysisEngine(AliAnalysisTaskDmesonJets::kDstartoKpipi, AliAnalysisTaskDmesonJets::kMCTruth, AliJetContainer::kFullJet, 0.4);
    eng->SetAcceptedDecayMap(AliAnalysisTaskDmesonJets::EMesonDecayChannel_t::kAnyDecay);
    eng->SetRejectedOriginMap(rejectOrigin);
  }

  AliEmcalJetTask* pJetTaskCh04 = AliEmcalJetTask::AddTaskEmcalJet("mcparticles", "", AliJetContainer::antikt_algorithm, 0.4, AliJetContainer::kChargedJet, 0., 0., 0.1, AliJetContainer::pt_scheme, "Jet", 0., kFALSE, kFALSE);
  pJetTaskCh04->SetVzRange(-999,999);
  pJetTaskCh04->SetNeedEmcalGeom(kFALSE);
  AliEmcalJetTask* pJetTaskCh06 = AliEmcalJetTask::AddTaskEmcalJet("mcparticles", "", AliJetContainer::antikt_algorithm, 0.6, AliJetContainer::kChargedJet, 0., 0., 0.1, AliJetContainer::pt_scheme, "Jet", 0., kFALSE, kFALSE);
  pJetTaskCh06->SetVzRange(-999,999);
  pJetTaskCh06->SetNeedEmcalGeom(kFALSE);
  AliEmcalJetTask* pJetTaskFu04 = AliEmcalJetTask::AddTaskEmcalJet("mcparticles", "", AliJetContainer::antikt_algorithm, 0.4, AliJetContainer::kFullJet, 0., 0., 0.1, AliJetContainer::pt_scheme, "Jet", 0., kFALSE, kFALSE);
  pJetTaskFu04->SetVzRange(-999,999);
  pJetTaskFu04->SetNeedEmcalGeom(kFALSE);

  AliAnalysisTaskEmcalJetTreeBase* pJetSpectraTask = AliAnalysisTaskEmcalJetTreeBase::AddTaskEmcalJetTree("mcparticles", "");
  pJetSpectraTask->SetForceBeamType(AliAnalysisTaskEmcalLight::kpp);
  pJetSpectraTask->SetVzRange(-999,999);
  pJetSpectraTask->SetIsPythia(kTRUE);
  pJetSpectraTask->SetNeedEmcalGeom(kFALSE);
  pJetSpectraTask->GetParticleContainer("mcparticles")->SetMinPt(0.);
  pJetSpectraTask->AddJetContainer(AliJetContainer::kChargedJet, AliJetContainer::antikt_algorithm, AliJetContainer::pt_scheme, 0.4, AliJetContainer::kTPCfid);
  pJetSpectraTask->AddJetContainer(AliJetContainer::kChargedJet, AliJetContainer::antikt_algorithm, AliJetContainer::pt_scheme, 0.6, AliJetContainer::kTPCfid);
  pJetSpectraTask->AddJetContainer(AliJetContainer::kFullJet, AliJetContainer::antikt_algorithm, AliJetContainer::pt_scheme, 0.4, AliJetContainer::kTPCfid);

  if (!mgr->InitAnalysis()) return;
  mgr->PrintStatus();

  TFile *out = new TFile(Form("%s.root",taskname),"RECREATE");
  out->cd();
  mgr->Write();
  out->Close();
  delete out;

  if (!gAlice) new AliRun("gAlice","The ALICE Off-line Simulation Framework");

  // start analysis
  std::cout << "Starting Analysis...";
  mgr->SetUseProgressBar(1, 25);
  mgr->SetDebugLevel(0);
  //mgr->AddClassDebug("AliJetTriggerSelectionTask",AliLog::kDebug+100);
  mgr->SetCacheSize(0);
  mgr->EventLoop(numevents);
}

//________________________________________________________________________
AliGenPythia* CreatePythia6Gen(Float_t e_cms, EPythiaTune_t tune, Process_t proc,
    ESpecialParticle_t specialPart, Int_t ptHardMin, Int_t ptHardMax,
    Bool_t forceHadronicDecay, Float_t ptWeight)
{
  AliGenPythia* genP = new AliGenPythia(-1);
  genP->SetTune(tune);

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

  // Additional settings from A. Rossi
  genP->SetProjectile("p", 1, 1);
  genP->SetTarget(    "p", 1, 1);
  genP->SetMomentumRange(0, 999999.);
  genP->SetThetaRange(0., 180.);
  genP->SetYRange(-12.,12.);
  genP->SetPtRange(0,1000.);
  genP->SetTrackingFlag(0);

  genP->Print();
  return genP;
}
