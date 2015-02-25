// runJetSimulation.C

// std library
#include <iostream>

// Root classes
#include <TSystem.h>
#include <TString.h>
#include <TArrayI.h>
#include <TChain.h>

// Aliroot classes
class AliAnalysisManager;
class AliESDInputHandler;
class AliAODOutputHandler;
class AliAnalysisGrid;
class AliFastSimulationTask;
class AliGenPythia;
class AliAnalysisTaskHFexploration;

using namespace std;

void LoadLibs();
AliAnalysisGrid* CreateAlienHandler(const char *taskname, const char *gridmode);

//______________________________________________________________________________
void runJetSimulation(
         const char   *runtype      = "local",                     // local or grid
         const char   *gridmode     = "offline",                   // set the grid run mode (can be "full", "test", "offline", "submit" or "terminate")
	 const Int_t   numevents    = 50000,                       // number of events
         const char   *taskname     = "FastSim_new"                // sets name of grid generated macros
         )
{
  //gSystem->SetFPEMask(TSystem::kInvalid | TSystem::kDivByZero | TSystem::kOverflow | TSystem::kUnderflow);

  enum eRunType  { kLocal, kGrid, kPdsf, kCarver };

  eRunType rType;
  if (!strcmp(runtype, "grid")) 
    rType = kGrid;
  else if (!strcmp(runtype, "local")) 
    rType = kLocal;
  else if (!strcmp(runtype, "pdsf")) 
    rType = kPdsf;
  else if (!strcmp(runtype, "carver")) 
    rType = kCarver;
  else {
    cout << "Incorrect run option, check first argument of run macro" << endl;
    cout << "runtype = local, grid, pdsf, carver" << endl;
    return;
  }

  cout << runtype << " analysis chosen" << endl;

  LoadLibs();

  // Aliroot macros
  gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/train/AddESDHandler.C");
  gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/train/AddAODOutputHandler.C");
  gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/train/AddMCGenPythia.C");

  gROOT->LoadMacro("AddTaskFastSimulation.C");
  gROOT->LoadMacro("AddTaskHFexploration.C");
  
  //gROOT->LoadMacro("AliFastSimulationTask.cxx+g");
  //gROOT->LoadMacro("AliAnalysisTaskHFexploration.cxx+g");
  
  gROOT->ProcessLine(".L AliAnalysisTaskHFexploration.cxx+g");
  gROOT->ProcessLine(".L AliFastSimulationTask.cxx+g");

  // analysis manager
  AliAnalysisManager* mgr = new AliAnalysisManager(taskname);

  AliESDInputHandler* esdH = AddESDHandler();
  
  if (0) AliAODHandler* aodoutHandler = AddAODOutputHandler();

  AliGenPythia *gen = AddMCGenPythia(7000.);
  
  AddTaskFastSimulation(gen);

  AddTaskHFexploration();
	
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
    cout << "Starting Analysis...";
    mgr->SetDebugLevel(0);
    mgr->StartAnalysis("grid");
  }
  else {  // local, carver, pdsf

    Int_t numFiles = numevents / 10000 + 1;
    cout << "Number of files: " << numFiles << endl;

    TChain* chain = new TChain("esdTree");
    for (Int_t i = 0; i < numFiles; i++) chain->Add("esdempty.root");

    // start analysis
    cout << "Starting Analysis...";
    mgr->SetUseProgressBar(1, 25);
    mgr->SetDebugLevel(0);
    //mgr->AddClassDebug("AliJetTriggerSelectionTask",AliLog::kDebug+100);
    //mgr->AddClassDebug("AliEMCALClusterizerFixedWindow",AliLog::kDebug+1);
    //mgr->AddClassDebug("AliEMCALClusterizerNxN",AliLog::kDebug+9);
    //mgr->AddClassDebug("AliJetEmbeddingFromPYTHIATask",AliLog::kDebug+3);
    //mgr->AddClassDebug("AliAnalysisTaskSAQA",100);
    //mgr->AddClassDebug("AliAnalysisTaskDeltaPt",100);
    //mgr->AddClassDebug("AliEmcalJetTask",2);
    //mgr->AddClassDebug("AliJetConstituentTagCopier",AliLog::kDebug+1);
    //mgr->AddClassDebug("AliEMCALTenderSupply",AliLog::kDebug+1);
    mgr->StartAnalysis("local", chain, numevents);
  }
}

