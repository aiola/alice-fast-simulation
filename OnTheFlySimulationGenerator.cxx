// runJetSimulation.C

// std library
#include <iostream>
#include <set>

// Root classes
#include <TSystem.h>
#include <TArrayI.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TMath.h>

// AliRoot classes
#include <AliESDInputHandler.h>
#include <AliAnalysisManager.h>
#include <AliDummyHandler.h>
#include <AliMCGenHandler.h>
#include <AliRun.h>
#include <AliAnalysisTaskSE.h>
#include <AliGenCocktail.h>
#include <AliLog.h>

// AliPhysics classes
#include <AliAnalysisTaskDmesonJets.h>
#include <AliEmcalJetTask.h>
#include <AliAnalysisTaskEmcalJetTree.h>
#include <AliAnalysisTaskEmcalJetQA.h>
#include <AliEmcalMCTrackSelector.h>

#include "AliPythia6_dev.h"
#include "AliPythia8_dev.h"
#include "AliGenReaderHepMC_dev.h"
#include <AliAnalysisHFCorrOnFlySim.h>
#include <AliAnalysisDataContainer.h>

#include "OnTheFlySimulationGenerator.h"

//______________________________________________________________________________
OnTheFlySimulationGenerator::OnTheFlySimulationGenerator() :
  fName("FastSim"),
  fAnalysisManager(0),
  fEvents(50000),
  fProcess(kPyMbDefault),
  fSpecialParticle(kNoSpecialParticle),
  fSeed(0.),
  fLHEFile(),
  fHepMCFile(),
  fCMSEnergy(-1),
  fPythia6Tune(AliGenPythia_dev::kPerugia2011),
  fPythia8Tune(AliGenPythia_dev::kMonash2013),
  fMinPtHard(-1),
  fMaxPtHard(-1),
  fBeamType(kpp),
  fJetQA(kFALSE),
  fJetTree(kFALSE),
  fDMesonJets(kFALSE),
  fDMesonHadrCorr(kFALSE),
  fEnergyBeam1(3500),
  fEnergyBeam2(3500),
  fRejectISR(kFALSE),
  fPartonEvent(kPythia6),
  fHadronization(kPythia6),
  fDecayer(kPythia6),
  fExtendedEventInfo(kFALSE),
  fDebugClassNames({"AliGenPythia_dev", "AliPythia6_dev", "AliPythia8_dev", 
  "AliGenEvtGen_dev", "AliGenExtFile_dev", "AliGenReaderHepMC_dev", "THepMCParser_dev",
  "AliMCGenHandler", 
  "AliAnalysisTaskEmcalJetQA", "AliAnalysisTaskDmesonJets", "AliEmcalJetTask", "AliAnalysisHFCorrOnFlySim", 
  "AliAnalysisTaskEmcalJetTree<AliEmcalJetInfoSummaryPP, AliEmcalJetEventInfoSummaryPP>", 
  "AliAnalysisTaskDmesonJets::AnalysisEngine"})
{
}

//______________________________________________________________________________
OnTheFlySimulationGenerator::OnTheFlySimulationGenerator(TString taskname) :
  fName(taskname),
  fAnalysisManager(0),
  fEvents(50000),
  fProcess(kPyMbDefault),
  fSpecialParticle(kNoSpecialParticle),
  fSeed(0.),
  fLHEFile(),
  fHepMCFile(),
  fCMSEnergy(-1),
  fPythia6Tune(AliGenPythia_dev::kPerugia2011),
  fPythia8Tune(AliGenPythia_dev::kMonash2013),
  fMinPtHard(-1),
  fMaxPtHard(-1),
  fBeamType(kpp),
  fJetQA(kFALSE),
  fJetTree(kFALSE),
  fDMesonJets(kFALSE),
  fDMesonHadrCorr(kFALSE),
  fEnergyBeam1(3500),
  fEnergyBeam2(3500),
  fRejectISR(kFALSE),
  fPartonEvent(kPythia6),
  fHadronization(kPythia6),
  fDecayer(kPythia6),
  fExtendedEventInfo(kFALSE),
  fDebugClassNames({"AliGenPythia_dev", "AliPythia6_dev", "AliPythia8_dev", 
  "AliGenEvtGen_dev", "AliGenExtFile_dev", "AliGenReaderHepMC_dev", "THepMCParser_dev",
  "AliMCGenHandler", 
  "AliAnalysisTaskEmcalJetQA", "AliAnalysisTaskDmesonJets", "AliEmcalJetTask", "AliAnalysisHFCorrOnFlySim",
  "AliAnalysisTaskEmcalJetTree<AliEmcalJetInfoSummaryPP, AliEmcalJetEventInfoSummaryPP>", 
  "AliAnalysisTaskDmesonJets::AnalysisEngine"})
{
}

