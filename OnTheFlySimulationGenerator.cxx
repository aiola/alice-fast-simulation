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
#include <AliGenPythiaPlus.h>
#include <AliESDInputHandler.h>
#include <AliAnalysisAlien.h>
#include <AliAnalysisManager.h>
#include <AliDummyHandler.h>
#include <AliMCGenHandler.h>
#include <AliRun.h>
#include <AliAnalysisTaskSE.h>
#include <AliGenCocktail.h>
#include <AliPythia8.h>
#include <AliLog.h>

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
  fPythia6Tune(kPerugia2011),
  fPythia8Tune(kMonash2013),
  fMinPtHard(-1),
  fMaxPtHard(-1),
  fBeamType(kpp),
  fJetQA(kFALSE),
  fJetTree(kFALSE),
  fDMesonJets(kFALSE),
  fEnergyBeam1(3500),
  fEnergyBeam2(3500),
  fRejectISR(kFALSE),
  fPartonEvent(kPythia6),
  fHadronization(kPythia6),
  fDecayer(kPythia6)
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
  fPythia6Tune(kPerugia2011),
  fPythia8Tune(kMonash2013),
  fMinPtHard(-1),
  fMaxPtHard(-1),
  fBeamType(kpp),
  fJetQA(kFALSE),
  fJetTree(kFALSE),
  fDMesonJets(kFALSE),
  fEnergyBeam1(3500),
  fEnergyBeam2(3500),
  fRejectISR(kFALSE),
  fPartonEvent(kPythia6),
  fHadronization(kPythia6),
  fDecayer(kPythia6)
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
  fPythia6Tune(kPerugia2011),
  fPythia8Tune(kMonash2013),
  fMinPtHard(-1),
  fMaxPtHard(-1),
  fBeamType(kpp),
  fJetQA(kFALSE),
  fJetTree(kFALSE),
  fDMesonJets(kFALSE),
  fEnergyBeam1(3500),
  fEnergyBeam2(3500),
  fRejectISR(kFALSE),
  fPartonEvent(kPythia6),
  fHadronization(kPythia6),
  fDecayer(kPythia6)
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
  if (fJetTree) AddJetTree();
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
  //fAnalysisManager->AddClassDebug("AliGenEvtGen",AliLog::kDebug+100);
  fAnalysisManager->SetCacheSize(0);
  fAnalysisManager->EventLoop(fEvents);
}

//________________________________________________________________________
void OnTheFlySimulationGenerator::AddJetQA()
{
  AliAnalysisTaskEmcalJetQA* pJetQA = AliAnalysisTaskEmcalJetQA::AddTaskEmcalJetQA("mcparticles","","");
  pJetQA->SetPtHardRange(fMinPtHard, fMaxPtHard);
  if (fMinPtHard > -1 && fMaxPtHard > fMinPtHard) pJetQA->SetMCFilter();
  pJetQA->SetJetPtFactor(4);
  pJetQA->SetForceBeamType(AliAnalysisTaskEmcalLight::kpp);
  pJetQA->SetMC(kTRUE);
  pJetQA->SetParticleLevel(kTRUE);
  pJetQA->SetIsPythia(kTRUE);
  pJetQA->SetVzRange(-999,999);
}

