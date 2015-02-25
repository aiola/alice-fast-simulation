AliAnalysisTaskHFexploration* AddTaskHFexploration(
  const char *npart              = "GenParticles",
  const char *taskname           = "AliAnalysisTaskHFexploration"
)
{  
  // Get the pointer to the existing analysis manager via the static access method.
  //==============================================================================
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr)
  {
    ::Error("AddTaskHFexploration", "No analysis manager to connect to.");
    return NULL;
  }  
  
  // Check the analysis type using the event handlers connected to the analysis manager.
  //==============================================================================
  if (!mgr->GetInputEventHandler())
  {
    ::Error("AddTaskHFexploration", "This task requires an input event handler");
    return NULL;
  }
  
  //-------------------------------------------------------
  // Init the task and do settings
  //-------------------------------------------------------

  AliAnalysisTaskHFexploration* hfTask = new AliAnalysisTaskHFexploration(taskname);
  hfTask->SetNeedEmcalGeom(kFALSE);
  hfTask->SetHistoBins(400,0,100);
  
  AliParticleContainer *partCont = hfTask->AddParticleContainer(npart);
  partCont->SetClassName("AliMCParticle");

  //-------------------------------------------------------
  // Final settings, pass to manager and set the containers
  //-------------------------------------------------------
  
  mgr->AddTask(hfTask);
  
  // Create containers for input/output
  AliAnalysisDataContainer *cinput1  = mgr->GetCommonInputContainer()  ;
  TString contname(taskname);
  contname += "_histos";
  AliAnalysisDataContainer *coutput1 = mgr->CreateContainer(contname.Data(), TList::Class(),AliAnalysisManager::kOutputContainer,
							    Form("%s", AliAnalysisManager::GetCommonFileName()));
  mgr->ConnectInput(hfTask, 0, cinput1);
  mgr->ConnectOutput(hfTask, 1, coutput1);
  
  return hfTask;
}