//______________________________________________________________________________
OnTheFlySimulationGenerator::OnTheFlySimulationGenerator(TString taskname, Int_t numevents, Process_t proc, ESpecialParticle_t specialPart, Bool_t forceHadDecay, Int_t seed, TString lhe, TString hep) :
  fName(taskname),
  fAnalysisManager(0),
  fEvents(numevents),
  fProcess(proc),
  fSpecialParticle(specialPart),
  fSeed(seed),
  fLHEFile(lhe),
  fHepMCFile(hep),
  fCMSEnergy(-1),
  fPythia6Tune(AliGenPythia_dev::kPerugia2011),
  fPythia8Tune(AliGenPythia_dev::kMonash2013),
  fMinPtHard(-1),
  fMaxPtHard(-1),
  fBeamType(kpp),
  fJetQA(kFALSE),
  fJetTree(kFALSE),
  fDMesonJets(kFALSE),
  fDMesonHadrCorr(kFALSE),
  fEnergyBeam1(3500),
  fEnergyBeam2(3500),
  fRejectISR(kFALSE),
  fPartonEvent(kPythia6),
  fHadronization(kPythia6),
  fDecayer(kPythia6),
  fExtendedEventInfo(kFALSE),
  fDebugClassNames({"AliGenPythia_dev", "AliPythia6_dev", "AliPythia8_dev", "AliGenEvtGen_dev", "AliGenPythia", "AliPythia", "AliPythia8", "AliGenEvtGen", "AliMCGenHandler", "AliEmcalMCTrackSelector", "AliAnalysisTaskEmcalJetQA", "AliAnalysisTaskDmesonJets", "AliEmcalJetTask", "AliAnalysisHFCorrOnFlySim", "AliAnalysisTaskEmcalJetTree<AliEmcalJetInfoSummaryPP, AliEmcalJetEventInfoSummaryPP>"})
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
  AliGenerator* gen = CreateGenerator();

  AliMCGenHandler* mcInputHandler = new AliMCGenHandler();
  mcInputHandler->SetGenerator(gen);
  mcInputHandler->SetSeed(fSeed);
  mcInputHandler->SetSeedMode(1);
  fAnalysisManager->SetMCtruthEventHandler(mcInputHandler);

  AliEmcalMCTrackSelector* pMCTrackSel = AliEmcalMCTrackSelector::AddTaskMCTrackSelector("mcparticles",kFALSE,kFALSE,-1,kFALSE);

  if (fJetQA) AddJetQA();
  if (fDMesonJets) AddDJet();
  if (fDMesonHadrCorr) AddDhcorr();
  if (fJetTree) {
    if (fDMesonJets) {
      TString fname(AliAnalysisManager::GetCommonFileName());
      fname.ReplaceAll(".root", "_jets.root");
      AddJetTree(fname);
    }
    else {
      AddJetTree();
    }
  }
}

//________________________________________________________________________
void OnTheFlySimulationGenerator::Start(UInt_t debug_level)
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
  fAnalysisManager->SetDebugLevel(debug_level);
  if (debug_level > 5) {
    for (auto cname : fDebugClassNames) fAnalysisManager->AddClassDebug(cname.c_str(), AliLog::kDebug+debug_level);
  }
  fAnalysisManager->SetCacheSize(0);
  fAnalysisManager->EventLoop(fEvents);
}

