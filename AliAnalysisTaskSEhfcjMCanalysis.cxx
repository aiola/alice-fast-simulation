/*_____________________________________________________________

 Class AliAnalysisTaskSEhfcjMCanalysis: On the fly Simulation class for
 heavy flavour correlations and general event/part properties 
 AUTHROS:
 Jitendra Kumar (jitendra.kumar@cern.ch)
 Andrea Rossi   (andrea.rossi@cern.ch)
 _____________________________________________________________*/

#include <iostream>

#include <TH1F.h>
#include <TNtuple.h>
#include <TList.h>
#include <TChain.h>
#include <THnSparse.h>
#include <TMath.h>
#include <TObjArray.h>
#include <TArrayI.h>

#include <AliAnalysisManager.h>
#include <AliAnalysisTaskSE.h>
#include <AliMCEvent.h>
#include <AliVEvent.h>
#include <AliGenEventHeader.h>
#include <AliVParticle.h>
#include <AliStack.h>
#include <AliInputEventHandler.h>
#include <AliLog.h>
#include <AliGenPythiaEventHeader.h>

#include <fastjet/config.h>
#include <fastjet/PseudoJet.hh>
#include <fastjet/JetDefinition.hh>
#include <fastjet/ClusterSequence.hh>
#include <fastjet/ClusterSequenceArea.hh>
#include <fastjet/AreaDefinition.hh>

#include "AliAnalysisTaskSEhfcjMCanalysis.h"

ClassImp(AliAnalysisTaskSEhfcjMCanalysis)
//______________________________| Default Constructor
AliAnalysisTaskSEhfcjMCanalysis::AliAnalysisTaskSEhfcjMCanalysis():
  fMcEvent(0x0),
  fMcHandler(0x0),
  fLightOutput(kFALSE),
  fFillHFJet(kTRUE),
  fFillJetCorrelations(kTRUE),
  fHistEventsProcessed(0x0),
  fOutputQA(0),
  fOutputList(0),
  fhCheckNotDorBdaughters(0x0),
  fhSparseHFCounterSignals(0x0),
  fhSparseJetProp(0x0),
  fhSparseJetPropExtended(0x0),
  fhSparseFFstudy(0x0),
  fhSparseDJets(0x0),
  fhSparseBJets(0x0),
  fhSparseFeedDownDJets(0x0),
  fhSparseEleJets(0x0),
  fhSparseDJetCorrel(0x0),
  fhSparseBJetCorrel(0x0),
  fhSparseFeedDownDJetCorrel(0x0),
  fhSparseHFeJetCorrel(0x0),
  fXsectionNtuple(0x0),
  feventcounter(0),
  fEtaMin(-20),
  fEtaMax(20),
  fYMin(-20),
  fYMax(20),
  fPtMin(0),
  fPtMax(999),
  fMinMultiplicity(0),
  fMaxMultiplicity(99000),
  fCorrPart1(0x0),
  fCorrPart2(0x0),
  fChargeSel1(1),
  fChargeSel2(-1),
  fStack(0),
  fxsection(0),
  feventweight(1),
  feventtrials(1),
  fParticleArray(0),
  fcounter(0),
  fIsEventProp(kTRUE),
  fIsPartProp(kTRUE),
  fIsCorrOfQQbar(kFALSE),
  fIsCorrOfHeavyFlavor(kFALSE),
  fIsCorrOfHadronHadron(kFALSE),
  fIsHFjetsAnalysis(kTRUE),
  fSelectPhysPrimaryForJets(kTRUE),
  fSelOnlyEMuPiKProtForJets(kTRUE),
  fDCAmcSelEle(1.),
  fminptjettrack(0.05),
  fmaxptjettrack(99999),
  fminetajettrack(-4),
  fmaxetajettrack(4),
  fDetectorAcceptance(4),
  fJetRcut(0.4),
  fDeltaRSqforTagging(0.16),
  fArraySkipDDaugh(0x0),
  fArrayTrk(0x0),
  flastdaugh(0),
  fArraybquark(0x0),
  flastbquark(0),
  fArraycquark(0x0),
  flastcquark(0),
  fArraybhadron(0x0),
  flastbhadron(0),
  fArraychadron(0x0),
  flastchadron(0),
  fArraychadronfromB(0x0),
  flastchadronfromB(0),
  fArraybele(0x0),
  flastbele(0),
  fArraycele(0x0),
  flastcele(0),
  fArraycelefromB(0x0),
  flastcelefromB(0),
  fArrayGammaele(0x0),
  flastGammaele(0),
  fArrayDalitzele(0x0),
  flastDalitzele(0),
  fArrayUnknownele(0x0),
  flastUnknownele(0),
  fTempTagParticle(0x0),
  fInputParticlesJet(),
  fJetDef(),
  fGhostArea(),
  fAreaDef(),
  fCSA(),
  fJets(),
  fArrayTagJetQuark(0x0),
  fArrayTagJetParticle(0x0)
{
  // default constructor
}

//______________________________| Specific Constructor
AliAnalysisTaskSEhfcjMCanalysis::AliAnalysisTaskSEhfcjMCanalysis(const Char_t* name) :
  AliAnalysisTaskSE(name),
  fMcEvent(0x0),
  fMcHandler(0x0),
  fLightOutput(0x0),
  fFillHFJet(kTRUE),
  fFillJetCorrelations(kTRUE),
  fHistEventsProcessed(0x0),
  fOutputQA(0),
  fOutputList(0),
  fhCheckNotDorBdaughters(0x0),
  fhSparseHFCounterSignals(0x0),
  fhSparseJetProp(0x0),
  fhSparseJetPropExtended(0x0),
  fhSparseFFstudy(0x0),
  fhSparseDJets(0x0),
  fhSparseBJets(0x0),
  fhSparseFeedDownDJets(0x0),
  fhSparseEleJets(0x0),
  fhSparseDJetCorrel(0x0),
  fhSparseBJetCorrel(0x0),
  fhSparseFeedDownDJetCorrel(0x0),
  fhSparseHFeJetCorrel(0x0),
  fXsectionNtuple(0x0),
  feventcounter(0),
  fEtaMin(-20),
  fEtaMax(20),
  fYMin(-20),
  fYMax(20),
  fPtMin(0),
  fPtMax(999.0),
  fMinMultiplicity(0),
  fMaxMultiplicity(99000),
  fCorrPart1(0x0),
  fCorrPart2(0x0),
  fChargeSel1(1),
  fChargeSel2(-1),
  fStack(0),
  fxsection(0),
  feventweight(1),
  feventtrials(1),
  fParticleArray(0X0),
  fcounter(0),
  fIsEventProp(kTRUE),
  fIsPartProp(kTRUE),
  fIsCorrOfQQbar(kFALSE),
  fIsCorrOfHeavyFlavor(kFALSE),
  fIsCorrOfHadronHadron(kFALSE),
  fIsHFjetsAnalysis(kTRUE),
  fSelectPhysPrimaryForJets(kTRUE),
  fSelOnlyEMuPiKProtForJets(kTRUE),
  fDCAmcSelEle(1.),
  fminptjettrack(0.05),
  fmaxptjettrack(99999),
  fminetajettrack(-4),
  fmaxetajettrack(4),
  fDetectorAcceptance(4),
  fJetRcut(0.4),
  fDeltaRSqforTagging(0.16),
  fArraySkipDDaugh(0x0),
  fArrayTrk(0x0),
  flastdaugh(0),
  fArraybquark(0x0),
  flastbquark(0),
  fArraycquark(0x0),
  flastcquark(0),
  fArraybhadron(0x0),
  flastbhadron(0),
  fArraychadron(0x0),
  flastchadron(0),
  fArraychadronfromB(0x0),
  flastchadronfromB(0),
  fArraybele(0x0),
  flastbele(0),
  fArraycele(0x0),
  flastcele(0),
  fArraycelefromB(0x0),
  flastcelefromB(0),
  fArrayGammaele(0x0),
  flastGammaele(0),
  fArrayDalitzele(0x0),
  flastDalitzele(0),
  fArrayUnknownele(0x0),
  flastUnknownele(0),
  fTempTagParticle(0x0),
  fInputParticlesJet(),
  fJetDef(),
  fGhostArea(),
  fAreaDef(),
  fCSA(),
  fJets(),
  fArrayTagJetQuark(0x0),
  fArrayTagJetParticle(0x0)
{
  fArrayTrk        = new TArrayI(5000);
  fArraySkipDDaugh = new TArrayI(200);

  Info("AliAnalysisTaskSEhfcjMCanalysis","Calling Constructor");
  // Output slot #1 writes into a TList container (nevents histogram)
  DefineInput(0, TChain::Class());
  DefineOutput(1,TList::Class());
  DefineOutput(2,TList::Class());
}

//______________________________| Copy Constructor
AliAnalysisTaskSEhfcjMCanalysis::AliAnalysisTaskSEhfcjMCanalysis(const AliAnalysisTaskSEhfcjMCanalysis& c) :
  AliAnalysisTaskSE(c),
  fMcEvent(c.fMcEvent),
  fMcHandler(c.fMcHandler),
  fLightOutput(c.fLightOutput),
  fFillHFJet(c.fFillHFJet),
  fFillJetCorrelations(c.fFillJetCorrelations),
  fHistEventsProcessed(c.fHistEventsProcessed),
  fOutputQA(c.fOutputQA),
  fOutputList(c.fOutputList),
  fhCheckNotDorBdaughters(c.fhCheckNotDorBdaughters),
  fhSparseHFCounterSignals(c.fhSparseHFCounterSignals),
  fhSparseJetProp(c.fhSparseJetProp),
  fhSparseJetPropExtended(c.fhSparseJetPropExtended),
  fhSparseFFstudy(c.fhSparseFFstudy),
  fhSparseDJets(c.fhSparseDJets),
  fhSparseFeedDownDJets(c.fhSparseFeedDownDJets),
  fhSparseBJets(c.fhSparseBJets),
  fhSparseEleJets(c.fhSparseEleJets),
  fhSparseDJetCorrel(c.fhSparseDJetCorrel),
  fhSparseBJetCorrel(c.fhSparseBJetCorrel),
  fhSparseFeedDownDJetCorrel(c.fhSparseFeedDownDJetCorrel),
  fhSparseHFeJetCorrel(c.fhSparseHFeJetCorrel),
  fXsectionNtuple(c.fXsectionNtuple),
  feventcounter(0),
  fEtaMin(c.fEtaMin),
  fEtaMax(c.fEtaMax),
  fYMin(c.fYMin),
  fYMax(c.fYMax),
  fPtMin(c.fPtMin),
  fPtMax(c.fPtMax),
  fMinMultiplicity(c.fMinMultiplicity),
  fMaxMultiplicity(c.fMaxMultiplicity),
  fCorrPart1(c.fCorrPart1),
  fCorrPart2(c.fCorrPart2),
  fChargeSel1(c.fChargeSel1),
  fChargeSel2(c.fChargeSel2),
  fStack(c.fStack),
  fxsection(c.fxsection),
  feventweight(c.feventweight),
  feventtrials(c.feventtrials),
  fParticleArray(c.fParticleArray),
  fcounter(c.fcounter),
  fIsEventProp(c.fIsEventProp),
  fIsPartProp(c.fIsPartProp),
  fIsCorrOfQQbar(c.fIsCorrOfQQbar),
  fIsCorrOfHeavyFlavor(c.fIsCorrOfHeavyFlavor),
  fIsCorrOfHadronHadron(c.fIsCorrOfHadronHadron),
  fIsHFjetsAnalysis(c.fIsHFjetsAnalysis),
  fSelectPhysPrimaryForJets(c.fSelectPhysPrimaryForJets),
  fSelOnlyEMuPiKProtForJets(c.fSelOnlyEMuPiKProtForJets),
  fDCAmcSelEle(c.fDCAmcSelEle),
  fminptjettrack(c.fminptjettrack),
  fmaxptjettrack(c.fmaxptjettrack),
  fminetajettrack(c.fminetajettrack),
  fmaxetajettrack(c.fmaxetajettrack),
  fDetectorAcceptance(c.fDetectorAcceptance),
  fJetRcut(c.fJetRcut),
  fDeltaRSqforTagging(c.fDeltaRSqforTagging),
  fArraySkipDDaugh(c.fArraySkipDDaugh),
  fArrayTrk(c.fArrayTrk),
  flastdaugh(c.flastdaugh),
  fArraybquark(c.fArraybquark),
  flastbquark(c.flastbquark),
  fArraycquark(c.fArraycquark),
  flastcquark(c.flastcquark),
  fArraybhadron(c.fArraybhadron),
  flastbhadron(c.flastbhadron),
  fArraychadron(c.fArraychadron),
  flastchadron(c.flastchadron),
  fArraychadronfromB(c.fArraychadronfromB),
  flastchadronfromB(c.flastchadronfromB),
  fArraybele(c.fArraybele),
  flastbele(c.flastbele),
  fArraycele(c.fArraycele),
  flastcele(c.flastcele),
  fArraycelefromB(c.fArraycelefromB),
  flastcelefromB(c.flastcelefromB),
  fArrayGammaele(c.fArrayGammaele),
  flastGammaele(c.flastGammaele),
  fArrayDalitzele(c.fArrayDalitzele),
  flastDalitzele(c.flastDalitzele),
  fArrayUnknownele(c.fArrayUnknownele),
  flastUnknownele(c.flastUnknownele),
  fTempTagParticle(c.fTempTagParticle),
  fInputParticlesJet(c.fInputParticlesJet),
  fJetDef(c.fJetDef),
  fGhostArea(c.fGhostArea),
  fAreaDef(c.fAreaDef),
  fCSA(c.fCSA),
  fJets(c.fJets),
  fArrayTagJetQuark(c.fArrayTagJetQuark),  
  fArrayTagJetParticle(c.fArrayTagJetParticle)  
{
  // Copy Constructor
}

//______________________________| Destructor
AliAnalysisTaskSEhfcjMCanalysis::~AliAnalysisTaskSEhfcjMCanalysis()
{
  // Destructor
  Info("~AliAnalysisTaskSEhfcjMCanalysis","Calling Destructor");
  if (fHistEventsProcessed) delete fHistEventsProcessed;
  if (fOutputQA) delete fOutputQA;
  if (fOutputList) delete fOutputList;
  if(fhCheckNotDorBdaughters) delete fhCheckNotDorBdaughters;
  if(fhSparseHFCounterSignals)delete fhSparseHFCounterSignals;
  if(fhSparseJetProp) delete fhSparseJetProp;
  if(fhSparseJetPropExtended) delete fhSparseJetPropExtended;
  if(fhSparseFFstudy)delete fhSparseFFstudy;
  if(fhSparseDJets)delete fhSparseDJets;
  if(fhSparseBJets)delete fhSparseBJets;
  if(fhSparseEleJets)delete fhSparseEleJets;
  if(fhSparseDJetCorrel)delete fhSparseDJetCorrel;
  if(fhSparseBJetCorrel)delete fhSparseBJetCorrel;
  if(fhSparseFeedDownDJetCorrel)delete fhSparseFeedDownDJetCorrel;
  if(fhSparseHFeJetCorrel)delete fhSparseHFeJetCorrel;
  if(fXsectionNtuple)delete fXsectionNtuple;
  if (fArraySkipDDaugh) delete fArraySkipDDaugh;
  if (fArrayTrk) delete fArrayTrk;

  if(fArraybquark) delete fArraybquark;

  if(fArraycquark) delete fArraycquark;

  if(fArraybhadron) delete fArraybhadron;

  if(fArraychadron) delete fArraychadron;

  if(fArraychadronfromB) delete fArraychadronfromB;

  if(fArraybele) delete fArraybele;

  if(fArraycele) delete fArraycele;

  if(fArraycelefromB) delete fArraycelefromB;

  if(fArrayGammaele) delete fArrayGammaele;

  if(fArrayDalitzele) delete fArrayDalitzele;

  if(fArrayUnknownele) delete fArrayUnknownele;

  if(fTempTagParticle)delete fTempTagParticle;

  if(fJetDef)delete fJetDef;
  if(fGhostArea)delete fGhostArea;
  if(fAreaDef)delete fAreaDef;
  if(fCSA)delete fCSA;
  if(fArrayTagJetQuark)delete fArrayTagJetQuark;
  if(fArrayTagJetParticle)delete fArrayTagJetParticle;

}

//______________________________| Assignment Operator
AliAnalysisTaskSEhfcjMCanalysis& AliAnalysisTaskSEhfcjMCanalysis::operator=(const AliAnalysisTaskSEhfcjMCanalysis& c)
{
  if (this!=&c) {
    AliAnalysisTaskSE::operator=(c);
    fMcEvent = c.fMcEvent;
    fMcHandler = c.fMcHandler;
    fLightOutput = c.fLightOutput;
    fFillHFJet = c.fFillHFJet;
    fFillJetCorrelations = c.fFillJetCorrelations;
    fHistEventsProcessed = c.fHistEventsProcessed;
    fOutputQA = c.fOutputQA;
    fOutputList = c.fOutputList;
    fhCheckNotDorBdaughters = c.fhCheckNotDorBdaughters;
    fhSparseHFCounterSignals=c.fhSparseHFCounterSignals;
    fhSparseJetProp=c.fhSparseJetProp;
    fhSparseJetPropExtended=c.fhSparseJetPropExtended;
    fhSparseFFstudy=c.fhSparseFFstudy;
    fhSparseDJets=c.fhSparseDJets;
    fhSparseBJets=c.fhSparseBJets;
    fhSparseEleJets=c.fhSparseEleJets;
    fhSparseDJetCorrel=c.fhSparseDJetCorrel;
    fhSparseBJetCorrel=c.fhSparseBJetCorrel;
    fhSparseFeedDownDJetCorrel=c.fhSparseFeedDownDJetCorrel;
    fhSparseHFeJetCorrel=c.fhSparseHFeJetCorrel;
    fXsectionNtuple=c.fXsectionNtuple;
    fEtaMin = c.fEtaMin;
    fEtaMax = c.fEtaMax;
    fYMin = c.fYMin;
    fYMax = c.fYMax;
    fPtMin = c.fPtMin;
    fPtMax = c.fPtMax;
    fMinMultiplicity = c.fMinMultiplicity;
    fMaxMultiplicity = c.fMaxMultiplicity;
    fCorrPart1 = c.fCorrPart1;
    fCorrPart2 = c.fCorrPart2;
    fChargeSel1 = c.fChargeSel1;
    fChargeSel2 = c.fChargeSel2;
    fStack = c.fStack;
    fxsection=c.fxsection;
    feventweight=c.feventweight;
    feventtrials=c.feventtrials;
    fParticleArray = c.fParticleArray;
    fIsEventProp = c.fIsEventProp;
    fIsPartProp = c.fIsPartProp;
    fIsCorrOfQQbar = c.fIsCorrOfQQbar;
    fIsCorrOfHeavyFlavor = c.fIsCorrOfHeavyFlavor;
    fIsCorrOfHadronHadron = c.fIsCorrOfHadronHadron;
    fIsHFjetsAnalysis = c.fIsHFjetsAnalysis;
    fSelectPhysPrimaryForJets=c.fSelectPhysPrimaryForJets;
    fSelOnlyEMuPiKProtForJets=c.fSelOnlyEMuPiKProtForJets;
    fDCAmcSelEle=c.fDCAmcSelEle;
    fminptjettrack=c.fminptjettrack;
    fmaxptjettrack=c.fmaxptjettrack;
    fminetajettrack=c.fminetajettrack;
    fmaxetajettrack=c.fmaxetajettrack;
    fDetectorAcceptance=c.fDetectorAcceptance;
    fJetRcut=c.fJetRcut;
    fDeltaRSqforTagging=c.fDeltaRSqforTagging;
    fArraySkipDDaugh = c.fArraySkipDDaugh;
    fArrayTrk = c.fArrayTrk;
    flastdaugh = c.flastdaugh;
    fArraybquark = c.fArraybquark;
    flastbquark = c.flastbquark;
    fArraycquark = c.fArraycquark;
    flastcquark = c.flastcquark;
    fArraybhadron = c.fArraybhadron;
    flastbhadron = c.flastbhadron;
    fArraychadron = c.fArraychadron;
    flastchadron = c.flastchadron;
    fArraychadronfromB = c.fArraychadronfromB;
    flastchadronfromB = c.flastchadronfromB;
    fArraybele = c.fArraybele;
    flastbele = c.flastbele;
    fArraycele = c.fArraycele;
    flastcele = c.flastcele;
    fArraycelefromB = c.fArraycelefromB;
    flastcelefromB = c.flastcelefromB;
    fArrayGammaele = c.fArrayGammaele;
    flastGammaele = c.flastGammaele;
    fArrayDalitzele = c.fArrayDalitzele;
    flastDalitzele = c.flastDalitzele;
    fArrayUnknownele = c.fArrayUnknownele;
    flastUnknownele = c.flastUnknownele;
    fTempTagParticle = c.fTempTagParticle;

    fInputParticlesJet=c.fInputParticlesJet;
    fJetDef=c.fJetDef;
    fGhostArea=c.fGhostArea;
    fAreaDef=c.fAreaDef;
    fCSA=c.fCSA;
    fJets=c.fJets;
    fArrayTagJetQuark=c.fArrayTagJetQuark;
    fArrayTagJetParticle=c.fArrayTagJetParticle;
  }

  return *this;
}
//______________________________
void AliAnalysisTaskSEhfcjMCanalysis::InitInternalVariables(){

  fArraybquark=new TArrayI(50);
  fArraycquark=new TArrayI(150);// just in case of HIJING...
  fArraybhadron=new TArrayI(50);
  fArraychadron=new TArrayI(150);  
  fArraychadronfromB=new TArrayI(150);  
  fArraybele=new TArrayI(50);  
  fArraycele=new TArrayI(50);  
  fArraycelefromB=new TArrayI(50);  
  fArrayGammaele=new TArrayI(200);  
  fArrayDalitzele=new TArrayI(200);
  fArrayUnknownele=new TArrayI(100);
  fArrayTagJetQuark=new TArrayI(200);
  fArrayTagJetParticle=new TArrayI(200);
  fTempTagParticle=new TArrayI(20);
  feventcounter=0;
  fxsection=0.;

}
//______________________________
void AliAnalysisTaskSEhfcjMCanalysis::ResetInternalVariables(){

  fArraybquark->Reset(-1);
  fArraycquark->Reset(-1);
  fArraybhadron->Reset(-1);
  fArraychadron->Reset(-1);  
  fArraychadronfromB->Reset(-1);  
  fArraybele->Reset(-1);  
  fArraycele->Reset(-1);  
  fArraycelefromB->Reset(-1);  
  fArrayGammaele->Reset(-1);
  fArrayDalitzele->Reset(-1);
  fArrayUnknownele->Reset(-1);
  fTempTagParticle->Reset(-1);

  fArrayTagJetQuark->Reset(-1);
  fArrayTagJetParticle->Reset(-1);

  flastbquark=-1;
  flastcquark=-1;
  flastbhadron=-1;
  flastchadron=-1;  
  flastchadronfromB=-1;
  flastbele=-1;  
  flastcele=-1;  
  flastcelefromB=-1;  
  flastGammaele=-1;
  flastDalitzele=-1;
  flastUnknownele=-1;

  //  fJets.clear();  
  fInputParticlesJet.clear();
  if(fJetDef) delete fJetDef;
  if(fGhostArea) delete fGhostArea;
  if(fAreaDef) delete fAreaDef;
  if(fCSA) delete fCSA;


}

