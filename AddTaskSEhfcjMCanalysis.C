
/*_____________________________________________________________
 
 Addtask for simulations taks : HF Correlations
 Jitendra Kumar (jitendra.kumar@cern.ch)
 Andrea Rossi   (andrea.rossi@cern.ch)
 _____________________________________________________________*/


AliAnalysisTaskSEhfcjMCanalysis *AddTaskSEhfcjMCanalysis(TString suffixName ="")
{
  return AliAnalysisTaskSEhfcjMCanalysis::AddTaskSEhfcjMCanalysis(suffixName);
}
