const char *anatype = "ESD";

void FastSim()
{
// Analysis using ESD data
// Automatically generated analysis steering macro executed in grid subjobs

   TStopwatch timer;
   timer.Start();

// connect to AliEn and make the chain
   if (!TGrid::Connect("alien://")) return;
// Set temporary merging directory to current one
   gSystem->Setenv("TMPDIR", gSystem->pwd());

// Set temporary compilation directory to current one
   gSystem->SetBuildDir(gSystem->pwd(), kTRUE);

// Reset existing include path and add current directory first in the search
   gSystem->SetIncludePath("-I.");
// load base root libraries
   gSystem->Load("libTree");
   gSystem->Load("libGeom");
   gSystem->Load("libVMC");
   gSystem->Load("libPhysics");

   gSystem->Load("libMinuit");

// Add aditional libraries
   gSystem->Load("libGui.so");
   gSystem->Load("libXMLParser.so");
   gSystem->Load("libMinuit2.so");
   gSystem->Load("libProof.so");
// Load analysis framework libraries
   gSystem->Load("libSTEERBase");
   gSystem->Load("libESD");
   gSystem->Load("libAOD");
   gSystem->Load("libANALYSIS");
   gSystem->Load("libOADB");
   gSystem->Load("libANALYSISalice");
   gSystem->Load("libCORRFW");

// include path
   TString intPath = gInterpreter->GetIncludePath();
   TObjArray *listpaths = intPath.Tokenize(" ");
   TIter nextpath(listpaths);
   TObjString *pname;
   while ((pname=(TObjString*)nextpath())) {
      TString current = pname->GetName();
      if (current.Contains("AliRoot") || current.Contains("ALICE_ROOT")) continue;
      gSystem->AddIncludePath(current);
   }
   if (listpaths) delete listpaths;
   gSystem->AddIncludePath("-I$ALICE_ROOT/include -I$ALICE_ROOT/PWGJE/EMCALJetTasks/ -I$FASTJET/include -I$FASTJET/include/fastjet -I$PWD/. ");
   gROOT->ProcessLine(".include $ALICE_ROOT/include");
   gROOT->ProcessLine(".include $ALICE_ROOT/PYTHIA6");
   gROOT->ProcessLine(".include $ALICE_ROOT/EVGEN");

   printf("Include path: %s\n", gSystem->GetIncludePath());

// Add aditional AliRoot libraries
   gSystem->Load("libCDB.so");
   gSystem->Load("libRAWDatabase.so");
   gSystem->Load("libSTEER.so");
   gSystem->Load("libEMCALUtils.so");
   gSystem->Load("libEMCALraw.so");
   gSystem->Load("libEMCALbase.so");
   gSystem->Load("libEMCALrec.so");
   gSystem->Load("libPHOSUtils.so");
   gSystem->Load("libPWGCaloTrackCorrBase.so");
   gSystem->Load("libPWGGACaloTrackCorrelations.so");
   gSystem->Load("libPWGGACaloTasks.so");
   gSystem->Load("libTRDbase.so");
   gSystem->Load("libVZERObase.so");
   gSystem->Load("libVZEROrec.so");
   gSystem->Load("libTENDER.so");
   gSystem->Load("libTENDERSupplies.so");
   gSystem->Load("libJETAN.so");
   gSystem->Load("libCGAL.so");
   gSystem->Load("libfastjet.so");
   gSystem->Load("libsiscone.so");
   gSystem->Load("libSISConePlugin.so");
   gSystem->Load("libCDFConesPlugin.so");
   gSystem->Load("libFASTJETAN.so");
   gSystem->Load("libPWGTools.so");
   gSystem->Load("libPWGEMCAL.so");
   //if (!AliAnalysisAlien::SetupPar("PWGEMCAL.par")) return;;
   gSystem->Load("libESDfilter.so");
   gSystem->Load("libPWGGAEMCALTasks.so");
   gSystem->Load("libPWGJE.so");
   gSystem->Load("libPWGJEEMCALJetTasks.so");
   gSystem->Load("libmicrocern.so");
   gSystem->Load("libFASTSIM.so");
   gSystem->Load("libEVGEN.so");
   gSystem->Load("liblhapdf.so");
   gSystem->Load("libpythia6.4.25.so");
   gSystem->Load("libEGPythia6.so");
   gSystem->Load("libAliPythia6.so");

// analysis source to be compiled at runtime (if any)
   gROOT->ProcessLine(".L AliAnalysisTaskHFexploration.cxx+g");
   gROOT->ProcessLine(".L AliFastSimulationTask.cxx+g");

// read the analysis manager from file
   AliAnalysisManager *mgr = AliAnalysisAlien::LoadAnalysisManager("FastSim.root");
   if (!mgr) return;
   mgr->PrintStatus();
   AliLog::SetGlobalLogLevel(AliLog::kWarning);

   Int_t numevents = 500000;
   Int_t numFiles = numevents / 10000;

   TChain* chain = new TChain("esdTree");
   for (Int_t i = 0; i < numFiles; i++) chain->Add("alien:///alice/cern.ch/user/s/saiola/esdempty.root");

   mgr->StartAnalysis("localfile", chain);
   timer.Stop();
   timer.Print();
}