//______________________________| User Output
void AliAnalysisTaskSEhfcjMCanalysis::UserCreateOutputObjects()
{  
  Info("AliAnalysisTaskSEhfcjMCanalysis","CreateOutputObjects of task %s", GetName());
  fOutputList = new TList();
  fOutputList->SetOwner(kTRUE);

  fOutputQA = new TList();
  fOutputQA->SetOwner(kTRUE);

  fParticleArray = new TObjArray;
  fParticleArray->SetOwner(kTRUE);

  DefineHistoNames();


  if(!fArraySkipDDaugh){
    fArraySkipDDaugh=new TArrayI(200);
  }
  if(!fArrayTrk)fArrayTrk=new TArrayI(5000);

  InitInternalVariables();
  fhCheckNotDorBdaughters=new TH1D("fhCheckNotDorBdaughters","fhCheckNotDorBdaughters",100,0,20);
  fOutputList->Add(fhCheckNotDorBdaughters);
  Int_t binsHFCounterSignals[4]={200,40,23,200};//pt,eta,type,ptQ
  Double_t minHFCounterSignals[4]={0,-2,-1.5,0};
  Double_t maxHFCounterSignals[4]={100,2,21.5,100};

  fhSparseHFCounterSignals=new THnSparseF("fhSparseHFCounterSignals","fhSparseHFCounterSignals;p_{T} (GeV/c);#eta;HF signal tyep;p_{T} mother Quark (GeV/c);",4,binsHFCounterSignals,minHFCounterSignals,maxHFCounterSignals);
  fOutputList->Add(fhSparseHFCounterSignals);

  Int_t binsJetSparse[7]={50,10,32,20,6,20,20};
  Double_t minJetSparse[7]={0,0,0,-2,-0.5,0,0};
  Double_t maxJetSparse[7]={100,1,TMath::TwoPi(),2,5.5,40,40};

  fhSparseJetProp=new THnSparseF("fhSparseJetProp","fhSparseJetProp;jet pt (GeV/c);jet area; jet phi (rad); jet #eta; tag info; N costituents; quark pt (GeV/c);",7,binsJetSparse,minJetSparse,maxJetSparse);

  fOutputList->Add(fhSparseJetProp);

  Int_t binsJetSparseExtended[12]={50,50,10,50,32,20,6,10,10,20,20,20};
  Double_t minJetSparseExtended[12]={0,0,0,0,0,-2,-0.5,-0.5,-0.5,0,0,0};
  Double_t maxJetSparseExtended[12]={100,100,1,100,TMath::TwoPi(),2,5.5,9,9,40,40,40};

  fhSparseJetPropExtended=new THnSparseF("fhSparseJetPropExtended","fhSparseJetPropExtended;jet pt (GeV/c);jet pt costit (GeV/c);jet area; jet momentum (GeV/c);jet phi (rad); jet #eta; tag info; nbquarks; ncquarks; N costituents; <pt> (GeV/c); costituent ptmax (GeV/c);",12,binsJetSparseExtended,minJetSparseExtended,maxJetSparseExtended);

  fOutputList->Add(fhSparseJetPropExtended);
  Int_t binsSparseFF[11]={24,50,20,20,20,20,10,6,10,10,20};
  Double_t minSparseFF[11]={0,0,0,-1.0,0,0,0,-0.5,-0.5,-0.5,0};
  Double_t maxSparseFF[11]={1.2,100,40,1.0,40,40,1,5.5,9.5,9.5,40};

  fhSparseFFstudy=new THnSparseF("fhSparseFFstudy","fhSparseJetFFstudy; mom fraction; track pt (GeV/c) ; jet pt (GeV/c); jet #eta; <pt> (GeV/c); costituent ptmax (GeV/c); jet area; tag info; nbquarks; ncquarks; N costituents;",11,binsSparseFF,minSparseFF,maxSparseFF);
  fOutputList->Add(fhSparseFFstudy);

  Int_t binSparseDJets[9]={50,20,100,20,10,20,5,6,6};
  Double_t minSparseDJets[9]={0,-2,0,-0.5,0,-0.5,0.5,-0.5,-0.5};
  Double_t maxSparseDJets[9]={100,2,100,1.5,0.5,1.5,5.5,5.5,5.5};

  fhSparseDJets=new THnSparseF("fhSparseDJets","fhSparseDJets; jet pt (GeV/c); jet eta; pt D (GeV/c); z_{||}^{D,jet};#DeltaR^{D,jet};p(D)/p(c);order of the D;n c-quark in jet;n D in jet;",9,binSparseDJets,minSparseDJets,maxSparseDJets);
  fOutputList->Add(fhSparseDJets);

  fhSparseBJets=new THnSparseF("fhSparseBJets","fhSparseBJets; jet pt (GeV/c); jet eta; pt B (GeV/c); z_{||}^{B,jet};#DeltaR^{B,jet};p(B)/p(b);order of the B;n b-quark in jet;n B in jet;",9,binSparseDJets,minSparseDJets,maxSparseDJets);
  fOutputList->Add(fhSparseBJets);

  fhSparseFeedDownDJets=new THnSparseF("fhSparseFeedDownDJets","fhSparseFeedDownDJets; jet pt (GeV/c); jet eta; pt D (GeV/c); z_{||}^{D,jet};#DeltaR^{D,jet};p(D)/p(Q);order of the D;n b-quark in jet;n D in jet;",9,binSparseDJets,minSparseDJets,maxSparseDJets);
  fOutputList->Add(fhSparseFeedDownDJets);


  Int_t binSparseEleJets[16]={50,20,100,20,10,20,5,7,6,6,5,5,5,5,5,5};
  Double_t minSparseEleJets[16]={0,-2,0,-0.5,0,-0.5,0.5,-0.5,-0.5,-0.5,-0.5,-0.5,-0.5,-0.5,-0.5,-0.5};
  Double_t maxSparseEleJets[16]={100,2,100,1.5,0.5,1.5,5.5,6.5,5.5,5.5,4.5,4.5,4.5,4.5,4.5,4.5};
  fhSparseEleJets=new THnSparseF("fhSparseEleJets","fhSparseEleJets; jet pt (GeV/c); jet eta; pt ele (GeV/c); z_{||}^{ele,jet};#DeltaR^{ele,jet};p(ele)/p(c);order of the ele;ele type; n b-quark in jet;n c-quark in jet;n ele b in jet;n ele cb in jet;n ele c in jet;n ele conv in jet;n ele Dalitz in jet;n ele Unknown in jet;",16,binSparseEleJets,minSparseEleJets,maxSparseEleJets);
  fOutputList->Add(fhSparseEleJets);

  //      Double_t pointsparse[16]={jetpt,fJets[i].eta(),-1,-1,-1,-1,-2,-1,nbq,ncq,neleb,nelecb,nelec,neleConv,neleDalitz,neleUnknown};

  Int_t binSparsePromptDJetCorrel[10]={60,20,8,60,50,20,64,20,2,50};
  Double_t minSparsePromptDJetCorrel[10]={0.,-2.,-0.5,0,0,-2,-TMath::PiOver2(),-2,-0.5,0};
  Double_t maxSparsePromptDJetCorrel[10]={30.,2.,7.5,30,100,2,1.5*TMath::Pi(),2,1.5,100};
  fhSparseDJetCorrel=new THnSparseF("fhSparseDJetCorrel","fhSparseDJetCorrel;p_{T}^{Trig};#eta^{Trig};D species;p_{T}^{Q, trig};p_{T}^{jet};#eta^{jet};#Delta#varphi (rad);#Delta#eta;is costituent;p_{T}^{Q, jet};",10,binSparsePromptDJetCorrel,minSparsePromptDJetCorrel,maxSparsePromptDJetCorrel);
  fOutputList->Add(fhSparseDJetCorrel);

  fhSparseBJetCorrel=new THnSparseF("fhSparseBJetCorrel","fhSparseBJetCorrel;p_{T}^{Trig};#eta^{Trig};B species;p_{T}^{Q, trig};p_{T}^{jet};#eta^{jet};#Delta#varphi (rad);#Delta#eta;is costituent;p_{T}^{Q, jet};",10,binSparsePromptDJetCorrel,minSparsePromptDJetCorrel,maxSparsePromptDJetCorrel);
  fOutputList->Add(fhSparseBJetCorrel);

  fhSparseFeedDownDJetCorrel=new THnSparseF("fhSparseFeedDownDJetCorrel","fhSparseFeedDownDJetCorrel;p_{T}^{Trig};#eta^{Trig};D species;p_{T}^{Q, trig};p_{T}^{jet};#eta^{jet};#Delta#varphi (rad);#Delta#eta;is costituent;p_{T}^{Q, jet};",10,binSparsePromptDJetCorrel,minSparsePromptDJetCorrel,maxSparsePromptDJetCorrel);
  fOutputList->Add(fhSparseFeedDownDJetCorrel);

  fhSparseHFeJetCorrel=new THnSparseF("fhSparseHFeJetCorrel","fhSparseHFeJetCorrel;p_{T}^{Trig};#eta^{Trig};ele origin;p_{T}^{Q, trig};p_{T}^{jet};#eta^{jet};#Delta#varphi (rad);#Delta#eta;is costituent;p_{T}^{Q, jet};",10,binSparsePromptDJetCorrel,minSparsePromptDJetCorrel,maxSparsePromptDJetCorrel);
  fOutputList->Add(fhSparseHFeJetCorrel);

  fXsectionNtuple=new TNtuple("fXsectionNtuple","event cross section","xsecEv:xsecAv");
  fOutputList->Add(fXsectionNtuple);

  PostData(1, fOutputQA);
  PostData(2, fOutputList);
}

//______________________________| Init
void AliAnalysisTaskSEhfcjMCanalysis::Init()
{
  if(fDebug > 1) printf("AliAnalysisTaskSEhfcjMCanalysis::Init() \n");
  fMcHandler = dynamic_cast<AliInputEventHandler*> (AliAnalysisManager::GetAnalysisManager()->GetMCtruthEventHandler());
}


void AliAnalysisTaskSEhfcjMCanalysis::CalculateGeneralProperties(){
  if(fIsEventProp)CalculateEventProperties(fMcEvent);
  if(fIsPartProp)CalculateParticleProperties(fMcEvent);

}
//______________________________| User Exec
void AliAnalysisTaskSEhfcjMCanalysis::UserExec(Option_t *)
{

  if(fDebug > 1) printf("AliAnalysisTaskSEhfcjMCanalysis::UserExec \n");
  Init();

  if(fMcHandler){
    fMcEvent = fMcHandler->MCEvent();
  }else{
    if(fDebug > 1) printf("AliAnalysisTaskSEhfcjMCanalysis::Handler() fMcHandler=NULL\n");
    return;
  }
  if(!fMcEvent){
    if(fDebug > 1) printf("AliAnalysisTaskSEhfcjMCanalysis::UserExec()   fMcEvent=NULL \n");
    return;
  }

  fStack = ((AliMCEvent*)fMcEvent)->Stack();

  Double_t ww=0.,xsec=0.;
  AliGenEventHeader *gheader=fMcEvent->GenEventHeader();
  TString strheadname=gheader->GetName();
  TString strheadtitle=gheader->GetTitle();
  AliGenPythiaEventHeader *pythiaGenHeader=0x0;
  //  Printf("header name %s. title %s #",strheadname.Data(),strheadtitle.Data());
  if(strheadtitle.Contains("pythia") || strheadtitle.Contains("PYTHIA") || strheadtitle.EqualTo("Event Header") )pythiaGenHeader=(AliGenPythiaEventHeader*)gheader;
  //  AliGenPythiaEventHeader*  pythiaGenHeader = AliAnalysisHelperJetTasks::GetPythiaEventHeader(fMcEvent);

  if(!IsMCEventSelected(fMcEvent)){// NOTE THAT THIS EVENT SELECTION CHECKS ONLY PROPERTIES THAT CANNOT CHANGE THE CROSS_SECTION, e.g. Z-vertex! IT IS IMPORTANT!!
    return;
  }
  if(pythiaGenHeader){
    Double_t xsection=pythiaGenHeader->GetXsection();
    feventweight=pythiaGenHeader->EventWeight();
    feventtrials=pythiaGenHeader->Trials();
    ((TH1F*)fOutputQA->FindObject(Form("fWeight")))->Fill(feventweight);
    ((TH1F*)fOutputQA->FindObject(Form("fSigma")))->Fill(xsection);//mb
    fxsection+=xsection;
    Double_t xsecAv=fxsection/(Double_t)(feventcounter+1);
    if(feventcounter<100)fXsectionNtuple->Fill(xsection,xsecAv);// sample first 100 events
    else if(TMath::Abs(xsection-xsecAv)>0.1*xsecAv)fXsectionNtuple->Fill(xsection,xsecAv); // note down all xsection values which deviate more than 10% from current average
    else if(feventcounter%200==0)fXsectionNtuple->Fill(xsection,xsecAv,feventcounter+1);// sample a value each 200 events

    //    printf("******* weight=%f, xsec=%f , trials=%f\n",feventweight,xsection,feventtrials);
  }
  feventcounter++;


  //  Printf("N tracks: %d",fMcEvent->GetNumberOfTracks());

  CalculateGeneralProperties();

  for(Int_t iPart = 0; iPart < fMcEvent->GetNumberOfTracks(); iPart++){

    AliVParticle* mcPart = (AliVParticle*)fMcEvent->GetTrack(iPart);
    if (!mcPart)continue;
    fHistEventsProcessed->Fill(3.5);

    Bool_t IsParticleMCSelected = IsMCParticleGenerated(mcPart);
    if(!IsParticleMCSelected)continue;
    fHistEventsProcessed->Fill(4.5);

    Bool_t IsParticleMCKineAccepted = IsMCParticleInKineCriteria(mcPart);
    if(!IsParticleMCKineAccepted)continue;
    fHistEventsProcessed->Fill(5.5);

    //Storing part array after event+part selections but right now its not used
    //    fParticleArray->Add(mcPart);  
  }


  if(fIsCorrOfHeavyFlavor)CalculateHFHadronCorrelations();
  if(fIsCorrOfQQbar)CalculateQQBarCorrelations();
  if(fIsCorrOfHadronHadron)CalculateHadronHadronCorrelations(fParticleArray);

  SetBeautyAndCharmPartIndex();
  FillHFCounterPtEtaSparse();

  if(fIsHFjetsAnalysis){


    ReconstructChargedJets();
    CalculateChargedJetProperties();

    if(fFillHFJet)FillParticleTaggedJetProperties();
    if(fFillJetCorrelations)FillHFJetCorrelations();

    //     if(fJetDef) delete fJetDef;
    //     if(fGhostArea) delete fGhostArea;
    //     if(fAreaDef) delete fAreaDef;
    //     if(fCSA) delete fCSA;
    //     fJets.clear();
    //    fInputParticlesJet.clear();
  }

  ResetInternalVariables();

  PostData(1, fOutputQA);
  PostData(2, fOutputList);


  return;
}
//______________________________________________________ 
Bool_t AliAnalysisTaskSEhfcjMCanalysis::IsMCParticleReconstructableForChargedJets(AliVParticle *part){
  // check that part is physical primary and a charge reconstructable particle (pion, muon, kaon, electron, proton)
  if((!fStack->IsPhysicalPrimary(part->GetLabel()))&&fSelectPhysPrimaryForJets)return kFALSE;

  // apply DCA cut
  Double_t xyz[3];
  part->XvYvZv(xyz);
  if(xyz[0]*xyz[0]+xyz[1]*xyz[1]>fDCAmcSelEle*fDCAmcSelEle)return kFALSE;

  Int_t pdg=TMath::Abs(part->PdgCode());
  if(fSelOnlyEMuPiKProtForJets &&pdg!=11 && pdg!=13 && pdg!=211 && pdg!=321 && pdg!=2212)return kFALSE;
  Double_t pt=part->Pt();
  Double_t eta=part->Eta();
  if(pt<fminptjettrack)return kFALSE;
  if(pt>fmaxptjettrack)return kFALSE;
  if(eta<fminetajettrack)return kFALSE;
  if(eta>fmaxetajettrack)return kFALSE;

  return kTRUE;
}

//______________________________________________________ 
void AliAnalysisTaskSEhfcjMCanalysis::ReconstructChargedJets(){

  Int_t counter=0;
  //  Printf("Reconstructed charged jets, total number of tracks: %d",fMcEvent->GetNumberOfTracks());
  for(Int_t j=0;j<fMcEvent->GetNumberOfTracks();j++){  
    AliVParticle *mcPart=(AliVParticle*)fMcEvent->GetTrack(j);
    if(!IsMCParticleReconstructableForChargedJets(mcPart)){
      //      if(!CheckIsFromDdecay(mcPart))Printf("A %d not from D was rejected",mcPart->PdgCode());
      continue;
    }
    if(!CheckIsFromDdecay(mcPart)){
      fhCheckNotDorBdaughters->Fill(mcPart->Pt());
      //      Printf("A %d not from D was selected", mcPart->PdgCode());
    }
    //    else Printf("A %d from D was selected", mcPart->PdgCode());
    fastjet::PseudoJet partPseudoJet(mcPart->Px(),mcPart->Py(),mcPart->Pz(),mcPart->P());
    partPseudoJet.set_user_index(j);
    fInputParticlesJet.push_back(partPseudoJet);
    counter++;
  }
  //  Printf("Selected particle: %d",counter);


  fJetDef = new fastjet::JetDefinition(fastjet::antikt_algorithm, fJetRcut,fastjet::pt_scheme, fastjet::Best);
  // fJetDef = new fastjet::JetDefinition(fastjet::antikt_algorithm, fJetRcut,fastjet::BIpt_scheme, fastjet::Best);
  fGhostArea = new fastjet::GhostedAreaSpec(0.9, 1, 0.01);
  // fGhostArea = new fastjet::GhostedAreaSpec(0.9, 1, 0.005);
  fAreaDef = new fastjet::AreaDefinition(fastjet::active_area_explicit_ghosts, *fGhostArea);
  fCSA = new fastjet::ClusterSequenceArea(fInputParticlesJet, *fJetDef, *fAreaDef);
  fJets = sorted_by_pt(fCSA->inclusive_jets(0.0));

}
//______________________________________________________ 
Bool_t AliAnalysisTaskSEhfcjMCanalysis::CheckIsFromBdecay(AliVParticle *vpart){
  AliVParticle *partM=0x0;
  Int_t labelM=vpart->GetMother();
  if(labelM<0)return kFALSE;
  while(labelM>=0){    
    partM=fMcEvent->GetTrack(labelM);
    Int_t pdgM=TMath::Abs(partM->PdgCode());
    if((500<=pdgM && pdgM<=599) || (5000<=pdgM && pdgM <=5999))return kTRUE;
    labelM=partM->GetMother();    
  }
  return kFALSE;
}

//______________________________________________________ 
Bool_t AliAnalysisTaskSEhfcjMCanalysis::CheckIsFromDdecay(AliVParticle *vpart){
  AliVParticle *partM=0x0;
  Int_t labelM=vpart->GetMother();
  if(labelM<0)return kFALSE;
  while(labelM>=0){    
    partM=fMcEvent->GetTrack(labelM);
    Int_t pdgM=TMath::Abs(partM->PdgCode());
    if((400<=pdgM && pdgM<=499) || (4000<=pdgM && pdgM <=4999))return kTRUE;
    labelM=partM->GetMother();    
  }
  return kFALSE;
}

