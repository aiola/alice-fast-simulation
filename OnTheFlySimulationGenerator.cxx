// runJetSimulation.C

// std library
#include <iostream>

// Root classes
#include <TSystem.h>
#include <TArrayI.h>
#include <TChain.h>
#include <TFile.h>
#include <TMath.h>

// AliRoot classes
#include <AliGenPythia.h>
#include <AliESDInputHandler.h>
#include <AliAnalysisAlien.h>
#include <AliAnalysisManager.h>
#include <AliDummyHandler.h>
#include <AliMCGenHandler.h>
#include <AliRun.h>
#include <AliAnalysisTaskSE.h>

// AliPhysics classes
#include <AliAnalysisTaskDmesonJets.h>
#include <AliEmcalJetTask.h>
#include <AliAnalysisTaskEmcalJetTree.h>
#include <AliAnalysisTaskEmcalJetQA.h>
#include <AliEmcalMCTrackSelector.h>

#include "OnTheFlySimulationGenerator.h"

//______________________________________________________________________________
OnTheFlySimulationGenerator::OnTheFlySimulationGenerator() :
  fName("FastSim"),
  fAnalysisManager(0),
  fEvents(50000),
  fProcess(kPyMbDefault),
  fSpecialParticle(kNoSpecialParticle),
  fForceHadDecay(kFALSE),
  fSeed(0.),
  fLHEFile(),
  fCMSEnergy(-1),
  fTune(kPerugia2011),
  fPtHardMin(0),
  fPtHardMax(1),
  fJetQA(kFALSE),
  fBeamType(kpp),
  fJetTree(kFALSE),
  fEnergyBeam1(3500),
  fEnergyBeam2(3500)
{
}

//______________________________________________________________________________
OnTheFlySimulationGenerator::OnTheFlySimulationGenerator(TString taskname) :
  fName(taskname),
  fAnalysisManager(0),
  fEvents(50000),
  fProcess(kPyMbDefault),
  fSpecialParticle(kNoSpecialParticle),
  fForceHadDecay(kFALSE),
  fSeed(0.),
  fLHEFile(),
  fCMSEnergy(-1),
  fTune(kPerugia2011),
  fPtHardMin(0),
  fPtHardMax(1),
  fJetQA(kFALSE),
  fBeamType(kpp),
  fJetTree(kFALSE),
  fEnergyBeam1(3500),
  fEnergyBeam2(3500)
{
}

//______________________________________________________________________________
OnTheFlySimulationGenerator::OnTheFlySimulationGenerator(TString taskname, Int_t numevents, Process_t proc, ESpecialParticle_t specialPart, Bool_t forceHadDecay, Int_t seed, TString lhe) :
  fName(taskname),
  fAnalysisManager(0),
  fEvents(numevents),
  fProcess(proc),
  fSpecialParticle(specialPart),
  fForceHadDecay(forceHadDecay),
  fSeed(seed),
  fLHEFile(lhe),
  fCMSEnergy(-1),
  fTune(kPerugia2011),
  fPtHardMin(0),
  fPtHardMax(1),
  fJetQA(kFALSE),
  fBeamType(kpp),
  fJetTree(kFALSE),
  fEnergyBeam1(3500),
  fEnergyBeam2(3500)
{
}

//______________________________________________________________________________
void OnTheFlySimulationGenerator::PrepareAnalysisManager()
{
  // analysis manager
  fAnalysisManager = new AliAnalysisManager(fName);

  AliAnalysisManager::SetCommonFileName(Form("AnalysisResults_%s.root",fName.Data()));

  // Dummy ESD event and ESD handler
  AliESDEvent *esdE = new AliESDEvent();
  esdE->CreateStdContent();
  esdE->GetESDRun()->GetBeamType();
  esdE->GetESDRun()->Print();
  AliESDVertex *vtx = new AliESDVertex(0.,0.,100);
  vtx->SetName("VertexTracks");
  vtx->SetTitle("VertexTracks");
  esdE->SetPrimaryVertexTracks(vtx);
  AliDummyHandler* dumH = new AliDummyHandler();
  fAnalysisManager->SetInputEventHandler(dumH);
  dumH->SetEvent(esdE);

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
  AliGenPythia* gen = CreatePythia6Gen(fBeamType, GetCMSEnergy(), fTune, fProcess, fSpecialParticle, fPtHardMin, fPtHardMax, fForceHadDecay);
  if (!fLHEFile.IsNull()) gen->SetReadLHEF(fLHEFile);

  AliMCGenHandler* mcInputHandler = new AliMCGenHandler();
  mcInputHandler->SetGenerator(gen);
  mcInputHandler->SetSeed(fSeed);
  mcInputHandler->SetSeedMode(1);
  fAnalysisManager->SetMCtruthEventHandler(mcInputHandler);

  AliEmcalMCTrackSelector* pMCTrackSel = AliEmcalMCTrackSelector::AddTaskMCTrackSelector("mcparticles",kFALSE,kFALSE,-1,kFALSE);

  if (fJetQA) AddJetQA();
  if (fBeamType == kpp) AddDJet_pp();
  else if (fBeamType == kpPb) AddDJet_pPb();
  if (fJetTree) AddJetTree();
}

