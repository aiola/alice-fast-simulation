#ifndef AliANALYSISTASKSEHFCJMCANALYSIS_H
#define AliANALYSISTASKSEHFCJMCANALYSIS_H

/*_____________________________________________________________

Class AliAnalysisTaskSEhfcjMCanalysis: On the fly Simulation class for
heavy flavour correlations and general event/part properties

Authors:
Jitendra Kumar (jitendra.kumar@cern.ch)
Andrea Rossi   (andrea.rossi@cern.ch)
 _____________________________________________________________*/

#include <AliAnalysisTaskSE.h>

class TH1;
class TNtuple;
class AliStack;
class AliVParticle;

#include <fastjet/config.h>
#include <fastjet/PseudoJet.hh>
#include <fastjet/JetDefinition.hh>
#include <fastjet/ClusterSequence.hh>
#include <fastjet/ClusterSequenceArea.hh>
#include <fastjet/AreaDefinition.hh>

class AliAnalysisTaskSEhfcjMCanalysis : public AliAnalysisTaskSE{

public:
  enum ESpecieTags {kbquark,kbhadron,kDfromB,kelefromB,kelefromDfromB,kcquark,kchadron,kelefromD,keleconversion,keleDalitz,keleunknown};

  AliAnalysisTaskSEhfcjMCanalysis();
  AliAnalysisTaskSEhfcjMCanalysis(const Char_t* name);
  AliAnalysisTaskSEhfcjMCanalysis(const AliAnalysisTaskSEhfcjMCanalysis& c);
  AliAnalysisTaskSEhfcjMCanalysis& operator= (const AliAnalysisTaskSEhfcjMCanalysis& c);
  virtual ~AliAnalysisTaskSEhfcjMCanalysis();

  virtual void UserCreateOutputObjects();
  virtual void Init();
  virtual void LocalInit(){Init();}
  virtual void UserExec(Option_t *option);
  virtual void Terminate(Option_t *);

  void SetEtaRange(Float_t etamin, Float_t etamax){ fEtaMin=etamin; fEtaMax=etamax; }
  void SetYRange(Float_t ymin, Float_t ymax){ fYMin=ymin; fYMax=ymax; }
  void SetPtRange(Float_t ptmin, Float_t ptmax){ fPtMin=ptmin; fPtMax=ptmax; }
  void SetMultRange(Int_t Mmin, Int_t Mmax){ fMinMultiplicity=Mmin; fMaxMultiplicity=Mmax; }
  void SetQQbarCorrBetween(TString part1, Int_t charge1, TString part2, Int_t charge2)
  {fCorrPart1=part1; fChargeSel1 = charge1; fCorrPart2=part2; fChargeSel2 = charge2;}
  void SetUseOnlyEleMuonChargedPionKaonProtonsForJets(Bool_t useEMPKPr){fSelOnlyEMuPiKProtForJets=useEMPKPr;}

  void SetPartProperties(Bool_t pYorN0){   fIsPartProp  = pYorN0; }
  void SetEventProperties(Bool_t eYorN0){  fIsEventProp = eYorN0; }
  void SetHFCorrelations(Bool_t YorN2){   fIsCorrOfHeavyFlavor    = YorN2; }
  void SetHHCorrelations(Bool_t YorN3){   fIsCorrOfHadronHadron   = YorN3; }
  void SetQQbarCorrelations(Bool_t YorN1){fIsCorrOfQQbar          = YorN1; }

  Double_t GetZparallel(Double_t *pPart,Double_t *pJet);
  AliVParticle* GetMotherQuark(AliVParticle *part);

  void SetFillHFJetCorrelationHistos(Bool_t fill){fFillJetCorrelations=fill;}
  void SetFillHFjetHistos(Bool_t fill){fFillHFJet=fill;}
  static AliAnalysisTaskSEhfcjMCanalysis* AddTaskSEhfcjMCanalysis(TString suffixName ="");

private:
  void CalculateEventProperties(TObject* obj);
  void CalculateParticleProperties(TObject *obj);
  void CalculateHFHadronCorrelations();
  void CalculateHadronHadronCorrelations(TObjArray *ParticleArray);
  void Calculate3PCorrelations();
  void CalculateQQBarCorrelations();
  void FillParticleTaggedJetProperties();
  void FillPromptDTaggedJetProperties();
  void FillFeedDownDTaggedJetProperties();
  void FillHFeTaggedBeautyJetProperties();
  void FillBTaggedJetProperties();
  void FillHFJetCorrelations();
  void FillbquarkCorrelations(){return ;}
  void FillcquarkCorrelations(){return ;}
  void FillPromptDmesonJetCorrelations();
  void FillBmesonJetCorrelations();
  void FillFeedDownDmesonJetCorrelations();
  void FillHFeJetCorrelations();


