/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

// Analysis task for net-lambda fluctuations analysis
// Author: Alice Ohlson (alice.ohlson@cern.ch)

// aliroot
#include "AliAnalysisManager.h"
#include "AliInputEventHandler.h"
#include "AliESDEvent.h"
#include "AliAODEvent.h"
#include "AliMCEvent.h"
#include "AliAODTrack.h"
#include "AliESDtrack.h"
#include "AliExternalTrackParam.h"
#include "AliAnalysisFilter.h"
#include "AliVMultiplicity.h"
#include "AliAnalysisUtils.h"
#include "AliAODMCParticle.h"
#include "AliStack.h"
#include "AliPIDResponse.h"
#include "AliMCEventHandler.h"
#include "AliV0vertexer.h"
#include "AliESDv0Cuts.h"
#include "AliMultSelection.h"
// root
#include "TMath.h"
#include "TFile.h"
#include "TList.h"
#include "TChain.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TH3F.h"

//#include "AliNetLambdaHelper.h"
#include "AliAnalysisTaskNetLambdaIdent.h"

ClassImp(AliAnalysisTaskNetLambdaIdent)

//-----------------------------------------------------------------------------
AliAnalysisTaskNetLambdaIdent::AliAnalysisTaskNetLambdaIdent(const char* name) :
AliAnalysisTaskSE(name),
    fESD(0x0),
    fAOD(0x0),
    fPIDResponse(0x0),
    fEventCuts(0),
    fListOfHistos(0x0),
    fTree(0x0),
    hEventStatistics(0x0),
    hGenPt(0x0),
    hGenPhi(0x0),
    hGenEta(0x0),
    hTrackPt(0x0),
    hTrackPhi(0x0),
    hTrackEta(0x0),
    hNSigmaProton(0x0),
    hArmPod(0x0),
    hLambdaPtGen(0x0),
    hAntiLambdaPtGen(0x0),
    hLambdaPtReco(0x0),
    hAntiLambdaPtReco(0x0),
    hInvMassLambda(0x0),
    hInvMassAntiLambda(0x0),
    hInvMassLambdaOnTheFly(0x0),
    hInvMassAntiLambdaOnTheFly(0x0),
    hInvMassLambdaReco(0x0),
    hInvMassAntiLambdaReco(0x0),
    hInvMassLambdaSecFromMaterial(0x0),
    hInvMassAntiLambdaSecFromMaterial(0x0),
    hInvMassLambdaSecFromWeakDecay(0x0),
    hInvMassAntiLambdaSecFromWeakDecay(0x0),
    hInvMassLike(0x0),