//______________________________________________________ 
Bool_t AliAnalysisTaskSEhfcjMCanalysis::CheckIsFinal(AliVParticle *part,Int_t pdgRange){/// check that the particle has no daughters equal to pdgRange; pdgRange = 4--> charm quark, 5 beauty quark, 400 charm hadron, 500 b hadron; numbers for light quarks and gluons: 2 = u,d,s, quarks, 21 = gluon ; However for LQ and gluons it might be not so well defined
  Int_t pdg=TMath::Abs(part->PdgCode());
  if(pdg!=pdgRange){
    if(pdgRange==400){ if( ! ( (400<=pdg && pdg<=499) || (4000<=pdg && pdg <=4999)))return kFALSE;}
    else if(pdgRange==500){ if( !( (500<=pdg && pdg<=599) || (5000<=pdg && pdg <=5999)))return kFALSE;}
    else if(pdgRange==2){if(! (1<=pdg && pdg<=3)) return kFALSE;}
    else return kFALSE;
  }

  Int_t fd=part->GetFirstDaughter();
  Int_t ld=part->GetLastDaughter();
  if(fd>0 && ld>0){
    for(Int_t jd=fd;jd<=ld;jd++){
      AliVParticle *part=(AliVParticle*)fMcEvent->GetTrack(jd);
      Int_t pdgd=TMath::Abs(part->PdgCode());
      if(pdgd==pdg)return kFALSE;
      if(pdgRange==400){ if(  (400<=pdgd && pdgd<=499) || (4000<=pdgd && pdgd <=4999))return kFALSE;}
      else if(pdgRange==500){ if( (500<=pdgd && pdgd<=599) || (5000<=pdg && pdgd <=5999))return kFALSE;}
      else if(pdgRange==2){
        if( 1<=pdgd && pdgd<=3 ) return kFALSE;
      } 
    }
  }

  return kTRUE;

}

//______________________________________________________ 
Bool_t AliAnalysisTaskSEhfcjMCanalysis::CheckIsFirst(AliVParticle *part,Int_t pdgRange){/// check that the particle has no daughters equal to pdgRange; pdgRange = 4--> charm quark, 5 beauty quark, 400 charm hadron, 500 b hadron; numbers for light quarks and gluons: 2 = u,d,s, quarks, 21 = gluon ; However for LQ and gluons it might be not so well defined
  Int_t pdg=TMath::Abs(part->PdgCode());
  if(pdg!=pdgRange){
    if(pdgRange==400){ if( ! ( (400<=pdg && pdg<=499) || (4000<=pdg && pdg <=4999)))return kFALSE;}
    else if(pdgRange==500){ if( !( (500<=pdg && pdg<=599) || (5000<=pdg && pdg <=5999)))return kFALSE;}
    else if(pdgRange==2){if(! (1<=pdg && pdg<=3)) return kFALSE;}
    else return kFALSE;
  }

  Int_t lmum=part->GetMother();
  if(lmum<0)return kTRUE;
  AliVParticle *mpart=(AliVParticle*)fMcEvent->GetTrack(lmum);

  Int_t mpdg=mpart->PdgCode();

  if(mpdg==pdg)return kFALSE;
  if(pdgRange==400){ if(  (400<=mpdg && mpdg<=499) || (4000<=mpdg && mpdg <=4999))return kFALSE;}
  else if(pdgRange==500){ if( (500<=mpdg && mpdg<=599) || (5000<=pdg && mpdg <=5999))return kFALSE;}
  else if(pdgRange==2){
    if( 1<=mpdg && mpdg<=3 ) return kFALSE;
  }

  return CheckIsFirst(mpart,pdgRange);

}

void AliAnalysisTaskSEhfcjMCanalysis::FillHFCounterPtEtaSparse(){


  if(flastbquark==-1)SetBeautyAndCharmPartIndex();
  Double_t point[4];

  point[2]=1;
  point[3]=-1;
  for(Int_t i=0;i<flastbquark;i++){
    AliVParticle *part=(AliVParticle*)fMCEvent->GetTrack(fArraybquark->At(i));
    if(!part){
      AliFatal("INDEX MISMATCH");
      continue;
    }
    point[0]=part->Pt();
    point[1]=part->Eta();
    fhSparseHFCounterSignals->Fill(point);
  }


  point[2]=2;
  point[3]=-1;
  for(Int_t i=0;i<flastcquark;i++){
    AliVParticle *part=(AliVParticle*)fMCEvent->GetTrack(fArraycquark->At(i));
    if(!part){
      AliFatal("INDEX MISMATCH");
      continue;
    }
    point[0]=part->Pt();
    point[1]=part->Eta();
    fhSparseHFCounterSignals->Fill(point);
  }

  point[2]=6;
  for(Int_t i=0;i<flastbhadron;i++){
    point[3]=-1;
    AliVParticle *part=(AliVParticle*)fMCEvent->GetTrack(fArraybhadron->At(i));
    if(!part){
      AliFatal("INDEX MISMATCH");
      continue;
    }
    UInt_t pdg=part->PdgCode();
    if(pdg==511)point[2]=3;// B0
    if(pdg==521)point[2]=4;// B+
    if(pdg==513 || pdg==523)point[2]=5;// B*0 or B*+
    point[0]=part->Pt();
    point[1]=part->Eta();
    AliVParticle *pquark=GetMotherQuark(part);	        
    if(pquark){	    
      point[3]=pquark->Pt();	      
    }
    fhSparseHFCounterSignals->Fill(point);
  }


  point[2]=10;
  for(Int_t i=0;i<flastchadronfromB;i++){
    point[3]=-1;
    AliVParticle *part=(AliVParticle*)fMCEvent->GetTrack(fArraychadronfromB->At(i));
    if(!part){
      AliFatal("INDEX MISMATCH");
      continue;
    }
    UInt_t pdg=part->PdgCode();
    if(pdg==421)point[2]=7;// D0
    if(pdg==411)point[2]=8;// D+
    if(pdg==413 || pdg==423)point[2]=9;// D*+ or D*0
    point[0]=part->Pt();
    point[1]=part->Eta();
    AliVParticle *pquark=GetMotherQuark(part);	        
    if(pquark){	    
      point[3]=pquark->Pt();	      
    }
    fhSparseHFCounterSignals->Fill(point);
  }


  point[2]=14;
  for(Int_t i=0;i<flastchadron;i++){
    point[3]=-1;
    AliVParticle *part=(AliVParticle*)fMCEvent->GetTrack(fArraychadron->At(i));
    if(!part){
      AliFatal("INDEX MISMATCH");
      continue;
    }
    UInt_t pdg=part->PdgCode();
    if(pdg==421)point[2]=11;// D0
    if(pdg==411)point[2]=12;// D+
    if(pdg==413 || pdg==423)point[2]=13;// D*+ or D*0
    point[0]=part->Pt();
    point[1]=part->Eta();
    AliVParticle *pquark=GetMotherQuark(part);	        
    if(pquark){	    
      point[3]=pquark->Pt();	      
    }
    fhSparseHFCounterSignals->Fill(point);
  }


  point[2]=15;
  for(Int_t i=0;i<flastbele;i++){
    point[3]=-1;
    AliVParticle *part=(AliVParticle*)fMCEvent->GetTrack(fArraybele->At(i));
    if(!part){
      AliFatal("INDEX MISMATCH");
      continue;
    }
    point[0]=part->Pt();
    point[1]=part->Eta();
    AliVParticle *pquark=GetMotherQuark(part);	        
    if(pquark){	    
      point[3]=pquark->Pt();	      
    }
    fhSparseHFCounterSignals->Fill(point);
  }


  point[2]=16;
  for(Int_t i=0;i<flastcelefromB;i++){
    point[3]=-1;
    AliVParticle *part=(AliVParticle*)fMCEvent->GetTrack(fArraycelefromB->At(i));
    if(!part){
      AliFatal("INDEX MISMATCH");
      continue;
    }
    point[0]=part->Pt();
    point[1]=part->Eta();
    AliVParticle *pquark=GetMotherQuark(part);	        
    if(pquark){	    
      point[3]=pquark->Pt();	      
    }
    fhSparseHFCounterSignals->Fill(point);
  }

  point[2]=17;
  for(Int_t i=0;i<flastcele;i++){
    point[3]=-1;
    AliVParticle *part=(AliVParticle*)fMCEvent->GetTrack(fArraycele->At(i));
    if(!part){
      AliFatal("INDEX MISMATCH");
      continue;
    }
    point[0]=part->Pt();
    point[1]=part->Eta();
    AliVParticle *pquark=GetMotherQuark(part);	        
    if(pquark){	    
      point[3]=pquark->Pt();	      
    }
    fhSparseHFCounterSignals->Fill(point);
  }

  point[2]=18;
  for(Int_t i=0;i<flastGammaele;i++){
    point[3]=-1;
    AliVParticle *part=(AliVParticle*)fMCEvent->GetTrack(fArrayGammaele->At(i));
    if(!part){
      AliFatal("INDEX MISMATCH");
      continue;
    }
    point[0]=part->Pt();
    point[1]=part->Eta();
    AliVParticle *pquark=GetMotherQuark(part);	        
    if(pquark){	    
      point[3]=pquark->Pt();	      
    }
    fhSparseHFCounterSignals->Fill(point);
  }


  point[2]=19;
  for(Int_t i=0;i<flastDalitzele;i++){
    point[3]=-1;
    AliVParticle *part=(AliVParticle*)fMCEvent->GetTrack(fArrayDalitzele->At(i));
    if(!part){
      AliFatal("INDEX MISMATCH");
      continue;
    }
    point[0]=part->Pt();
    point[1]=part->Eta();
    AliVParticle *pquark=GetMotherQuark(part);	        
    if(pquark){	    
      point[3]=pquark->Pt();	      
    }
    fhSparseHFCounterSignals->Fill(point);
  }


  point[2]=20;
  for(Int_t i=0;i<flastUnknownele;i++){
    point[3]=-1;
    AliVParticle *part=(AliVParticle*)fMCEvent->GetTrack(fArrayUnknownele->At(i));
    if(!part){
      AliFatal("INDEX MISMATCH");
      continue;
    }
    point[0]=part->Pt();
    point[1]=part->Eta();
    AliVParticle *pquark=GetMotherQuark(part);	        
    if(pquark){	    
      point[3]=pquark->Pt();	      
    }
    fhSparseHFCounterSignals->Fill(point);
  }



}

//______________________________________________________ 
void AliAnalysisTaskSEhfcjMCanalysis::SetBeautyAndCharmPartIndex(){

  flastbquark=0;
  flastcquark=0;
  flastbhadron=0;
  flastchadron=0;  
  flastchadronfromB=0;
  flastbele=0;  
  flastcele=0;  
  flastcelefromB=0;  
  flastGammaele=0;
  flastDalitzele=0;
  flastUnknownele=0;

  for(Int_t k=0;k<fMcEvent->GetNumberOfTracks();k++){

    AliVParticle *vpart=(AliVParticle*)fMcEvent->GetTrack(k);
    Int_t pdgcode=TMath::Abs(vpart->PdgCode());
    if(pdgcode==5){
      if(CheckIsFirst(vpart,5)){
        fArraybquark->AddAt(k,flastbquark);
        flastbquark++;
      }
    }
    else if(pdgcode==4){
      if(CheckIsFirst(vpart,4)){
        fArraycquark->AddAt(k,flastcquark);
        flastcquark++;
      }
    }
    else if( (pdgcode>=500&&pdgcode<600) || (pdgcode>=5000&&pdgcode<6000) ){
      if(CheckIsFinal(vpart,500)){
        fArraybhadron->AddAt(k,flastbhadron);
        flastbhadron++;
      }
    }
    else if( (pdgcode>=400&&pdgcode<500) || (pdgcode>=4000&&pdgcode<5000) ){
      if(CheckIsFinal(vpart,400)){
        if(!CheckIsFromBdecay(vpart)){
          fArraychadron->AddAt(k,flastchadron);
          flastchadron++;
        }
        else {
          fArraychadronfromB->AddAt(k,flastchadronfromB);
          flastchadronfromB++;
        }
      }
    }
    else if(pdgcode==11){ //Electron Stuff
      Int_t im=vpart->GetMother();
      Double_t xyz[3];
      vpart->XvYvZv(xyz);
      if(xyz[0]*xyz[0]+xyz[1]*xyz[1]<fDCAmcSelEle*fDCAmcSelEle){//outside outer SPD radius --> do not store these conversions
        if(im>0){
          AliVParticle *vpartM=(AliVParticle*)fMcEvent->GetTrack(im);
          Int_t pdgMother=vpartM->PdgCode();
          if( (pdgMother>=500&&pdgMother<600) || (pdgMother>=5000&&pdgMother<6000) ){
            fArraybele->AddAt(k,flastbele);
            flastbele++;
          }
          else if( (pdgMother>=400&&pdgMother<500) || (pdgMother>=4000&&pdgMother<5000) ){
            if(!CheckIsFromBdecay(vpart)){
              fArraycele->AddAt(k,flastcele);
              flastcele++;
            }
            else {
              fArraycelefromB->AddAt(k,flastcelefromB);
              flastcelefromB++;
            }
          }
          else if(pdgMother==22){// GAMMA CONVERSIONS
            fArrayGammaele->AddAt(k,flastGammaele);
            flastGammaele++;
          }
          else if(pdgMother==111||pdgMother==113||pdgMother==221||pdgMother==331||pdgMother==223||pdgMother==333){ //DALITZ DECAY (from Pi0,rho0, eta, eta', omega, phi...)
            fArrayDalitzele->AddAt(k,flastDalitzele);
            flastDalitzele++;
          }
          else {// unknonw source ele
            fArrayUnknownele->AddAt(k,flastUnknownele);
            flastUnknownele++;
          }
        }
      }    
    }
  }
  return ;
}

//___________________________________________________________________________________________________________ 
AliVParticle* AliAnalysisTaskSEhfcjMCanalysis::TagJetPartonDeltaR(fastjet::PseudoJet jet,Double_t radiusSq,Int_t &tag){/// check for bquarks and cquarks within sqrt(readiusSq); first tag digit =0,1,2,3 -> jet is light quarks/gluons;  4 jet is charm, 5 jet is beauty; second digit=ncquarks; 3rd digits = nbquarks. Therefore, tag = 100 *nb +10 *nc + tag selection; if nb,nc =9 --> means that there are more than 8

  if(flastbquark==-1){
    SetBeautyAndCharmPartIndex();
  }
  Int_t nb=0,nc=0;
  AliVParticle *vpartTag=0x0;
  Double_t pttag=0,ptprob=0;
  for(Int_t j=0;j<flastbquark;j++){
    AliVParticle *vpart=(AliVParticle*)fMcEvent->GetTrack(fArraybquark->At(j));
    fastjet::PseudoJet partPseudoJet(vpart->Px(),vpart->Py(),vpart->Pz(),vpart->P());
    if(jet.plain_distance(partPseudoJet)<radiusSq){
      tag=5;      
      nb++;
      ptprob=vpart->Px()*vpart->Px()+vpart->Py()*vpart->Py();
      if(pttag<ptprob){
        vpartTag=vpart;
        pttag=ptprob;
      }

    }
  }

  ptprob=0,pttag=0;
  for(Int_t j=0;j<flastcquark;j++){
    AliVParticle *vpart=(AliVParticle*)fMcEvent->GetTrack(fArraycquark->At(j));
    fastjet::PseudoJet partPseudoJet(vpart->Px(),vpart->Py(),vpart->Pz(),vpart->P());
    if(jet.plain_distance(partPseudoJet)<radiusSq){
      nc++;
      ptprob=vpart->Px()*vpart->Px()+vpart->Py()*vpart->Py();
      if(!vpartTag){	
        tag=4;
        vpartTag=vpart;
      }
      else {
        if(tag==4 && pttag<ptprob){
          vpartTag=vpart;
          pttag=ptprob;
        }
      }
    }
  }

  if(nb>9)nb=9;
  if(nc>9)nc=9;
  tag+=nb*100+nc*10;
  if(vpartTag)return vpartTag;  
  tag=3; 
  return 0x0;// temporary --> do nothing for light quarks and gluons

  //   for(Int_t k=0;k<fMcEvent->GetNumberOfTracks();k++){

  //     AliVParticle *vpart=(AliVParticle*)obarconstituents->At(ic);
  //     Int_t pdgcode=TMath::Abs(vpart->PdgCode()){
  //       if(pdgcode==5){

  //       }
  //     }
  //   }


  //     Int_t mcEntries=arrayMC->GetEntriesFast();
  //     Double_t ptpart=-1;
  //     Double_t dR=-99;
  //     const Int_t arraySize=99;

  //   Int_t countpart[arraySize],countpartcode[arraySize],maxInd=-1,count=0;
  //   Double_t maxPt=0;
  //   for(Int_t ii=0;ii<arraySize;ii++){
  //     countpart[ii]=0;
  //     countpartcode[ii]=0;
  //   }

  //   for(Int_t ii=0;ii<mcEntries;ii++){
  //     AliAODMCParticle* part =  (AliAODMCParticle*)  arrayMC->At(ii);
  //     if(!part)continue;
  //     Int_t pdgcode=part->GetPdgCode();
  //     if(abs(pdgcode)==21 || ( abs(pdgcode)>=1 && abs(pdgcode)<=5)){
  //       ptpart=part->Pt();
  //       dR = jet->DeltaR(part);

  //       if(dR<radius){
  //         if(abs(pdgcode)==5){
  // 	  return part;
  // 	}
  //         else{
  // 	  if (count >arraySize-1) return 0x0;
  //           countpartcode[count]=pdgcode;
  //           countpart[count]=ii;
  //           if(ptpart>maxPt){
  //             maxPt=ptpart;
  //             maxInd=ii;
  //           }
  //           count++;
  //         }
  //       }
  //     }
  //   }

  //   for(Int_t ij=0;ij<count;ij++){
  //       if(abs(countpartcode[ij])==4)
  // 	return (AliAODMCParticle*)arrayMC->At(countpart[ij]);
  //     }
  //   if(maxInd>-1){
  //     AliAODMCParticle* partmax = (AliAODMCParticle*)arrayMC->At(maxInd);
  //     return partmax;
  //   }
  //   return 0x0;
  // }



  // }

}

