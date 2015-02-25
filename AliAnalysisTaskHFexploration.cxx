//
// HF exploration analysis task.
//
// Author: S.Aiola

#include <TClonesArray.h>
#include <TObjArray.h>
#include <TH1F.h>
#include <TList.h>
#include <TLorentzVector.h>

#include "AliParticleContainer.h"
#include "AliMCParticle.h"
#include "AliLog.h"
#include "AliStack.h"

#include "AliAnalysisTaskHFexploration.h"

ClassImp(AliAnalysisTaskHFexploration)

//________________________________________________________________________
AliAnalysisTaskHFexploration::AliAnalysisTaskHFexploration() : 
AliAnalysisTaskEmcalJet("AliAnalysisTaskHFexploration", kTRUE),
  fStack(0),
  fHistParticlePt(0),
  fHistPionPt(0),
  fHistKaonPt(0),
  fHistElectronPt(0),
  fHistNonPhotonElecPt(0),
  fHistDmesonElecPt(0),
  fHistDmesonElecPtDeltaEta(0),
  fHistDmesonElecPtDeltaPhi(0),
  fHistCharmBaryonElecPt(0),
  fHistCharmBaryonElecPtDeltaEta(0),
  fHistCharmBaryonElecPtDeltaPhi(0),
  fHistDmesonPtDeltaEta(0),
  fHistDmesonPtDeltaPhi(0),
  fHistCharmBaryonPtDeltaEta(0),
  fHistCharmBaryonPtDeltaPhi(0),
  fHistInvariantMassPionKaonUS(0),
  fHistInvariantMassPionKaonLS(0),
  fHistBmesonElecPt(0),
  fHistBmesonElecPtDeltaEta(0),
  fHistBmesonElecPtDeltaPhi(0),
  fHistBottomBaryonElecPt(0),
  fHistBottomBaryonElecPtDeltaEta(0),
  fHistBottomBaryonElecPtDeltaPhi(0)
  
{
  // Default constructor.

  SetMakeGeneralHistograms(kFALSE);
}

//________________________________________________________________________
AliAnalysisTaskHFexploration::AliAnalysisTaskHFexploration(const char *name) : 
  AliAnalysisTaskEmcalJet(name, kTRUE),
  fStack(0),
  fHistParticlePt(0),
  fHistPionPt(0),
  fHistKaonPt(0),
  fHistElectronPt(0),
  fHistNonPhotonElecPt(0),
  fHistDmesonElecPt(0),
  fHistDmesonElecPtDeltaEta(0),
  fHistDmesonElecPtDeltaPhi(0),
  fHistCharmBaryonElecPt(0),
  fHistCharmBaryonElecPtDeltaEta(0),
  fHistCharmBaryonElecPtDeltaPhi(0),
  fHistDmesonPtDeltaEta(0),
  fHistDmesonPtDeltaPhi(0),
  fHistCharmBaryonPtDeltaEta(0),
  fHistCharmBaryonPtDeltaPhi(0),
  fHistInvariantMassPionKaonUS(0),
  fHistInvariantMassPionKaonLS(0),
  fHistBmesonElecPt(0),
  fHistBmesonElecPtDeltaEta(0),
  fHistBmesonElecPtDeltaPhi(0),
  fHistBottomBaryonElecPt(0),
  fHistBottomBaryonElecPtDeltaEta(0),
  fHistBottomBaryonElecPtDeltaPhi(0)
{
  // Standard constructor.

  SetMakeGeneralHistograms(kFALSE);
}