//________________________________________________________________________
void OnTheFlySimulationGenerator::AddDJet()
{
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
  pDMesonJetsTask->SetOutputType(AliAnalysisTaskDmesonJets::kTreeOutput);
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
  pJetSpectraTask->SetPtHardRange(fMinPtHard, fMaxPtHard);
  if (fMinPtHard > -1 && fMaxPtHard > fMinPtHard) pJetSpectraTask->SetMCFilter();
  pJetSpectraTask->SetJetPtFactor(4);
  pJetSpectraTask->SetForceBeamType(AliAnalysisTaskEmcalLight::kpp);
  pJetSpectraTask->SetVzRange(-999,999);
  pJetSpectraTask->SetIsPythia(kTRUE);
  pJetSpectraTask->SetNeedEmcalGeom(kFALSE);
  pJetSpectraTask->GetParticleContainer("mcparticles")->SetMinPt(0.);
  pJetSpectraTask->AddJetContainer(AliJetContainer::kChargedJet, AliJetContainer::antikt_algorithm, AliJetContainer::pt_scheme, 0.4, AliJetContainer::kTPCfid, "mcparticles", "");
  pJetSpectraTask->AddJetContainer(AliJetContainer::kChargedJet, AliJetContainer::antikt_algorithm, AliJetContainer::pt_scheme, 0.6, AliJetContainer::kTPCfid, "mcparticles", "");
  pJetSpectraTask->AddJetContainer(AliJetContainer::kFullJet, AliJetContainer::antikt_algorithm, AliJetContainer::pt_scheme, 0.4, AliJetContainer::kTPCfid, "mcparticles", "");
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

  Decay_t forceDecay = kAll;

  if (fForceHadDecay) {
    forceDecay = kHadronicDWithout4BodiesWithV0;
    //kHadronicDWithout4Bodies
  }

  AliGenerator* genHadronization = nullptr;

  if (fHadronization == kPythia6) {
    if (fDecayer == kPythia6) {
      genHadronization = CreatePythia6Gen(fBeamType, GetCMSEnergy(), fPythia6Tune, fProcess, fSpecialParticle, fMinPtHard, fMaxPtHard, forceDecay);
    }
    else if (fDecayer == kEvtGen) {
      // Assuming that the decayer EvtGen is used only for BEUATY
      genHadronization = CreatePythia6Gen(fBeamType, GetCMSEnergy(), fPythia6Tune, fProcess, fSpecialParticle, fMinPtHard, fMaxPtHard, kNoDecayBeauty);
    }
    else {
      AliErrorGeneralStream("OnTheFlySimulationGenerator") << "Decayer '" << fDecayer << "' not valid!!!" << std::endl;
      return nullptr;
    }
    if (!fLHEFile.IsNull() && fPartonEvent == kPowheg) static_cast<AliGenPythia*>(genHadronization)->SetReadLHEF(fLHEFile);
  }
  else if (fHadronization == kPythia8) {
    if (fDecayer == kPythia8) {
      genHadronization = CreatePythia8Gen(fBeamType, GetCMSEnergy(), fPythia8Tune, fProcess, fMinPtHard, fMaxPtHard, forceDecay);
    }
    else if (fDecayer == kEvtGen) {
      // Assuming that the decayer EvtGen is used only for BEUATY
      genHadronization = CreatePythia8Gen(fBeamType, GetCMSEnergy(), fPythia8Tune, fProcess, fMinPtHard, fMaxPtHard, kNoDecayBeauty);
    }
    else {
      AliErrorGeneralStream("OnTheFlySimulationGenerator") << "Decayer '" << fDecayer << "' not valid!!!" << std::endl;
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
      AliGenEvtGen *gene = CreateEvtGen(forceDecay, AliGenEvtGen::kBeautyPart);
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
AliGenEvtGen* OnTheFlySimulationGenerator::CreateEvtGen(Decay_t forceDecay, AliGenEvtGen::DecayOff_t decayOff)
{
  AliGenEvtGen *gene = new AliGenEvtGen();
  gene->SetForceDecay(forceDecay);
  gene->SetParticleSwitchedOff(decayOff);
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
AliGenPythia* OnTheFlySimulationGenerator::CreatePythia6Gen(EBeamType_t beam, Float_t e_cms, EPythia6Tune_t tune, Process_t proc, ESpecialParticle_t specialPart, Double_t ptHardMin, Double_t ptHardMax, Decay_t forceDecay)
{
  AliInfoGeneralStream("OnTheFlySimulationGenerator") << "PYTHIA6 generator with CMS energy = " << e_cms << " TeV" << std::endl;

  AliGenPythia* genP = new AliGenPythia(-1);
  genP->SetTune(tune);

  // vertex position and smearing
  genP->SetVertexSmear(kPerEvent);
  genP->SetProcess(proc);

  if (ptHardMin >= 0. && ptHardMax >= 0. && ptHardMax > ptHardMin) {
    genP->SetPtHard(ptHardMin, ptHardMax);
    AliInfoGeneralStream("OnTheFlySimulationGenerator") << "Setting pt hard bin limits: " << ptHardMin << ", " << ptHardMax << std::endl;
  }

  Float_t randcharge = gRandom->Rndm() > 0.5 ? -1 : 1;
  if (specialPart == kccbar) {
    genP->SetTriggerParticle(4*randcharge, 3., -1., 1000);
  }
  else if (specialPart == kbbbar) {
    genP->SetTriggerParticle(5*randcharge, 3., -1., 1000);
  }

  genP->SetHeavyQuarkYRange(-1.5, 1.5);

  genP->SetForceDecay(forceDecay);

  //   Center of mass energy
  genP->SetEnergyCMS(e_cms*1000); // in GeV

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
  genP->SetMomentumRange(0, 999999.);
  genP->SetThetaRange(0., 180.);
  genP->SetYRange(-12.,12.);
  genP->SetPtRange(0,1000.);
  //genP->SetTrackingFlag(0);

  genP->Print();
  return genP;
}

//________________________________________________________________________
AliGenPythiaPlus* OnTheFlySimulationGenerator::CreatePythia8Gen(EBeamType_t beam, Float_t e_cms, EPythia8Tune_t tune, Process_t proc, Double_t ptHardMin, Double_t ptHardMax, Decay_t forceDecay)
{
  AliInfoGeneralStream("OnTheFlySimulationGenerator") << "PYTHIA8 generator with CMS energy = " << e_cms << " TeV" << std::endl;

  AliGenPythiaPlus* genP = new AliGenPythiaPlus(AliPythia8::Instance());
  genP->SetTune(tune);

  // vertex position and smearing
  genP->SetVertexSmear(kPerEvent);
  genP->SetProcess(proc);

  if (ptHardMin >= 0. && ptHardMax >= 0. && ptHardMax > ptHardMin) {
    genP->SetPtHard(ptHardMin, ptHardMax);
    AliInfoGeneralStream("OnTheFlySimulationGenerator") << "Setting pt hard bin limits: " << ptHardMin << ", " << ptHardMax << std::endl;
  }

  genP->SetForceDecay(forceDecay);

  //   Center of mass energy
  genP->SetEnergyCMS(e_cms*1000); // in GeV

  //genP->UseNewMultipleInteractionsScenario(); // for all Pythia versions >= 6.3 BUT DO NOT USE FOR PYTHIA 8!

  if (beam == kpp) {
    genP->SetProjectile("p", 1, 1);
    genP->SetTarget(    "p", 1, 1);
  }
  else if (beam == kpPb) {
    genP->SetProjectile("p",208,82);
    genP->SetTarget("p",1,1);
  }

  genP->Print();
  return genP;
}