//hInvMassPtPidLambda(0x0),
//hInvMassPtPidAntiLambda(0x0),
    hXiPlus(0x0),
    hXiMinus(0x0),
    hXiZero(0x0),
    hXiZeroAnti(0x0),
    hPtResLambda(0x0),
    hPtResAntiLambda(0x0),
    hPtResLambdaPrim(0x0),
    hPtResAntiLambdaPrim(0x0),
    centcut(80.),
    ptminlambda(0.5),
    etacutlambda(0.8),
    cospacut(0.95),
    minradius(5.),
    ncrossedrows(70),
    crossedrowsclustercut(0.8),
    fCent(-1),
    fNSpdTracklets(-1),
    fVtxZ(-20),
    fRunNumber(-1),
    fAcceptV0(0x0),
    fGenLambda(0x0),
    fGenCascade(0x0),
    fIsMC(kFALSE),
    fIsAOD(kTRUE),
    fEvSel(AliVEvent::kINT7),
    fK0STree(kFALSE),
    fLambdaTree(kTRUE)
{
  Info("AliAnalysisTaskNetLambdaIdent","Calling Constructor");
  DefineInput(0,TChain::Class());
  DefineOutput(1,TList::Class());
  DefineOutput(2,TTree::Class());
  
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void AliAnalysisTaskNetLambdaIdent::UserCreateOutputObjects(){
  fListOfHistos = new TList();
  fListOfHistos->SetOwner();

  hEventStatistics = new TH1I("hEventStatistics","",20,0,20);
  //hEventStatistics->SetBit(TH1::kCanRebin);
  fListOfHistos->Add(hEventStatistics);
  
  // single-track QA plots
  hTrackPt = new TH1F("hTrackPt","track p_{T};p_{T} (GeV/c);",100,0,10);
  fListOfHistos->Add(hTrackPt);
  hTrackPhi = new TH1F("hTrackPhi","track #varphi;#varphi;",100,0,2*TMath::Pi());
  fListOfHistos->Add(hTrackPhi);
  hTrackEta = new TH1F("hTrackEta","track #eta;#eta;",100,-0.8,0.8);
  fListOfHistos->Add(hTrackEta);
  hGenPt = new TH1F("hGenPt","generated p_{T};p_{T} (GeV/c);",100,0,10);
  fListOfHistos->Add(hGenPt);
  hGenPhi = new TH1F("hGenPhi","generated #varphi;#varphi;",100,0,2*TMath::Pi());
  fListOfHistos->Add(hGenPhi);
  hGenEta = new TH1F("hGenEta","generated #eta;#eta;",100,-0.8,0.8);
  fListOfHistos->Add(hGenEta);

  // PID QA
  hNSigmaProton = new TH2F("hNSigmaProton","n#sigma for protons;p_{T};n#sigma",100,-10,10,100,-10,10);
  fListOfHistos->Add(hNSigmaProton);
  hArmPod = new TH2F("hArmPod","Armenteros-Podolanski plot;#alpha;p_{T}",100,-1,1,100,0,0.25);
  fListOfHistos->Add(hArmPod);
  
  // lambda reco plots
  hLambdaPtGen = new TH1F("hLambdaPtGen","generated #Lambda p_{T}",100,0,10);
  fListOfHistos->Add(hLambdaPtGen);
  hAntiLambdaPtGen = new TH1F("hAntiLambdaPtGen","generated #bar{#Lambda} p_{T}",100,0,10);
  fListOfHistos->Add(hAntiLambdaPtGen);

  hLambdaPtReco = new TH1F("hLambdaPtReco","reconstructed #Lambda p_{T}",100,0,10);
  fListOfHistos->Add(hLambdaPtReco);
  hAntiLambdaPtReco = new TH1F("hAntiLambdaPtReco","reconstructed #bar{#Lambda} p_{T}",100,0,10);
  fListOfHistos->Add(hAntiLambdaPtReco);
  
  hInvMassLambda = new TH2F("hInvMassLambda","#Lambda invariant mass",100,1.05,1.25,100,0,10);
  fListOfHistos->Add(hInvMassLambda);
  hInvMassAntiLambda = new TH2F("hInvMassAntiLambda","#bar{#Lambda} invariant mass",100,1.05,1.25,100,0,10);
  fListOfHistos->Add(hInvMassAntiLambda);

  hInvMassLambdaOnTheFly = new TH2F("hInvMassLambdaOnTheFly","#Lambda invariant mass -- on-the-fly V0 finder",100,1.05,1.25,100,0,10);
  fListOfHistos->Add(hInvMassLambdaOnTheFly);
  hInvMassAntiLambdaOnTheFly = new TH2F("hInvMassAntiLambdaOnTheFly","#bar{#Lambda} invariant mass -- on-the-fly V0 finder",100,1.05,1.25,100,0,10);
  fListOfHistos->Add(hInvMassAntiLambdaOnTheFly);

  hInvMassLambdaReco = new TH2F("hInvMassLambdaReco","#Lambda invariant mass",100,1.05,1.25,100,0,10);
  fListOfHistos->Add(hInvMassLambdaReco);
  hInvMassAntiLambdaReco = new TH2F("hInvMassAntiLambdaReco","#bar{#Lambda} invariant mass",100,1.05,1.25,100,0,10);
  fListOfHistos->Add(hInvMassAntiLambdaReco);

  hInvMassLambdaSecFromMaterial = new TH2F("hInvMassLambdaSecFromMaterial","#Lambda from material",100,1.05,1.25,100,0,10);
  fListOfHistos->Add(hInvMassLambdaSecFromMaterial);
  hInvMassAntiLambdaSecFromMaterial = new TH2F("hInvMassAntiLambdaSecFromMaterial","#bar{#Lambda} from material",100,1.05,1.25,100,0,10);
  fListOfHistos->Add(hInvMassAntiLambdaSecFromMaterial);
  hInvMassLambdaSecFromWeakDecay = new TH2F("hInvMassLambdaSecFromWeakDecay","#Lambda from weak decays",100,1.05,1.25,100,0,10);
  fListOfHistos->Add(hInvMassLambdaSecFromWeakDecay);
  hInvMassAntiLambdaSecFromWeakDecay = new TH2F("hInvMassAntiLambdaSecFromWeakDecay","#bar{#Lambda} from weak decays",100,1.05,1.25,100,0,10);
  fListOfHistos->Add(hInvMassAntiLambdaSecFromWeakDecay);

  hInvMassLike = new TH2F("hInvMassLike","like-sign pair invariant mass",100,1.05,1.25,100,0,10);
  fListOfHistos->Add(hInvMassLike);

  //hInvMassPtPidLambda = new TH3F("hInvMassPtPidLambda","inv mass of lambda candidates",100,1.08,1.15,17,0.6,4,14,-0.5,13.5);
  //fListOfHistos->Add(hInvMassPtPidLambda);
  //hInvMassPtPidAntiLambda = new TH3F("hInvMassPtPidAntiLambda","inv mass of anti-lambda candidates",100,1.08,1.15,17,0.6,4,14,-0.5,13.5);
  //fListOfHistos->Add(hInvMassPtPidAntiLambda);

  hXiPlus = new TH3F("hXiPlus","Xi+ vs pt, eta, centrality",100,0,20,20,-1,1,80,0,80);
  fListOfHistos->Add(hXiPlus);
  hXiMinus = new TH3F("hXiMinus","Xi- vs pt, eta, centrality",100,0,20,20,-1,1,80,0,80);
  fListOfHistos->Add(hXiMinus);
  hXiZero = new TH3F("hXiZero","Xi0 vs pt, eta, centrality",100,0,20,20,-1,1,80,0,80);
  fListOfHistos->Add(hXiZero);
  hXiZeroAnti = new TH3F("hXiZeroAnti","anti-Xi0 vs pt, eta, centrality",100,0,20,20,-1,1,80,0,80);
  fListOfHistos->Add(hXiZeroAnti);

  hPtResLambda = new TH2F("hPtResLambda","#Lambda pt resolution;gen p_{T};reco p_{T}",100,0,10,100,0,10);
  fListOfHistos->Add(hPtResLambda);
  hPtResAntiLambda = new TH2F("hPtResAntiLambda","#bar{#Lambda} pt resolution;gen p_{T};reco p_{T}",100,0,10,100,0,10);
  fListOfHistos->Add(hPtResAntiLambda);
  hPtResLambdaPrim = new TH2F("hPtResLambdaPrim","primary #Lambda pt resolution;gen p_{T};reco p_{T}",100,0,10,100,0,10);
  fListOfHistos->Add(hPtResLambdaPrim);
  hPtResAntiLambdaPrim = new TH2F("hPtResAntiLambdaPrim","primary #bar{#Lambda} pt resolution;gen p_{T};reco p_{T}",100,0,10,100,0,10);
  fListOfHistos->Add(hPtResAntiLambdaPrim);
  
  fEventCuts.AddQAplotsToList(fListOfHistos);  
  
  fAcceptV0 = new TClonesArray("AliLightV0",1000);
  if(fIsMC)
    {
      fGenLambda = new TClonesArray("AliLightGenV0",1000);
      fGenCascade = new TClonesArray("AliLightGenV0",1000);
    }
  
  // create file-resident tree
  TDirectory *owd = gDirectory;
  OpenFile(2);
  fTree = new TTree("events","events");
  owd->cd();
  fTree->Branch("fCent",&fCent);
  fTree->Branch("fNSpdTracklets",&fNSpdTracklets);
  fTree->Branch("fVtxZ",&fVtxZ);
  fTree->Branch("fRunNumber",&fRunNumber);
  fTree->Branch("fAcceptV0",&fAcceptV0);
  if(fIsMC)
    {
      fTree->Branch("fGenLambda",&fGenLambda);
      fTree->Branch("fGenCascade",&fGenCascade);
    }
  
  //fUtils = new AliAnalysisUtils();
  //fUtils->SetMinPlpContribSPD(3);
  //fUtils->SetMinPlpContribMV(3);

  PostData(1,fListOfHistos);
  PostData(2,fTree);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void AliAnalysisTaskNetLambdaIdent::UserExec(Option_t *){
  
  hEventStatistics->Fill("before cuts",1);
  
  if (!fInputEvent) return;
  hEventStatistics->Fill("after event check",1);

  if(fIsAOD) // aod
    {
      fAOD = dynamic_cast<AliAODEvent*>(fInputEvent);
      if (!fAOD) return;
      hEventStatistics->Fill("after aod check",1);
    }
  else // esd
    {
      fESD = dynamic_cast<AliESDEvent*>(InputEvent());
      if (!fESD) return;
      hEventStatistics->Fill("after esd check",1);
    }

  fPIDResponse = fInputHandler->GetPIDResponse();
  if(!fPIDResponse) return;
  hEventStatistics->Fill("after pid check",1);

  AliStack *stack = 0x0;
  if(fIsMC)
    {
      if(!fIsAOD) // esd
	{
	  AliMCEventHandler *mcH = dynamic_cast<AliMCEventHandler*>((AliAnalysisManager::GetAnalysisManager())->GetMCtruthEventHandler());
	  if(!mcH) return;
	  hEventStatistics->Fill("found MC handler",1);
	  fMCEvent=mcH->MCEvent();
	  //fMCEvent = dynamic_cast<AliMCEvent*>(MCEvent());
	}
      
      if(!fMCEvent) return;
      hEventStatistics->Fill("found fMCEvent",1);

      if(!fIsAOD) //esd
	{
	  stack = fMCEvent->Stack();
	  if(!stack) return;
	  hEventStatistics->Fill("found MC stack",1);
	}
    }

  AliMultSelection *MultSelection = (AliMultSelection*) fInputEvent->FindListObject("MultSelection");
  if(!MultSelection) return;
  hEventStatistics->Fill("found MultSelection object",1);
  
  if(!(fInputHandler->IsEventSelected() & fEvSel)) return;
  hEventStatistics->Fill("physics selection",1);

  Double_t fVtx[3];
  /*if(fIsAOD) // aod
    {
      AliAODVertex *aodVertex = (AliAODVertex*)fInputEvent->GetPrimaryVertex();
      if (!aodVertex) return;
      fVtx[0] = aodVertex->GetX();
      fVtx[1] = aodVertex->GetY();
      fVtx[2] = aodVertex->GetZ();
    }
  else // esd
    {
      AliESDVertex *esdVertex = (AliESDVertex*)fInputEvent->GetPrimaryVertex();
      if (!esdVertex) return;
      fVtx[0] = esdVertex->GetX();
      fVtx[1] = esdVertex->GetY();
      fVtx[2] = esdVertex->GetZ();
      }*/
  AliVVertex *vvertex = (AliVVertex*)fInputEvent->GetPrimaryVertex();
      if (!vvertex) return;
      fVtx[0] = vvertex->GetX();
      fVtx[1] = vvertex->GetY();
      fVtx[2] = vvertex->GetZ();
  
  hEventStatistics->Fill("found primary vertex",1);

  if(fVtx[2] < -10. || fVtx[2] > 10.) return;
  hEventStatistics->Fill("vz cut",1);
  fVtxZ = fVtx[2];

  fCent = MultSelection->GetMultiplicityPercentile("V0M");
  if(fCent > centcut) return;
  fNSpdTracklets = MultSelection->GetEstimator("SPDTracklets")->GetValue();
  hEventStatistics->Fill("centrality selection",1);

  /*Printf("require track vertex = %i",fEventCuts.fRequireTrackVertex);
  Printf("min z-vertex = %lf",fEventCuts.fMinVtz);
  Printf("max z-vertex = %lf",fEventCuts.fMaxVtz);
  Printf("spd to track vertex = %lf",fEventCuts.fMaxDeltaSpdTrackAbsolute);
  Printf("resolution spd vertex = %lf",fEventCuts.fMaxResolutionSPDvertex);
  Printf("trigger mask = %lf",fEventCuts.fTriggerMask);
  Printf("reject incomplete daq = %i",fEventCuts.fRejectDAQincomplete);
  Printf("min spd pileup contribs = %lf",fEventCuts.fSPDpileupMinContributors);
  Printf("spd pileup min z distance = %lf",fEventCuts.fSPDpileupMinZdist);
  Printf("spd pileup nsigma z distance = %lf",fEventCuts.fSPDpileupNsigmaZdist);
  Printf("spd pileup nsigma diam xy = %lf",fEventCuts.fSPDpileupNsigmaDiamXY);
  Printf("spd pileup nsigma diam z = %lf",fEventCuts.fSPDpileupNsigmaDiamZ);
  Printf("tracklet background cut = %i",fEventCuts.fTrackletBGcut);*/
  
  if (!fEventCuts.AcceptEvent(fInputEvent)) return;
  hEventStatistics->Fill("AliEventCuts",1);
  
  // loop over reconstructed tracks
  Int_t nTracks = 0;
  if(fIsAOD) nTracks = fAOD->GetNumberOfTracks(); // aod
  else nTracks = fESD->GetNumberOfTracks(); // esd
  
  for(Int_t iTrack = 0; iTrack < nTracks; iTrack++)
    {
      Float_t pt = -999, eta = -999, phi = -999;
      if(fIsAOD) // aod
	{
	  AliAODTrack* track = (AliAODTrack*)fAOD->GetTrack(iTrack);
	  if(!track) continue;
	  if(TMath::Abs(track->Eta()) > 0.8) continue;
	  if(!(track->TestFilterBit(96))) continue; //filter bits 5+6
	  pt = track->Pt();
	  eta = track->Eta();
	  phi = track->Phi();
	}
      else //esd
	{
	  AliESDtrack* track = (AliESDtrack*)fESD->GetTrack(iTrack);
	  if(!track) continue;
	  if(TMath::Abs(track->Eta()) > 0.8) continue;
	  pt = track->Pt();
	  eta = track->Eta();
	  phi = track->Phi();
	}
      hTrackPt->Fill(pt);
      hTrackPhi->Fill(phi);
      hTrackEta->Fill(eta);
    } // end loop over reconstructed tracks

  fRunNumber = fInputEvent->GetRunNumber();

  Int_t nGen = 0;
  
  if(fIsMC)
    { 
      fGenLambda->Clear();
      fGenCascade->Clear();
      
      // loop over generated particles to find lambdas
      if(fIsAOD) nGen = fMCEvent->GetNumberOfTracks(); // aod
      else nGen = stack->GetNtrack(); // esd
      for(Int_t iGen = 0; iGen < nGen; iGen++)
	{
	  Int_t pid = -999, abspid = -999, nd = -999, fd = -999, ld = -999;
	  Float_t pt = -999, phi = -999, eta = -999, abseta = -999;
	  
	  if(fIsAOD) // aod
	    {
	      AliAODMCParticle* mctrack = (AliAODMCParticle*)fMCEvent->GetTrack(iGen);
	      if(!mctrack) continue;
	      if(!(mctrack->IsPhysicalPrimary())) continue;
	      pid = mctrack->PdgCode();
	      pt = mctrack->Pt();
	      eta = mctrack->Eta();
	      phi = mctrack->Phi();
	      nd = mctrack->GetNDaughters();
	      fd = mctrack->GetFirstDaughter();
	      ld = mctrack->GetLastDaughter();
	    }
	  else // esd
	    {
	      TParticle* mctrack = stack->Particle(iGen);
	      if(!mctrack) continue;
	      if(!(stack->IsPhysicalPrimary(iGen))) continue;
	      pid = mctrack->GetPdgCode();
	      pt = mctrack->Pt();
	      eta = mctrack->Eta();
	      phi = mctrack->Phi();
	      nd = mctrack->GetNDaughters();
	      fd = mctrack->GetFirstDaughter();
	      ld = mctrack->GetLastDaughter();
	    }
	  
	  abspid = TMath::Abs(pid);
	  abseta = TMath::Abs(eta);
      
	  // basic QA of pi/K/p
	  if(abseta < 0.8 && (abspid == 211 || abspid == 321 || abspid == 2212))
	    {
	      hGenPt->Fill(pt);
	      hGenPhi->Fill(phi);
	      hGenEta->Fill(eta);
	    }

	  if(abseta > etacutlambda) continue;
	  
	  if(fK0STree)
	    {
	      if(abspid == 310 && pt >= ptminlambda) AliLightGenV0* tempGenK0S = new((*fGenLambda)[fGenLambda->GetEntriesFast()]) AliLightGenV0(pt,eta,phi,0);
	      continue;
	    }


	  // cascades for feeddown correction
	      AliLightGenV0* tempGenCascade = 0x0;
	      if(pid == -3312) // xi+
		{
		  hXiPlus->Fill(pt,eta,fCent);
		  tempGenCascade = new((*fGenCascade)[fGenCascade->GetEntriesFast()]) AliLightGenV0(pt,eta,phi,1);
		}
	      else if(pid == 3312) // xi-
		{
		  hXiMinus->Fill(pt,eta,fCent);
		  tempGenCascade = new((*fGenCascade)[fGenCascade->GetEntriesFast()]) AliLightGenV0(pt,eta,phi,-1);
		}
	      else if(pid == 3322) // xi0
		{
		  hXiZero->Fill(pt,eta,fCent);
		  tempGenCascade = new((*fGenCascade)[fGenCascade->GetEntriesFast()]) AliLightGenV0(pt,eta,phi,-2);
		}
	      else if(pid == -3322) // anti-xi0
		{
		  hXiZeroAnti->Fill(pt,eta,fCent);
		  tempGenCascade = new((*fGenCascade)[fGenCascade->GetEntriesFast()]) AliLightGenV0(pt,eta,phi,2);
		}

	  
	  //if(abspid != 3122) continue;
	  
	  // generated (anti-)lambda pt spectra
	  AliLightGenV0* tempGenLambda = 0x0;
	  if(pid == 3122)
	    {
	      hLambdaPtGen->Fill(pt);
	      if(pt >= ptminlambda)
		{
		  tempGenLambda = new((*fGenLambda)[fGenLambda->GetEntriesFast()]) AliLightGenV0(pt,eta,phi,1);
		}
	    }
	  else if(pid == -3122)
	    {
	      hAntiLambdaPtGen->Fill(pt);
	      if(pt >= ptminlambda)
		{
		  tempGenLambda = new((*fGenLambda)[fGenLambda->GetEntriesFast()]) AliLightGenV0(pt,eta,phi,-1);
		}
	    }

	  if(tempGenLambda)
	    {
	      tempGenLambda->SetPosDaughter(-999,-999,-999);
	      tempGenLambda->SetNegDaughter(-999,-999,-999);
	    }
	  else if(tempGenCascade)
	    {
	      tempGenCascade->SetPosDaughter(-999,-999,-999);
	      tempGenCascade->SetNegDaughter(-999,-999,-999);
	    }
	  else
	    {
	      continue;
	    }
	  
	  // remove weird (1-prong, 3-prong) decays
	  if(nd != 2) continue;

	  Int_t pdg1 = -999, pdg2 = -999;
	  Float_t pt1 = -999, phi1 = -999, eta1 = -999;
	  Float_t pt2 = -999, phi2 = -999, eta2 = -999;
	  if(fIsAOD) // aod
	    {
	      AliAODMCParticle *daughtertrack1 = (AliAODMCParticle*)fMCEvent->GetTrack(fd);
	      if(!daughtertrack1) continue;
	      AliAODMCParticle *daughtertrack2 = (AliAODMCParticle*)fMCEvent->GetTrack(ld);
	      if(!daughtertrack2) continue;
	      pdg1 = daughtertrack1->GetPdgCode();
	      pdg2 = daughtertrack2->GetPdgCode();
	      pt1 = daughtertrack1->Pt();
	      eta1 = daughtertrack1->Eta();
	      phi1 = daughtertrack1->Phi();
	      pt2 = daughtertrack2->Pt();
	      eta2 = daughtertrack2->Eta();
	      phi2 = daughtertrack2->Phi();
	    }
	  else // esd
	    {
	      TParticle *daughtertrack1 = (TParticle*)stack->Particle(fd);
	      if(!daughtertrack1) continue;
	      TParticle *daughtertrack2 = (TParticle*)stack->Particle(ld);
	      if(!daughtertrack2) continue;
	      pdg1 = daughtertrack1->GetPdgCode();
	      pdg2 = daughtertrack2->GetPdgCode();
	      pt1 = daughtertrack1->Pt();
	      eta1 = daughtertrack1->Eta();
	      phi1 = daughtertrack1->Phi();
	      pt2 = daughtertrack2->Pt();
	      eta2 = daughtertrack2->Eta();
	      phi2 = daughtertrack2->Phi();
	    }
	  
	  if(tempGenLambda)
	    {
	      if(pid == 3122 && pdg1 == 2212 && pdg2 == -211) // daughter 1 is proton, daughter 2 is pi-, mother is lambda
		{
		  tempGenLambda->SetPosDaughter(pt1,eta1,phi1);
		  tempGenLambda->SetNegDaughter(pt2,eta2,phi2);
		}
	      else if(pid == -3122 && pdg1 == -2212 && pdg2 == 211) // daughter 1 is antiproton, daughter 2 is pi+, mother is anti-lambda
		{
		  tempGenLambda->SetNegDaughter(pt1,eta1,phi1);
		  tempGenLambda->SetPosDaughter(pt2,eta2,phi2);
		}
	      else if(pid == 3122 && pdg1 == -211 && pdg2 == 2212) // daughter 1 is pi-, daughter 2 is proton, mother is lambda
		{
		  tempGenLambda->SetNegDaughter(pt1,eta1,phi1);
		  tempGenLambda->SetPosDaughter(pt2,eta2,phi2);
		}
	      else if(pid == -3122 && pdg1 == 211 && pdg2 == -2212) // daughter 1 is pi+, daughter 2 is antiproton, mother is anti-lambda
		{
		  tempGenLambda->SetPosDaughter(pt1,eta1,phi1);
		  tempGenLambda->SetNegDaughter(pt2,eta2,phi2);
		}
	    }
	  if(tempGenCascade)
	    {
	      if(TMath::Abs(pdg1) == 3122 && (TMath::Abs(pdg2) == 211 || TMath::Abs(pdg2) == 111)) // daughter 1 is lambda, daughter 2 is pion
		{
		  tempGenCascade->SetPosDaughter(pt1,eta1,phi1);
		  tempGenCascade->SetNegDaughter(pt2,eta2,phi2);
		}
	      else if(TMath::Abs(pdg2) == 3122 && (TMath::Abs(pdg1) == 211 || TMath::Abs(pdg1) == 111)) // daughter 1 is pion, daughter 2 is lambda
		{
		  tempGenCascade->SetPosDaughter(pt2,eta2,phi2);
		  tempGenCascade->SetNegDaughter(pt1,eta1,phi1);
		}
	    }
	} // end loop over generated particles
    }

    /*AliESDv0Cuts* v0cuts = new AliESDv0Cuts();
  v0cuts->SetMinDcaPosToVertex(0.05);
  v0cuts->SetMinDcaNegToVertex(0.05);
  v0cuts->SetMaxChi2(33.);
  v0cuts->SetMaxDcaV0Daughters(1.5);
  v0cuts->SetMinRadius(0.2);
  v0cuts->SetMaxRadius(200.);
  v0cuts->SetMinCosinePointingAngle(0.99);
  v0cuts->SetMaxDcaV0ToVertex(5*7.89);
  Int_t nV0 = v0cuts->CountAcceptedV0s(fESD);
  TObjArray* v0array = v0cuts->GetAcceptedV0s(fESD);*/
  
  fAcceptV0->Clear();

  Int_t nV0 = 0;
  //TObjArray *v0array = 0x0;
  if(fIsAOD) nV0 = fAOD->GetNumberOfV0s(); // aod
  else nV0 = fESD->GetNumberOfV0s(); // esd
  
  AliESDv0 *esdv0 = 0x0;
  AliESDtrack *esdTrackPos = 0x0;
  AliESDtrack *esdTrackNeg = 0x0;
  AliAODv0 *aodv0 = 0x0;
  AliAODTrack *aodTrackPos = 0x0;
  AliAODTrack *aodTrackNeg = 0x0;
   
  for(Int_t iV0 = 0; iV0 < nV0; iV0++)
    {
      esdv0 = 0x0;
      esdTrackPos = 0x0;
      esdTrackNeg = 0x0;
      aodv0 = 0x0;
      aodTrackPos = 0x0;
      aodTrackNeg = 0x0;

      Double_t  secvtx[3];

      Float_t invMassLambda = -999, invMassAntiLambda = -999, invMassK0S = -999;
      Float_t alpha = -999, ptarm = -999;
      Float_t pt = -999, phi = -999, eta = -999, pmom = -999;
      Float_t ppt = -999, pphi = -999, peta = -999, pnsigmapr = -999;
      Float_t npt = -999, nphi = -999, neta = -999, nnsigmapr = -999;
      Bool_t swapflag = kFALSE, ontheflystat = kFALSE;
      Float_t dcaV0ToVertex = -999, dcaPosToVertex = -999, dcaNegToVertex = -999, dcaDaughters = -999, cosPointingAngle = -999;
      Float_t ncrossedrowsPos = -999, ncrossedratioPos = -999, ncrossedrowsNeg = -999, ncrossedratioNeg = -999;
      
      if(fIsAOD) // aod
	{
	  aodv0 = fAOD->GetV0(iV0);
	  if(!aodv0) continue;
	  AliAODVertex* v0vtx = (AliAODVertex*)aodv0->GetSecondaryVtx();
	  if(!v0vtx) continue;
	  v0vtx->GetPosition(secvtx);
	  aodTrackPos = (AliAODTrack*)v0vtx->GetDaughter(0);
	  if(!aodTrackPos) continue;
	  aodTrackNeg = (AliAODTrack*)v0vtx->GetDaughter(1);
	  if(!aodTrackNeg) continue;

	  invMassLambda = aodv0->MassLambda();
	  invMassAntiLambda = aodv0->MassAntiLambda();
	  invMassK0S = aodv0->MassK0Short();
	  alpha = aodv0->AlphaV0();
	  ptarm = aodv0->PtArmV0();

	  if(aodTrackPos->Charge() == aodTrackNeg->Charge()) // "like-sign V0s" 
	    {
	      hInvMassLike->Fill(invMassLambda,aodv0->Pt());  
	      continue;
	    }
	  if(aodTrackPos->Charge() < 0 && aodTrackNeg->Charge() > 0) // fix wrongly assigned charges
	    {
	      AliAODTrack* swaptrack = aodTrackPos;
	      aodTrackPos = aodTrackNeg;
	      aodTrackNeg = swaptrack;
	      invMassLambda = aodv0->MassAntiLambda();
	      invMassAntiLambda = aodv0->MassLambda();
	      ptarm = aodv0->QtProng(1);
	      alpha = -1.*alpha;
	      swapflag = kTRUE;
	    }
	  
	  pt = aodv0->Pt();
	  pmom = aodv0->P();
	  eta = aodv0->Eta();
	  phi = aodv0->Phi();
	  ppt = aodTrackPos->Pt();
	  peta = aodTrackPos->Eta();
	  pphi = aodTrackPos->Phi();
	  npt = aodTrackNeg->Pt();
	  neta = aodTrackNeg->Eta();
	  nphi = aodTrackNeg->Phi();
	  ontheflystat = aodv0->GetOnFlyStatus();
	  pnsigmapr = fPIDResponse->NumberOfSigmasTPC(aodTrackPos, AliPID::kProton);
	  nnsigmapr = fPIDResponse->NumberOfSigmasTPC(aodTrackNeg, AliPID::kProton);

	  dcaV0ToVertex = aodv0->DcaV0ToPrimVertex();
	  dcaPosToVertex = aodv0->DcaPosToPrimVertex();
	  dcaNegToVertex = aodv0->DcaNegToPrimVertex();
	  cosPointingAngle = aodv0->CosPointingAngle(fVtx);
	  dcaDaughters = aodv0->DcaV0Daughters();

	  ncrossedrowsPos = aodTrackPos->GetTPCNCrossedRows();
	  if(aodTrackPos->GetTPCNclsF() > 0) ncrossedratioPos = aodTrackPos->GetTPCNCrossedRows()/Float_t(aodTrackPos->GetTPCNclsF());
	  ncrossedrowsNeg = aodTrackNeg->GetTPCNCrossedRows();
	  if(aodTrackNeg->GetTPCNclsF() > 0) ncrossedratioNeg = aodTrackNeg->GetTPCNCrossedRows()/Float_t(aodTrackNeg->GetTPCNclsF());
	}
      else // esd
	{
	  esdv0 = fESD->GetV0(iV0);
	  //esdv0 = (AliESDv0*)v0array->At(iV0);
	  if(!esdv0) continue;
	  esdv0->GetXYZ(secvtx[0], secvtx[1], secvtx[2]);
	  esdTrackPos =  (AliESDtrack*)fESD->GetTrack(TMath::Abs(esdv0->GetPindex()));
	  if(!esdTrackPos) continue;
	  esdTrackNeg = (AliESDtrack*)fESD->GetTrack(TMath::Abs(esdv0->GetNindex()));
	  if(!esdTrackNeg) continue;

	  invMassLambda = esdv0->GetEffMass(4,2);
	  invMassAntiLambda = esdv0->GetEffMass(2,4);
	  invMassK0S = esdv0->GetEffMass(2,2);
	  alpha = esdv0->AlphaV0();
	  ptarm = esdv0->PtArmV0();
	  
	  if(esdTrackPos->Charge() == esdTrackNeg->Charge()) // "like-sign V0s" 
	    {
	      hInvMassLike->Fill(invMassLambda,esdv0->Pt());  
	      continue;
	    }
	  if(esdTrackPos->Charge() < 0 && esdTrackNeg->Charge() > 0) // fix wrongly assigned charges
	    {
	      AliESDtrack* swaptrack = esdTrackPos;
	      esdTrackPos = esdTrackNeg;
	      esdTrackNeg = swaptrack;
	      
	      invMassLambda = esdv0->GetEffMass(2,4);
	      invMassAntiLambda = esdv0->GetEffMass(4,2);
	      ptarm = 0;//esdv0->QtProng(1); // fix later
	      alpha = -1.*alpha;
	      swapflag = kTRUE;
	    }

	  pt = esdv0->Pt();
	  pmom = esdv0->P();
	  eta = esdv0->Eta();
	  phi = esdv0->Phi();
	  ppt = esdTrackPos->Pt();
	  peta = esdTrackPos->Eta();
	  pphi = esdTrackPos->Phi();
	  npt = esdTrackNeg->Pt();
	  neta = esdTrackNeg->Eta();
	  nphi = esdTrackNeg->Phi();
	  ontheflystat = esdv0->GetOnFlyStatus();
	  pnsigmapr = fPIDResponse->NumberOfSigmasTPC(esdTrackPos, AliPID::kProton);
	  nnsigmapr = fPIDResponse->NumberOfSigmasTPC(esdTrackNeg, AliPID::kProton);

	  dcaV0ToVertex = esdv0->GetD(fVtx[0],fVtx[1],fVtx[2]);
	  Float_t d1, d2;
	  esdTrackPos->GetImpactParameters(d1,d2);
	  dcaPosToVertex = TMath::Sqrt(d1*d1+d2*d2);
	  esdTrackNeg->GetImpactParameters(d1,d2);
	  dcaNegToVertex = TMath::Sqrt(d1*d1+d2*d2);
	  cosPointingAngle = esdv0->GetV0CosineOfPointingAngle();
	  dcaDaughters = esdv0->GetDcaV0Daughters();
	  
	  ncrossedrowsPos = esdTrackPos->GetTPCCrossedRows();
	  if(esdTrackPos->GetTPCNclsF() > 0) ncrossedratioPos = esdTrackPos->GetTPCCrossedRows()/Float_t(esdTrackPos->GetTPCNclsF());
	  ncrossedrowsNeg = esdTrackNeg->GetTPCCrossedRows();
	  if(esdTrackNeg->GetTPCNclsF() > 0) ncrossedratioNeg = esdTrackNeg->GetTPCCrossedRows()/Float_t(esdTrackNeg->GetTPCNclsF());
	}

      if(TMath::Abs(eta) > etacutlambda) continue;
      if(!(TMath::Abs(peta) < 1.)) continue;
      if(!(TMath::Abs(neta) < 1.)) continue;
     
      if(ontheflystat == kTRUE)
	{
	  hInvMassLambdaOnTheFly->Fill(invMassLambda,pt);
	  hInvMassAntiLambdaOnTheFly->Fill(invMassAntiLambda,pt);
	  continue;
	}
      
      hInvMassLambda->Fill(invMassLambda,pt);
      hInvMassAntiLambda->Fill(invMassAntiLambda,pt);

      hNSigmaProton->Fill(ppt,pnsigmapr);
      hNSigmaProton->Fill(-1.*npt,nnsigmapr);

      Float_t v0Radius      = TMath::Sqrt(secvtx[0]*secvtx[0]+secvtx[1]*secvtx[1]);
      Float_t v0DecayLength = TMath::Sqrt(TMath::Power(secvtx[0] - fVtx[0],2) +
					   TMath::Power(secvtx[1] - fVtx[1],2) +
					   TMath::Power(secvtx[2] - fVtx[2],2 ));

      if(!swapflag) hArmPod->Fill(alpha,ptarm);

      // put some basic cuts to reduce data volume here 
      if(!fIsAOD) // (esd only)
	{
	  if(cosPointingAngle < cospacut) continue; // looser cut than in AODs
	  if(v0Radius < minradius) continue; // stricter cut than in AODs
	  if(dcaPosToVertex < 0.05) continue; // these cuts are the same as in AODs
	  if(dcaNegToVertex < 0.05) continue;
	  if(dcaDaughters > 1.5) continue;
	  if(v0Radius > 200.) continue;
	}
      if(ncrossedrowsPos < ncrossedrows) continue;
      if(ncrossedrowsNeg < ncrossedrows) continue;
      if(ncrossedratioPos < crossedrowsclustercut) continue;
      if(ncrossedratioNeg < crossedrowsclustercut) continue;
      
      AliLightV0* tempLightV0 = 0x0;
      // make tree      
      if(pt >= ptminlambda)
	{
	  if(fK0STree)
	    {
	      if(invMassK0S < 0.65 && invMassK0S > 0.35)
		{
		  tempLightV0 = new((*fAcceptV0)[fAcceptV0->GetEntriesFast()]) AliLightV0(pt,eta,phi);
		  tempLightV0->SetInvMassLambda(invMassK0S);
		  tempLightV0->SetInvMassAntiLambda(invMassK0S);
		  tempLightV0->SetCosPointingAngle(cosPointingAngle);
		  tempLightV0->SetDecayR(v0Radius);
		  tempLightV0->SetDecayL(pmom > 0. ? v0DecayLength/pmom : 0.);
		  tempLightV0->SetDCAV0(dcaV0ToVertex);
		  tempLightV0->SetDCADaughters(dcaDaughters);
		  tempLightV0->SetMcStatus(0);
		  tempLightV0->SetPosDaughter(ppt,peta,pphi, 0, dcaPosToVertex);
		  tempLightV0->SetNegDaughter(npt,neta,nphi, 0, dcaNegToVertex);
		}
	    }
	  else
	    {
	      if(invMassLambda < 1.16 || invMassAntiLambda < 1.16)
		{
		  if(TMath::Abs(pnsigmapr) < 5 || TMath::Abs(nnsigmapr) < 5)
		  {
		      tempLightV0 = new((*fAcceptV0)[fAcceptV0->GetEntriesFast()]) AliLightV0(pt,eta,phi);
		      tempLightV0->SetInvMassLambda(invMassLambda);
		      tempLightV0->SetInvMassAntiLambda(invMassAntiLambda);
		      tempLightV0->SetCosPointingAngle(cosPointingAngle);
		      tempLightV0->SetDecayR(v0Radius);
		      tempLightV0->SetDecayL(pmom > 0. ? v0DecayLength/pmom : 0.);
		      tempLightV0->SetDCAV0(dcaV0ToVertex);
		      tempLightV0->SetDCADaughters(dcaDaughters);
		      tempLightV0->SetMcStatus(0);
		      tempLightV0->SetPosDaughter(ppt,peta,pphi, pnsigmapr, dcaPosToVertex);
		      tempLightV0->SetNegDaughter(npt,neta,nphi, nnsigmapr, dcaNegToVertex);
		  }
		}
	    }
	}

      //Armenteros-Podolanski plot
      //calculate variables manually
      /*Float_t pLplus = trackPos->Px()*v0cand->Px()+trackPos->Py()*v0cand->Py()+trackPos->Pz()*v0cand->Pz();
	Float_t pLminus = trackNeg->Px()*v0cand->Px()+trackNeg->Py()*v0cand->Py()+trackNeg->Pz()*v0cand->Pz();
	Float_t pLpL = v0cand->Px()*v0cand->Px()+v0cand->Py()*v0cand->Py()+v0cand->Pz()*v0cand->Pz();
	pLpL = sqrt(pLpL);
	pLplus /= pLpL;
	pLminus /= pLpL;
	Float_t alphatest = (pLplus-pLminus)/(pLplus+pLminus);
	Float_t qttest = (trackNeg->Py()*v0cand->Pz()-trackNeg->Pz()*v0cand->Py())*(trackNeg->Py()*v0cand->Pz()-trackNeg->Pz()*v0cand->Py())+(trackNeg->Pz()*v0cand->Px()-trackNeg->Px()*v0cand->Pz())*(trackNeg->Pz()*v0cand->Px()-trackNeg->Px()*v0cand->Pz())+(trackNeg->Px()*v0cand->Py()-trackNeg->Py()*v0cand->Px())*(trackNeg->Px()*v0cand->Py()-trackNeg->Py()*v0cand->Px());
	qttest = sqrt(qttest);
	qttest /= pLpL;
	Printf("%.3lf   %.3lf   %.3lf   %.3lf",alpha,alphatest,ptarm,qttest);*/
      //hArmPod->Fill(alpha,ptarm);
      
      // check if V0 corresponds to a real generated lambda
      if(fIsMC)
	{
	  Int_t mpid = -999;
	  Float_t mpt = -999, meta = -999;
	  Bool_t isPrim = kFALSE, isSecFromMaterial = kFALSE, isSecFromWeakDecay = kFALSE;
	  Float_t cascpt = -999, casceta = -999;
	  
	  if(fIsAOD) // aod
	    {
	      if(TMath::Abs(aodTrackPos->GetLabel()) >= nGen || TMath::Abs(aodTrackNeg->GetLabel()) >= nGen) continue;
	      AliAODMCParticle *aodGenTrackPos = (AliAODMCParticle*)fMCEvent->GetTrack(TMath::Abs(aodTrackPos->GetLabel()));
	      if(!aodGenTrackPos) continue;
	      AliAODMCParticle *aodGenTrackNeg = (AliAODMCParticle*)fMCEvent->GetTrack(TMath::Abs(aodTrackNeg->GetLabel()));
	      if(!aodGenTrackNeg) continue;
	      Int_t m1 = aodGenTrackPos->GetMother();
	      if(m1 < 0) continue;
	      Int_t m2 = aodGenTrackNeg->GetMother();
	      if(m2 < 0) continue;
	      if(m1 != m2) continue;
	      
	      AliVParticle *aodTestMother = fMCEvent->GetTrack(m1);
	      if(!aodTestMother) continue;
	      mpid = aodTestMother->PdgCode();
	      mpt = aodTestMother->Pt();
	      meta = aodTestMother->Eta();
	      isSecFromMaterial = aodTestMother->IsSecondaryFromMaterial();
	      isSecFromWeakDecay = aodTestMother->IsSecondaryFromWeakDecay();
	      isPrim = aodTestMother->IsPhysicalPrimary();

	      if(mpid == 3122 || mpid == -3122)
		{
		  if(isSecFromWeakDecay)
		    {
		      Int_t gm1 = aodTestMother->GetMother();
		      if(gm1 >= 0)
			{
			  AliVParticle *aodGrandmother = fMCEvent->GetTrack(gm1);
			  if(aodGrandmother)
			    {
			      if(aodGrandmother->IsPhysicalPrimary())
				{
				  Int_t gmpid = aodGrandmother->PdgCode();
				  if(gmpid == 3322 || gmpid == -3322) // xi0
				    {
				      cascpt = -1.*aodGrandmother->Pt();
				      casceta = aodGrandmother->Eta();
				    }
				  if(gmpid == 3312 || gmpid == -3312) // xi+, xi-
				    {
				      cascpt = aodGrandmother->Pt();
				      casceta = aodGrandmother->Eta();
				    }
				}
			    }
			}
		    }
		}
	    }
	  else // esd
	    {
	      if(TMath::Abs(esdTrackPos->GetLabel()) >= nGen || TMath::Abs(esdTrackNeg->GetLabel()) >= nGen) continue;
	      TParticle *esdGenTrackPos = (TParticle*)stack->Particle(TMath::Abs(esdTrackPos->GetLabel()));
	      if(!esdGenTrackPos) continue;
	      TParticle *esdGenTrackNeg = (TParticle*)stack->Particle(TMath::Abs(esdTrackNeg->GetLabel()));
	      if(!esdGenTrackNeg) continue;
	      Int_t m1 = esdGenTrackPos->GetMother(0);
	      if(m1 < 0) continue;
	      Int_t m2 = esdGenTrackNeg->GetMother(0);
	      if(m2 < 0) continue;
	      if(m1 != m2) continue;
	      
	      TParticle *esdTestMother = stack->Particle(m1);
	      if(!esdTestMother) continue;
	      mpid = esdTestMother->GetPdgCode();
	      mpt = esdTestMother->Pt();
	      meta = esdTestMother->Eta();
	      isSecFromMaterial = stack->IsSecondaryFromMaterial(m1);
	      isSecFromWeakDecay = stack->IsSecondaryFromWeakDecay(m1);
	      isPrim = stack->IsPhysicalPrimary(m1);

	      if(mpid == 3122 || mpid == -3122)
		{
		  if(isSecFromWeakDecay)
		    {
		      Int_t gm1 = esdTestMother->GetMother(0);
		      if(gm1 >= 0)
			{
			  TParticle *esdGrandmother = stack->Particle(gm1);
			  if(esdGrandmother)
			    {
			      if(stack->IsPhysicalPrimary(gm1))
				{
				  Int_t gmpid = esdGrandmother->GetPdgCode();
				  if(gmpid == 3322 || gmpid == -3322) // xi0
				    {
				      cascpt = -1.*esdGrandmother->Pt();
				      casceta = esdGrandmother->Eta();
				    }
				  if(gmpid == 3312 || gmpid == -3312) // xi+, xi-
				    {
				      cascpt = esdGrandmother->Pt();
				      casceta = esdGrandmother->Eta();
				    }
				}
			    }
			}
		    }
		}
	    }

	  //if((invMassLambda > 1.08 && invMassLambda < 1.15) || (invMassAntiLambda > 1.08 && invMassAntiLambda < 1.15))
	  //{
	  //Printf("%i   %.4lf   %.4lf   %.4lf",mpid,mpt,invMassLambda,invMassAntiLambda);
	  //}
	  /*if(pt > 0.8)
	    {
	      if(mpid==3122) {hInvMassPtPidLambda->Fill(invMassLambda,mpt,0); hInvMassPtPidAntiLambda->Fill(invMassAntiLambda,mpt,0);}
	      else if(mpid==-3122) {hInvMassPtPidLambda->Fill(invMassLambda,mpt,1); hInvMassPtPidAntiLambda->Fill(invMassAntiLambda,mpt,1);}
	      else if(mpid==22) {hInvMassPtPidLambda->Fill(invMassLambda,mpt,2); hInvMassPtPidAntiLambda->Fill(invMassAntiLambda,mpt,2);}
	      else if(mpid==111) {hInvMassPtPidLambda->Fill(invMassLambda,mpt,3); hInvMassPtPidAntiLambda->Fill(invMassAntiLambda,mpt,3);}
	      else if(mpid==130) {hInvMassPtPidLambda->Fill(invMassLambda,mpt,4); hInvMassPtPidAntiLambda->Fill(invMassAntiLambda,mpt,4);}
	      else if(mpid==211) {hInvMassPtPidLambda->Fill(invMassLambda,mpt,5); hInvMassPtPidAntiLambda->Fill(invMassAntiLambda,mpt,5);}
	      else if(mpid==-211) {hInvMassPtPidLambda->Fill(invMassLambda,mpt,6); hInvMassPtPidAntiLambda->Fill(invMassAntiLambda,mpt,6);}
	      else if(mpid==310) {hInvMassPtPidLambda->Fill(invMassLambda,mpt,7); hInvMassPtPidAntiLambda->Fill(invMassAntiLambda,mpt,7);}
	      else if(mpid==321) {hInvMassPtPidLambda->Fill(invMassLambda,mpt,8); hInvMassPtPidAntiLambda->Fill(invMassAntiLambda,mpt,8);}
	      else if(mpid==-321) {hInvMassPtPidLambda->Fill(invMassLambda,mpt,9); hInvMassPtPidAntiLambda->Fill(invMassAntiLambda,mpt,9);}
	      else if(mpid==2112) {hInvMassPtPidLambda->Fill(invMassLambda,mpt,10); hInvMassPtPidAntiLambda->Fill(invMassAntiLambda,mpt,10);}
	      else if(mpid==-2112) {hInvMassPtPidLambda->Fill(invMassLambda,mpt,11); hInvMassPtPidAntiLambda->Fill(invMassAntiLambda,mpt,11);}
	      else if(mpid==2212) {hInvMassPtPidLambda->Fill(invMassLambda,mpt,12); hInvMassPtPidAntiLambda->Fill(invMassAntiLambda,mpt,12);}
	      else if(mpid==-2212) {hInvMassPtPidLambda->Fill(invMassLambda,mpt,13); hInvMassPtPidAntiLambda->Fill(invMassAntiLambda,mpt,13);}
	      //else Printf("mid = %i  mL = %.4lf  mAL = %.4lf  pt = %.4lf",mpid,invMassLambda,invMassAntiLambda,pt);
	      }*/
	  
	  
	  if(fK0STree)
	    {
	      if(TMath::Abs(mpid) != 310) continue;
	    }
	  else
	    {
	      if(TMath::Abs(mpid) != 3122) continue;
	    }
	  
	  Int_t mcstatus = 1;
      
	  if(TMath::Abs(meta) > etacutlambda) mcstatus += 10; //mcstatus > 10 means it falls outside acceptance
	  
	  if(isSecFromMaterial)
	    {
	      mcstatus += 1; // mcstatus = 3 means secondary from material
	      if(mpid == 3122)
		hInvMassLambdaSecFromMaterial->Fill(invMassLambda,pt);
	      if(mpid == -3122)
		hInvMassAntiLambdaSecFromMaterial->Fill(invMassAntiLambda,pt);
	    }
	  if(isSecFromWeakDecay)
	    {
	      mcstatus += 2; // mcstatus = 4 means secondary from weak decay
	      if(mpid == 3122)
		hInvMassLambdaSecFromWeakDecay->Fill(invMassLambda,pt);
	      if(mpid == -3122)
		hInvMassAntiLambdaSecFromWeakDecay->Fill(invMassAntiLambda,pt);
	      if(tempLightV0) tempLightV0->SetCascadePtEta(cascpt,casceta);
	    }
	  
	  //if(!(stack->IsPhysicalPrimary(m1))) continue;
	  if(!isPrim) mcstatus += 1; // mcstatus = 2 means secondary
	  
	  if(fK0STree)
	    {
	      if(tempLightV0) tempLightV0->SetMcStatus(mcstatus); 
	    }

	  if(tempLightV0) tempLightV0->SetGenPtEta(mpt,meta);
	  
	  if(mpid == 3122)
	    {
	      if(tempLightV0) tempLightV0->SetMcStatus(mcstatus);
	      hPtResLambda->Fill(mpt,pt);
	      if(isPrim) hPtResLambdaPrim->Fill(mpt,pt);
	      if(TMath::Abs(meta) <= etacutlambda)
		{
		  hLambdaPtReco->Fill(mpt);
		  hInvMassLambdaReco->Fill(invMassLambda,pt);
		}
	    }
	  else if(mpid == -3122)
	    {
	      if(tempLightV0) tempLightV0->SetMcStatus(-1*mcstatus); // mcstatus < 0 means antilambda
	      hPtResAntiLambda->Fill(mpt,pt);
	      if(isPrim) hPtResAntiLambdaPrim->Fill(mpt,pt);
	      if(TMath::Abs(meta) <= etacutlambda)
		{
		  hAntiLambdaPtReco->Fill(mpt);
		  hInvMassAntiLambdaReco->Fill(invMassAntiLambda,pt);
		}
	    }
	}
    }
  fTree->Fill();
  
  hEventStatistics->Fill("after event loop",1);
  
  PostData(1,fListOfHistos);
  PostData(2,fTree);
}