//___________________________________________________________________________________________________________ 
AliVParticle* AliAnalysisTaskSEhfcjMCanalysis::TagJetParticleSpecieDeltaR(fastjet::PseudoJet jet,Double_t radiusSq,Int_t &ntagPart,AliAnalysisTaskSEhfcjMCanalysis::ESpecieTags specie){

  if(flastbquark==-1){
    SetBeautyAndCharmPartIndex();
  }
  AliVParticle *vpartTag=0x0;
  Double_t pttag=0,ptprob=0;
  ntagPart=0;
  fTempTagParticle->Reset(-1);

  if(specie==AliAnalysisTaskSEhfcjMCanalysis::kbquark){
    for(Int_t j=0;j<flastbquark;j++){
      AliVParticle *vpart=(AliVParticle*)fMcEvent->GetTrack(fArraybquark->At(j));
      fastjet::PseudoJet partPseudoJet(vpart->Px(),vpart->Py(),vpart->Pz(),vpart->P());
      if(jet.plain_distance(partPseudoJet)<radiusSq){ 
        ptprob=vpart->Px()*vpart->Px()+vpart->Py()*vpart->Py();
        fTempTagParticle->AddAt(fArraybquark->At(j),ntagPart);
        ntagPart++;
        if(pttag<ptprob){
          vpartTag=vpart;
          pttag=ptprob;
        }
      }
    }
  }
  else if(specie==AliAnalysisTaskSEhfcjMCanalysis::kcquark){
    for(Int_t j=0;j<flastcquark;j++){
      AliVParticle *vpart=(AliVParticle*)fMcEvent->GetTrack(fArraycquark->At(j));
      fastjet::PseudoJet partPseudoJet(vpart->Px(),vpart->Py(),vpart->Pz(),vpart->P());
      if(jet.plain_distance(partPseudoJet)<radiusSq){
        fTempTagParticle->AddAt(fArraycquark->At(j),ntagPart);
        ntagPart++;
        ptprob=vpart->Px()*vpart->Px()+vpart->Py()*vpart->Py();
        if(pttag<ptprob){
          vpartTag=vpart;
          pttag=ptprob;
        }
      }
    }
  }
  else if(specie==AliAnalysisTaskSEhfcjMCanalysis::kbhadron){ 
    for(Int_t j=0;j<flastbhadron;j++){
      AliVParticle *vpart=(AliVParticle*)fMcEvent->GetTrack(fArraybhadron->At(j));
      fastjet::PseudoJet partPseudoJet(vpart->Px(),vpart->Py(),vpart->Pz(),vpart->P());
      if(jet.plain_distance(partPseudoJet)<radiusSq){ 
        fTempTagParticle->AddAt(fArraybhadron->At(j),ntagPart);
        ntagPart++;
        ptprob=vpart->Px()*vpart->Px()+vpart->Py()*vpart->Py();
        if(pttag<ptprob){
          vpartTag=vpart;
          pttag=ptprob;
        }
      }
    }    
  }
  else if(specie==AliAnalysisTaskSEhfcjMCanalysis::kchadron){ 
    for(Int_t j=0;j<flastchadron;j++){
      AliVParticle *vpart=(AliVParticle*)fMcEvent->GetTrack(fArraychadron->At(j));
      fastjet::PseudoJet partPseudoJet(vpart->Px(),vpart->Py(),vpart->Pz(),vpart->P());
      if(jet.plain_distance(partPseudoJet)<radiusSq){    
        fTempTagParticle->AddAt(fArraychadron->At(j),ntagPart);
        ntagPart++;
        ptprob=vpart->Px()*vpart->Px()+vpart->Py()*vpart->Py();
        if(pttag<ptprob){
          vpartTag=vpart;
          pttag=ptprob;
        }
      }
    }    
  }
  else if(specie==AliAnalysisTaskSEhfcjMCanalysis::kDfromB){ 
    for(Int_t j=0;j<flastchadronfromB;j++){
      AliVParticle *vpart=(AliVParticle*)fMcEvent->GetTrack(fArraychadronfromB->At(j));
      fastjet::PseudoJet partPseudoJet(vpart->Px(),vpart->Py(),vpart->Pz(),vpart->P());
      if(jet.plain_distance(partPseudoJet)<radiusSq){    
        fTempTagParticle->AddAt(fArraychadronfromB->At(j),ntagPart);
        ntagPart++;
        ptprob=vpart->Px()*vpart->Px()+vpart->Py()*vpart->Py();
        if(pttag<ptprob){
          vpartTag=vpart;
          pttag=ptprob;
        }
      }
    }    
  }
  else if(specie==AliAnalysisTaskSEhfcjMCanalysis::kelefromB){ 
    for(Int_t j=0;j<flastbele;j++){
      AliVParticle *vpart=(AliVParticle*)fMcEvent->GetTrack(fArraybele->At(j));
      fastjet::PseudoJet partPseudoJet(vpart->Px(),vpart->Py(),vpart->Pz(),vpart->P());
      if(jet.plain_distance(partPseudoJet)<radiusSq){    
        fTempTagParticle->AddAt(fArraybele->At(j),ntagPart);
        ntagPart++;
        ptprob=vpart->Px()*vpart->Px()+vpart->Py()*vpart->Py();
        if(pttag<ptprob){
          vpartTag=vpart;
          pttag=ptprob;
        }
      }
    }    
  }
  else if(specie==AliAnalysisTaskSEhfcjMCanalysis::kelefromD){ 
    for(Int_t j=0;j<flastcele;j++){
      AliVParticle *vpart=(AliVParticle*)fMcEvent->GetTrack(fArraycele->At(j));
      fastjet::PseudoJet partPseudoJet(vpart->Px(),vpart->Py(),vpart->Pz(),vpart->P());
      if(jet.plain_distance(partPseudoJet)<radiusSq){    
        fTempTagParticle->AddAt(fArraycele->At(j),ntagPart);
        ntagPart++;
        ptprob=vpart->Px()*vpart->Px()+vpart->Py()*vpart->Py();
        if(pttag<ptprob){
          vpartTag=vpart;
          pttag=ptprob;
        }
      }
    }    
  }
  else if(specie==AliAnalysisTaskSEhfcjMCanalysis::kelefromDfromB){ 
    for(Int_t j=0;j<flastcelefromB;j++){
      AliVParticle *vpart=(AliVParticle*)fMcEvent->GetTrack(fArraycelefromB->At(j));
      fastjet::PseudoJet partPseudoJet(vpart->Px(),vpart->Py(),vpart->Pz(),vpart->P());
      if(jet.plain_distance(partPseudoJet)<radiusSq){    
        fTempTagParticle->AddAt(fArraycelefromB->At(j),ntagPart);
        ntagPart++;
        ptprob=vpart->Px()*vpart->Px()+vpart->Py()*vpart->Py();
        if(pttag<ptprob){
          vpartTag=vpart;
          pttag=ptprob;
        }
      }
    }    
  }
  else if(specie==AliAnalysisTaskSEhfcjMCanalysis::keleconversion){ 
    for(Int_t j=0;j<flastGammaele;j++){
      AliVParticle *vpart=(AliVParticle*)fMcEvent->GetTrack(fArrayGammaele->At(j));
      fastjet::PseudoJet partPseudoJet(vpart->Px(),vpart->Py(),vpart->Pz(),vpart->P());
      if(jet.plain_distance(partPseudoJet)<radiusSq){    
        fTempTagParticle->AddAt(fArrayGammaele->At(j),ntagPart);
        ntagPart++;
        ptprob=vpart->Px()*vpart->Px()+vpart->Py()*vpart->Py();
        if(pttag<ptprob){
          vpartTag=vpart;
          pttag=ptprob;
        }
      }
    }    
  }
  else if(specie==AliAnalysisTaskSEhfcjMCanalysis::keleDalitz){ 
    for(Int_t j=0;j<flastDalitzele;j++){
      AliVParticle *vpart=(AliVParticle*)fMcEvent->GetTrack(fArrayDalitzele->At(j));
      fastjet::PseudoJet partPseudoJet(vpart->Px(),vpart->Py(),vpart->Pz(),vpart->P());
      if(jet.plain_distance(partPseudoJet)<radiusSq){    
        fTempTagParticle->AddAt(fArrayDalitzele->At(j),ntagPart);
        ntagPart++;
        ptprob=vpart->Px()*vpart->Px()+vpart->Py()*vpart->Py();
        if(pttag<ptprob){
          vpartTag=vpart;
          pttag=ptprob;
        }
      }
    }    
  }
  else if(specie==AliAnalysisTaskSEhfcjMCanalysis::keleunknown){ 
    for(Int_t j=0;j<flastUnknownele;j++){
      AliVParticle *vpart=(AliVParticle*)fMcEvent->GetTrack(fArrayUnknownele->At(j));
      fastjet::PseudoJet partPseudoJet(vpart->Px(),vpart->Py(),vpart->Pz(),vpart->P());
      if(jet.plain_distance(partPseudoJet)<radiusSq){    
        fTempTagParticle->AddAt(fArrayUnknownele->At(j),ntagPart);
        ntagPart++;
        ptprob=vpart->Px()*vpart->Px()+vpart->Py()*vpart->Py();
        if(pttag<ptprob){
          vpartTag=vpart;
          pttag=ptprob;
        }
      }
    }    
  }

  return vpartTag;
}

//___________________________________________________________________________________________________________ 
AliVParticle* AliAnalysisTaskSEhfcjMCanalysis::TagJetParticleDeltaR(fastjet::PseudoJet jet,Double_t radiusSq,Int_t &tag){/// check for bquarks and cquarks within sqrt(radiusSq); first tag digit =0,1,2,3 -> jet is light quarks/gluons;  4 jet is charm, 5 jet is beauty; second digit=ncquarks; 3rd digits = nbquarks. Therefore, tag = tag selection + 10*nB + 100*nDfromB+1000*nelefromB+10000*nBDele +100000*nD*1000000*ncele ; 

  if(flastbquark==-1){
    SetBeautyAndCharmPartIndex();
  }

  Int_t nb=0,nD=0,nDfromB=0,nBele=0,ncele=0,nBDele=0;
  AliVParticle *vpartTag=0x0;
  AliVParticle *vpart=0x0;
  Double_t pttag=0,ptprob=0;

  vpartTag=TagJetParticleSpecieDeltaR(jet,radiusSq,nb,AliAnalysisTaskSEhfcjMCanalysis::kbhadron);

  vpart=TagJetParticleSpecieDeltaR(jet,radiusSq,nDfromB,AliAnalysisTaskSEhfcjMCanalysis::kDfromB);
  if(!vpartTag)vpartTag=vpart;
  vpart=TagJetParticleSpecieDeltaR(jet,radiusSq,nBele,AliAnalysisTaskSEhfcjMCanalysis::kelefromB);
  if(!vpartTag)vpartTag=vpart;
  vpart=TagJetParticleSpecieDeltaR(jet,radiusSq,nBDele,AliAnalysisTaskSEhfcjMCanalysis::kelefromDfromB);
  if(!vpartTag)vpartTag=vpart;
  vpart=TagJetParticleSpecieDeltaR(jet,radiusSq,nD,AliAnalysisTaskSEhfcjMCanalysis::kchadron);
  if(!vpartTag)vpartTag=vpart;
  vpart=TagJetParticleSpecieDeltaR(jet,radiusSq,ncele,AliAnalysisTaskSEhfcjMCanalysis::kelefromD);
  if(!vpartTag)vpartTag=vpart;


  if(nb>0)tag=5;
  else if(nD>0)tag=4;

  if(nb>9)nb=9;
  if(nD>9)nD=9;
  if(nDfromB>9)nDfromB=9;
  if(nBele>9)nBele=9;
  if(ncele>9)ncele=9;
  if(nBDele>9)nBDele=9;

  tag += 10*nb + 100*nDfromB+1000*nBele+10000*nBDele+100000*nD+1000000*ncele;

  if(vpartTag)return vpartTag;  
  tag=3; 
  return 0x0;// temporary --> do nothing for light quarks and gluons
}
//______________________________________________________ 
void AliAnalysisTaskSEhfcjMCanalysis::CalculateChargedJetProperties(){

  //  Printf("\n \n N jets: %d",fJets.size());
  for(unsigned i = 0; i < fJets.size(); i++)
  {

    unsigned long nghosts=0;
    std::vector<fastjet::PseudoJet> constituents = fCSA->constituents(fJets[i]);
    for(UInt_t ic=0;ic<constituents.size();ic++){
      if(constituents[ic].is_pure_ghost())nghosts++;
    }
    if(nghosts==constituents.size()){
      fArrayTagJetQuark->AddAt(-2,i);
      //	Printf("Rejecting pure ghost jet");
      continue;
    }

    Int_t tag=0,tagQ=0;
    AliVParticle *partQ=TagJetPartonDeltaR(fJets[i],fDeltaRSqforTagging,tagQ);
    fArrayTagJetQuark->AddAt(tagQ,i);
    TagJetParticleDeltaR(fJets[i],fDeltaRSqforTagging,tag);
    if(tag!=3)Printf("Tag: %d",tag);
    fArrayTagJetParticle->AddAt(tag,i);


    // MODIFY HERE :
    //
    if(fabs(fJets[i].eta()) > fDetectorAcceptance-fJetRcut){
      //	Printf("Rejecting out of acc jet");
      continue;
    }

    Double_t jetphi = fJets[i].phi();
    Double_t jeteta = fJets[i].eta();
    Double_t jetpt=fJets[i].perp();
    Double_t jetmom = sqrt(fJets[i].perp2()+pow(fJets[i].pz(),2));
    Double_t ptmax=0;
    Double_t ptav=0,ptx=0,pty=0,jetptcostit=0;
    UInt_t icostptMax=-1;
    // calculate max, average pt of costituents, z distribution
    TArrayD arrayz=TArrayD(constituents.size());
    TArrayD arraypt=TArrayD(constituents.size());
    // check number of costituents w/o ghosts

    //      Printf("\n");
    for(UInt_t ic=0;ic<constituents.size();ic++){
      if(constituents[ic].is_pure_ghost())continue;
      AliVParticle *partcost=(AliVParticle *)fMcEvent->GetTrack(constituents[ic].user_index());
      //	Printf("Jet cost, pdg: %d",partcost->PdgCode());
      Int_t labMoth=partcost->GetMother();
      if(labMoth>0){
        AliVParticle *partMoth=(AliVParticle*)fMCEvent->GetTrack(labMoth);
        Int_t pdg=partMoth->PdgCode();
        //    Printf("mother pdg: %d, pt %f, y, %f, ndaught %d",pdg,partMoth->Pt(),partMoth->Y(),partMoth->GetLastDaughter()-partMoth->GetFirstDaughter()+1);
      }

      Double_t pt=partcost->Pt();
      ptx+=partcost->Px();
      pty+=partcost->Py();
      Double_t ppart[3]={partcost->Px(),partcost->Py(),partcost->Pz()};
      Double_t pjet[3]={fJets[i].px(),fJets[i].py(),fJets[i].pz()};
      arrayz.AddAt(GetZparallel(ppart,pjet),ic);
      arraypt.AddAt(pt,ic);
      ptav+=pt;
      if(pt>ptmax){
        ptmax=pt;
        icostptMax=ic;
      }
    }

    ptav/=(constituents.size()-nghosts);
    jetptcostit=TMath::Sqrt(ptx*ptx+pty*pty);
    Double_t area = fCSA->area(fJets[i]);

    Double_t pointSparseJetExtended[12]={jetpt,jetptcostit,
        area,jetmom,jetphi,jeteta,(Double_t)(tag%10),
        (Double_t)(tagQ/100),(Double_t)(tagQ/10),(Double_t)(constituents.size()-nghosts),ptav,ptmax};
    Double_t ptQ=-1;
    if(partQ)ptQ=partQ->Pt();
    Double_t pointSparseJet[7]={jetpt,area,jetphi,jeteta,(Double_t)(tag%10),(Double_t)(constituents.size()-nghosts),ptQ};


    if(!fLightOutput)fhSparseJetPropExtended->Fill(pointSparseJetExtended);
    fhSparseJetProp->Fill(pointSparseJet);


    Double_t pointSparseFF[11]={0,0,jetpt,jeteta,ptav,ptmax,area,(Double_t)(tag%10),(Double_t)(tagQ/100),(Double_t)(tagQ/10),(Double_t)(constituents.size()-nghosts)};
    for(UInt_t ic=0;ic<constituents.size();ic++){
      if(constituents[ic].is_pure_ghost())continue;

      pointSparseFF[0]=arrayz.At(ic);
      pointSparseFF[1]=arraypt.At(ic);

      fhSparseFFstudy->Fill(pointSparseFF);
    }



  }

}

//______________________________________________________ 
Double_t AliAnalysisTaskSEhfcjMCanalysis::GetZparallel(Double_t *pPart,Double_t *pJet){
  Double_t den=pJet[0]*pJet[0]+pJet[1]*pJet[1]+pJet[2]*pJet[2];
  Double_t num=pPart[0]*pJet[0]+pPart[1]*pJet[1]+pPart[2]*pJet[2];
  return num/den; 
}
//_____________________________________________________
AliVParticle* AliAnalysisTaskSEhfcjMCanalysis::GetMotherQuark(AliVParticle *part){
  Int_t labMoth=part->GetMother();  
  while(labMoth>0){
    AliVParticle *partMoth=(AliVParticle*)fMCEvent->GetTrack(labMoth);
    UInt_t pdg=TMath::Abs(partMoth->PdgCode());
    if(pdg>0&&pdg<=6){
      return partMoth;
    }
    labMoth=partMoth->GetMother();  
  }
  return 0x0;
}

//______________________________________________________ 
void AliAnalysisTaskSEhfcjMCanalysis::FillParticleTaggedJetProperties(){

  FillPromptDTaggedJetProperties();
  FillBTaggedJetProperties();
  FillFeedDownDTaggedJetProperties();
  FillHFeTaggedBeautyJetProperties();

}

//__________________________________________________________________ 
void AliAnalysisTaskSEhfcjMCanalysis::FillPromptDTaggedJetProperties(){

  for(unsigned i = 0; i < fJets.size(); i++)
  {
    if(fArrayTagJetQuark->At(i)==-2)continue;// remove full ghost jets

    if(fabs(fJets[i].eta()) > fDetectorAcceptance-fJetRcut){
      continue;
    }

    Int_t ntag=0;
    Int_t ncq,nD;
    TagJetParticleSpecieDeltaR(fJets[i],fDeltaRSqforTagging,ncq,AliAnalysisTaskSEhfcjMCanalysis::kcquark);
    AliVParticle* partTagMaxPt=TagJetParticleSpecieDeltaR(fJets[i],fDeltaRSqforTagging,nD,AliAnalysisTaskSEhfcjMCanalysis::kchadron);
    if(ncq>0 || nD>0){
      Double_t jetpt=fJets[i].perp();
      Double_t pointsparse[9]={jetpt,fJets[i].eta(),-1,-1,-1,-1,-2,(Double_t)ncq,(Double_t)nD};
      Double_t pjet[3]={fJets[i].px(),fJets[i].py(),fJets[i].pz()};
      if(nD>0){
        Double_t pttag=partTagMaxPt->Pt();
        fastjet::PseudoJet partPseudoJet(partTagMaxPt->Px(),partTagMaxPt->Py(),partTagMaxPt->Pz(),partTagMaxPt->P());
        Double_t dR=fJets[i].plain_distance(partPseudoJet);
        Double_t ppart[3]={partTagMaxPt->Px(),partTagMaxPt->Py(),partTagMaxPt->Pz()};
        Double_t z=GetZparallel(ppart,pjet);
        AliVParticle *pquark=GetMotherQuark(partTagMaxPt);
        Double_t momFracDoverc=-2;
        if(pquark){
          UInt_t pdg=TMath::Abs(pquark->PdgCode());
          if(pdg==4)momFracDoverc=partTagMaxPt->P()/pquark->P();
        }
        pointsparse[2]=pttag;
        pointsparse[3]=z;
        pointsparse[4]=dR;
        pointsparse[5]=momFracDoverc;
        pointsparse[6]=1;


        fhSparseDJets->Fill(pointsparse);
        for(Int_t j=0;j<nD;j++){
          AliVParticle *part=fMCEvent->GetTrack(fTempTagParticle->At(j));
          if(part==partTagMaxPt)continue;
          pttag=part->Pt();
          partPseudoJet.reset_momentum(part->Px(),part->Py(),part->Pz(),part->P());
          dR=fJets[i].plain_distance(partPseudoJet);
          ppart[0]=part->Px();
          ppart[1]=part->Py();
          ppart[2]=part->Pz();
          z=GetZparallel(ppart,pjet);
          pquark=GetMotherQuark(part);
          momFracDoverc=-2;
          if(pquark){
            UInt_t pdg=TMath::Abs(pquark->PdgCode());
            if(pdg==4)momFracDoverc=part->P()/pquark->P();
          }
          pointsparse[2]=pttag;
          pointsparse[3]=z;
          pointsparse[4]=dR;
          pointsparse[5]=momFracDoverc;
          pointsparse[6]=j+2;
          fhSparseDJets->Fill(pointsparse);
        }
      }
      else{
        fhSparseDJets->Fill(pointsparse);
      }
    }
  }

}

//__________________________________________________________________ 
void AliAnalysisTaskSEhfcjMCanalysis::FillBTaggedJetProperties(){

  for(unsigned i = 0; i < fJets.size(); i++)
  {
    if(fArrayTagJetQuark->At(i)==-2)continue;// remove full ghost jets
    if(fabs(fJets[i].eta()) > fDetectorAcceptance-fJetRcut){
      continue;
    }
    Int_t ntag=0;
    Int_t nbq,nB;
    TagJetParticleSpecieDeltaR(fJets[i],fDeltaRSqforTagging,nbq,AliAnalysisTaskSEhfcjMCanalysis::kbquark);
    AliVParticle* partTagMaxPt=TagJetParticleSpecieDeltaR(fJets[i],fDeltaRSqforTagging,nB,AliAnalysisTaskSEhfcjMCanalysis::kbhadron);
    if(nbq>0 || nB>0){
      Double_t jetpt=fJets[i].perp();
      Double_t pointsparse[9]={jetpt,fJets[i].eta(),-1,-1,-1,-1,-2,(Double_t)nbq,(Double_t)nB};
      Double_t pjet[3]={fJets[i].px(),fJets[i].py(),fJets[i].pz()};
      if(nB>0){
        Double_t pttag=partTagMaxPt->Pt();
        fastjet::PseudoJet partPseudoJet(partTagMaxPt->Px(),partTagMaxPt->Py(),partTagMaxPt->Pz(),partTagMaxPt->P());
        Double_t dR=fJets[i].plain_distance(partPseudoJet);
        Double_t ppart[3]={partTagMaxPt->Px(),partTagMaxPt->Py(),partTagMaxPt->Pz()};
        Double_t z=GetZparallel(ppart,pjet);
        AliVParticle *pquark=GetMotherQuark(partTagMaxPt);
        Double_t momFracDoverc=-2;
        if(pquark){
          UInt_t pdg=TMath::Abs(pquark->PdgCode());
          if(pdg==5)momFracDoverc=partTagMaxPt->P()/pquark->P();
        }
        pointsparse[2]=pttag;
        pointsparse[3]=z;
        pointsparse[4]=dR;
        pointsparse[5]=momFracDoverc;
        pointsparse[6]=1;


        fhSparseBJets->Fill(pointsparse);
        for(Int_t j=0;j<nB;j++){
          AliVParticle *part=fMCEvent->GetTrack(fTempTagParticle->At(j));
          if(part==partTagMaxPt)continue;
          pttag=part->Pt();
          partPseudoJet.reset_momentum(part->Px(),part->Py(),part->Pz(),part->P());
          dR=fJets[i].plain_distance(partPseudoJet);
          ppart[0]=part->Px();
          ppart[1]=part->Py();
          ppart[2]=part->Pz();
          z=GetZparallel(ppart,pjet);
          pquark=GetMotherQuark(part);
          momFracDoverc=-2;
          if(pquark){
            UInt_t pdg=TMath::Abs(pquark->PdgCode());
            if(pdg==5)momFracDoverc=part->P()/pquark->P();
          }
          pointsparse[2]=pttag;
          pointsparse[3]=z;
          pointsparse[4]=dR;
          pointsparse[5]=momFracDoverc;
          pointsparse[6]=j+2;
          fhSparseBJets->Fill(pointsparse);
        }
      }
      else{
        fhSparseBJets->Fill(pointsparse);
      }
    }
  }

}