//________________________________________________________________________
void OnTheFlySimulationGenerator::AddJetQA(const char* file_name)
{
  TString fname(file_name);
  TString old_file_name;
  if (!fname.IsNull()) {
    old_file_name = AliAnalysisManager::GetCommonFileName();
    AliAnalysisManager::SetCommonFileName(fname);
  }
  AliAnalysisTaskEmcalJetQA* pJetQA = AliAnalysisTaskEmcalJetQA::AddTaskEmcalJetQA("mcparticles","","");
  pJetQA->SetPtHardRange(fMinPtHard, fMaxPtHard);
  if (fMinPtHard > -1 && fMaxPtHard > fMinPtHard) pJetQA->SetMCFilter();
  pJetQA->SetJetPtFactor(4);
  pJetQA->SetForceBeamType(AliAnalysisTaskEmcalLight::kpp);
  pJetQA->SetCentralityEstimation(AliAnalysisTaskEmcalLight::kNoCentrality);
  pJetQA->SetMC(kTRUE);
  pJetQA->SetParticleLevel(kTRUE);
  pJetQA->SetIsPythia(kTRUE);
  pJetQA->SetVzRange(-999,999);
  if (!fname.IsNull()) {
    AliAnalysisManager::SetCommonFileName(old_file_name);
  }
}

//________________________________________________________________________
void OnTheFlySimulationGenerator::AddDJet(const char* file_name)
{
  TString fname(file_name);
  TString old_file_name;
  if (!fname.IsNull()) {
    old_file_name = AliAnalysisManager::GetCommonFileName();
    AliAnalysisManager::SetCommonFileName(fname);
  }

  UInt_t rejectOrigin = 0;

  AliAnalysisTaskDmesonJets* pDMesonJetsTask = AliAnalysisTaskDmesonJets::AddTaskDmesonJets("", "", "usedefault", 2);
  pDMesonJetsTask->SetVzRange(-999,999);
  pDMesonJetsTask->SetPtHardRange(fMinPtHard, fMaxPtHard);
  if (fMinPtHard > -1 && fMaxPtHard > fMinPtHard) pDMesonJetsTask->SetMCFilter();
  pDMesonJetsTask->SetJetPtFactor(4);
  pDMesonJetsTask->SetIsPythia(kTRUE);
  pDMesonJetsTask->SetNeedEmcalGeom(kFALSE);
  pDMesonJetsTask->SetForceBeamType(AliAnalysisTaskEmcalLight::kpp);
  pDMesonJetsTask->SetCentralityEstimation(AliAnalysisTaskEmcalLight::kNoCentrality);
  if (fExtendedEventInfo) {
    pDMesonJetsTask->SetOutputType(AliAnalysisTaskDmesonJets::kTreeExtendedOutput);
  }
  else {
    pDMesonJetsTask->SetOutputType(AliAnalysisTaskDmesonJets::kTreeOutput);
  }
  pDMesonJetsTask->SetApplyKinematicCuts(kTRUE);
  pDMesonJetsTask->SetRejectISR(fRejectISR);
  AliAnalysisTaskDmesonJets::AnalysisEngine* eng = 0;
  eng = pDMesonJetsTask->AddAnalysisEngine(AliAnalysisTaskDmesonJets::kD0toKpi, "", "", AliAnalysisTaskDmesonJets::kMCTruth, AliJetContainer::kChargedJet, 0.4);
  eng->SetAcceptedDecayMap(AliAnalysisTaskDmesonJets::EMesonDecayChannel_t::kAnyDecay);
  eng->SetRejectedOriginMap(rejectOrigin);
  eng = pDMesonJetsTask->AddAnalysisEngine(AliAnalysisTaskDmesonJets::kD0toKpi, "", "", AliAnalysisTaskDmesonJets::kMCTruth, AliJetContainer::kChargedJet, 0.6);
  eng->SetAcceptedDecayMap(AliAnalysisTaskDmesonJets::EMesonDecayChannel_t::kAnyDecay);
  eng->SetRejectedOriginMap(rejectOrigin);
  eng = pDMesonJetsTask->AddAnalysisEngine(AliAnalysisTaskDmesonJets::kD0toKpi, "", "", AliAnalysisTaskDmesonJets::kMCTruth, AliJetContainer::kFullJet, 0.4);
  eng->SetAcceptedDecayMap(AliAnalysisTaskDmesonJets::EMesonDecayChannel_t::kAnyDecay);
  eng->SetRejectedOriginMap(rejectOrigin);
  eng = pDMesonJetsTask->AddAnalysisEngine(AliAnalysisTaskDmesonJets::kDstartoKpipi, "", "", AliAnalysisTaskDmesonJets::kMCTruth, AliJetContainer::kChargedJet, 0.4);
  eng->SetAcceptedDecayMap(AliAnalysisTaskDmesonJets::EMesonDecayChannel_t::kAnyDecay);
  eng->SetRejectedOriginMap(rejectOrigin);
  eng = pDMesonJetsTask->AddAnalysisEngine(AliAnalysisTaskDmesonJets::kDstartoKpipi, "", "", AliAnalysisTaskDmesonJets::kMCTruth, AliJetContainer::kChargedJet, 0.6);
  eng->SetAcceptedDecayMap(AliAnalysisTaskDmesonJets::EMesonDecayChannel_t::kAnyDecay);
  eng->SetRejectedOriginMap(rejectOrigin);
  eng = pDMesonJetsTask->AddAnalysisEngine(AliAnalysisTaskDmesonJets::kDstartoKpipi, "", "", AliAnalysisTaskDmesonJets::kMCTruth, AliJetContainer::kFullJet, 0.4);
  eng->SetAcceptedDecayMap(AliAnalysisTaskDmesonJets::EMesonDecayChannel_t::kAnyDecay);
  eng->SetRejectedOriginMap(rejectOrigin);

  if (!fname.IsNull()) {
    AliAnalysisManager::SetCommonFileName(old_file_name);
  }
}