//______________________________________________________________________________
void LoadLibs()
{
  // load ROOT libraries
  gSystem->Load("libTree");
  gSystem->Load("libGeom");
  gSystem->Load("libVMC");
  gSystem->Load("libPhysics");

  gSystem->Load("libMinuit");
  gSystem->Load("libGui");
  gSystem->Load("libXMLParser");
  gSystem->Load("libMinuit2");
  gSystem->Load("libProof");

  // load AliRoot libraries
  gSystem->Load("libSTEERBase");
  gSystem->Load("libESD");
  gSystem->Load("libAOD");
  gSystem->Load("libOADB");
  gSystem->Load("libANALYSIS");
  gSystem->Load("libANALYSISalice");
  gSystem->Load("libCDB");
  gSystem->Load("libRAWDatabase");
  gSystem->Load("libSTEER");
  
  gSystem->Load("libCORRFW");
  gSystem->Load("libEMCALUtils");
  gSystem->Load("libPHOSUtils");
  gSystem->Load("libPWGCaloTrackCorrBase");
  gSystem->Load("libPWGGACaloTrackCorrelations");
  gSystem->Load("libPWGGACaloTasks");
  gSystem->Load("libPWGLFforward2");
  gSystem->Load("libEMCALraw");
  gSystem->Load("libEMCALbase");
  gSystem->Load("libEMCALrec");
  gSystem->Load("libTRDbase");
  gSystem->Load("libVZERObase");
  gSystem->Load("libVZEROrec");
  gSystem->Load("libTENDER");   
  gSystem->Load("libTENDERSupplies"); 

  // load fastjet libraries 3.x
  //gSystem->Load("$FASTJET/lib/libfastjet");
  //gSystem->Load("$FASTJET/lib/libsiscone");
  //gSystem->Load("$FASTJET/lib/libsiscone_spherical");
  //gSystem->Load("$FASTJET/lib/libfastjetplugins");

  // load fastjet libraries 2.x
  gSystem->Load("$FASTJET/lib/libfastjet");
  gSystem->Load("$FASTJET/lib/libsiscone");
  gSystem->Load("$FASTJET/lib/libSISConePlugin");
  gSystem->Load("$FASTJET/lib/libCDFConesPlugin");

  // Aliroot jet libs
  gSystem->Load("libPWGTools");
  gSystem->Load("libPWGEMCAL");
  gSystem->Load("libESDfilter");
  gSystem->Load("libPWGGAEMCALTasks");
  gSystem->Load("libPWGJE");
  gSystem->Load("libPWGJEEMCALJetTasks");

  // Pythia
  gSystem->Load("libmicrocern.so");
  gSystem->Load("libFASTSIM.so");
  gSystem->Load("libEVGEN.so");
  gSystem->Load("liblhapdf.so");
  gSystem->Load("libpythia6.4.25.so");
  gSystem->Load("libEGPythia6.so");
  gSystem->Load("libAliPythia6.so");
   
  // include path
  gSystem->AddIncludePath("-I$ALICE_ROOT/include");
  gSystem->AddIncludePath("-I$ALICE_ROOT/PWGJE/EMCALJetTasks/");
  gSystem->AddIncludePath("-I$FASTJET/include -I$FASTJET/include/fastjet");
  gSystem->AddIncludePath("-I$PWD/.");
  gROOT->ProcessLine(".include $ALICE_ROOT/include");
  gROOT->ProcessLine(".include $ALICE_ROOT/PYTHIA6");
  gROOT->ProcessLine(".include $ALICE_ROOT/EVGEN");
}

//______________________________________________________________________________
AliAnalysisGrid* CreateAlienHandler(const char *taskname, const char *gridmode)
{
  AliAnalysisAlien *plugin = new AliAnalysisAlien();
  plugin->SetRunMode(gridmode);
  
  // Set versions of used packages
  plugin->SetAPIVersion("V1.1x");
  plugin->SetROOTVersion("v5-34-08-6");
  plugin->SetAliROOTVersion("vAN-20140617");
  plugin->AddExternalPackage("fastjet::v2.4.2");

  //plugin->SetMergeViaJDL(kTRUE);

  // Declare input data to be processed.
  plugin->AddDataFile("/alice/cern.ch/user/s/saiola/esdempty.root");
  
  // Define alien work directory where all files will be copied. Relative to alien $HOME.
  plugin->SetGridWorkingDir(taskname);
  
  // Declare alien output directory. Relative to working directory.
  plugin->SetGridOutputDir("out"); // In this case will be $HOME/taskname/out
  
  plugin->AddIncludePath("-I$ALICE_ROOT/include");
  plugin->AddIncludePath("-I$ALICE_ROOT/PWGJE/EMCALJetTasks/");
  plugin->AddIncludePath("-I$FASTJET/include -I$FASTJET/include/fastjet");
  plugin->AddIncludePath("-I$PWD/.");

  // Declare the analysis source files names separated by blancs. To be compiled runtime
  // using ACLiC on the worker nodes.
  plugin->SetAnalysisSource("AliAnalysisTaskHFexploration.cxx AliFastSimulationTask.cxx");
  
  // Declare all libraries (other than the default ones for the framework. These will be
  // loaded by the generated analysis macro. Add all extra files (task .cxx/.h) here.
  plugin->SetAdditionalRootLibs("libGui.so libXMLParser.so libMinuit2.so libProof.so");
  plugin->SetAdditionalLibs("libCDB.so libRAWDatabase.so libSTEER.so libEMCALUtils.so libEMCALraw.so " 
			    "libEMCALbase.so libEMCALrec.so libPHOSUtils.so libPWGCaloTrackCorrBase.so " 
			    "libPWGGACaloTrackCorrelations.so libPWGGACaloTasks.so libTRDbase.so libVZERObase.so "
			    "libVZEROrec.so libTENDER.so libTENDERSupplies.so libJETAN.so libCGAL.so libfastjet.so " 
			    "libsiscone.so libSISConePlugin.so libCDFConesPlugin.so libFASTJETAN.so libPWGTools.so libPWGEMCAL.so libESDfilter.so "
			    "libPWGGAEMCALTasks.so libPWGJE.so libPWGJEEMCALJetTasks.so libmicrocern.so libFASTSIM.so "
			    "libEVGEN.so liblhapdf.so libpythia6.4.25.so libEGPythia6.so libAliPythia6.so "
			    "AliAnalysisTaskHFexploration.h AliAnalysisTaskHFexploration.cxx AliFastSimulationTask.h AliFastSimulationTask.cxx ");

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