  TArrayI* CalculateNPartType(TString pname, Int_t &count, Int_t ChargeSel);
  void HeavyFlavourCorrelations(TObject* obj);
  void RemoveNDaughterParticleArray(TObject* obj);
  Bool_t SelectElectronForCorrel(TObject*){return kFALSE;}
  void DefineHistoNames();

  Double_t GetPhiInStandardRange(Double_t phi){
    if (phi > +1.5 * TMath::Pi())phi -= TMath::TwoPi();
    if (phi < -0.5 * TMath::Pi())phi += TMath::TwoPi();
    return phi;
  }

  Bool_t CheckIsFinal(AliVParticle *part,Int_t pdgRange);
  Bool_t CheckIsFirst(AliVParticle *part,Int_t pdgRange);
  Bool_t CheckIsFromBdecay(AliVParticle *vpart);
  Bool_t CheckIsFromDdecay(AliVParticle *vpart);
  Bool_t IsMCParticleReconstructableForChargedJets(AliVParticle *part);

protected:
  Bool_t IsMCEventSelected(TObject* obj);
  Bool_t IsMCParticleGenerated(TObject* obj);
  Bool_t IsMCParticleInKineCriteria(TObject* obj);
  void InitInternalVariables();
  void ResetInternalVariables();
  void ReconstructChargedJets();
  AliVParticle* TagJetPartonDeltaR(fastjet::PseudoJet jet,Double_t radiusSq,Int_t &tag);
  AliVParticle* TagJetParticleSpecieDeltaR(fastjet::PseudoJet jet,Double_t radiusSq,Int_t &ntagPart,AliAnalysisTaskSEhfcjMCanalysis::ESpecieTags specie);
  AliVParticle* TagJetParticleDeltaR(fastjet::PseudoJet jet,Double_t radiusSq,Int_t &tag);
  void CalculateGeneralProperties();
  void SetBeautyAndCharmPartIndex();
  void CalculateChargedJetProperties();
  void FillHFCounterPtEtaSparse();
  void SetLightOutput(Bool_t lightOutput){fLightOutput=lightOutput;}

  AliMCEvent*              fMcEvent;    //! MC event
  AliInputEventHandler*    fMcHandler;  //! MCEventHandler
  Bool_t fLightOutput;           // flag to fill only light sparses
  Bool_t fFillHFJet;             // flag to fill HF jet histograms
  Bool_t fFillJetCorrelations;   // flag to fill HF jet correlation histograms
  TH1  *fHistEventsProcessed;   //! histo for monitoring the number of events processed slot 1
  TList       *fOutputQA; //! Output list
  TList       *fOutputList; //! Output list
  TH1 *fhCheckNotDorBdaughters; //! monitoring plot with pt distributions of physical primary particles not coming from D and B
  THnSparse *fhSparseHFCounterSignals; //! sparse for counting HF signals (pt, eta distr.)
  THnSparse *fhSparseJetProp; //! sparse for jet properties
  THnSparse *fhSparseJetPropExtended; //! sparse for jet properties with more info
  THnSparse *fhSparseFFstudy; //! sparse for FF studies
  THnSparse *fhSparseDJets; //! sparse for D in jet study
  THnSparse *fhSparseBJets; //! sparse for B in jet study
  THnSparse *fhSparseFeedDownDJets; //! sparse for D in jet study
  THnSparse *fhSparseEleJets;//! sparse for HFe-tagged jet study
  THnSparse *fhSparseDJetCorrel;//! sparse for prompt D- jet correlations
  THnSparse *fhSparseBJetCorrel;//! sparse for prompt B- jet correlations
  THnSparse *fhSparseFeedDownDJetCorrel;//! sparse for feed-down D-jet correlations
  THnSparse *fhSparseHFeJetCorrel;//! sparse for HFe-jet correlations
  TNtuple *fXsectionNtuple;//! array for storing event cross-sections
  Int_t feventcounter; //! internal event counter
  Float_t fEtaMin;   // minimum eta cut
  Float_t fEtaMax;   // maximum eta cut
  Float_t fYMin;     // minimum Y cut
  Float_t fYMax;     // maximum Y cut
  Float_t fPtMin;    // minimum Pt cut
  Float_t fPtMax;    // maximum Pt cut
  Int_t fMinMultiplicity; //Max Mult Limit
  Int_t fMaxMultiplicity; //Min Mult Limit
  TString  fCorrPart1;    //Particle 1 for Corr
  TString  fCorrPart2;    //Particle 2 for Corr
  Int_t fChargeSel1;      //Charge of Part1
  Int_t fChargeSel2;      //Charge of Part2
  AliStack* fStack; 
  Double_t fxsection;
  Double_t feventweight;
  Double_t feventtrials;
  TObjArray* fParticleArray;
  Int_t  fcounter;
  Bool_t fIsEventProp;
  Bool_t fIsPartProp;
  Bool_t fIsCorrOfQQbar;
  Bool_t fIsCorrOfHeavyFlavor;
  Bool_t fIsCorrOfHadronHadron;
  Bool_t fIsHFjetsAnalysis;
  Bool_t fSelectPhysPrimaryForJets; /// select physical primary particles only for jet recontruction 
  Bool_t fSelOnlyEMuPiKProtForJets; /// selct only electrons, muon, charged pions, charged kaons and protons for jet reconstruction
  Double_t fDCAmcSelEle;/// DCA selection applied on particle production radius for jet reconstruction and for keeping electrons gamma conversion
  Double_t fminptjettrack; //
  Double_t fmaxptjettrack; //
  Double_t fminetajettrack; //
  Double_t fmaxetajettrack; //
  Double_t fDetectorAcceptance; //
  Double_t fJetRcut; //
  Double_t fDeltaRSqforTagging; //