//________________________________________________________________________
void AliAnalysisTaskHFexploration::UserCreateOutputObjects()
{
  // Create user output.

  AliAnalysisTaskEmcalJet::UserCreateOutputObjects();

  TString histname;

  histname = "fHistParticlePt";
  fHistParticlePt = new TH1F(histname.Data(), histname.Data(), fNbins, fMinBinPt, fMaxBinPt);
  fHistParticlePt->GetXaxis()->SetTitle("#it{p}_{T} (GeV/#it{c})");
  fHistParticlePt->GetYaxis()->SetTitle("counts");
  fOutput->Add(fHistParticlePt);

  histname = "fHistPionPt";
  fHistPionPt = new TH1F(histname.Data(), histname.Data(), fNbins, fMinBinPt, fMaxBinPt);
  fHistPionPt->GetXaxis()->SetTitle("#it{p}_{T} (GeV/#it{c})");
  fHistPionPt->GetYaxis()->SetTitle("counts");
  fOutput->Add(fHistPionPt);

  histname = "fHistKaonPt";
  fHistKaonPt = new TH1F(histname.Data(), histname.Data(), fNbins, fMinBinPt, fMaxBinPt);
  fHistKaonPt->GetXaxis()->SetTitle("#it{p}_{T} (GeV/#it{c})");
  fHistKaonPt->GetYaxis()->SetTitle("counts");
  fOutput->Add(fHistKaonPt);

  histname = "fHistElectronPt";
  fHistElectronPt = new TH1F(histname.Data(), histname.Data(), fNbins, fMinBinPt, fMaxBinPt);
  fHistElectronPt->GetXaxis()->SetTitle("#it{p}_{T} (GeV/#it{c})");
  fHistElectronPt->GetYaxis()->SetTitle("counts");
  fOutput->Add(fHistElectronPt);

  histname = "fHistNonPhotonElecPt";
  fHistNonPhotonElecPt = new TH1F(histname.Data(), histname.Data(), fNbins, fMinBinPt, fMaxBinPt);
  fHistNonPhotonElecPt->GetXaxis()->SetTitle("#it{p}_{T} (GeV/#it{c})");
  fHistNonPhotonElecPt->GetYaxis()->SetTitle("counts");
  fOutput->Add(fHistNonPhotonElecPt);

  histname = "fHistDmesonElecPt";
  fHistDmesonElecPt = new TH2F(histname.Data(), histname.Data(), fNbins, fMinBinPt, fMaxBinPt, fNbins, fMinBinPt, fMaxBinPt);
  fHistDmesonElecPt->GetXaxis()->SetTitle("#it{p}_{T}^{el} (GeV/#it{c})");
  fHistDmesonElecPt->GetYaxis()->SetTitle("#it{p}_{T}^{D} (GeV/#it{c})");
  fHistDmesonElecPt->GetZaxis()->SetTitle("counts");
  fOutput->Add(fHistDmesonElecPt);

  histname = "fHistDmesonElecPtDeltaEta";
  fHistDmesonElecPtDeltaEta = new TH2F(histname.Data(), histname.Data(), fNbins, fMinBinPt, fMaxBinPt, 200, 0, 2);
  fHistDmesonElecPtDeltaEta->GetXaxis()->SetTitle("#it{p}_{T}^{el} (GeV/#it{c})");
  fHistDmesonElecPtDeltaEta->GetYaxis()->SetTitle("|#Delta#eta_{el-D}|");
  fHistDmesonElecPtDeltaEta->GetZaxis()->SetTitle("counts");
  fOutput->Add(fHistDmesonElecPtDeltaEta);

  histname = "fHistDmesonElecPtDeltaPhi";
  fHistDmesonElecPtDeltaPhi = new TH2F(histname.Data(), histname.Data(), fNbins, fMinBinPt, fMaxBinPt, 100, 0, TMath::Pi());
  fHistDmesonElecPtDeltaPhi->GetXaxis()->SetTitle("#it{p}_{T}^{el} (GeV/#it{c})");
  fHistDmesonElecPtDeltaPhi->GetYaxis()->SetTitle("#Delta#phi_{el-D}");
  fHistDmesonElecPtDeltaPhi->GetZaxis()->SetTitle("counts");
  fOutput->Add(fHistDmesonElecPtDeltaPhi);

  histname = "fHistCharmBaryonElecPt";
  fHistCharmBaryonElecPt = new TH2F(histname.Data(), histname.Data(), fNbins, fMinBinPt, fMaxBinPt, fNbins, fMinBinPt, fMaxBinPt);
  fHistCharmBaryonElecPt->GetXaxis()->SetTitle("#it{p}_{T}^{el} (GeV/#it{c})");
  fHistCharmBaryonElecPt->GetYaxis()->SetTitle("#it{p}_{T}^{c-baryon} (GeV/#it{c})");
  fHistCharmBaryonElecPt->GetZaxis()->SetTitle("counts");
  fOutput->Add(fHistCharmBaryonElecPt);

  histname = "fHistCharmBaryonElecPtDeltaEta";
  fHistCharmBaryonElecPtDeltaEta = new TH2F(histname.Data(), histname.Data(), fNbins, fMinBinPt, fMaxBinPt, 200, 0, 2);
  fHistCharmBaryonElecPtDeltaEta->GetXaxis()->SetTitle("#it{p}_{T}^{el} (GeV/#it{c})");
  fHistCharmBaryonElecPtDeltaEta->GetYaxis()->SetTitle("|#Delta#eta_{el-c-baryon}|");
  fHistCharmBaryonElecPtDeltaEta->GetZaxis()->SetTitle("counts");
  fOutput->Add(fHistCharmBaryonElecPtDeltaEta);

  histname = "fHistCharmBaryonElecPtDeltaPhi";
  fHistCharmBaryonElecPtDeltaPhi = new TH2F(histname.Data(), histname.Data(), fNbins, fMinBinPt, fMaxBinPt, 100, 0, TMath::Pi());
  fHistCharmBaryonElecPtDeltaPhi->GetXaxis()->SetTitle("#it{p}_{T}^{el} (GeV/#it{c})");
  fHistCharmBaryonElecPtDeltaPhi->GetYaxis()->SetTitle("#Delta#phi_{el-c-baryon}");
  fHistCharmBaryonElecPtDeltaPhi->GetZaxis()->SetTitle("counts");
  fOutput->Add(fHistCharmBaryonElecPtDeltaPhi);

  histname = "fHistDmesonPtDeltaEta";
  fHistDmesonPtDeltaEta = new TH2F(histname.Data(), histname.Data(), fNbins, fMinBinPt, fMaxBinPt, 200, 0, 2);
  fHistDmesonPtDeltaEta->GetXaxis()->SetTitle("#it{p}_{T}^{D} (GeV/#it{c})");
  fHistDmesonPtDeltaEta->GetYaxis()->SetTitle("|#Delta#eta_{el-D}|");
  fHistDmesonPtDeltaEta->GetZaxis()->SetTitle("counts");
  fOutput->Add(fHistDmesonPtDeltaEta);

  histname = "fHistDmesonPtDeltaPhi";
  fHistDmesonPtDeltaPhi = new TH2F(histname.Data(), histname.Data(), fNbins, fMinBinPt, fMaxBinPt, 100, 0, TMath::Pi());
  fHistDmesonPtDeltaPhi->GetXaxis()->SetTitle("#it{p}_{T}^{D} (GeV/#it{c})");
  fHistDmesonPtDeltaPhi->GetYaxis()->SetTitle("#Delta#phi_{el-D}");
  fHistDmesonPtDeltaPhi->GetZaxis()->SetTitle("counts");
  fOutput->Add(fHistDmesonPtDeltaPhi);

  histname = "fHistCharmBaryonPtDeltaEta";
  fHistCharmBaryonPtDeltaEta = new TH2F(histname.Data(), histname.Data(), fNbins, fMinBinPt, fMaxBinPt, 200, 0, 2);
  fHistCharmBaryonPtDeltaEta->GetXaxis()->SetTitle("#it{p}_{T}^{c-baryon} (GeV/#it{c})");
  fHistCharmBaryonPtDeltaEta->GetYaxis()->SetTitle("|#Delta#eta_{el-c-baryon}|");
  fHistCharmBaryonPtDeltaEta->GetZaxis()->SetTitle("counts");
  fOutput->Add(fHistCharmBaryonPtDeltaEta);

  histname = "fHistCharmBaryonPtDeltaPhi";
  fHistCharmBaryonPtDeltaPhi = new TH2F(histname.Data(), histname.Data(), fNbins, fMinBinPt, fMaxBinPt, 100, 0, TMath::Pi());
  fHistCharmBaryonPtDeltaPhi->GetXaxis()->SetTitle("#it{p}_{T}^{c-baryon} (GeV/#it{c})");
  fHistCharmBaryonPtDeltaPhi->GetYaxis()->SetTitle("#Delta#phi_{el-c-baryon}");
  fHistCharmBaryonPtDeltaPhi->GetZaxis()->SetTitle("counts");
  fOutput->Add(fHistCharmBaryonPtDeltaPhi);

  histname = "fHistInvariantMassPionKaonUS";
  fHistInvariantMassPionKaonUS = new TH2F(histname.Data(), histname.Data(), fNbins, fMinBinPt, fMaxBinPt, 100, 0, 10);
  fHistInvariantMassPionKaonUS->GetXaxis()->SetTitle("#it{p}_{T}^{K#pi} (GeV/#it{c})");
  fHistInvariantMassPionKaonUS->GetYaxis()->SetTitle("M(K#pi) (GeV/#it{c}^{2})");
  fHistInvariantMassPionKaonUS->GetZaxis()->SetTitle("counts");
  fOutput->Add(fHistInvariantMassPionKaonUS);

  histname = "fHistInvariantMassPionKaonLS";
  fHistInvariantMassPionKaonLS = new TH2F(histname.Data(), histname.Data(), fNbins, fMinBinPt, fMaxBinPt, 100, 0, 10);
  fHistInvariantMassPionKaonLS->GetXaxis()->SetTitle("#it{p}_{T}^{K#pi} (GeV/#it{c})");
  fHistInvariantMassPionKaonLS->GetYaxis()->SetTitle("M(K#pi) (GeV/#it{c}^{2})");
  fHistInvariantMassPionKaonLS->GetZaxis()->SetTitle("counts");
  fOutput->Add(fHistInvariantMassPionKaonLS);

  histname = "fHistBmesonElecPt";
  fHistBmesonElecPt = new TH2F(histname.Data(), histname.Data(), fNbins, fMinBinPt, fMaxBinPt, fNbins, fMinBinPt, fMaxBinPt);
  fHistBmesonElecPt->GetXaxis()->SetTitle("#it{p}_{T}^{el} (GeV/#it{c})");
  fHistBmesonElecPt->GetYaxis()->SetTitle("#it{p}_{T}^{B} (GeV/#it{c})");
  fHistBmesonElecPt->GetZaxis()->SetTitle("counts");
  fOutput->Add(fHistBmesonElecPt);

  histname = "fHistBmesonElecPtDeltaEta";
  fHistBmesonElecPtDeltaEta = new TH2F(histname.Data(), histname.Data(), fNbins, fMinBinPt, fMaxBinPt, 200, 0, 2);
  fHistBmesonElecPtDeltaEta->GetXaxis()->SetTitle("#it{p}_{T}^{el} (GeV/#it{c})");
  fHistBmesonElecPtDeltaEta->GetYaxis()->SetTitle("|#Delta#eta_{el-B}|");
  fHistBmesonElecPtDeltaEta->GetZaxis()->SetTitle("counts");
  fOutput->Add(fHistBmesonElecPtDeltaEta);

  histname = "fHistBmesonElecPtDeltaPhi";
  fHistBmesonElecPtDeltaPhi = new TH2F(histname.Data(), histname.Data(), fNbins, fMinBinPt, fMaxBinPt, 100, 0, TMath::Pi());
  fHistBmesonElecPtDeltaPhi->GetXaxis()->SetTitle("#it{p}_{T}^{el} (GeV/#it{c})");
  fHistBmesonElecPtDeltaPhi->GetYaxis()->SetTitle("#Delta#phi_{el-B}");
  fHistBmesonElecPtDeltaPhi->GetZaxis()->SetTitle("counts");
  fOutput->Add(fHistBmesonElecPtDeltaPhi);

  histname = "fHistBottomBaryonElecPt";
  fHistBottomBaryonElecPt = new TH2F(histname.Data(), histname.Data(), fNbins, fMinBinPt, fMaxBinPt, fNbins, fMinBinPt, fMaxBinPt);
  fHistBottomBaryonElecPt->GetXaxis()->SetTitle("#it{p}_{T}^{el} (GeV/#it{c})");
  fHistBottomBaryonElecPt->GetYaxis()->SetTitle("#it{p}_{T}^{b-baryon} (GeV/#it{c})");
  fHistBottomBaryonElecPt->GetZaxis()->SetTitle("counts");
  fOutput->Add(fHistBottomBaryonElecPt);

  histname = "fHistBottomBaryonElecPtDeltaEta";
  fHistBottomBaryonElecPtDeltaEta = new TH2F(histname.Data(), histname.Data(), fNbins, fMinBinPt, fMaxBinPt, 200, 0, 2);
  fHistBottomBaryonElecPtDeltaEta->GetXaxis()->SetTitle("#it{p}_{T}^{el} (GeV/#it{c})");
  fHistBottomBaryonElecPtDeltaEta->GetYaxis()->SetTitle("|#Delta#eta_{el-b-baryon}|");
  fHistBottomBaryonElecPtDeltaEta->GetZaxis()->SetTitle("counts");
  fOutput->Add(fHistBottomBaryonElecPtDeltaEta);

  histname = "fHistBottomBaryonElecPtDeltaPhi";
  fHistBottomBaryonElecPtDeltaPhi = new TH2F(histname.Data(), histname.Data(), fNbins, fMinBinPt, fMaxBinPt, 100, 0, TMath::Pi());
  fHistBottomBaryonElecPtDeltaPhi->GetXaxis()->SetTitle("#it{p}_{T}^{el} (GeV/#it{c})");
  fHistBottomBaryonElecPtDeltaPhi->GetYaxis()->SetTitle("#Delta#phi_{el-b-baryon}");
  fHistBottomBaryonElecPtDeltaPhi->GetZaxis()->SetTitle("counts");
  fOutput->Add(fHistBottomBaryonElecPtDeltaPhi);

  PostData(1, fOutput); // Post data for ALL output slots >0 here, to get at least an empty histogram
}