//______________________________________________________ 
void AliAnalysisTaskSEhfcjMCanalysis::FillFeedDownDTaggedJetProperties(){

  for(unsigned i = 0; i < fJets.size(); i++)
  {
    if(fArrayTagJetQuark->At(i)==-2)continue;// remove full ghost jets
    if(fabs(fJets[i].eta()) > fDetectorAcceptance-fJetRcut){
      continue;
    }
    Int_t ntag=0;
    Int_t nbq,nD;
    TagJetParticleSpecieDeltaR(fJets[i],fDeltaRSqforTagging,nbq,AliAnalysisTaskSEhfcjMCanalysis::kbquark);
    AliVParticle* partTagMaxPt=TagJetParticleSpecieDeltaR(fJets[i],fDeltaRSqforTagging,nD,AliAnalysisTaskSEhfcjMCanalysis::kDfromB);
    if(nbq>0 || nD>0){
      Double_t jetpt=fJets[i].perp();
      Double_t pointsparse[9]={jetpt,fJets[i].eta(),-1,-1,-1,-1,-2,(Double_t)nbq,(Double_t)nD};
      Double_t pjet[3]={fJets[i].px(),fJets[i].py(),fJets[i].pz()};
      if(nD>0){
        Double_t pttag=partTagMaxPt->Pt();
        fastjet::PseudoJet partPseudoJet(partTagMaxPt->Px(),partTagMaxPt->Py(),partTagMaxPt->Pz(),partTagMaxPt->P());
        Double_t dR=fJets[i].plain_distance(partPseudoJet);
        Double_t ppart[3]={partTagMaxPt->Px(),partTagMaxPt->Py(),partTagMaxPt->Pz()};
        Double_t z=GetZparallel(ppart,pjet);
        AliVParticle *pquark=GetMotherQuark(partTagMaxPt);
        Double_t momFracDoverb=-2;
        if(pquark){
          UInt_t pdg=TMath::Abs(pquark->PdgCode());
          if(pdg==5)momFracDoverb=partTagMaxPt->P()/pquark->P();
        }
        pointsparse[2]=pttag;
        pointsparse[3]=z;
        pointsparse[4]=dR;
        pointsparse[5]=momFracDoverb;
        pointsparse[6]=1;


        fhSparseFeedDownDJets->Fill(pointsparse);
        for(Int_t j=0;j<nD;j++){
          AliVParticle *part=fMCEvent->GetTrack(fTempTagParticle->At(j));
          if(part==partTagMaxPt)continue;
          pttag=part->Pt();
          partPseudoJet.reset_momentum(part->Px(),part->Py(),part->Pz(),part->P());
          dR=fJets[i].plain_distance(partPseudoJet);
          ppart[0]=part->Px();
          ppart[1]=part->Py();
          ppart[2]=part->Pz();
          z=GetZparallel(ppart,pjet);
          pquark=GetMotherQuark(part);
          momFracDoverb=-2;
          if(pquark){
            UInt_t pdg=TMath::Abs(pquark->PdgCode());
            if(pdg==5)momFracDoverb=part->P()/pquark->P();
          }
          pointsparse[2]=pttag;
          pointsparse[3]=z;
          pointsparse[4]=dR;
          pointsparse[5]=momFracDoverb;
          pointsparse[6]=j+2;
          fhSparseFeedDownDJets->Fill(pointsparse);
        }
      }
      else{
        fhSparseFeedDownDJets->Fill(pointsparse);
      }
    }
  }

}

//______________________________________________________ 
void AliAnalysisTaskSEhfcjMCanalysis::FillHFJetCorrelations(){
  FillbquarkCorrelations();
  FillcquarkCorrelations();
  FillPromptDmesonJetCorrelations();
  FillBmesonJetCorrelations();
  FillFeedDownDmesonJetCorrelations();
  FillHFeJetCorrelations();
}

//______________________________________________________ 
void AliAnalysisTaskSEhfcjMCanalysis::FillPromptDmesonJetCorrelations(){

  if(flastbquark==-1)SetBeautyAndCharmPartIndex();

  for(Int_t jPart = 0; jPart < flastchadron; jPart++){

    AliVParticle* mcPartHF = (AliVParticle*)fMcEvent->GetTrack(fArraychadron->At(jPart));
    if (!mcPartHF)continue;
    Int_t pdgHF = TMath::Abs(mcPartHF->PdgCode());
    Double_t pdg=4.;
    if(pdgHF==421)pdg=1;
    if(pdgHF==411)pdg=2;
    if(pdgHF==413)pdg=3;

    Double_t ptTrig=mcPartHF->Pt();
    Double_t phiTrig=mcPartHF->Phi();
    Double_t etaTrig=mcPartHF->Eta();
    AliVParticle *pquark=GetMotherQuark(mcPartHF);	    
    Double_t ptQparticle=-1;
    if(pquark){	    
      ptQparticle=pquark->Pt();	      
    }

    for(unsigned ijets=0;ijets<fJets.size();ijets++){
      if(fArrayTagJetQuark->At(ijets)==-2)continue;// remove full ghost jets
      Int_t tag=fArrayTagJetQuark->At(ijets);
      Double_t ptQjet=-1;      
      if(tag==-1){	 
        AliVParticle *pqjet=TagJetPartonDeltaR(fJets[ijets],fDeltaRSqforTagging,tag);
        fArrayTagJetQuark->AddAt(tag,ijets);
        if(pqjet)ptQjet=pqjet->Pt();	// equivalent to ask tag=4 or 5
      }
      else if(tag%10==4){
        AliVParticle *pqjet=TagJetPartonDeltaR(fJets[ijets],fDeltaRSqforTagging,tag); //needed
        if(pqjet)ptQjet=pqjet->Pt();	// the if should be useless
      }
      Double_t jetpt=fJets[ijets].perp();	
      Double_t jetphi=fJets[ijets].phi();	
      Double_t jeteta=fJets[ijets].eta();	
      Double_t deltaPhi=jetphi-phiTrig;
      if(deltaPhi<-TMath::PiOver2())deltaPhi+=TMath::TwoPi();
      if(deltaPhi>TMath::Pi()+TMath::PiOver2())deltaPhi-=TMath::TwoPi();
      Double_t iscost=-1;
      std::vector<fastjet::PseudoJet> constituents = fCSA->constituents(fJets[ijets]);
      for(unsigned ic=0;ic<constituents.size();ic++){
        // check if particle is among costituents
        if(constituents[ic].user_index()==fArraychadron->At(jPart))iscost=1;
      }
      Double_t point[10]={ptTrig,etaTrig,pdg,ptQparticle,jetpt,jeteta,deltaPhi,jeteta-etaTrig,iscost,ptQjet};
      fhSparseDJetCorrel->Fill(point);
    }
  }  
}


//______________________________________________________ 
void AliAnalysisTaskSEhfcjMCanalysis::FillFeedDownDmesonJetCorrelations(){

  if(flastbquark==-1)SetBeautyAndCharmPartIndex();

  for(Int_t jPart = 0; jPart < flastchadronfromB; jPart++){

    AliVParticle* mcPartHF = (AliVParticle*)fMcEvent->GetTrack(fArraychadronfromB->At(jPart));
    if (!mcPartHF)continue;
    Int_t pdgHF = TMath::Abs(mcPartHF->PdgCode());
    Double_t pdg=4.;
    if(pdgHF==421)pdg=1;
    if(pdgHF==411)pdg=2;
    if(pdgHF==413)pdg=3;

    Double_t ptTrig=mcPartHF->Pt();
    Double_t phiTrig=mcPartHF->Phi();
    Double_t etaTrig=mcPartHF->Eta();
    AliVParticle *pquark=GetMotherQuark(mcPartHF);	    
    Double_t ptQparticle=-1;
    if(pquark){	    
      ptQparticle=pquark->Pt();	      
    }

    for(unsigned ijets=0;ijets<fJets.size();ijets++){
      if(fArrayTagJetQuark->At(ijets)==-2)continue;// remove full ghost jets
      Int_t tag=fArrayTagJetQuark->At(ijets);
      Double_t ptQjet=-1;      
      if(tag==-1){	 
        AliVParticle *pqjet=TagJetPartonDeltaR(fJets[ijets],fDeltaRSqforTagging,tag);
        fArrayTagJetQuark->AddAt(tag,ijets);
        if(pqjet)ptQjet=pqjet->Pt();	// equivalent to ask tag=4 or 5
      }
      else if (tag%10==5){
        AliVParticle *pqjet=TagJetPartonDeltaR(fJets[ijets],fDeltaRSqforTagging,tag); //needed
        if(pqjet)ptQjet=pqjet->Pt();	// the if should be useless
      }
      Double_t jetpt=fJets[ijets].perp();	
      Double_t jetphi=fJets[ijets].phi();	
      Double_t jeteta=fJets[ijets].eta();	
      Double_t deltaPhi=jetphi-phiTrig;
      if(deltaPhi<-TMath::PiOver2())deltaPhi+=TMath::TwoPi();
      if(deltaPhi>TMath::Pi()+TMath::PiOver2())deltaPhi-=TMath::TwoPi();
      Double_t iscost=-1;
      std::vector<fastjet::PseudoJet> constituents = fCSA->constituents(fJets[ijets]);
      for(unsigned ic=0;ic<constituents.size();ic++){
        // check if particle is among costituents
        if(constituents[ic].user_index()==fArraychadron->At(jPart))iscost=1;
      }
      Double_t point[10]={ptTrig,etaTrig,pdg,ptQparticle,jetpt,jeteta,deltaPhi,jeteta-etaTrig,iscost,ptQjet};
      fhSparseFeedDownDJetCorrel->Fill(point);
    }
  }  
}


//______________________________________________________ 
void AliAnalysisTaskSEhfcjMCanalysis::FillBmesonJetCorrelations(){

  if(flastbquark==-1)SetBeautyAndCharmPartIndex();

  for(Int_t jPart = 0; jPart < flastbhadron; jPart++){

    AliVParticle* mcPartHF = (AliVParticle*)fMcEvent->GetTrack(fArraybhadron->At(jPart));
    if (!mcPartHF)continue;
    Int_t pdgHF = TMath::Abs(mcPartHF->PdgCode());
    Double_t pdg=4.;
    if(pdgHF==511)pdg=1;// B0
    if(pdgHF==521)pdg=2;// B+
    if(pdgHF==513 || pdgHF==523)pdg=3;// B*0 or B*+

    Double_t ptTrig=mcPartHF->Pt();
    Double_t phiTrig=mcPartHF->Phi();
    Double_t etaTrig=mcPartHF->Eta();
    AliVParticle *pquark=GetMotherQuark(mcPartHF);	    
    Double_t ptQparticle=-1;
    if(pquark){	    
      ptQparticle=pquark->Pt();	      
    }

    for(unsigned ijets=0;ijets<fJets.size();ijets++){
      if(fArrayTagJetQuark->At(ijets)==-2)continue;// remove full ghost jets
      Int_t tag=fArrayTagJetQuark->At(ijets);
      Double_t ptQjet=-1;      
      if(tag==-1){	 
        AliVParticle *pqjet=TagJetPartonDeltaR(fJets[ijets],fDeltaRSqforTagging,tag);
        fArrayTagJetQuark->AddAt(tag,ijets);
        if(pqjet)ptQjet=pqjet->Pt();	// equivalent to ask tag=4 or 5
      }
      else if (tag%10==5){
        AliVParticle *pqjet=TagJetPartonDeltaR(fJets[ijets],fDeltaRSqforTagging,tag); //needed
        if(pqjet)ptQjet=pqjet->Pt();	// the if should be useless
      }
      Double_t jetpt=fJets[ijets].perp();	
      Double_t jetphi=fJets[ijets].phi();	
      Double_t jeteta=fJets[ijets].eta();	
      Double_t deltaPhi=jetphi-phiTrig;
      if(deltaPhi<-TMath::PiOver2())deltaPhi+=TMath::TwoPi();
      if(deltaPhi>TMath::Pi()+TMath::PiOver2())deltaPhi-=TMath::TwoPi();
      Double_t iscost=-1;
      std::vector<fastjet::PseudoJet> constituents = fCSA->constituents(fJets[ijets]);
      for(unsigned ic=0;ic<constituents.size();ic++){
        // check if particle is among costituents
        if(constituents[ic].user_index()==fArraybhadron->At(jPart))iscost=1;
      }
      Double_t point[10]={ptTrig,etaTrig,pdg,ptQparticle,jetpt,jeteta,deltaPhi,jeteta-etaTrig,iscost,ptQjet};
      fhSparseBJetCorrel->Fill(point);
    }
  }  
}




//______________________________________________________ 
void AliAnalysisTaskSEhfcjMCanalysis::FillHFeJetCorrelations(){

  if(flastbquark==-1)SetBeautyAndCharmPartIndex();

  // start with B electrons
  Double_t pdg=1.;
  for(Int_t jPart = 0; jPart < flastbele; jPart++){

    AliVParticle* mcPartHF = (AliVParticle*)fMcEvent->GetTrack(fArraybele->At(jPart));
    if (!mcPartHF)continue;
    Int_t pdgHF = TMath::Abs(mcPartHF->PdgCode());

    Double_t ptTrig=mcPartHF->Pt();
    Double_t phiTrig=mcPartHF->Phi();
    Double_t etaTrig=mcPartHF->Eta();
    AliVParticle *pquark=GetMotherQuark(mcPartHF);	    
    Double_t ptQparticle=-1;
    if(pquark){	    
      ptQparticle=pquark->Pt();	      
    }

    for(unsigned ijets=0;ijets<fJets.size();ijets++){
      if(fArrayTagJetQuark->At(ijets)==-2)continue;// remove full ghost jets
      Int_t tag=fArrayTagJetQuark->At(ijets);
      Double_t ptQjet=-1;      
      if(tag==-1){	 
        AliVParticle *pqjet=TagJetPartonDeltaR(fJets[ijets],fDeltaRSqforTagging,tag);
        fArrayTagJetQuark->AddAt(tag,ijets);
        if(pqjet)ptQjet=pqjet->Pt();	// equivalent to ask tag=4 or 5
      }
      else if (tag%10==5){
        AliVParticle *pqjet=TagJetPartonDeltaR(fJets[ijets],fDeltaRSqforTagging,tag); //needed
        if(pqjet)ptQjet=pqjet->Pt();	// the if should be useless
      }
      Double_t jetpt=fJets[ijets].perp();	
      Double_t jetphi=fJets[ijets].phi();	
      Double_t jeteta=fJets[ijets].eta();	
      Double_t deltaPhi=jetphi-phiTrig;
      if(deltaPhi<-TMath::PiOver2())deltaPhi+=TMath::TwoPi();
      if(deltaPhi>TMath::Pi()+TMath::PiOver2())deltaPhi-=TMath::TwoPi();
      Double_t iscost=-1;
      std::vector<fastjet::PseudoJet> constituents = fCSA->constituents(fJets[ijets]);
      for(unsigned ic=0;ic<constituents.size();ic++){
        // check if particle is among costituents
        if(constituents[ic].user_index()==fArraybhadron->At(jPart))iscost=1;
      }
      Double_t point[10]={ptTrig,etaTrig,pdg,ptQparticle,jetpt,jeteta,deltaPhi,jeteta-etaTrig,iscost,ptQjet};
      fhSparseHFeJetCorrel->Fill(point);
    }
  }


  // now Feed-Down D meson electrons
  pdg=2.;
  for(Int_t jPart = 0; jPart < flastcelefromB; jPart++){

    AliVParticle* mcPartHF = (AliVParticle*)fMcEvent->GetTrack(fArraycelefromB->At(jPart));
    if (!mcPartHF)continue;
    Int_t pdgHF = TMath::Abs(mcPartHF->PdgCode());

    Double_t ptTrig=mcPartHF->Pt();
    Double_t phiTrig=mcPartHF->Phi();
    Double_t etaTrig=mcPartHF->Eta();
    AliVParticle *pquark=GetMotherQuark(mcPartHF);	    
    Double_t ptQparticle=-1;
    if(pquark){	    
      ptQparticle=pquark->Pt();	      
    }

    for(unsigned ijets=0;ijets<fJets.size();ijets++){
      Int_t tag=fArrayTagJetQuark->At(ijets);
      Double_t ptQjet=-1;      
      if(tag==-1){	 
        AliVParticle *pqjet=TagJetPartonDeltaR(fJets[ijets],fDeltaRSqforTagging,tag);
        fArrayTagJetQuark->AddAt(tag,ijets);
        if(pqjet)ptQjet=pqjet->Pt();	// equivalent to ask tag=4 or 5
      }
      else if (tag%10==5){
        AliVParticle *pqjet=TagJetPartonDeltaR(fJets[ijets],fDeltaRSqforTagging,tag); //needed
        if(pqjet)ptQjet=pqjet->Pt();	// the if should be useless
      }
      Double_t jetpt=fJets[ijets].perp();	
      Double_t jetphi=fJets[ijets].phi();	
      Double_t jeteta=fJets[ijets].eta();	
      Double_t deltaPhi=jetphi-phiTrig;
      if(deltaPhi<-TMath::PiOver2())deltaPhi+=TMath::TwoPi();
      if(deltaPhi>TMath::Pi()+TMath::PiOver2())deltaPhi-=TMath::TwoPi();
      Double_t iscost=-1;
      std::vector<fastjet::PseudoJet> constituents = fCSA->constituents(fJets[ijets]);
      for(unsigned ic=0;ic<constituents.size();ic++){
        // check if particle is among costituents
        if(constituents[ic].user_index()==fArraybhadron->At(jPart))iscost=1;
      }
      Double_t point[10]={ptTrig,etaTrig,pdg,ptQparticle,jetpt,jeteta,deltaPhi,jeteta-etaTrig,iscost,ptQjet};
      fhSparseHFeJetCorrel->Fill(point);
    }
  }


  // now prompt D meson electrons
  pdg=3.;
  for(Int_t jPart = 0; jPart < flastcele; jPart++){

    AliVParticle* mcPartHF = (AliVParticle*)fMcEvent->GetTrack(fArraycele->At(jPart));
    if (!mcPartHF)continue;
    Int_t pdgHF = TMath::Abs(mcPartHF->PdgCode());

    Double_t ptTrig=mcPartHF->Pt();
    Double_t phiTrig=mcPartHF->Phi();
    Double_t etaTrig=mcPartHF->Eta();
    AliVParticle *pquark=GetMotherQuark(mcPartHF);	    
    Double_t ptQparticle=-1;
    if(pquark){	    
      ptQparticle=pquark->Pt();	      
    }

    for(unsigned ijets=0;ijets<fJets.size();ijets++){
      Int_t tag=fArrayTagJetQuark->At(ijets);
      Double_t ptQjet=-1;      
      if(tag==-1){	 
        AliVParticle *pqjet=TagJetPartonDeltaR(fJets[ijets],fDeltaRSqforTagging,tag);
        fArrayTagJetQuark->AddAt(tag,ijets);
        if(pqjet)ptQjet=pqjet->Pt();	// equivalent to ask tag=4 or 5
      }
      else if (tag%10==4){
        AliVParticle *pqjet=TagJetPartonDeltaR(fJets[ijets],fDeltaRSqforTagging,tag); //needed
        if(pqjet)ptQjet=pqjet->Pt();	// the if should be useless
      }
      Double_t jetpt=fJets[ijets].perp();	
      Double_t jetphi=fJets[ijets].phi();	
      Double_t jeteta=fJets[ijets].eta();	
      Double_t deltaPhi=jetphi-phiTrig;
      if(deltaPhi<-TMath::PiOver2())deltaPhi+=TMath::TwoPi();
      if(deltaPhi>TMath::Pi()+TMath::PiOver2())deltaPhi-=TMath::TwoPi();
      Double_t iscost=-1;
      std::vector<fastjet::PseudoJet> constituents = fCSA->constituents(fJets[ijets]);
      for(unsigned ic=0;ic<constituents.size();ic++){
        // check if particle is among costituents
        if(constituents[ic].user_index()==fArraybhadron->At(jPart))iscost=1;
      }
      Double_t point[10]={ptTrig,etaTrig,pdg,ptQparticle,jetpt,jeteta,deltaPhi,jeteta-etaTrig,iscost,ptQjet};
      fhSparseHFeJetCorrel->Fill(point);
    }
  }


  // now conversion electrons
  pdg=4.;
  for(Int_t jPart = 0; jPart < flastGammaele; jPart++){

    AliVParticle* mcPartHF = (AliVParticle*)fMcEvent->GetTrack(fArrayGammaele->At(jPart));
    if (!mcPartHF)continue;
    Int_t pdgHF = TMath::Abs(mcPartHF->PdgCode());

    Double_t ptTrig=mcPartHF->Pt();
    Double_t phiTrig=mcPartHF->Phi();
    Double_t etaTrig=mcPartHF->Eta();
    AliVParticle *pquark=GetMotherQuark(mcPartHF);	    
    Double_t ptQparticle=-1;
    if(pquark){	    
      ptQparticle=pquark->Pt();	      
    }

    for(unsigned ijets=0;ijets<fJets.size();ijets++){
      Int_t tag=fArrayTagJetQuark->At(ijets);
      Double_t ptQjet=-1;      
      if(tag==-1){	 
        AliVParticle *pqjet=TagJetPartonDeltaR(fJets[ijets],fDeltaRSqforTagging,tag);
        fArrayTagJetQuark->AddAt(tag,ijets);
        if(pqjet)ptQjet=pqjet->Pt();	// equivalent to ask tag=4 or 5
      }
      else if (tag%10==4||tag%10==5){
        AliVParticle *pqjet=TagJetPartonDeltaR(fJets[ijets],fDeltaRSqforTagging,tag); //needed
        if(pqjet)ptQjet=pqjet->Pt();	// the if should be useless
      }
      Double_t jetpt=fJets[ijets].perp();	
      Double_t jetphi=fJets[ijets].phi();	
      Double_t jeteta=fJets[ijets].eta();	
      Double_t deltaPhi=jetphi-phiTrig;
      if(deltaPhi<-TMath::PiOver2())deltaPhi+=TMath::TwoPi();
      if(deltaPhi>TMath::Pi()+TMath::PiOver2())deltaPhi-=TMath::TwoPi();
      Double_t iscost=-1;
      std::vector<fastjet::PseudoJet> constituents = fCSA->constituents(fJets[ijets]);
      for(unsigned ic=0;ic<constituents.size();ic++){
        // check if particle is among costituents
        if(constituents[ic].user_index()==fArraybhadron->At(jPart))iscost=1;
      }
      Double_t point[10]={ptTrig,etaTrig,pdg,ptQparticle,jetpt,jeteta,deltaPhi,jeteta-etaTrig,iscost,ptQjet};
      fhSparseHFeJetCorrel->Fill(point);
    }
  }

  // now Dalitz electrons
  pdg=5.;
  for(Int_t jPart = 0; jPart < flastDalitzele; jPart++){

    AliVParticle* mcPartHF = (AliVParticle*)fMcEvent->GetTrack(fArrayDalitzele->At(jPart));
    if (!mcPartHF)continue;
    Int_t pdgHF = TMath::Abs(mcPartHF->PdgCode());

    Double_t ptTrig=mcPartHF->Pt();
    Double_t phiTrig=mcPartHF->Phi();
    Double_t etaTrig=mcPartHF->Eta();
    AliVParticle *pquark=GetMotherQuark(mcPartHF);	    
    Double_t ptQparticle=-1;
    if(pquark){	    
      ptQparticle=pquark->Pt();	      
    }

    for(unsigned ijets=0;ijets<fJets.size();ijets++){
      Int_t tag=fArrayTagJetQuark->At(ijets);
      Double_t ptQjet=-1;      
      if(tag==-1){	 
        AliVParticle *pqjet=TagJetPartonDeltaR(fJets[ijets],fDeltaRSqforTagging,tag);
        fArrayTagJetQuark->AddAt(tag,ijets);
        if(pqjet)ptQjet=pqjet->Pt();	// equivalent to ask tag=4 or 5
      }
      else if (tag%10==4||tag%10==5){
        AliVParticle *pqjet=TagJetPartonDeltaR(fJets[ijets],fDeltaRSqforTagging,tag); //needed
        if(pqjet)ptQjet=pqjet->Pt();	// the if should be useless
      }
      Double_t jetpt=fJets[ijets].perp();	
      Double_t jetphi=fJets[ijets].phi();	
      Double_t jeteta=fJets[ijets].eta();	
      Double_t deltaPhi=jetphi-phiTrig;
      if(deltaPhi<-TMath::PiOver2())deltaPhi+=TMath::TwoPi();
      if(deltaPhi>TMath::Pi()+TMath::PiOver2())deltaPhi-=TMath::TwoPi();
      Double_t iscost=-1;
      std::vector<fastjet::PseudoJet> constituents = fCSA->constituents(fJets[ijets]);
      for(unsigned ic=0;ic<constituents.size();ic++){
        // check if particle is among costituents
        if(constituents[ic].user_index()==fArraybhadron->At(jPart))iscost=1;
      }
      Double_t point[10]={ptTrig,etaTrig,pdg,ptQparticle,jetpt,jeteta,deltaPhi,jeteta-etaTrig,iscost,ptQjet};
      fhSparseHFeJetCorrel->Fill(point);
    }
  }


  // now Unknown electrons
  pdg=6.;
  for(Int_t jPart = 0; jPart < flastUnknownele; jPart++){

    AliVParticle* mcPartHF = (AliVParticle*)fMcEvent->GetTrack(fArrayUnknownele->At(jPart));
    if (!mcPartHF)continue;
    Int_t pdgHF = TMath::Abs(mcPartHF->PdgCode());

    Double_t ptTrig=mcPartHF->Pt();
    Double_t phiTrig=mcPartHF->Phi();
    Double_t etaTrig=mcPartHF->Eta();
    AliVParticle *pquark=GetMotherQuark(mcPartHF);	    
    Double_t ptQparticle=-1;
    if(pquark){	    
      ptQparticle=pquark->Pt();	      
    }

    for(unsigned ijets=0;ijets<fJets.size();ijets++){
      Int_t tag=fArrayTagJetQuark->At(ijets);
      Double_t ptQjet=-1;      
      if(tag==-1){	 
        AliVParticle *pqjet=TagJetPartonDeltaR(fJets[ijets],fDeltaRSqforTagging,tag);
        fArrayTagJetQuark->AddAt(tag,ijets);
        if(pqjet)ptQjet=pqjet->Pt();	// equivalent to ask tag=4 or 5
      }
      else if(tag%10==4||tag%10==5){
        AliVParticle *pqjet=TagJetPartonDeltaR(fJets[ijets],fDeltaRSqforTagging,tag); //needed
        if(pqjet)ptQjet=pqjet->Pt();	// the if should be useless
      }
      Double_t jetpt=fJets[ijets].perp();	
      Double_t jetphi=fJets[ijets].phi();	
      Double_t jeteta=fJets[ijets].eta();	
      Double_t deltaPhi=jetphi-phiTrig;
      if(deltaPhi<-TMath::PiOver2())deltaPhi+=TMath::TwoPi();
      if(deltaPhi>TMath::Pi()+TMath::PiOver2())deltaPhi-=TMath::TwoPi();
      Double_t iscost=-1;
      std::vector<fastjet::PseudoJet> constituents = fCSA->constituents(fJets[ijets]);
      for(unsigned ic=0;ic<constituents.size();ic++){
        // check if particle is among costituents
        if(constituents[ic].user_index()==fArraybhadron->At(jPart))iscost=1;
      }
      Double_t point[10]={ptTrig,etaTrig,pdg,ptQparticle,jetpt,jeteta,deltaPhi,jeteta-etaTrig,iscost,ptQjet};
      fhSparseHFeJetCorrel->Fill(point);
    }
  }


}

