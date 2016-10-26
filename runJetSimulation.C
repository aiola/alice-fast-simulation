// runJetSimulation.C

// std library
#include <iostream>

// Root classes
#include <TSystem.h>
#include <TString.h>
#include <TArrayI.h>
#include <TChain.h>
#include <TFile.h>

#include <PythiaProcesses.h>
#include <AliGenPythia.h>
#include <AliESDInputHandler.h>
#include <AliAnalysisAlien.h>
#include <AliAnalysisManager.h>

#include <AliAnalysisTaskDmesonJets.h>
#include <AliEmcalJetTask.h>
#include <AliAnalysisTaskEmcalJetTree.h>
#include <AliAnalysisTaskEmcalJetQA.h>

#include "AliFastSimulationTask.h"

AliAnalysisGrid* CreateAlienHandler(const char *taskname, const char *gridmode);

//______________________________________________________________________________
void runJetSimulation(
    const char   *runtype      = "local",                     // local or grid
    const char   *gridmode     = "offline",                   // set the grid run mode (can be "full", "test", "offline", "submit" or "terminate")
    const Int_t   numevents    = 50000,                       // number of events
    Process_t     proc         = kPyMb,
    Int_t         specialPart  = 0,
    Bool_t        forceHadDecay=kFALSE,
    const char   *taskname     = "FastSim"                    // sets name of grid generated macros
  )
{
  //gSystem->SetFPEMask(TSystem::kInvalid | TSystem::kDivByZero | TSystem::kOverflow | TSystem::kUnderflow);

  enum eRunType  { kLocal, kGrid, kYaleHEP };

  eRunType rType;
  if (!strcmp(runtype, "grid")) 
    rType = kGrid;
  else if (!strcmp(runtype, "local")) 
    rType = kLocal;
  else if (!strcmp(runtype, "hep"))
    rType = kYaleHEP;
  else {
    std::cout << "Incorrect run option, check first argument of run macro" << std::endl;
    std::cout << "runtype = local, grid, hep" << std::endl;
    return;
  }

  std::cout << runtype << " analysis chosen" << std::endl;

  // analysis manager
  AliAnalysisManager* mgr = new AliAnalysisManager(taskname);

  AliESDInputHandler* esdH = AliAnalysisTaskEmcal::AddESDHandler();

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
  AliGenPythia* gen = AliFastSimulationTask::CreatePythia6Gen(7000., AliFastSimulationTask::kPerugia2012, proc,
      (AliFastSimulationTask::ESpecialParticle_t)specialPart, 0, 1, forceHadDecay);
  if (proc == kPyJetsPWHG || proc ==  kPyCharmPWHG || proc ==  kPyBeautyPWHG || proc ==  kPyWPWHG) {
    gen->SetReadLHEF("pwgevents.lhe");
  }

  AliFastSimulationTask::AddTaskFastSimulation(gen);
  //AddTaskHFexploration();

  AliAnalysisTaskEmcalJetQA* pJetQA = AliAnalysisTaskEmcalJetQA::AddTaskEmcalJetQA("mcparticles","","");
  pJetQA->SetForceBeamType(AliAnalysisTaskEmcalLight::kpp);
  pJetQA->SetMC(kTRUE);
  pJetQA->SetParticleLevel(kTRUE);
  pJetQA->SetVzRange(-999,999);

  AliAnalysisTaskDmesonJets* pDMesonJetsTask = AliAnalysisTaskDmesonJets::AddTaskDmesonJets("", "", "usedefault", 2);
  pDMesonJetsTask->SetVzRange(-999,999);
  pDMesonJetsTask->SetNeedEmcalGeom(kFALSE);
  pDMesonJetsTask->SetForceBeamType(AliAnalysisTaskEmcalLight::kpp);
  pDMesonJetsTask->SetOutputType(AliAnalysisTaskDmesonJets::kTreeOutput);
  pDMesonJetsTask->AddAnalysisEngine(AliAnalysisTaskDmesonJets::kD0toKpi, AliAnalysisTaskDmesonJets::kMCTruth, AliJetContainer::kChargedJet, 0.4);
  pDMesonJetsTask->AddAnalysisEngine(AliAnalysisTaskDmesonJets::kD0toKpi, AliAnalysisTaskDmesonJets::kMCTruth, AliJetContainer::kChargedJet, 0.6);
  pDMesonJetsTask->AddAnalysisEngine(AliAnalysisTaskDmesonJets::kD0toKpi, AliAnalysisTaskDmesonJets::kMCTruth, AliJetContainer::kFullJet, 0.4);
  pDMesonJetsTask->AddAnalysisEngine(AliAnalysisTaskDmesonJets::kDstartoKpipi, AliAnalysisTaskDmesonJets::kMCTruth, AliJetContainer::kChargedJet, 0.4);
  pDMesonJetsTask->AddAnalysisEngine(AliAnalysisTaskDmesonJets::kDstartoKpipi, AliAnalysisTaskDmesonJets::kMCTruth, AliJetContainer::kChargedJet, 0.6);
  pDMesonJetsTask->AddAnalysisEngine(AliAnalysisTaskDmesonJets::kDstartoKpipi, AliAnalysisTaskDmesonJets::kMCTruth, AliJetContainer::kFullJet, 0.4);

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

  if (rType == kGrid) {
    AliAnalysisGrid *plugin = CreateAlienHandler(taskname, gridmode); 
    mgr->SetGridHandler(plugin);

    // start analysis
    std::cout << "Starting Analysis...";
    mgr->SetDebugLevel(0);
    mgr->StartAnalysis("grid");
  }
  else {  // local, hep

    Int_t numFiles = numevents / 10000 + 1;
    std::cout << "Number of files: " << numFiles << std::endl;

    TChain* chain = new TChain("esdTree");
    for (Int_t i = 0; i < numFiles; i++) chain->Add("esdempty.root");

    // start analysis
    std::cout << "Starting Analysis...";
    mgr->SetUseProgressBar(1, 25);
    mgr->SetDebugLevel(0);
    //mgr->AddClassDebug("AliJetTriggerSelectionTask",AliLog::kDebug+100);
    mgr->StartAnalysis("local", chain, numevents);
  }
}