//________________________________________________________________________
void OnTheFlySimulationGenerator::Start()
{
  if (!fAnalysisManager) PrepareAnalysisManager();

  if (!fAnalysisManager->InitAnalysis()) return;
  fAnalysisManager->PrintStatus();

  TFile *out = new TFile(Form("%s.root",fName.Data()),"RECREATE");
  out->cd();
  fAnalysisManager->Write();
  out->Close();
  delete out;

  if (!gAlice) new AliRun("gAlice","The ALICE Off-line Simulation Framework");

  // start analysis
  std::cout << "Starting Analysis...";
  fAnalysisManager->SetUseProgressBar(1, 25);
  fAnalysisManager->SetDebugLevel(0);
  //mgr->AddClassDebug("AliJetTriggerSelectionTask",AliLog::kDebug+100);
  fAnalysisManager->SetCacheSize(0);
  fAnalysisManager->EventLoop(fEvents);
}

//________________________________________________________________________
void OnTheFlySimulationGenerator::AddJetQA()
{
  AliAnalysisTaskEmcalJetQA* pJetQA = AliAnalysisTaskEmcalJetQA::AddTaskEmcalJetQA("mcparticles","","");
  pJetQA->SetForceBeamType(AliAnalysisTaskEmcalLight::kpp);
  pJetQA->SetMC(kTRUE);
  pJetQA->SetParticleLevel(kTRUE);
  pJetQA->SetIsPythia(kTRUE);
  pJetQA->SetVzRange(-999,999);
}

//________________________________________________________________________
void OnTheFlySimulationGenerator::AddDJet_pp()
{
  UInt_t rejectOrigin = 0;
  if (fProcess ==  kPyCharmPWHG || fProcess ==  kPyCharm) {
    rejectOrigin = AliAnalysisTaskDmesonJets::EMesonOrigin_t::kFromBottom;
  }
  else if (fProcess ==  kPyBeautyPWHG || fProcess ==  kPyBeauty) {
    rejectOrigin = AliAnalysisTaskDmesonJets::EMesonOrigin_t::kAnyOrigin & ~AliAnalysisTaskDmesonJets::EMesonOrigin_t::kFromBottom;
  }

  rejectOrigin |= AliAnalysisTaskDmesonJets::EMesonOrigin_t::kUnknownQuark;

  AliAnalysisTaskDmesonJets* pDMesonJetsTask = AliAnalysisTaskDmesonJets::AddTaskDmesonJets("", "", "usedefault", 2);
  pDMesonJetsTask->SetVzRange(-999,999);
  pDMesonJetsTask->SetIsPythia(kTRUE);
  pDMesonJetsTask->SetNeedEmcalGeom(kFALSE);
  pDMesonJetsTask->SetForceBeamType(AliAnalysisTaskEmcalLight::kpp);
  pDMesonJetsTask->SetOutputType(AliAnalysisTaskDmesonJets::kTreeOutput);
  pDMesonJetsTask->SetApplyKinematicCuts(kFALSE);
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

//________________________________________________________________________
void OnTheFlySimulationGenerator::AddDJet_pPb()
{
  // Implement user task for pPb analysis here!
  // For the moment, use pp user task
  AddDJet_pp();
}

//________________________________________________________________________
void OnTheFlySimulationGenerator::AddJetTree()
{
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
}

//________________________________________________________________________
void OnTheFlySimulationGenerator::CalculateCMSEnergy()
{
  fCMSEnergy = 2*TMath::Sqrt(fEnergyBeam1*fEnergyBeam2) / 1000; // In GeV
}

//________________________________________________________________________
AliGenPythia* OnTheFlySimulationGenerator::CreatePythia6Gen(EBeamType_t beam, Float_t e_cms, EPythiaTune_t tune, Process_t proc, ESpecialParticle_t specialPart, Int_t ptHardMin, Int_t ptHardMax, Bool_t forceHadronicDecay)
{
  Printf("PYTHIA generator with CMS energy = %.3f TeV", e_cms);

  AliGenPythia* genP = new AliGenPythia(-1);
  genP->SetTune(tune);

  // vertex position and smearing
  genP->SetVertexSmear(kPerEvent);
  genP->SetProcess(proc);

  if (ptHardMin > 0.) genP->SetPtHard(ptHardMin, ptHardMax);

  if (specialPart == kccbar) {
    genP->SetHeavyQuarkYRange(-5, 5);
  }
  else if (specialPart == kbbbar) {
    genP->SetHeavyQuarkYRange(-5, 5);
  }

  if (forceHadronicDecay) genP->SetForceDecay(kHadronicDWithout4BodiesWithV0);

  //   Center of mass energy
  genP->SetEnergyCMS(e_cms); // in GeV

  genP->UseNewMultipleInteractionsScenario(); // for all Pythia versions >= 6.3

  if (beam == kpp) {
    genP->SetProjectile("p", 1, 1);
    genP->SetTarget(    "p", 1, 1);
  }
  else if (beam == kpPb) {
    genP->SetProjectile("p",208,82);
    genP->SetTarget("p",1,1);
    //genP->SetNuclearPDF(19);
    //genP->SetUseNuclearPDF(kTRUE);
    genP->SetUseLorentzBoost(kTRUE);
  }

  // Additional settings from A. Rossi
  //genP->SetMomentumRange(0, 999999.);
  //genP->SetThetaRange(0., 180.);
  //genP->SetYRange(-12.,12.);
  //genP->SetPtRange(0,1000.);
  //genP->SetTrackingFlag(0);

  genP->Print();
  return genP;
}