//______________________________________________________ 
void AliAnalysisTaskSEhfcjMCanalysis::FillHFeTaggedBeautyJetProperties(){

  AliVParticle* partTagMaxPt;
  for(unsigned i = 0; i < fJets.size(); i++)
  {
    if(fArrayTagJetQuark->At(i)==-2)continue;// remove full ghost jets
    if(fabs(fJets[i].eta()) > fDetectorAcceptance-fJetRcut){
      continue;
    }
    Int_t ntag=0;
    Int_t nbq=0,ncq=0,neleb=0,nelecb=0,nelec=0,neleConv=0,neleDalitz=0,neleUnknown=0;
    // Just to count
    TagJetParticleSpecieDeltaR(fJets[i],fDeltaRSqforTagging,neleb,AliAnalysisTaskSEhfcjMCanalysis::kelefromB);
    TagJetParticleSpecieDeltaR(fJets[i],fDeltaRSqforTagging,nelecb,AliAnalysisTaskSEhfcjMCanalysis::kelefromDfromB);
    TagJetParticleSpecieDeltaR(fJets[i],fDeltaRSqforTagging,nelec,AliAnalysisTaskSEhfcjMCanalysis::kelefromD);
    TagJetParticleSpecieDeltaR(fJets[i],fDeltaRSqforTagging,neleConv,AliAnalysisTaskSEhfcjMCanalysis::keleconversion);
    TagJetParticleSpecieDeltaR(fJets[i],fDeltaRSqforTagging,neleDalitz,AliAnalysisTaskSEhfcjMCanalysis::keleDalitz);
    TagJetParticleSpecieDeltaR(fJets[i],fDeltaRSqforTagging,neleUnknown,AliAnalysisTaskSEhfcjMCanalysis::keleunknown);


    TagJetParticleSpecieDeltaR(fJets[i],fDeltaRSqforTagging,nbq,AliAnalysisTaskSEhfcjMCanalysis::kbquark);
    TagJetParticleSpecieDeltaR(fJets[i],fDeltaRSqforTagging,ncq,AliAnalysisTaskSEhfcjMCanalysis::kcquark);
    // Now count + need all particles that could potentially tag the jet
    partTagMaxPt=TagJetParticleSpecieDeltaR(fJets[i],fDeltaRSqforTagging,neleb,AliAnalysisTaskSEhfcjMCanalysis::kelefromB);
    Double_t jetpt=fJets[i].perp();

    Double_t pointsparse[16]={jetpt,fJets[i].eta(),-1,-1,-1,-1,-2,-1,(Double_t)nbq,(Double_t)ncq,(Double_t)neleb,(Double_t)nelecb,(Double_t)nelec,(Double_t)neleConv,(Double_t)neleDalitz,(Double_t)neleUnknown};
    Double_t pjet[3]={fJets[i].px(),fJets[i].py(),fJets[i].pz()};

    if(neleb>0){
      pointsparse[7]=1;// 1 is a ele b; 2 elecb, 3 elec, 4 eleConv, 5 ele Dalitz, 6 ele unknown
      Double_t pttag=partTagMaxPt->Pt();
      fastjet::PseudoJet partPseudoJet(partTagMaxPt->Px(),partTagMaxPt->Py(),partTagMaxPt->Pz(),partTagMaxPt->P());
      Double_t dR=fJets[i].plain_distance(partPseudoJet);
      Double_t ppart[3]={partTagMaxPt->Px(),partTagMaxPt->Py(),partTagMaxPt->Pz()};
      Double_t z=GetZparallel(ppart,pjet);
      AliVParticle *pquark=GetMotherQuark(partTagMaxPt);
      Double_t momFracEleoverQ=-2;
      if(pquark){
        UInt_t pdg=TMath::Abs(pquark->PdgCode());
        if(pdg==5)momFracEleoverQ=partTagMaxPt->P()/pquark->P();
      }
      pointsparse[2]=pttag;
      pointsparse[3]=z;
      pointsparse[4]=dR;
      pointsparse[5]=momFracEleoverQ;
      pointsparse[6]=1;


      fhSparseEleJets->Fill(pointsparse);
      for(Int_t j=0;j<neleb;j++){
        AliVParticle *part=fMCEvent->GetTrack(fTempTagParticle->At(j));
        if(part==partTagMaxPt)continue;
        pttag=part->Pt();
        partPseudoJet.reset_momentum(part->Px(),part->Py(),part->Pz(),part->P());
        dR=fJets[i].plain_distance(partPseudoJet);
        ppart[0]=part->Px();
        ppart[1]=part->Py();
        ppart[2]=part->Pz();
        z=GetZparallel(ppart,pjet);
        pquark=GetMotherQuark(part);
        momFracEleoverQ=-2;
        if(pquark){
          UInt_t pdg=TMath::Abs(pquark->PdgCode());
          if(pdg==5)momFracEleoverQ=part->P()/pquark->P();
        }
        pointsparse[2]=pttag;
        pointsparse[3]=z;
        pointsparse[4]=dR;
        pointsparse[5]=momFracEleoverQ;
        pointsparse[6]=j+2;
        fhSparseEleJets->Fill(pointsparse);
      }
    }

    partTagMaxPt=TagJetParticleSpecieDeltaR(fJets[i],fDeltaRSqforTagging,nelecb,AliAnalysisTaskSEhfcjMCanalysis::kelefromDfromB);
    if(nelecb>0){
      pointsparse[7]=2;// 1 is a ele b; 2 elecb, 3 elec, 4 eleConv, 5 ele Dalitz, 6 ele unknown
      Double_t pttag=partTagMaxPt->Pt();
      fastjet::PseudoJet partPseudoJet(partTagMaxPt->Px(),partTagMaxPt->Py(),partTagMaxPt->Pz(),partTagMaxPt->P());
      Double_t dR=fJets[i].plain_distance(partPseudoJet);
      Double_t ppart[3]={partTagMaxPt->Px(),partTagMaxPt->Py(),partTagMaxPt->Pz()};
      Double_t z=GetZparallel(ppart,pjet);
      AliVParticle *pquark=GetMotherQuark(partTagMaxPt);
      Double_t momFracEleoverQ=-2;
      if(pquark){
        UInt_t pdg=TMath::Abs(pquark->PdgCode());
        if(pdg==5)momFracEleoverQ=partTagMaxPt->P()/pquark->P();
      }
      pointsparse[2]=pttag;
      pointsparse[3]=z;
      pointsparse[4]=dR;
      pointsparse[5]=momFracEleoverQ;
      pointsparse[6]=1+neleb;


      fhSparseEleJets->Fill(pointsparse);
      for(Int_t j=0;j<nelecb;j++){
        AliVParticle *part=fMCEvent->GetTrack(fTempTagParticle->At(j));
        if(part==partTagMaxPt)continue;
        pttag=part->Pt();
        partPseudoJet.reset_momentum(part->Px(),part->Py(),part->Pz(),part->P());
        dR=fJets[i].plain_distance(partPseudoJet);
        ppart[0]=part->Px();
        ppart[1]=part->Py();
        ppart[2]=part->Pz();
        z=GetZparallel(ppart,pjet);
        pquark=GetMotherQuark(part);
        momFracEleoverQ=-2;
        if(pquark){
          UInt_t pdg=TMath::Abs(pquark->PdgCode());
          if(pdg==5)momFracEleoverQ=part->P()/pquark->P();
        }
        pointsparse[2]=pttag;
        pointsparse[3]=z;
        pointsparse[4]=dR;
        pointsparse[5]=momFracEleoverQ;
        pointsparse[6]=j+2+neleb;
        fhSparseEleJets->Fill(pointsparse);
      }
    }

    partTagMaxPt=TagJetParticleSpecieDeltaR(fJets[i],fDeltaRSqforTagging,nelec,AliAnalysisTaskSEhfcjMCanalysis::kelefromD);
    if(nelec>0){
      pointsparse[7]=3;// 1 is a ele b; 2 elecb, 3 elec, 4 eleConv, 5 ele Dalitz, 6 ele unknown
      Double_t pttag=partTagMaxPt->Pt();
      fastjet::PseudoJet partPseudoJet(partTagMaxPt->Px(),partTagMaxPt->Py(),partTagMaxPt->Pz(),partTagMaxPt->P());
      Double_t dR=fJets[i].plain_distance(partPseudoJet);
      Double_t ppart[3]={partTagMaxPt->Px(),partTagMaxPt->Py(),partTagMaxPt->Pz()};
      Double_t z=GetZparallel(ppart,pjet);
      AliVParticle *pquark=GetMotherQuark(partTagMaxPt);
      Double_t momFracEleoverQ=-2;
      if(pquark){
        UInt_t pdg=TMath::Abs(pquark->PdgCode());
        if(pdg==5)momFracEleoverQ=partTagMaxPt->P()/pquark->P();
      }
      pointsparse[2]=pttag;
      pointsparse[3]=z;
      pointsparse[4]=dR;
      pointsparse[5]=momFracEleoverQ;
      pointsparse[6]=1+neleb+nelecb;


      fhSparseEleJets->Fill(pointsparse);
      for(Int_t j=0;j<nelec;j++){
        AliVParticle *part=fMCEvent->GetTrack(fTempTagParticle->At(j));
        if(part==partTagMaxPt)continue;
        pttag=part->Pt();
        partPseudoJet.reset_momentum(part->Px(),part->Py(),part->Pz(),part->P());
        dR=fJets[i].plain_distance(partPseudoJet);
        ppart[0]=part->Px();
        ppart[1]=part->Py();
        ppart[2]=part->Pz();
        z=GetZparallel(ppart,pjet);
        pquark=GetMotherQuark(part);
        momFracEleoverQ=-2;
        if(pquark){
          UInt_t pdg=TMath::Abs(pquark->PdgCode());
          if(pdg==5)momFracEleoverQ=part->P()/pquark->P();
        }
        pointsparse[2]=pttag;
        pointsparse[3]=z;
        pointsparse[4]=dR;
        pointsparse[5]=momFracEleoverQ;
        pointsparse[6]=j+2+neleb+nelecb;
        fhSparseEleJets->Fill(pointsparse);
      }
    }


    partTagMaxPt=TagJetParticleSpecieDeltaR(fJets[i],fDeltaRSqforTagging,neleConv,AliAnalysisTaskSEhfcjMCanalysis::keleconversion);
    if(neleConv>0){
      pointsparse[7]=4;// 1 is a ele b; 2 elecb, 3 elec, 4 eleConv, 5 ele Dalitz, 6 ele unknown
      Double_t pttag=partTagMaxPt->Pt();
      fastjet::PseudoJet partPseudoJet(partTagMaxPt->Px(),partTagMaxPt->Py(),partTagMaxPt->Pz(),partTagMaxPt->P());
      Double_t dR=fJets[i].plain_distance(partPseudoJet);
      Double_t ppart[3]={partTagMaxPt->Px(),partTagMaxPt->Py(),partTagMaxPt->Pz()};
      Double_t z=GetZparallel(ppart,pjet);
      AliVParticle *pquark=GetMotherQuark(partTagMaxPt);
      Double_t momFracEleoverQ=-2;
      if(pquark){
        UInt_t pdg=TMath::Abs(pquark->PdgCode());
        if(pdg==5)momFracEleoverQ=partTagMaxPt->P()/pquark->P();
      }
      pointsparse[2]=pttag;
      pointsparse[3]=z;
      pointsparse[4]=dR;
      pointsparse[5]=momFracEleoverQ;
      pointsparse[6]=1+neleb+nelecb+nelec;


      fhSparseEleJets->Fill(pointsparse);
      for(Int_t j=0;j<nelecb;j++){
        AliVParticle *part=fMCEvent->GetTrack(fTempTagParticle->At(j));
        if(part==partTagMaxPt)continue;
        pttag=part->Pt();
        partPseudoJet.reset_momentum(part->Px(),part->Py(),part->Pz(),part->P());
        dR=fJets[i].plain_distance(partPseudoJet);
        ppart[0]=part->Px();
        ppart[1]=part->Py();
        ppart[2]=part->Pz();
        z=GetZparallel(ppart,pjet);
        pquark=GetMotherQuark(part);
        momFracEleoverQ=-2;
        if(pquark){
          UInt_t pdg=TMath::Abs(pquark->PdgCode());
          if(pdg==5)momFracEleoverQ=part->P()/pquark->P();
        }
        pointsparse[2]=pttag;
        pointsparse[3]=z;
        pointsparse[4]=dR;
        pointsparse[5]=momFracEleoverQ;
        pointsparse[6]=j+2+neleb+nelecb+nelec;
        fhSparseEleJets->Fill(pointsparse);
      }
    }


    partTagMaxPt=TagJetParticleSpecieDeltaR(fJets[i],fDeltaRSqforTagging,neleDalitz,AliAnalysisTaskSEhfcjMCanalysis::keleDalitz);
    if(neleDalitz>0){
      pointsparse[7]=5;// 1 is a ele b; 2 elecb, 3 elec, 4 eleConv, 5 ele Dalitz, 6 ele unknown
      Double_t pttag=partTagMaxPt->Pt();
      fastjet::PseudoJet partPseudoJet(partTagMaxPt->Px(),partTagMaxPt->Py(),partTagMaxPt->Pz(),partTagMaxPt->P());
      Double_t dR=fJets[i].plain_distance(partPseudoJet);
      Double_t ppart[3]={partTagMaxPt->Px(),partTagMaxPt->Py(),partTagMaxPt->Pz()};
      Double_t z=GetZparallel(ppart,pjet);
      AliVParticle *pquark=GetMotherQuark(partTagMaxPt);
      Double_t momFracEleoverQ=-2;
      if(pquark){
        UInt_t pdg=TMath::Abs(pquark->PdgCode());
        if(pdg==5)momFracEleoverQ=partTagMaxPt->P()/pquark->P();
      }
      pointsparse[2]=pttag;
      pointsparse[3]=z;
      pointsparse[4]=dR;
      pointsparse[5]=momFracEleoverQ;
      pointsparse[6]=1+neleb+nelecb+nelec+neleConv;


      fhSparseEleJets->Fill(pointsparse);
      for(Int_t j=0;j<nelecb;j++){
        AliVParticle *part=fMCEvent->GetTrack(fTempTagParticle->At(j));
        if(part==partTagMaxPt)continue;
        pttag=part->Pt();
        partPseudoJet.reset_momentum(part->Px(),part->Py(),part->Pz(),part->P());
        dR=fJets[i].plain_distance(partPseudoJet);
        ppart[0]=part->Px();
        ppart[1]=part->Py();
        ppart[2]=part->Pz();
        z=GetZparallel(ppart,pjet);
        pquark=GetMotherQuark(part);
        momFracEleoverQ=-2;
        if(pquark){
          UInt_t pdg=TMath::Abs(pquark->PdgCode());
          if(pdg==5)momFracEleoverQ=part->P()/pquark->P();
        }
        pointsparse[2]=pttag;
        pointsparse[3]=z;
        pointsparse[4]=dR;
        pointsparse[5]=momFracEleoverQ;
        pointsparse[6]=j+2+neleb+nelecb+nelec+neleConv;
        fhSparseEleJets->Fill(pointsparse);
      }
    }


    partTagMaxPt=TagJetParticleSpecieDeltaR(fJets[i],fDeltaRSqforTagging,neleUnknown,AliAnalysisTaskSEhfcjMCanalysis::keleunknown);
    if(neleUnknown>0){
      pointsparse[7]=6;// 1 is a ele b; 2 elecb, 3 elec, 4 eleConv, 5 ele Dalitz, 6 ele unknown
      Double_t pttag=partTagMaxPt->Pt();
      fastjet::PseudoJet partPseudoJet(partTagMaxPt->Px(),partTagMaxPt->Py(),partTagMaxPt->Pz(),partTagMaxPt->P());
      Double_t dR=fJets[i].plain_distance(partPseudoJet);
      Double_t ppart[3]={partTagMaxPt->Px(),partTagMaxPt->Py(),partTagMaxPt->Pz()};
      Double_t z=GetZparallel(ppart,pjet);
      AliVParticle *pquark=GetMotherQuark(partTagMaxPt);
      Double_t momFracEleoverQ=-2;
      if(pquark){
        UInt_t pdg=TMath::Abs(pquark->PdgCode());
        if(pdg==5)momFracEleoverQ=partTagMaxPt->P()/pquark->P();
      }
      pointsparse[2]=pttag;
      pointsparse[3]=z;
      pointsparse[4]=dR;
      pointsparse[5]=momFracEleoverQ;
      pointsparse[6]=1+neleb+nelecb+nelec+neleConv+neleDalitz;


      fhSparseEleJets->Fill(pointsparse);
      for(Int_t j=0;j<nelecb;j++){
        AliVParticle *part=fMCEvent->GetTrack(fTempTagParticle->At(j));
        if(part==partTagMaxPt)continue;
        pttag=part->Pt();
        partPseudoJet.reset_momentum(part->Px(),part->Py(),part->Pz(),part->P());
        dR=fJets[i].plain_distance(partPseudoJet);
        ppart[0]=part->Px();
        ppart[1]=part->Py();
        ppart[2]=part->Pz();
        z=GetZparallel(ppart,pjet);
        pquark=GetMotherQuark(part);
        momFracEleoverQ=-2;
        if(pquark){
          UInt_t pdg=TMath::Abs(pquark->PdgCode());
          if(pdg==5)momFracEleoverQ=part->P()/pquark->P();
        }
        pointsparse[2]=pttag;
        pointsparse[3]=z;
        pointsparse[4]=dR;
        pointsparse[5]=momFracEleoverQ;
        pointsparse[6]=j+2+neleb+nelecb+nelec+neleConv+neleDalitz;
        fhSparseEleJets->Fill(pointsparse);
      }
    }




    if(neleb<=0&&nelecb<=0&&neleDalitz<=0){
      fhSparseEleJets->Fill(pointsparse);
    }
  }



}
//______________________________| Terminate
void AliAnalysisTaskSEhfcjMCanalysis::Terminate(Option_t*)
{  
  Info("Terminate","Start and end of Method");

  fOutputQA = dynamic_cast<TList*> (GetOutputData(1));
  if (!fOutputQA) {
    printf("ERROR: Output list not available\n");
    return;
  }

  fOutputList = dynamic_cast<TList*> (GetOutputData(2));
  if (!fOutputList) {
    Printf("ERROR: Output list not available");
    return;
  }

  return;  
}