//________________________________________________________________________
void OnTheFlySimulationGenerator::AddDhcorr(const char* file_name)
{
  TString fname(file_name);
  TString old_file_name;
  if (!fname.IsNull()) {
    old_file_name = AliAnalysisManager::GetCommonFileName();
    AliAnalysisManager::SetCommonFileName(old_file_name);
  }

   AliAnalysisHFCorrOnFlySim* clus = new  AliAnalysisHFCorrOnFlySim("");
   clus->SetEtaRange(-20.0, 20.0);
   clus->SetPtRange(0.3, 1000.0);
   clus->SetYRange(-20., 20.);
   clus->SetMultRange(0,5000);
   clus->SetEventProperties(kTRUE);
   clus->SetPartProperties(kTRUE);
   clus->SetHFCorrelations(kTRUE);
   clus->SetHHCorrelations(kFALSE);
   clus->SetQQbarCorrBetween("c", 1, "c", -1);
   clus->SetQQbarCorrelations(kTRUE);

/* DISABLED WEIGHTING
   if (useweights) {
     TFile *f = TFile::Open(namefile.Data());
     TH1D* hwgt = (TH1D*)f->Get(namehistowgt.Data());
     clus->SetUsePtWeights(kTRUE);
     clus->SetHistoWeights(hwgt);
   }
*/

   AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
   if (!mgr) {
     Printf("AliAnalysisHFCorrOnFlySim, No analysis manager to connect to.");
     return;
   }
  
   if(!mgr->GetMCtruthEventHandler()){
     Printf("AliAnalysisHFCorrOnFlySim; This task requires an input MC event handler");
     return;
   }
   
   mgr->AddTask(clus);
  
  // Create containers for input/output
  TString finDirname   = "DhTemplate";
  TString inname       = "cinput";
  TString outBasic     = "BasicPlots";
  TString Specific     = "Specific";
  
  inname           +=   finDirname.Data();
  outBasic         +=   finDirname.Data();
  Specific         +=   finDirname.Data();
   

  //Input and Output Slots:
  AliAnalysisDataContainer *cinputSim = mgr->CreateContainer(inname,TChain::Class(), AliAnalysisManager::kInputContainer);
  TString outputfile = AliAnalysisManager::GetCommonFileName();
  outputfile += ":KineSimulations";
  //TString outputfile = "AnaKineResults.root";

  AliAnalysisDataContainer *coutputSim1 = mgr->CreateContainer(outBasic,TList::Class(),AliAnalysisManager::kOutputContainer,outputfile.Data());
  AliAnalysisDataContainer *coutputSim2 = mgr->CreateContainer(Specific,TList::Class(),AliAnalysisManager::kOutputContainer,outputfile.Data());

  mgr->ConnectInput(clus,0,mgr->GetCommonInputContainer());
  mgr->ConnectOutput(clus,1,coutputSim1);
  mgr->ConnectOutput(clus,2,coutputSim2);

}