//________________________________________________________________________
void AliAnalysisTaskHFexploration::ExecOnce()
{
  fStack = dynamic_cast<AliStack*>(InputEvent()->FindListObject("AliStack"));
  if (!fStack) {
    AliFatal("Could not get AliStack!");
  }

  AliAnalysisTaskEmcalJet::ExecOnce();
}

//________________________________________________________________________
Bool_t AliAnalysisTaskHFexploration::FillHistograms()
{
  // Fill histograms.

  AliParticleContainer *partCont = static_cast<AliParticleContainer*>(fParticleCollArray.At(0));
  if (!partCont) {
    AliError(Form("%s - Track array not provided, returning...", GetName()));
    return kFALSE;
  }

  TClonesArray *partArray = partCont->GetArray();

  for (Int_t i = 0; i < partArray->GetEntriesFast(); i++){
    AliMCParticle *part = static_cast<AliMCParticle*>(partArray->At(i));
    if (!part) continue;

    if (!fStack->IsPhysicalPrimary(i)) continue;

    fHistParticlePt->Fill(part->Pt());

    Int_t pdg = TMath::Abs(part->PdgCode());
    if (pdg == 211) { // pi+ / pi-
      fHistPionPt->Fill(part->Pt());
    }
    else if (pdg == 321) { // K+ / K-
      fHistKaonPt->Fill(part->Pt());

      for (Int_t j = 0; j < partArray->GetEntriesFast(); j++) {
	AliMCParticle *part2 = static_cast<AliMCParticle*>(partArray->At(j));
	if (!part2) continue;

	if (!fStack->IsPhysicalPrimary(j)) continue;

	Int_t pdg2 = TMath::Abs(part2->PdgCode());

	if (pdg2 == 211) { // pi+ / pi-
	  Double_t invM = (part->E()+part2->E())*(part->E()+part2->E()) - 
	    (part->Px()+part2->Px())*(part->Px()+part2->Px()) - 
	    (part->Py()+part2->Py())*(part->Py()+part2->Py()) -
	    (part->Pz()+part2->Pz())*(part->Pz()+part2->Pz());
	  Double_t pT = TMath::Sqrt((part->Px()+part2->Px())*(part->Px()+part2->Px()) + 
				    (part->Py()+part2->Py())*(part->Py()+part2->Py()));
	  if (part->Charge() * part2->Charge() < 0)
	    fHistInvariantMassPionKaonUS->Fill(pT, invM);
	  else
	    fHistInvariantMassPionKaonLS->Fill(pT, invM);
	}
      }
    }
    else if (pdg == 11) { // electron
      fHistElectronPt->Fill(part->Pt());

      Int_t motherId = part->GetMother();
      if (motherId < 0 || motherId > fStack->GetNprimary()) continue;

      TParticle *mother = fStack->Particle(motherId);

      if (!mother) continue;

      Int_t motherPdg = TMath::Abs(mother->GetPdgCode());
      
      if (motherPdg != 22  &&   // photon
	  motherPdg != 111 &&   // pi0
	  motherPdg != 211 &&   // pi+
	  motherPdg != 221 &&   // eta
	  motherPdg != 331 &&   // eta'
	  motherPdg != 113 &&   // rho
	  motherPdg != 223 &&   // omega
	  motherPdg != 333  ) { // phi
	fHistNonPhotonElecPt->Fill(part->Pt());
	if (motherPdg == 411  ||  // D+
	    motherPdg == 421  ||  // D0
	    motherPdg == 431  ) { // D+s
	  fHistDmesonElecPt->Fill(part->Pt(), mother->Pt());
	  fHistDmesonElecPtDeltaEta->Fill(part->Pt(), TMath::Abs(mother->Eta() - part->Eta()));
	  fHistDmesonPtDeltaEta->Fill(mother->Pt(), TMath::Abs(mother->Eta() - part->Eta()));
	  Double_t dphi = TMath::Abs(mother->Phi() - part->Phi());
	  while (dphi > TMath::Pi()) dphi -= TMath::Pi();
	  fHistDmesonElecPtDeltaPhi->Fill(part->Pt(), dphi);
	  fHistDmesonPtDeltaPhi->Fill(mother->Pt(), dphi);
	}
	else if (motherPdg == 4122 ||  // Lambda c +
		 motherPdg == 4332 ||  // Omega c 0
		 motherPdg == 4132 ||  // Sigma c 0
		 motherPdg == 4232 ) { // Sigma c +
	  fHistCharmBaryonElecPt->Fill(part->Pt(), mother->Pt());
	  fHistCharmBaryonElecPtDeltaEta->Fill(part->Pt(), TMath::Abs(mother->Eta() - part->Eta()));
	  fHistCharmBaryonPtDeltaEta->Fill(mother->Pt(), TMath::Abs(mother->Eta() - part->Eta()));
	  Double_t dphi = TMath::Abs(mother->Phi() - part->Phi());
	  while (dphi > TMath::Pi()) dphi -= TMath::Pi();
	  fHistCharmBaryonElecPtDeltaPhi->Fill(part->Pt(), dphi);
	  fHistCharmBaryonPtDeltaPhi->Fill(mother->Pt(), dphi);
	}
	else if (motherPdg == 511 ||   // B0
		 motherPdg == 521 ||   // B+
		 motherPdg == 531  ) { // B0s
	  fHistBmesonElecPt->Fill(part->Pt(), mother->Pt());
	  fHistBmesonElecPtDeltaEta->Fill(part->Pt(), TMath::Abs(mother->Eta() - part->Eta()));
	  Double_t dphi = TMath::Abs(mother->Phi() - part->Phi());
	  while (dphi > TMath::Pi()) dphi -= TMath::Pi();
	  fHistBmesonElecPtDeltaPhi->Fill(part->Pt(), dphi);
	}
	else if	(motherPdg == 5122 ||   // Lamba b 0
		 motherPdg == 5132  ) { // Sigma b -
	  fHistBottomBaryonElecPt->Fill(part->Pt(), mother->Pt());
	  fHistBottomBaryonElecPtDeltaEta->Fill(part->Pt(), TMath::Abs(mother->Eta() - part->Eta()));
	  Double_t dphi = TMath::Abs(mother->Phi() - part->Phi());
	  while (dphi > TMath::Pi()) dphi -= TMath::Pi();
	  fHistBottomBaryonElecPtDeltaPhi->Fill(part->Pt(), dphi);
	}
	else if (motherPdg != 15  &&   // tau
		 motherPdg != 443  ) { // j/psi
	  Printf("pdg = %d", motherPdg);
	}
      } 
    }
  } 

  return kTRUE;
}
