AliFastSimulationTask* AddTaskFastSimulation(
  AliGenerator   *genGen,
  const char     *partName     = "mcparticles",
  const char     *taskName     = "FastSimulationTask",
  const Bool_t    drawQA       = kTRUE
)
{  
  return AliFastSimulationTask::AddTaskFastSimulation(genGen, partName, taskName, drawQA);
}