//______________________________________________________________________________
AliAnalysisGrid* CreateAlienHandler(const char *taskname, const char *gridmode)
{
  AliAnalysisAlien *plugin = new AliAnalysisAlien();
  plugin->SetRunMode(gridmode);
  
  // Set versions of used packages
  plugin->SetAliPhysicsVersion("vAN-20161012");

  //plugin->SetMergeViaJDL(kTRUE);

  // Declare input data to be processed.
  plugin->AddDataFile("/alice/cern.ch/user/s/saiola/esdempty.root");
  
  // Define alien work directory where all files will be copied. Relative to alien $HOME.
  plugin->SetGridWorkingDir(taskname);
  
  // Declare alien output directory. Relative to working directory.
  plugin->SetGridOutputDir("out"); // In this case will be $HOME/taskname/out
  
  //plugin->AddIncludePath("-I$ALICE_ROOT/include");
  //plugin->AddIncludePath("-I$ALICE_ROOT/PWGJE/EMCALJetTasks/");
  //plugin->AddIncludePath("-I$FASTJET/include -I$FASTJET/include/fastjet");
  //plugin->AddIncludePath("-I$PWD/.");

  // Declare the analysis source files names separated by blancs. To be compiled runtime
  // using ACLiC on the worker nodes.
  plugin->SetAnalysisSource("AliAnalysisTaskHFexploration.cxx AliFastSimulationTask.cxx");
  
  // Declare all libraries (other than the default ones for the framework. These will be
  // loaded by the generated analysis macro. Add all extra files (task .cxx/.h) here.
  plugin->SetAdditionalLibs("AliAnalysisTaskHFexploration.h AliAnalysisTaskHFexploration.cxx AliFastSimulationTask.h AliFastSimulationTask.cxx ");

  // Declare the output file names separated by blancs.
  // (can be like: file.root or file.root@ALICE::Niham::File)
  // To only save certain files, use SetDefaultOutputs(kFALSE), and then
  // SetOutputFiles("list.root other.filename") to choose which files to save
  plugin->SetDefaultOutputs(kFALSE);
  plugin->SetOutputFiles("AnalysisResults.root std*");
  
  // Optionally set a name for the generated analysis macro (default MyAnalysis.C)
  plugin->SetAnalysisMacro(Form("%s.C",taskname));
  
  // Optionally set maximum number of input files/subjob (default 100, put 0 to ignore)
  plugin->SetSplitMaxInputFileNumber(0);
  
  // Optionally modify the executable name (default analysis.sh)
  plugin->SetExecutable(Form("%s.sh",taskname));
  
  // set number of test files to use in "test" mode
  plugin->SetNtestFiles(1);
  
  // Optionally resubmit threshold.
  plugin->SetMasterResubmitThreshold(90);
  
  // Optionally set time to live (default 30000 sec)
  plugin->SetTTL(50000);
  
  // Optionally modify the name of the generated JDL (default analysis.jdl)
  plugin->SetJDLName(Form("%s.jdl",taskname));
  
  // Optionally modify job price (default 1)
  plugin->SetPrice(1);      
  
  // Optionally modify split mode (default 'se')    
  plugin->SetSplitMode("production:1-10");
  
  return plugin;
}

