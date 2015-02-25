#ifndef ALIANALYSISTASKHFEXPLORATION_H
#define ALIANALYSISTASKHFEXPLORATION_H

class TH1;
class TH3;
class AliStack;

#include "AliAnalysisTaskEmcalJet.h"

class AliAnalysisTaskHFexploration : public AliAnalysisTaskEmcalJet {
 public:

  AliAnalysisTaskHFexploration();
  AliAnalysisTaskHFexploration(const char *name);
  virtual ~AliAnalysisTaskHFexploration() {;}

  void                        UserCreateOutputObjects();

 protected:

  Bool_t                      FillHistograms();
  void                        ExecOnce();

  AliStack                   *fStack;                           //!Particle stack
  TH1                        *fHistParticlePt;                  //!Inclusive primary particle pt spectrum
  TH1                        *fHistPionPt;                      //!Inclusive pion pt spectrum
  TH1                        *fHistKaonPt;                      //!Inclusive kaon pt spectrum
  TH1                        *fHistElectronPt;                  //!Inclusive electron pt spectrum
  TH1                        *fHistNonPhotonElecPt;             //!Non-photonic electron pt spectrum
  TH2                        *fHistDmesonElecPt;                //!Electron pt vs. mother d meson pt
  TH2                        *fHistDmesonElecPtDeltaEta;        //!Electron pt vs. mother d meson delta eta
  TH2                        *fHistDmesonElecPtDeltaPhi;        //!Electron pt vs. mother d meson delta phi
  TH2                        *fHistCharmBaryonElecPt;           //!Electron pt vs. mother charm baryon pt
  TH2                        *fHistCharmBaryonElecPtDeltaEta;   //!Electron pt vs. mother charm baryon delta eta
  TH2                        *fHistCharmBaryonElecPtDeltaPhi;   //!Electron pt vs. mother charm baryon delta phi
  TH2                        *fHistDmesonPtDeltaEta;            //!Meson pt vs. mother d meson delta eta
  TH2                        *fHistDmesonPtDeltaPhi;            //!Meson pt vs. mother d meson delta phi
  TH2                        *fHistCharmBaryonPtDeltaEta;       //!Charm baryon pt vs. mother charm baryon delta eta
  TH2                        *fHistCharmBaryonPtDeltaPhi;       //!Charm baryon pt vs. mother charm baryon delta phi
  TH2                        *fHistInvariantMassPionKaonUS;     //!Invariant mass of pion-kaon pairs - unlike signs
  TH2                        *fHistInvariantMassPionKaonLS;     //!Invariant mass of pion-kaon pairs - like signs
  TH2                        *fHistBmesonElecPt;                //!Electron pt vs. mother b meson pt
  TH2                        *fHistBmesonElecPtDeltaEta;        //!Electron pt vs. mother b meson delta eta
  TH2                        *fHistBmesonElecPtDeltaPhi;        //!Electron pt vs. mother b meson delta phi
  TH2                        *fHistBottomBaryonElecPt;          //!Electron pt vs. mother bottom baryon pt
  TH2                        *fHistBottomBaryonElecPtDeltaEta;  //!Electron pt vs. mother bottom baryon delta eta
  TH2                        *fHistBottomBaryonElecPtDeltaPhi;  //!Electron pt vs. mother bottom baryon delta phi

 private:
  AliAnalysisTaskHFexploration(const AliAnalysisTaskHFexploration&);            // not implemented
  AliAnalysisTaskHFexploration &operator=(const AliAnalysisTaskHFexploration&); // not implemented

  ClassDef(AliAnalysisTaskHFexploration, 1) // HF exploration task
};
#endif