//______________________________| Event Cut
Bool_t AliAnalysisTaskSEhfcjMCanalysis::IsMCEventSelected(TObject* obj)
{
  if(!obj)return kFALSE;
  AliMCEvent *event = dynamic_cast<AliMCEvent*>(obj);
  if(!event)return kFALSE;

  Bool_t isSelected = kTRUE;
  fHistEventsProcessed->Fill(0.5);

  const AliVVertex *vtxMC = event->GetPrimaryVertex();
  if(!vtxMC)return kFALSE;
  if(TMath::Abs(vtxMC->GetZ()) > 10.00)return kFALSE;
  else fHistEventsProcessed->Fill(1.5);

  //  Int_t PartMultiplicity = event->GetNumberOfTracks();
  //  if(PartMultiplicity < fMinMultiplicity || PartMultiplicity > fMaxMultiplicity)return kFALSE;
  //  else fHistEventsProcessed->Fill(2.5);

  return isSelected;
}

//______________________________| Event Properties
void AliAnalysisTaskSEhfcjMCanalysis::CalculateEventProperties(TObject* obj){

  if(!obj)return;
  AliMCEvent *event = dynamic_cast<AliMCEvent*>(obj);
  if(!event) return;

  //Zvtx
  const AliVVertex *vtxMC = event->GetPrimaryVertex();
  if(!vtxMC) return;
  Float_t zVtx = vtxMC->GetZ();
  ((TH1F*)fOutputQA->FindObject(Form("fHistZvtx")))->Fill(zVtx);

  //Multiplicities Distributions
  //1. all
  Int_t MultTot = event->GetNumberOfTracks(); //fStack->GetNtrack();
  ((TH1F*)fOutputQA->FindObject(Form("fMultTot")))->Fill(MultTot);

  //2. all Primary
  Int_t MultPrimary = fStack->GetNprimary();
  ((TH1F*)fOutputQA->FindObject(Form("fMultPrimary")))->Fill(MultPrimary);

  //3. PhyPrimary and NOT PhyPrimary (eta<1.0)
  Int_t MultPrmyPartYes=0, MultPrmyPartYesCharge=0, MultPrmyPartNO=0;

  for(Int_t jAss=0; jAss<event->GetNumberOfTracks(); jAss++){

    AliVParticle *mcPartN=(AliVParticle*)event->GetTrack(jAss);
    if(!mcPartN) return;
    Int_t pdgAss = TMath::Abs(mcPartN->PdgCode());
    if(TMath::Abs(mcPartN->Eta())>1.0)continue; // Mult

    if(event->IsPhysicalPrimary(jAss)){
      ((TH1F*)fOutputQA->FindObject(Form("fHistNCharge")))->Fill(0.5);
      if(mcPartN->Charge()>0)((TH1F*)fOutputQA->FindObject(Form("fHistNCharge")))->Fill(1.5);
      else if(mcPartN->Charge()<0)((TH1F*)fOutputQA->FindObject(Form("fHistNCharge")))->Fill(2.5);
      else if(mcPartN->Charge()==0)((TH1F*)fOutputQA->FindObject(Form("fHistNCharge")))->Fill(3.5);

      MultPrmyPartYes++;
      if(pdgAss==11||pdgAss==13||pdgAss==211||pdgAss==321||pdgAss==2212)MultPrmyPartYesCharge++;
    }
    else MultPrmyPartNO++;
  }

  if(MultPrmyPartYes!=0)((TH1F*)fOutputQA->FindObject(Form("fMultPhyPriPart")))->Fill(MultPrmyPartYes);
  if(MultPrmyPartNO!=0)((TH1F*)fOutputQA->FindObject(Form("fMultNotPhyPriPart")))->Fill(MultPrmyPartNO);
  if(MultPrmyPartYesCharge!=0)((TH1F*)fOutputQA->FindObject(Form("fMultPhyPriPartCharge")))->Fill(MultPrmyPartYesCharge);

}


//______________________________| Generated Particle Cut
Bool_t AliAnalysisTaskSEhfcjMCanalysis::IsMCParticleGenerated(TObject* obj)
{
  // Check generated particles (primary, charged, pdgcode)
  // Add more related to issue..
  if(!obj) return kFALSE;
  if(!obj->InheritsFrom("AliVParticle")) {
    printf("object must derived from AliVParticle !");
    return kFALSE;
  }

  Bool_t isSelected = kTRUE;
  AliVParticle* particle = dynamic_cast<AliVParticle *>(obj);
  if(!particle) return kFALSE;

  //if(!fstack->IsPhysicalPrimary(particle->GetLabel())){
  //if(particle->PdgCode()!=11)
  //isSelected = kFALSE;
  //}

  return isSelected;
}

//______________________________| Kinematic Particle Cut
Bool_t AliAnalysisTaskSEhfcjMCanalysis::IsMCParticleInKineCriteria(TObject *obj)
{
  //Check generated particles (eta, y, pt)
  if(!obj) return  kFALSE;
  if(!obj->InheritsFrom("AliVParticle")){
    printf("object must derived from AliVParticle !");
    return kFALSE;   
  }

  Bool_t isSelected = kTRUE;
  AliVParticle* particle = dynamic_cast<AliVParticle *>(obj);
  if(!particle) return kFALSE;

  if(particle->Pt()<fPtMin || particle->Pt()>fPtMax) isSelected = kFALSE;
  if(particle->Eta()<fEtaMin || particle->Eta()>fEtaMax) isSelected = kFALSE;
  //if(particle->Y()<fYMin || particle->Y()>fYMax) isSelected = kFALSE; //Prblem now..
  //Double_t energy = particle->E(), pz = particle->Pz();
  //Double_t particleY = (TMath::Abs(energy-pz)>1e-10) ?  0.5*TMath::Log( (energy+pz)/(energy-pz) ) : 1e6;

  return isSelected;
}

//______________________________| Particle Properties
void AliAnalysisTaskSEhfcjMCanalysis::CalculateParticleProperties(TObject* obj){

  if(!obj)return;
  AliMCEvent *event = dynamic_cast<AliMCEvent*>(obj);
  if(!event) return;
  Double_t MCPartProperties[4] = {-999};

  for (Int_t jPart = 0; jPart < event->GetNumberOfTracks(); jPart++){

    AliVParticle *mcPartN=(AliVParticle*)event->GetTrack(jPart);
    if(!mcPartN)continue;

    Bool_t IsParticleMCSelected = IsMCParticleGenerated(mcPartN);
    if(!IsParticleMCSelected)continue;

    Bool_t IsParticleMCKineAccepted = IsMCParticleInKineCriteria(mcPartN);
    if(!IsParticleMCKineAccepted)continue;

    MCPartProperties[1] = mcPartN->Pt();
    MCPartProperties[2] = mcPartN->Phi();
    MCPartProperties[3] = mcPartN->Eta();

    Int_t PDGv = TMath::Abs(mcPartN->PdgCode());

    if(PDGv==1||PDGv==2||PDGv==3)MCPartProperties[0] = 0.5;
    else if(PDGv==4)    MCPartProperties[0] = 1.5;
    else if(PDGv==5)    MCPartProperties[0] = 2.5;
    else if(PDGv==421||PDGv==411||PDGv==413){
      if(PDGv==421)   MCPartProperties[0] = 3.5;
      if(PDGv==411)   MCPartProperties[0] = 4.5;
      if(PDGv==413)   MCPartProperties[0] = 5.5;
    }
    else if(PDGv==511||PDGv==521){
      if(PDGv==511)   MCPartProperties[0] = 6.5;
      if(PDGv==521)   MCPartProperties[0] = 7.5;
    }
    else if(PDGv==11||PDGv==13||PDGv==211||PDGv==321||PDGv==2212){

      if(event->IsPhysicalPrimary(jPart)){
        if(PDGv==211)   MCPartProperties[0] = 8.5;
        if(PDGv==321)   MCPartProperties[0] = 9.5;
        if(PDGv==2212)  MCPartProperties[0] = 10.5;
        if(PDGv==11)    MCPartProperties[0] = 11.5;
        if(PDGv==13)    MCPartProperties[0] = 12.5;
        ((TH1F*)fOutputQA->FindObject(Form("fPhiDist")))->Fill(mcPartN->Phi());
        ((TH1F*)fOutputQA->FindObject(Form("fPhiEta")))->Fill(mcPartN->Eta());
        ((TH1F*)fOutputQA->FindObject(Form("fPhiY")))->Fill(mcPartN->Y());

      }
      else {
        if(PDGv==211)   MCPartProperties[0] = 13.5;
        if(PDGv==321)   MCPartProperties[0] = 14.5;
        if(PDGv==2212)  continue;//MCPartProperties[0] = 15.5; //beam particle
        if(PDGv==11)    MCPartProperties[0] = 16.5;
        if(PDGv==13)    MCPartProperties[0] = 17.5;
      }
    }
    else if(event->IsPhysicalPrimary(jPart))MCPartProperties[0] = 18.5;
    else  MCPartProperties[0] = 19.5;

    ((THnSparseD*)fOutputQA->FindObject(Form("fHistNParticle")))->Fill(MCPartProperties);
    //Comment --> Beam Proton giving peak in Phi at = Pi();
  }
}


//______________________________| HF-Hadron Correlations
void AliAnalysisTaskSEhfcjMCanalysis::CalculateHFHadronCorrelations(){

  for (Int_t jPart = 0; jPart < fMcEvent->GetNumberOfTracks(); jPart++){

    AliVParticle* mcPartHF = (AliVParticle*)fMcEvent->GetTrack(jPart);
    if (!mcPartHF)continue;
    Int_t pdgHF = TMath::Abs(mcPartHF->PdgCode());

    if(pdgHF==421 || pdgHF==411 || pdgHF==413){ ///D0, D+, D*

      if(mcPartHF->Pt() <2 || mcPartHF->Pt() >20)continue;
      if(TMath::Abs(mcPartHF->Y()) > 0.5)continue;
      fArraySkipDDaugh->Reset(-1);
      fArraySkipDDaugh->AddAt(jPart,0);
      flastdaugh=1;
      RemoveNDaughterParticleArray(mcPartHF);
      HeavyFlavourCorrelations(mcPartHF);
    }
    else if(pdgHF==11){
      if(SelectElectronForCorrel(mcPartHF)){		\
        fArraySkipDDaugh->Reset(-1);
      fArraySkipDDaugh->AddAt(jPart,0);
      flastdaugh=1;
      RemoveNDaughterParticleArray(mcPartHF);
      HeavyFlavourCorrelations(mcPartHF);
      }
    }
  }
}

//______________________________| Remove decay particles of trigger
void AliAnalysisTaskSEhfcjMCanalysis::RemoveNDaughterParticleArray(TObject* obj){

  if(!obj)return;
  AliVParticle* TrgPart = (AliVParticle*)obj;
  if(!TrgPart)return;

  Int_t DauPosF = TrgPart->GetFirstDaughter();
  Int_t DauPosL = TrgPart->GetLastDaughter();

  if(DauPosL<0)DauPosL=DauPosF;
  if(DauPosF > 0){
    for(Int_t jd = DauPosF; jd <= DauPosL; jd++){
      AliVParticle* Trg_DouPart = (AliVParticle*)fMcEvent->GetTrack(jd);
      if (!Trg_DouPart)continue;
      Int_t PDG_DouPart = TMath::Abs(Trg_DouPart->PdgCode());

      if(PDG_DouPart==11||PDG_DouPart==13||PDG_DouPart==211||PDG_DouPart==321||PDG_DouPart==2212){
        fArraySkipDDaugh->AddAt(jd,flastdaugh);
        flastdaugh++;
        RemoveNDaughterParticleArray(Trg_DouPart);
      }
      else {
        RemoveNDaughterParticleArray(Trg_DouPart);
      }
    }
  }
}


//______________________________| HF-Correlations Calculations
void AliAnalysisTaskSEhfcjMCanalysis::HeavyFlavourCorrelations(TObject *obj){

  if(!obj) return;
  AliVParticle* TrigPart = (AliVParticle*)obj;
  if(!TrigPart) return;

  Int_t PDG_TrigPart  = TMath::Abs(TrigPart->PdgCode());
  if(PDG_TrigPart==421)    PDG_TrigPart = 1; //D0
  else if(PDG_TrigPart==411)    PDG_TrigPart = 2; //D+
  else if(PDG_TrigPart==413)    PDG_TrigPart = 3; //D*
  else if(PDG_TrigPart==11)     PDG_TrigPart = 4; //e
  else                          PDG_TrigPart = 0; //NULL

  Bool_t hasToSkip = kFALSE;
  Int_t softpi = -1;
  Int_t TrgMomPos  = TrigPart->GetMother();

  if(TrgMomPos > 0){

    AliVParticle *MotherOfTrg = (AliVParticle*)fMcEvent->GetTrack(TrgMomPos);
    if(!MotherOfTrg) return;
    Int_t pdgOfMother = TMath::Abs(MotherOfTrg->PdgCode());


    if(PDG_TrigPart==4){ //Electron Stuff
      if(!(pdgOfMother==5||pdgOfMother==4||(400<pdgOfMother&&pdgOfMother<600)||(4000<pdgOfMother&&pdgOfMother<6000))){// NON HF electron
        if(pdgOfMother==22) PDG_TrigPart=5; //GAMMA CONV
        else if(pdgOfMother==111||pdgOfMother==113||pdgOfMother==221||pdgOfMother==223) PDG_TrigPart=6; //DALITZ DECAY
        else PDG_TrigPart=7; // Others NonHF
      }
    }

    if(pdgOfMother==413){ // Dstar --> D0: check soft pion
      if(PDG_TrigPart==1){
        for(Int_t isp = MotherOfTrg->GetFirstDaughter(); isp <= MotherOfTrg->GetLastDaughter(); isp++){
          AliVParticle *sfp=(AliVParticle*)fMcEvent->GetTrack(isp);
          Int_t pdgsp=TMath::Abs(sfp->PdgCode());
          if(pdgsp==211)softpi=isp;
        }
      }
      TrgMomPos        =  MotherOfTrg->GetMother();
      if(TrgMomPos<0)return;
      MotherOfTrg      =  (AliVParticle*)fMcEvent->GetTrack(TrgMomPos);
      pdgOfMother      =  TMath::Abs(MotherOfTrg->PdgCode());

    }else if(pdgOfMother==423){// D*0 -> D0 or D+
      TrgMomPos        =  MotherOfTrg->GetMother();
      if(TrgMomPos<0)return;
      MotherOfTrg      =  (AliVParticle*)fMcEvent->GetTrack(TrgMomPos);
      pdgOfMother      =  TMath::Abs(MotherOfTrg->PdgCode());
    }

    if(pdgOfMother==5||(500<pdgOfMother&&pdgOfMother<600)||(5000<pdgOfMother&&pdgOfMother<6000)){
      PDG_TrigPart*=-1;
    }else if(pdgOfMother==4){
      TrgMomPos = MotherOfTrg->GetMother();
      if(TrgMomPos >= 0){
        MotherOfTrg   =  (AliVParticle*)fMcEvent->GetTrack(TrgMomPos);
        pdgOfMother   =  TMath::Abs(MotherOfTrg->PdgCode());
        if(pdgOfMother==5||(500<pdgOfMother&&pdgOfMother<600)||(5000<pdgOfMother&&pdgOfMother<6000)){
          PDG_TrigPart*=-1;
        }
      }
    }
  }


  Double_t phiTrig = TrigPart->Phi();
  Double_t etaTrig = TrigPart->Eta();
  Double_t ptTrig  = TrigPart->Pt();

  Double_t PartProperties[8] = {static_cast<Double_t>(PDG_TrigPart),ptTrig,etaTrig,0,0,0,0,0};
  ((THnSparseD*)fOutputList->FindObject(Form("HFTrgiggerProp")))->Fill(PartProperties);

  Int_t nPartAss = 0;
  TArrayI* fArrayAssoPart = CalculateNPartType("Charge", nPartAss, 0);
  if(!fArrayAssoPart)return;

  for(Int_t ipartAsso = 0; ipartAsso < nPartAss; ipartAsso++){

    AliVParticle *partAsso  = (AliVParticle*)fMcEvent->GetTrack(fArrayAssoPart->At(ipartAsso));
    if(!partAsso)continue;
    hasToSkip=kFALSE;

    for(Int_t jdskip = 1; jdskip < flastdaugh; jdskip++){
      if(fArrayAssoPart->At(ipartAsso)==fArraySkipDDaugh->At(jdskip)){
        hasToSkip=kTRUE;
        break;
      }
    }

    if(hasToSkip)continue;

    PartProperties[3] = partAsso->Pt();
    PartProperties[4] = partAsso->Eta();
    PartProperties[5] = GetPhiInStandardRange(phiTrig-partAsso->Phi());
    PartProperties[6] = etaTrig-partAsso->Eta();
    UInt_t pdgassmum  = TMath::Abs(partAsso->PdgCode());

    if(fArrayAssoPart->At(ipartAsso) == softpi)PartProperties[7]=-1;
    else if(pdgassmum==13)  PartProperties[7]=2;
    else if(pdgassmum==211) PartProperties[7]=3;
    else if(pdgassmum==321) PartProperties[7]=4;
    else if(pdgassmum==2212)PartProperties[7]=5;
    else if(pdgassmum==11){ //e-

      if(fStack->IsPhysicalPrimary(partAsso->GetLabel()))PartProperties[7]=1;//Prim e-

      Int_t eleMother = partAsso->GetMother();
      if(eleMother>0){

        AliVParticle *partEleMum=(AliVParticle*)fMcEvent->GetTrack(eleMother);
        if(!partEleMum)continue;
        UInt_t pdgeleMother = TMath::Abs(partEleMum->PdgCode());

        if(pdgeleMother==111||pdgeleMother==113||pdgeleMother==221||pdgeleMother==223)PartProperties[7]=6;// DALITZ DECAY
        else if(pdgeleMother==22)PartProperties[7]=7;// GAMMA CONV
        else if(pdgeleMother==4||pdgeleMother==5||(pdgeleMother>400&&pdgeleMother<600)||(pdgeleMother>4000&&pdgeleMother<6000))PartProperties[7]=8;
      }
      else PartProperties[7]=9;
    }
    else PartProperties[7]=0;

    Double_t ptLim_Sparse = ((THnSparseD*)fOutputList->FindObject(Form("2PCorrBtwn_HF-hadron")))->GetAxis(3)->GetXmax();
    if(PartProperties[3] > ptLim_Sparse) PartProperties[3] = ptLim_Sparse - 0.01;

    ((THnSparseD*)fOutputList->FindObject(Form("2PCorrBtwn_HF-hadron")))->Fill(PartProperties);

  }
}

