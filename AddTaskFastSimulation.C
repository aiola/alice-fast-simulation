// $Id$

AliFastSimulationTask* AddTaskFastSimulation(
  AliGenerator   *genGen,
  const char     *partName     = "GenParticles",
  const char     *taskName     = "FastSimulationTask",
  const Bool_t    drawQA       = kTRUE
)
{  
  // Get the pointer to the existing analysis manager via the static access method.
  //==============================================================================
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr)
  {
    ::Error("AddTaskFastSimulation", "No analysis manager to connect to.");
    return NULL;
  }  
  
  // Check the analysis type using the event handlers connected to the analysis manager.
  //==============================================================================
  if (!mgr->GetInputEventHandler())
  {
    ::Error("AddTaskFastSimulation", "This task requires an input event handler");
    return NULL;
  }
  
  //-------------------------------------------------------
  // Init the task and do settings
  //-------------------------------------------------------

  AliFastSimulationTask *fastSimTask = new AliFastSimulationTask(taskName,drawQA);
  fastSimTask->SetGen(genGen);
  fastSimTask->SetMCParticlesName(partName);

  //-------------------------------------------------------
  // Final settings, pass to manager and set the containers
  //-------------------------------------------------------

  mgr->AddTask(fastSimTask);
    
  // Create containers for input/output
  mgr->ConnectInput(fastSimTask, 0, mgr->GetCommonInputContainer() );

  if (drawQA) {
    TString contName = taskName;
    contName += "_histos";
    AliAnalysisDataContainer *outc = mgr->CreateContainer(contName,
                                                          TList::Class(),
                                                          AliAnalysisManager::kOutputContainer,
                                                          "AnalysisResults.root");
    mgr->ConnectOutput(fastSimTask, 1, outc);
  }


  return fastSimTask;
}