//________________________________________________________________________
void OnTheFlySimulationGenerator::AddJetTree(const char* file_name)
{
  TString fname(file_name);
  TString old_file_name;
  if (!fname.IsNull()) {
    old_file_name = AliAnalysisManager::GetCommonFileName();
    AliAnalysisManager::SetCommonFileName(fname);
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

  AliAnalysisTaskEmcalJetTreeBase* pJetSpectraTask = nullptr;
  if (fExtendedEventInfo) {
    pJetSpectraTask = AliAnalysisTaskEmcalJetTreeBase::AddTaskEmcalJetTree("mcparticles", "", 0, 0, AliAnalysisTaskEmcalJetTreeBase::kJetPPChargedSimulation);
  }
  else {
    pJetSpectraTask = AliAnalysisTaskEmcalJetTreeBase::AddTaskEmcalJetTree("mcparticles", "", 0, 0, AliAnalysisTaskEmcalJetTreeBase::kJetPPCharged);
  }
  pJetSpectraTask->SetPtHardRange(fMinPtHard, fMaxPtHard);
  pJetSpectraTask->SetCentralityEstimation(AliAnalysisTaskEmcalLight::kNoCentrality);
  if (fMinPtHard > -1 && fMaxPtHard > fMinPtHard) {
    pJetSpectraTask->SetMCFilter();
    pJetSpectraTask->SetJetPtFactor(4);
  }
  pJetSpectraTask->SetForceBeamType(AliAnalysisTaskEmcalLight::kpp);
  pJetSpectraTask->SetVzRange(-999,999);
  pJetSpectraTask->SetIsPythia(kTRUE);
  pJetSpectraTask->SetNeedEmcalGeom(kFALSE);
  pJetSpectraTask->GetParticleContainer("mcparticles")->SetMinPt(0.);
  pJetSpectraTask->AddJetContainer(AliJetContainer::kChargedJet, AliJetContainer::antikt_algorithm, AliJetContainer::pt_scheme, 0.4, AliJetContainer::kTPCfid, "mcparticles", "");
  pJetSpectraTask->AddJetContainer(AliJetContainer::kChargedJet, AliJetContainer::antikt_algorithm, AliJetContainer::pt_scheme, 0.6, AliJetContainer::kTPCfid, "mcparticles", "");
  pJetSpectraTask->AddJetContainer(AliJetContainer::kFullJet, AliJetContainer::antikt_algorithm, AliJetContainer::pt_scheme, 0.4, AliJetContainer::kTPCfid, "mcparticles", "");

  if (!fname.IsNull()) {
    AliAnalysisManager::SetCommonFileName(old_file_name);
  }
}

//________________________________________________________________________
void OnTheFlySimulationGenerator::CalculateCMSEnergy()
{
  fCMSEnergy = 2*TMath::Sqrt(fEnergyBeam1*fEnergyBeam2) / 1000; // In GeV
}

//________________________________________________________________________
AliGenerator* OnTheFlySimulationGenerator::CreateGenerator()
{
  AliGenerator* gen = nullptr;

  AliGenMC* genHadronization = nullptr;

  if (fHadronization == kPythia6) {
    genHadronization = CreatePythia6Gen(fBeamType, GetCMSEnergy(), fPartonEvent, fLHEFile, fPythia6Tune, fProcess, fSpecialParticle, fMinPtHard, fMaxPtHard);
    if (fDecayer != kPythia6 && fDecayer != kEvtGen) {
      AliErrorGeneralStream("OnTheFlySimulationGenerator") << "Decayer '" << fDecayer << "' not valid!!!" << std::endl;
      return nullptr;
    }
  }
  else if (fHadronization == kPythia8) {
    genHadronization = CreatePythia8Gen(fBeamType, GetCMSEnergy(), fPartonEvent, fLHEFile, fPythia8Tune, fProcess, fSpecialParticle, fMinPtHard, fMaxPtHard);
    if (fDecayer != kPythia8 && fDecayer != kEvtGen) {
      AliErrorGeneralStream("OnTheFlySimulationGenerator") << "Decayer '" << fDecayer << "' not valid!!!" << std::endl;
      return nullptr;
    }
  }
  else if (fHadronization == kHerwig7) {
    genHadronization = CreateHerwig7Gen(fBeamType, GetCMSEnergy(), fHepMCFile, fSpecialParticle);
    if (fDecayer != kHerwig7) {
      AliErrorGeneralStream("OnTheFlySimulationGenerator") << "When using Herwig 7 it is not allowed to attach an external decayer!!!" << std::endl;
      return nullptr;
    }
  }
  else {
    AliErrorGeneralStream("OnTheFlySimulationGenerator") << "Hadronizator '" << fHadronization << "' not valid!!!" << std::endl;
    return nullptr;
  }

  if (fHadronization != fDecayer) {
    // Need a cocktail generator
    AliGenCocktail *cocktail = CreateCocktailGen(fBeamType, GetCMSEnergy());
    cocktail->AddGenerator(genHadronization, "MC_pythia", 1.);

    if (fDecayer == kEvtGen) {
      // Assuming you want to use EvtGen to decay beauty
      std::set<int> B_hadrons = {511,521,531,5122,5132,5232,5332};
      static_cast<AliGenPythia_dev*>(genHadronization)->SetDecayOff(B_hadrons);
      AliGenEvtGen_dev *gene = CreateEvtGen(AliGenEvtGen_dev::kBeautyPart);
      cocktail->AddGenerator(gene,"MC_evtGen", 1.);
    }
    else {
      AliErrorGeneralStream("OnTheFlySimulationGenerator") << "Decayer '" << fDecayer << "' not valid!!!" << std::endl;
      return nullptr;
    }
    gen = cocktail;
  }
  else {
    gen = genHadronization;
  }

  return gen;
}

//________________________________________________________________________
AliGenEvtGen_dev* OnTheFlySimulationGenerator::CreateEvtGen(AliGenEvtGen_dev::DecayOff_t decayOff)
{
  AliGenEvtGen_dev *gene = new AliGenEvtGen_dev();
  gene->SetParticleSwitchedOff(decayOff);
  //gene->SetUserDecayTable("./DECAY_fix.DEC");
  return gene;
}

//________________________________________________________________________
AliGenCocktail* OnTheFlySimulationGenerator::CreateCocktailGen(EBeamType_t beam, Float_t e_cms)
{
  AliGenCocktail *cocktail = new AliGenCocktail();
  if (beam == kpp) {
    cocktail->SetProjectile("p", 1, 1);
    cocktail->SetTarget(    "p", 1, 1);
  }
  else if (beam == kpPb) {
    cocktail->SetProjectile("p",208,82);
    cocktail->SetTarget("p",1,1);
  }

  cocktail->SetVertexSmear(kPerEvent);
  cocktail->SetEnergyCMS(e_cms*1000);

  // Additional settings from A. Rossi
  cocktail->SetMomentumRange(0, 999999.);
  cocktail->SetThetaRange(0., 180.);
  cocktail->SetYRange(-12.,12.);
  cocktail->SetPtRange(0,1000.);

  return cocktail;
}

//________________________________________________________________________
AliGenPythia_dev* OnTheFlySimulationGenerator::CreatePythia6Gen(EBeamType_t beam, Float_t e_cms, EGenerator_t partonEvent, TString lhe, EPythiaTune_t tune, Process_t proc, ESpecialParticle_t specialPart, Double_t ptHardMin, Double_t ptHardMax)
{
  AliInfoGeneralStream("OnTheFlySimulationGenerator") << "PYTHIA6 generator with CMS energy = " << e_cms << " TeV" << std::endl;

  AliPythia6_dev* pythia6 = new AliPythia6_dev();

  AliGenPythia_dev* genP = new AliGenPythia_dev(pythia6);
  genP->SetTune(tune);

  if (!lhe.IsNull() && partonEvent == kPowheg) {
    AliInfoGeneralStream("OnTheFlySimulationGenerator") << "Setting LHE file '" << lhe.Data() << "'" << std::endl;
    genP->SetLHEFile(lhe);
  }

  // vertex position and smearing
  genP->SetVertexSmear(kPerEvent);
  genP->SetProcess(proc);

  if (ptHardMin >= 0. && ptHardMax >= 0. && ptHardMax > ptHardMin) {
    genP->SetPtHard(ptHardMin, ptHardMax);
    AliInfoGeneralStream("OnTheFlySimulationGenerator") << "Setting pt hard bin limits: " << ptHardMin << ", " << ptHardMax << std::endl;
  }
  else if (proc == kPyJets) {
    genP->SetPtHard(5, 1e3);
    AliWarningGeneralStream("OnTheFlySimulationGenerator") << "kPyJets process selected but not pt hard limits: setting pt hard bin limits: " << 5 << ", " << 1e3 << std::endl;
  }

  if (specialPart == kccbar) {
    genP->SetTriggerParticle(4, -1, -1, -10, 10);
  }
  else if (specialPart == kbbbar) {
    genP->SetTriggerParticle(5, -1, -1, -10, 10);
  }

  //   Center of mass energy
  genP->SetEnergyCMS(e_cms*1000); // in GeV

  if (beam == kpp) {
    genP->SetProjectile("p", 1, 1);
    genP->SetTarget(    "p", 1, 1);
  }
  else if (beam == kpPb) {
    genP->SetProjectile("p",208,82);
    genP->SetTarget("p",1,1);
    genP->SetNuclearPDF(19);
    genP->SetUseNuclearPDF(kTRUE);
    genP->SetUseLorentzBoost(kTRUE);
  }

  genP->Print();
  return genP;
}

//________________________________________________________________________
AliGenPythia_dev* OnTheFlySimulationGenerator::CreatePythia8Gen(EBeamType_t beam, Float_t e_cms, EGenerator_t partonEvent, TString lhe, EPythiaTune_t tune, Process_t proc, ESpecialParticle_t specialPart, Double_t ptHardMin, Double_t ptHardMax)
{
  AliInfoGeneralStream("OnTheFlySimulationGenerator") << "PYTHIA8 generator with CMS energy = " << e_cms << " TeV" << std::endl;

  AliPythia8_dev* pythia8 = new AliPythia8_dev();

  AliGenPythia_dev* genP = new AliGenPythia_dev(pythia8);
  genP->SetTune(tune);

  if (!lhe.IsNull() && partonEvent == kPowheg) {
    AliInfoGeneralStream("OnTheFlySimulationGenerator") << "Setting LHE file '" << lhe.Data() << "'" << std::endl;
    genP->SetLHEFile(lhe);
  }

  // vertex position and smearing
  genP->SetVertexSmear(kPerEvent);
  genP->SetProcess(proc);

  if (ptHardMin >= 0. && ptHardMax >= 0. && ptHardMax > ptHardMin) {
    genP->SetPtHard(ptHardMin, ptHardMax);
    AliInfoGeneralStream("OnTheFlySimulationGenerator") << "Setting pt hard bin limits: " << ptHardMin << ", " << ptHardMax << std::endl;
  }
  else if (proc == kPyJets) {
    genP->SetPtHard(5, 1e3);
    AliWarningGeneralStream("OnTheFlySimulationGenerator") << "kPyJets process selected but not pt hard limits: setting pt hard bin limits: " << 5 << ", " << 1e3 << std::endl;
  }

  if (specialPart == kccbar) {
    genP->SetTriggerParticle(4, -1, -1, -10, 10);
  }
  else if (specialPart == kbbbar) {
    genP->SetTriggerParticle(5, -1, -1, -10, 10);
  }

  //   Center of mass energy
  genP->SetEnergyCMS(e_cms*1000); // in GeV

  if (beam == kpp) {
    genP->SetProjectile("p", 1, 1);
    genP->SetTarget(    "p", 1, 1);
  }
  else if (beam == kpPb) {
    genP->SetProjectile("p",208,82);
    genP->SetTarget("p",1,1);
    genP->SetNuclearPDF(19);
    genP->SetUseNuclearPDF(kTRUE);
    genP->SetUseLorentzBoost(kTRUE);
  }

  genP->Print();
  return genP;
}

AliGenExtFile_dev* OnTheFlySimulationGenerator::CreateHerwig7Gen(EBeamType_t beam, Float_t e_cms, TString hep, ESpecialParticle_t /*specialPart*/)
{
  AliGenExtFile_dev* gen = new AliGenExtFile_dev(-1);  // -1 read all particles
  gen->SetFileName(hep.Data());
  gen->SetReader(new AliGenReaderHepMC_dev());

  //   Center of mass energy
  gen->SetEnergyCMS(e_cms*1000); // in GeV

  if (beam == kpp) {
    gen->SetProjectile("p", 1, 1);
    gen->SetTarget(    "p", 1, 1);
  }
  else if (beam == kpPb) {
    gen->SetProjectile("p",208,82);
    gen->SetTarget("p",1,1);
  }

  return gen;
}