//______________________________| HH Calculations
void AliAnalysisTaskSEhfcjMCanalysis::CalculateHadronHadronCorrelations(TObjArray *ParticleArray){

  //Under Development !
  //Bool_t hasToSkip=kFALSE;
  Int_t nPartCharge =0;
  TArrayI* fArrayChargePart = CalculateNPartType("Charge", nPartCharge, 0);
  if(!fArrayChargePart)return;

  for(Int_t ipart1 = 0; ipart1 < nPartCharge ; ipart1++){

    AliVParticle *partTrgCharge=(AliVParticle*)ParticleArray->At(fArrayChargePart->At(ipart1));
    if(!partTrgCharge)return;

    Int_t pdgTrigCharge  =TMath::Abs(partTrgCharge->PdgCode());
    if(pdgTrigCharge  == 11)        pdgTrigCharge = 0;
    else if(pdgTrigCharge == 13)    pdgTrigCharge = 1;
    else if(pdgTrigCharge == 211)   pdgTrigCharge = 2;
    else if(pdgTrigCharge == 321)   pdgTrigCharge = 3;
    else if(pdgTrigCharge == 2212)  pdgTrigCharge = 4;
    else pdgTrigCharge =-1;

    Double_t etaTrigCharge = partTrgCharge->Pt();
    Double_t phiTrigCharge = partTrgCharge->Phi();
    Double_t pTTrigCharge  = partTrgCharge->Eta();

    Double_t PropertiesOfTrigCharge[3] = {static_cast<Double_t>(pdgTrigCharge), pTTrigCharge, etaTrigCharge};//
    ((THnSparseD*)fOutputList->FindObject(Form("ChargeTrgiggerProp")))->Fill(PropertiesOfTrigCharge);

    for(Int_t ipart2 = 0; ipart2 < nPartCharge ; ipart2++){

      if(ipart2==ipart1)continue;

      AliVParticle *partAssoCharge=(AliVParticle*)ParticleArray->At(fArrayChargePart->At(ipart2));
      if(!partAssoCharge)return;

      Double_t etaAssoCharge = partAssoCharge->Pt();
      Double_t phiAssoCharge = partAssoCharge->Phi();
      Double_t pTAssoCharge  = partAssoCharge->Eta();

      Double_t DeltaPhi = GetPhiInStandardRange(phiTrigCharge - phiAssoCharge);
      Double_t DeltaEta = etaTrigCharge - pTAssoCharge;

      Double_t HHCorr[8] = {static_cast<Double_t>(pdgTrigCharge), pTTrigCharge, etaTrigCharge, pTAssoCharge, etaAssoCharge, DeltaPhi, DeltaEta, 0};//

      Double_t ptLim_Sparse = ((THnSparseD*)fOutputList->FindObject(Form("2PCorrBtwn_%s-%s","TrgCharge","hadron")))->GetAxis(3)->GetXmax();
      if(pTAssoCharge > ptLim_Sparse) pTAssoCharge = ptLim_Sparse - 0.01;
      HHCorr[3] = pTAssoCharge;

      HHCorr[7]=TMath::Abs(partAssoCharge->PdgCode());
      if(HHCorr[7]==11)         HHCorr[7]=0;
      else if(HHCorr[7]==13)    HHCorr[7]=1;
      else if(HHCorr[7]==211)   HHCorr[7]=2;
      else if(HHCorr[7]==321)   HHCorr[7]=3;
      else if(HHCorr[7]==2212)  HHCorr[7]=4;
      else HHCorr[7]=-1;

      ((THnSparseD*)fOutputList->FindObject(Form("2PCorrBtwn_TrgCharge-hadron")))->Fill(HHCorr);
    }
  }
}


//______________________________| QQbar Calculations
void AliAnalysisTaskSEhfcjMCanalysis::CalculateQQBarCorrelations(){

  Int_t nPart1 =0, nPart2 = 0;

  TArrayI* fArray1Part = CalculateNPartType(fCorrPart1.Data(), nPart1, fChargeSel1);
  TArrayI* fArray1Part1 = new TArrayI(*fArray1Part);

  if(nPart1>0){

    TArrayI* fArray2Part = CalculateNPartType(fCorrPart2.Data(), nPart2, fChargeSel2);
    TArrayI* fArray2Part2 = new TArrayI(*fArray2Part);

    if(nPart2>0){

      for(Int_t ipart1 = 0; ipart1 < nPart1 ; ipart1++){

        AliVParticle *part1=(AliVParticle*)fMcEvent->GetTrack(fArray1Part1->At(ipart1));
        ((TH1F*)fOutputList->FindObject(Form("fHistPhiPart1")))->Fill(part1->Phi());

        for(Int_t ipart2 = 0; ipart2 < nPart2 ; ipart2++){
          AliVParticle *part2=(AliVParticle*)fMcEvent->GetTrack(fArray2Part2->At(ipart2));
          if(ipart2==0){((TH1F*)fOutputList->FindObject(Form("fHistPhiPart2")))->Fill(part2->Phi());
          }
          Double_t DeltaPhi = GetPhiInStandardRange(part1->Phi() - part2->Phi());
          Double_t DeltaEta = part1->Eta() - part2->Eta();
          Double_t CorrVar[2] = {DeltaEta, DeltaPhi};
          ((THnSparseD*)fOutputList->FindObject(Form("2PCorrBtwn_%s-%s",fCorrPart1.Data(),fCorrPart2.Data())))->Fill(CorrVar);
        }
      }
    }
    delete fArray2Part2;
  }
  delete fArray1Part1;
}


//______________________________| Particle Type Calculations
TArrayI* AliAnalysisTaskSEhfcjMCanalysis::CalculateNPartType(TString pname, Int_t &count, Int_t ChargeSel){

  count=0;
  fArrayTrk->Reset(-1);

  for(Int_t jAss=0; jAss<fMcEvent->GetNumberOfTracks(); jAss++){

    AliVParticle *partAss=(AliVParticle*)fMcEvent->GetTrack(jAss);

    Bool_t IsParticleMCSelected = IsMCParticleGenerated(partAss);
    if(!IsParticleMCSelected)continue;

    Bool_t IsParticleMCKineAccepted = IsMCParticleInKineCriteria(partAss);
    if(!IsParticleMCKineAccepted)continue;

    Int_t pdgAss=partAss->PdgCode();

    if(ChargeSel == -1 && pdgAss>0)continue;
    else if(ChargeSel == 1 && pdgAss<0)continue;

    if(pname == "c" && (TMath::Abs(pdgAss)==4)){
      fArrayTrk->AddAt(jAss,count);
      count++;
    }else if(pname == "b" && (TMath::Abs(pdgAss)==5)){
      fArrayTrk->AddAt(jAss,count);
      count++;
    }else if(pname == "D" && (TMath::Abs(pdgAss)==411||TMath::Abs(pdgAss)==413||TMath::Abs(pdgAss)==421)){
      fArrayTrk->AddAt(jAss,count);
      count++;
    }else if(pname == "B" && (TMath::Abs(pdgAss)==511||TMath::Abs(pdgAss)==521)){
      fArrayTrk->AddAt(jAss,count);
      count++;
    }else if(pname == "Charge" && (TMath::Abs(pdgAss)==11||TMath::Abs(pdgAss)==13||TMath::Abs(pdgAss)==211||TMath::Abs(pdgAss)==321||TMath::Abs(pdgAss)==2212)){
      if(!fStack->IsPhysicalPrimary(partAss->GetLabel()))continue;
      fArrayTrk->AddAt(jAss,count);
      count++;
    }
  }
  return fArrayTrk;
}



//______________________________| Definations of histo etc.
void AliAnalysisTaskSEhfcjMCanalysis::DefineHistoNames(){

  Double_t Pi = TMath::Pi();

  //1. Event Properties
  fHistEventsProcessed = new TH1I("fHistNEvents","fHistEventsProcessed",6,0,6) ;
  fHistEventsProcessed->GetXaxis()->SetBinLabel(1,"All events");
  fHistEventsProcessed->GetXaxis()->SetBinLabel(2,"Event with Ztx");
  fHistEventsProcessed->GetXaxis()->SetBinLabel(3,"Event with Mult");
  fHistEventsProcessed->GetXaxis()->SetBinLabel(4,"All Particle");
  fHistEventsProcessed->GetXaxis()->SetBinLabel(5,"Only Physical Particle");
  fHistEventsProcessed->GetXaxis()->SetBinLabel(6,"After Kine Cuts");
  fOutputQA->Add(fHistEventsProcessed);

  TH1F        *HistogramQA[13];
  for ( Int_t i = 0; i < 11; i++)HistogramQA[i] = NULL;
  HistogramQA[0] = new TH1F("fHistZvtx", "ZVtx distribution", 30, -30., 30.);
  HistogramQA[0]->GetXaxis()->SetTitle("ZVtx");
  HistogramQA[0]->GetYaxis()->SetTitle("N_tot");
  HistogramQA[0]->SetMarkerStyle(kFullCircle);

  HistogramQA[1] = new TH1F("fMultTot", "Mult Total", 100, 0., 2000.);
  HistogramQA[1]->GetXaxis()->SetTitle("Total Multiplicity");
  HistogramQA[1]->GetYaxis()->SetTitle("N_tot");
  HistogramQA[1]->SetMarkerStyle(kFullCircle);

  HistogramQA[2] = new TH1F("fMultPrimary", "Mult Primary", 100, 0., 2000.);
  HistogramQA[2]->GetXaxis()->SetTitle("Primary Multiplicity");
  HistogramQA[2]->GetYaxis()->SetTitle("N_tot");
  HistogramQA[2]->SetMarkerStyle(kFullCircle);

  HistogramQA[3] = new TH1F("fMultPhyPriPart", "Mult PhyPrimary", 200, 0, 400);
  HistogramQA[3]->GetXaxis()->SetTitle("PhyPrimary Multiplicity");
  HistogramQA[3]->GetYaxis()->SetTitle("N_Pri");
  HistogramQA[3]->SetMarkerStyle(kFullCircle);

  HistogramQA[4] = new TH1F("fMultNotPhyPriPart", "Mult NotPhyPrimary", 200, 0, 400.);
  HistogramQA[4]->GetXaxis()->SetTitle("NotPhyPrimary Multiplicity");
  HistogramQA[4]->GetYaxis()->SetTitle("No_Pri");
  HistogramQA[4]->SetMarkerStyle(kFullCircle);

  HistogramQA[5] = new TH1F("fMultPhyPriPartCharge", "Mult Charge PhyPrimary", 150, 0, 150);
  HistogramQA[5]->GetXaxis()->SetTitle("Chrg_PhyPrimaryMultiplicity");
  HistogramQA[5]->GetYaxis()->SetTitle("N_PhyPri_Charge");
  HistogramQA[5]->SetMarkerStyle(kFullCircle);

  HistogramQA[6] = new TH1F("fHistNCharge","Particle Counts",4,0,4) ;
  HistogramQA[6]->GetXaxis()->SetBinLabel(1,"All Particle");
  HistogramQA[6]->GetXaxis()->SetBinLabel(2,"+ive Charge");
  HistogramQA[6]->GetXaxis()->SetBinLabel(3,"-ive Charge");
  HistogramQA[6]->GetXaxis()->SetBinLabel(4,"Neutral Charge");

  HistogramQA[7] = new TH1F("fRemoveDaugh", "Remove Daughters", 20, 0., 20);
  HistogramQA[7]->GetXaxis()->SetTitle("N Daughters");
  HistogramQA[7]->GetYaxis()->SetTitle("N_removed");
  HistogramQA[7]->SetMarkerStyle(kFullCircle);

  HistogramQA[8] = new TH1F("fPhiDist", "Phi Dist", 45, -0.5*Pi, 2*Pi);
  HistogramQA[8]->GetXaxis()->SetTitle("#phi");
  HistogramQA[8]->GetYaxis()->SetTitle("N_pri");
  HistogramQA[8]->SetMarkerStyle(kFullCircle);

  HistogramQA[9] = new TH1F("fPhiEta", "Eta Dist", 90, -9., 9.);
  HistogramQA[9]->GetXaxis()->SetTitle("#eta");
  HistogramQA[9]->GetYaxis()->SetTitle("N_pri");
  HistogramQA[9]->SetMarkerStyle(kFullCircle);

  HistogramQA[10] = new TH1F("fPhiY", "Y Dist", 90, -9., 9.);
  HistogramQA[10]->GetXaxis()->SetTitle("Y");
  HistogramQA[10]->GetYaxis()->SetTitle("N_pri");
  HistogramQA[10]->SetMarkerStyle(kFullCircle);

  HistogramQA[11] = new TH1F("fWeight", "event weight", 3, -1.5,1.5);
  HistogramQA[11]->SetMarkerStyle(kFullCircle);

  HistogramQA[12] = new TH1F("fSigma", "sigma weight", 200, 2.5, 5.5);
  HistogramQA[12]->SetMarkerStyle(kFullCircle);

  for(Int_t i = 0; i <13; i++)
  {
    HistogramQA[i]->Sumw2();
    fOutputQA->Add(HistogramQA[i]);
  }

  //2. Particle Properties
  Int_t    fBins[4] = {20,  200,             18,    30 };
  Double_t  fMin[4] = {0,   0,    0*TMath::Pi(),   -15.};
  Double_t  fMax[4] = {20,  40,   2*TMath::Pi(),    15.};
  THnSparseD *HistogramQAPartProp   = new THnSparseD("fHistNParticle",  "Particle Properties",  4, fBins, fMin, fMax);
  HistogramQAPartProp->GetAxis(0)->SetBinLabel(1, "LtQuarks");
  HistogramQAPartProp->GetAxis(0)->SetBinLabel(2, "Charm");
  HistogramQAPartProp->GetAxis(0)->SetBinLabel(3, "Beauty");
  HistogramQAPartProp->GetAxis(0)->SetBinLabel(4, "DZero");
  HistogramQAPartProp->GetAxis(0)->SetBinLabel(5, "DPlus");
  HistogramQAPartProp->GetAxis(0)->SetBinLabel(6, "DStar");
  HistogramQAPartProp->GetAxis(0)->SetBinLabel(7, "BZero");
  HistogramQAPartProp->GetAxis(0)->SetBinLabel(8, "BPlus");
  HistogramQAPartProp->GetAxis(0)->SetBinLabel(9, "PPions");
  HistogramQAPartProp->GetAxis(0)->SetBinLabel(10,"PKaons");
  HistogramQAPartProp->GetAxis(0)->SetBinLabel(11,"PProtons");
  HistogramQAPartProp->GetAxis(0)->SetBinLabel(12,"PElectoon");
  HistogramQAPartProp->GetAxis(0)->SetBinLabel(13,"PMuons");
  //Physics Primary..
  HistogramQAPartProp->GetAxis(0)->SetBinLabel(14,"nPPions");
  HistogramQAPartProp->GetAxis(0)->SetBinLabel(15,"nPKaons");
  HistogramQAPartProp->GetAxis(0)->SetBinLabel(16,"nPProtons");
  HistogramQAPartProp->GetAxis(0)->SetBinLabel(17,"nPElectoon");
  HistogramQAPartProp->GetAxis(0)->SetBinLabel(18,"nPMuons");
  HistogramQAPartProp->GetAxis(0)->SetBinLabel(19,"Others PhyPri");
  HistogramQAPartProp->GetAxis(0)->SetBinLabel(20,"Nt PPryOthers");

  HistogramQAPartProp->GetAxis(1)->SetTitle("p_{T}");
  HistogramQAPartProp->GetAxis(2)->SetTitle("#phi");
  HistogramQAPartProp->GetAxis(3)->SetTitle("#eta");
  fOutputQA->Add(HistogramQAPartProp);


  //3. Correlations histograms
  //3a. Quark-AntiQuark Correlations
  if(fIsCorrOfQQbar){

    Int_t    fBinsCorr[2] = {20,     32}; //DEta, DPhi
    Double_t  fMinCorr[2] = {-2.0, -0.5*TMath::Pi()};
    Double_t  fMaxCorr[2] = {+2.0,  1.5*TMath::Pi()};

    TString Corr2PQQbar = "2PCorrBtwn_";
    Corr2PQQbar += fCorrPart1.Data();
    Corr2PQQbar += "-";
    Corr2PQQbar += fCorrPart2.Data();

    THnSparseD *Correlation2PQQbar    = new THnSparseD(Corr2PQQbar.Data(),"2PCorrQQbar", 2, fBinsCorr, fMinCorr, fMaxCorr);
    Correlation2PQQbar->Sumw2();
    fOutputList->Add(Correlation2PQQbar);

    TH1F *hPhiPart1 = new TH1F("fHistPhiPart1", "Phi_Dist_of_Part1", 45, -0.5*Pi, 2*Pi);
    TH1F *hPhiPart2 = new TH1F("fHistPhiPart2", "Phi_Dist_of_Part2", 45, -0.5*Pi, 2*Pi);
    hPhiPart1->Sumw2();
    hPhiPart2->Sumw2();
    fOutputList->Add(hPhiPart1);
    fOutputList->Add(hPhiPart2);

  }


  //3b. D-Hadron Correlations
  if(fIsCorrOfHeavyFlavor){

    Int_t     nbinsTrigHF[3] = {  15, 20,  20};
    Double_t binlowTrigHF[3] = {-7.5, 0., -2.};
    Double_t  binupTrigHF[3] = { 7.5, 20., 2.};

    Int_t     nbinsCorrHF[8] = {  15, 20,  20,  6,   30,               32,   20,    11};
    Double_t binlowCorrHF[8] = {-7.5, 0., -2., 0., -15., -0.5*TMath::Pi(),  -5.,  -1.5};
    Double_t  binupCorrHF[8] = { 7.5, 20., 2., 3.,  15.,  1.5*TMath::Pi(),   5.,   9.5};

    THnSparseD *trigDPartPr   = new THnSparseD("HFTrgiggerProp","fHFTrgiggerProp;pdg;ptTrig;etaTrig;",3,nbinsTrigHF,binlowTrigHF,binupTrigHF);
    THnSparseD *trigDPartCorr = new THnSparseD("2PCorrBtwn_HF-hadron","HFCorrelations;pdg;ptTrig;etaTrig;ptAss;etaAss;deltaPhi;deltaEta;pdgAss;",8,nbinsCorrHF,binlowCorrHF,binupCorrHF);

    trigDPartPr->Sumw2();
    trigDPartCorr->Sumw2();
    fOutputList->Add(trigDPartPr);
    fOutputList->Add(trigDPartCorr);
  }

  //3c. Hadron-Hadron Correlations
  if(fIsCorrOfHadronHadron){

    Int_t     nbinsTrigHH[3] = {   6, 30,  20 };
    Double_t binlowTrigHH[3] = {-1.5, 0., -2.};
    Double_t  binupTrigHH[3] = { 4.5, 30., 2.};

    Int_t     nbinsCorrHH[8] = {  6,  30,  20,  6,   30,               72,    20,    6};
    Double_t binlowCorrHH[8] = {-1.5, 0., -2., 0., -15., -0.5*TMath::Pi(),  -5,  -1.5};
    Double_t  binupCorrHH[8] = { 4.5, 30., 2., 3.,  15.,  1.5*TMath::Pi(),   5.,  4.5};

    THnSparseD *trigChargePartPr   = new THnSparseD("ChargeTrgiggerProp","fChargeTrgiggerProp;pdg;ptTrig;etaTrig;",3,nbinsTrigHH,binlowTrigHH,binupTrigHH);
    THnSparseD *trigHHPartCorr = new THnSparseD("2PCorrBtwn_TrgCharge-hadron","HHCorrelations;pdg;ptTrig;etaTrig;ptAss;etaAss;deltaPhi;deltaEta;pdgAss;",8,nbinsCorrHH,binlowCorrHH,binupCorrHH);
    trigChargePartPr->Sumw2();
    trigHHPartCorr->Sumw2();
    fOutputList->Add(trigChargePartPr);
    fOutputList->Add(trigHHPartCorr);

  }

  PostData(1, fOutputQA);
  PostData(2, fOutputList);
}

AliAnalysisTaskSEhfcjMCanalysis* AliAnalysisTaskSEhfcjMCanalysis::AddTaskSEhfcjMCanalysis(TString suffixName)
{
  AliAnalysisTaskSEhfcjMCanalysis* clus = new  AliAnalysisTaskSEhfcjMCanalysis("");
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

  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) {
    Printf("AliAnalysisTaskSEhfcjMCanalysis, No analysis manager to connect to.");
    return NULL;
  }

  if(!mgr->GetMCtruthEventHandler()){
    Printf("AliAnalysisTaskSEhfcjMCanalysis; This task requires an input MC event handler");
    return NULL;
  }

  mgr->AddTask(clus);

  // Create containers for input/output
  TString finDirname   = "";
  TString inname       = "cinput";
  TString outBasic     = "BasicPlots";
  TString Specific     = "Specific";

  finDirname += suffixName.Data();
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

  return clus;
}