  TArrayI *fArraySkipDDaugh;//!
  TArrayI *fArrayTrk ;//!
  Int_t flastdaugh; //
  TArrayI *fArraybquark; //! internal array and indices (storing MC tagged particles in the event)
  Int_t flastbquark; //! internal array and indices (storing MC tagged particles in the event)
  TArrayI *fArraycquark; //! internal array and indices (storing MC tagged particles in the event)
  Int_t flastcquark; //! internal array and indices (storing MC tagged particles in the event)
  TArrayI *fArraybhadron; //! internal array and indices (storing MC tagged particles in the event)
  Int_t flastbhadron; //! internal array and indices (storing MC tagged particles in the event)
  TArrayI *fArraychadron; //! internal array and indices (storing MC tagged particles in the event)
  Int_t flastchadron; //! internal array and indices (storing MC tagged particles in the event)
  TArrayI *fArraychadronfromB; //! internal array and indices (storing MC tagged particles in the event)
  Int_t flastchadronfromB; //! internal array and indices (storing MC tagged particles in the event)
  TArrayI *fArraybele; //! internal array and indices (storing MC tagged particles in the event)
  Int_t flastbele; //! internal array and indices (storing MC tagged particles in the event)
  TArrayI *fArraycele; //! internal array and indices (storing MC tagged particles in the event)
  Int_t flastcele; //! internal array and indices (storing MC tagged particles in the event)
  TArrayI *fArraycelefromB; //! internal array and indices (storing MC tagged particles in the event)
  Int_t flastcelefromB; //! internal array and indices (storing MC tagged particles in the event)
  TArrayI *fArrayGammaele; //! internal array and indices (storing MC tagged particles in the event)
  Int_t flastGammaele; //! internal array and indices (storing MC tagged particles in the event)
  TArrayI *fArrayDalitzele; //! internal array and indices (storing MC tagged particles in the event)
  Int_t flastDalitzele; //! internal array and indices (storing MC tagged particles in the event)
  TArrayI *fArrayUnknownele; //! internal array and indices (storing MC tagged particles in the event)
  Int_t flastUnknownele; //! internal array and indices (storing MC tagged particles in the event)
  TArrayI *fTempTagParticle;//! internal array

  std::vector<fastjet::PseudoJet> fInputParticlesJet;//! vector of input particle for jet reco
  fastjet::JetDefinition *fJetDef;//!
  fastjet::GhostedAreaSpec *fGhostArea;//!
  fastjet::AreaDefinition *fAreaDef;//!
  fastjet::ClusterSequenceArea *fCSA;//!
  std::vector<fastjet::PseudoJet> fJets;//!

  TArrayI *fArrayTagJetQuark; //!
  TArrayI *fArrayTagJetParticle; //!
  ClassDef(AliAnalysisTaskSEhfcjMCanalysis,1)
}; //!

#endif
