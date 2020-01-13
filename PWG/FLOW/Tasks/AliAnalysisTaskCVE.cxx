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

/* $Id: AliAnalysisTaskCVE.cxx  Rihan Haque, 18/09/2019 (ver1) $ */

//-- general include---
#include "TChain.h"
#include "TTree.h"
#include "TGrid.h"
#include "TROOT.h"
#include "TArrayI.h"
#include "TObjArray.h"
#include "TMatrixDSym.h"
#include "TParticle.h"
#include "TMath.h"
#include "stdio.h"
#include "Riostream.h"




//---- manager and handler---
#include "AliAnalysisManager.h"
#include "AliInputEventHandler.h"

//---V0 and ZDC info---
#include "AliAODZDC.h"
#include "AliAODVZERO.h"
#include "AliAODVertex.h"

//---AOD,ESD event--
#include "AliESDEvent.h"
#include "AliAODHeader.h"
#include "AliAODEvent.h"
#include "AliAODTrack.h"

//----- For MC event------
#include "AliMCEvent.h"
#include "AliStack.h"
#include "AliAODMCParticle.h"
#include "AliAODMCHeader.h"

//----for PID-----
#include "AliPIDResponse.h"
#include "AliPIDCombined.h"

//----- Vevent and tracks
#include "AliVEventHandler.h"
#include "AliVEvent.h"
#include "AliVTrack.h"
#include "AliVParticle.h"
#include "AliCentrality.h"

//----- must include-------
#include "AliMultSelection.h"
#include "AliAnalysisUtils.h"
#include "AliPhysicsSelection.h"
#include "AliFlowEventSimple.h"
#include "AliAnalysisTaskSE.h"
#include "AliAnalysisTaskCVE.h"

using std::cout;
using std::endl;
using std::vector;


ClassImp(AliAnalysisTaskCVE)

AliAnalysisTaskCVE::AliAnalysisTaskCVE(const char *name): AliAnalysisTaskSE(name),
  fVevent(NULL),
  fESD(NULL),
  fAOD(NULL),
  fPIDResponse(NULL),
  fMultSelection(NULL),
  fAnalysisUtil(NULL),
  fListHist(NULL),
  fListTRKCorr(NULL),
  fListNUACorr(NULL),
  fListV0MCorr(NULL),
      
  fCentralityMin(0),
  fCentralityMax(90),
  fFilterBit(1),
  fTPCclustMin(70),
  bUseKinkTracks(kFALSE),
  fNSigmaCut(2.0),
  fMinPtCut(0.2),
  fMaxPtCut(5.0),
  fMinEtaCut(-0.8),
  fMaxEtaCut(0.8),
  fTrkChi2Min(0.1),    
  fdEdxMin(10.0),
  fMinVzCut(-10.0),
  fMaxVzCut(10.0),
  sCentrEstimator("V0M"),
  fCentDistBeforCut(NULL),
  fCentDistAfterCut(NULL),
  fHistEtaPtBeforCut(NULL),
  fHistEtaPhiBeforCut(NULL),
  fHistEtaPhiAfterCut(NULL),  
  fHistPtDistMBwithCut(NULL),
  fHCorrectMCposChrg(NULL),
  fHCorrectMCposPion(NULL),
  fHCorrectMCposKaon(NULL),
  fHCorrectMCposProt(NULL),
  fHCorrectMCnegChrg(NULL),
  fHCorrectMCnegPion(NULL),
  fHCorrectMCnegKaon(NULL),
  fHCorrectMCnegProt(NULL),      
  fHistAChrgVsCent(NULL),
  
  fHistEventCount(NULL)
{

  for(int i=0;i<2;i++){
    for(int j=0;j<10;j++){
      fHistv2AchChrgPos[i][j] = NULL;
      fHistv2AchKaonPos[i][j] = NULL;    
      fHistv2AchProtPos[i][j] = NULL;      
      fHistv2AchPionPos[i][j] = NULL;
      
      fHistv2AchChrgNeg[i][j] = NULL;
      fHistv2AchPionNeg[i][j] = NULL;      
      fHistv2AchKaonNeg[i][j] = NULL;
      fHistv2AchProtNeg[i][j] = NULL;      
    }
  }
  
  for(int i=0; i<5; i++){
    fHCorrectNUAposChrg[i] = NULL;  
    fHCorrectNUAnegChrg[i] = NULL;  
    fHCorrectNUAposPion[i] = NULL;  
    fHCorrectNUAnegPion[i] = NULL;  
    fHCorrectNUAposKaon[i] = NULL;  
    fHCorrectNUAnegKaon[i] = NULL;  
    fHCorrectNUAposProt[i] = NULL;  
    fHCorrectNUAnegProt[i] = NULL;  
  }
  
  
  //Must be here:
  DefineInput(0,TChain::Class());
  DefineOutput(1,TList::Class());
}

//_______________________empty constructor_______________________
AliAnalysisTaskCVE::AliAnalysisTaskCVE():
  fVevent(NULL),
  fESD(NULL),
  fAOD(NULL),
  fPIDResponse(NULL),
  fMultSelection(NULL),
  fAnalysisUtil(NULL),
  fListHist(NULL),
  fListTRKCorr(NULL),
  fListNUACorr(NULL),
  fListV0MCorr(NULL),
  
  fCentralityMin(0),
  fCentralityMax(90),
  fFilterBit(1),
  fTPCclustMin(70),
  bUseKinkTracks(kFALSE),
  fNSigmaCut(2.0),
  fMinPtCut(0.2),
  fMaxPtCut(5.0),
  fMinEtaCut(-0.8),
  fMaxEtaCut(0.8),
  fTrkChi2Min(0.1),    
  fdEdxMin(10.0),
  fMinVzCut(-10.0),
  fMaxVzCut(10.0),
  sCentrEstimator("V0M"),
  fCentDistBeforCut(NULL),
  fCentDistAfterCut(NULL),
  fHistEtaPtBeforCut(NULL),
  fHistEtaPhiBeforCut(NULL),
  fHistEtaPhiAfterCut(NULL),  
  fHistPtDistMBwithCut(NULL),
  fHCorrectMCposChrg(NULL),
  fHCorrectMCposPion(NULL),
  fHCorrectMCposKaon(NULL),
  fHCorrectMCposProt(NULL),
  fHCorrectMCnegChrg(NULL),
  fHCorrectMCnegPion(NULL),
  fHCorrectMCnegKaon(NULL),
  fHCorrectMCnegProt(NULL),    
  fHistAChrgVsCent(NULL),
  
  fHistEventCount(NULL)
{
  for(int i=0;i<2;i++){
    for(int j=0;j<10;j++){
      fHistv2AchChrgPos[i][j] = NULL;
      fHistv2AchKaonPos[i][j] = NULL;    
      fHistv2AchProtPos[i][j] = NULL;      
      fHistv2AchPionPos[i][j] = NULL;
      
      fHistv2AchChrgNeg[i][j] = NULL;
      fHistv2AchPionNeg[i][j] = NULL;      
      fHistv2AchKaonNeg[i][j] = NULL;
      fHistv2AchProtNeg[i][j] = NULL;      
    }
  }

  for(int i=0; i<5; i++){
    fHCorrectNUAposChrg[i] = NULL;  
    fHCorrectNUAnegChrg[i] = NULL;  
    fHCorrectNUAposPion[i] = NULL;  
    fHCorrectNUAnegPion[i] = NULL;  
    fHCorrectNUAposKaon[i] = NULL;  
    fHCorrectNUAnegKaon[i] = NULL;  
    fHCorrectNUAposProt[i] = NULL;  
    fHCorrectNUAnegProt[i] = NULL;  
  }
    
  //Not needed for Empty Constructor:
  //DefineInput(0,TChain::Class());
  //DefineOutput(1,TList::Class());
}
  
//__________________ destructor ___________________
AliAnalysisTaskCVE::~AliAnalysisTaskCVE()
{
  if(fListHist)      delete fListHist;  
  if(fAnalysisUtil)  delete fAnalysisUtil;   // because its 'new' !!
}










//________________ Define Histograms _______________
void AliAnalysisTaskCVE::UserCreateOutputObjects()
{
  //std::cout<<"\n UserCreateOutputObject: function begins...\n"<<endl; 
  //Get The Input Hander:
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  AliInputEventHandler *inputHandler=dynamic_cast<AliInputEventHandler*>(mgr->GetInputEventHandler());
  
  if (!inputHandler) {  printf("\n***** ERROR *****\n Input handler missing, Status:QUIT!\n");    exit(1);}

  //PileUp Multi-Vertex
  fAnalysisUtil = new AliAnalysisUtils();
  fAnalysisUtil->SetUseMVPlpSelection(kTRUE);
  fAnalysisUtil->SetUseOutOfBunchPileUp(kTRUE);
  
  //// obtain the PID response object if needed:
  fPIDResponse=inputHandler->GetPIDResponse();
  if (!fPIDResponse) {  printf("\n***** ERROR *****\n fPIDResponse missing, Status:QUIT!\n");    exit(1);}    
    
  fListHist = new TList();
  fListHist->SetOwner(kTRUE);

  SetupEventAndTaskConfigInfo();


  /// Bunch of QA histograms:  
  fCentDistBeforCut = new TH1F("fCentDistBeforCut","Cent w/o any Cuts; Cent (%); no.Events ",100,0,100);
  fListHist->Add(fCentDistBeforCut);
  fCentDistAfterCut = new TH1F("fCentDistAfterCut","Cent with all Cut; Cent (%); no.Events ",100,0,100);
  fListHist->Add(fCentDistAfterCut);
  
  fHistEtaPtBeforCut = new TH2F("fHistEtaPtBeforCut","#eta vs p_{T} (wFB, w/o  cut)",120,-1.2,1.2,100,0,10);
  fListHist->Add(fHistEtaPtBeforCut);
  fHistEtaPhiBeforCut = new TH2F("fHistPhiEtaBeforCut","#phi vs #eta (wFB, w/o cut)",100,0,6.2835,24,-1.2,1.2);
  fListHist->Add(fHistEtaPhiBeforCut);  
  fHistEtaPhiAfterCut = new TH2F("fHistPhiEtaAfterCut","#phi vs #eta (with Wgts)",100,0,6.2835,24,-1.2,1.2);
  fListHist->Add(fHistEtaPhiAfterCut);
  
  fHistPtDistMBwithCut = new TH1F("fHistPtDistMBwithCut","p_{T} with Cut; p_{T} (GeV/c); entries ",200,0,10);
  fListHist->Add(fHistPtDistMBwithCut);
  //Double_t fCentArray[11] = {0,5,10,20,30,40,50,60,70,80,90};


  
  //----------- User's histograms: --------------
  Char_t  name[100];
  Char_t title[100];
 

  Double_t centRange[11] = {0,5,10,20,30,40,50,60,70,80,90};

  fHistAChrgVsCent = new TH2F("fHistAChrgVsCent","Ach vs Cent;Cent;Ach",18,0,90,50,-1,1);
  fListHist->Add(fHistAChrgVsCent);

		 
  // v2 vs Ach
  for(int i=0;i<2;i++){
    for(int j=0;j<10;j++){
      ////Charge:
      sprintf(name,"fHistv2AchChrgPos_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchChrgPos[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchChrgPos[i][j]->Sumw2();
      fListHist->Add(fHistv2AchChrgPos[i][j]);
      sprintf(name,"fHistv2AchChrgNeg_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchChrgNeg[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchChrgNeg[i][j]->Sumw2();
      fListHist->Add(fHistv2AchChrgNeg[i][j]);      

      //// Pion:
      sprintf(name,"fHistv2AchPionPos_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchPionPos[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchPionPos[i][j]->Sumw2();
      fListHist->Add(fHistv2AchPionPos[i][j]);
      sprintf(name,"fHistv2AchPionNeg_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchPionNeg[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchPionNeg[i][j]->Sumw2();
      fListHist->Add(fHistv2AchPionNeg[i][j]);      
 
      //// Kaon:
      sprintf(name,"fHistv2AchKaonPos_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchKaonPos[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchKaonPos[i][j]->Sumw2();
      fListHist->Add(fHistv2AchKaonPos[i][j]);
      sprintf(name,"fHistv2AchKaonNeg_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchKaonNeg[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchKaonNeg[i][j]->Sumw2();
      fListHist->Add(fHistv2AchKaonNeg[i][j]);      

      //// Proton:
      sprintf(name,"fHistv2AchProtPos_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchProtPos[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchProtPos[i][j]->Sumw2();
      fListHist->Add(fHistv2AchProtPos[i][j]);
      sprintf(name,"fHistv2AchProtNeg_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchProtNeg[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchProtNeg[i][j]->Sumw2();
      fListHist->Add(fHistv2AchProtNeg[i][j]);      
    }
  }



  if(fListTRKCorr){
    std::cout<<"\n UserCreateOutputObject::Info() Tlist for MC tracking Efficiency Found.!!\n"<<std::endl;
  }
  else{
    std::cout<<"\n\n ******* WARNING No TList for Trk Efficiency Correction!!\n using TrkWgt = 1.0 \n "<<std::endl;
  }

  if(fListNUACorr){
    std::cout<<"\n UserCreateOutputObject::Info() Tlist for NUA Correction Found.!!\n"<<std::endl;
  }
  else{
    std::cout<<"\n\n ******* WARNING No TList NUA Correction!!\n using NUAWgt = 1.0 \n "<<std::endl;
  }



  
  PostData(1,fListHist);
 //std::cout<<"\n UserCreateOutputObject: function Ends...\n"<<endl;
}










//____________________________ Call Event by Event ___________________________________
void AliAnalysisTaskCVE::UserExec(Option_t*) {
 
  //cout<<"\n Info:UserExec() called ..!!!\n";
  //watch.Start(kTRUE);
  
  Float_t stepCount = 0.5;

  fHistEventCount->Fill(stepCount);   //1
  stepCount++;

  fAOD = dynamic_cast <AliAODEvent*> (InputEvent());
  fESD = dynamic_cast <AliESDEvent*> (InputEvent());
  if(!(fESD || fAOD)) {
    printf("ERROR: fESD & fAOD not available\n");
    return;
  }

  
  fVevent = dynamic_cast<AliVEvent*>(InputEvent());
  if (!fVevent){
    printf("ERROR: fVevent not available\n");
    return;
  }



  fHistEventCount->Fill(stepCount);
  stepCount++;

  
  //fHistEventCount->Fill(stepCount); //2
  //stepCount++;

  

  //-------------- Vtx cuts ---------------
  const AliVVertex *pVtx = fVevent->GetPrimaryVertex();
  Double_t pVtxZ = -999;
  pVtxZ  = pVtx->GetZ();
    
  if(pVtxZ < fMinVzCut || pVtxZ > fMaxVzCut){
    return;
  }
  
  fHistEventCount->Fill(stepCount); //3
  stepCount++;

  Float_t centrality = -99.0;
  Float_t centrV0M   = -99.0;
  //Float_t centrCL1   = -99.0;

  fMultSelection = (AliMultSelection*) InputEvent()->FindListObject("MultSelection");  // Must never comment this
  if(!fMultSelection) {
    printf("\n...**ERROR**...\n UserExec() AliMultSelection object not found\n Status:Quit!! \n");
    exit(1);
  }


  centrV0M = fMultSelection->GetMultiplicityPercentile("V0M");
  //centrCL1 = fMultSelection->GetMultiplicityPercentile("CL1");

 
  centrality = centrV0M;  // This Is Always Default, changes below for other options:
  
  if(sCentrEstimator=="CL0"){
    centrality = fMultSelection->GetMultiplicityPercentile("CL0");;
  }
  else if(sCentrEstimator=="CL1"){
    centrality = fMultSelection->GetMultiplicityPercentile("CL1");;
  }
  else if(sCentrEstimator=="V0C"){
    centrality = fMultSelection->GetMultiplicityPercentile("V0C");
  }
  else if(sCentrEstimator=="V0A"){
    centrality = fMultSelection->GetMultiplicityPercentile("V0A");
  }
  else if(sCentrEstimator=="TRK"){
    centrality = fMultSelection->GetMultiplicityPercentile("TRK");
  }
  else{
    centrality = centrV0M;
  }

  
  fCentDistBeforCut->Fill(centrality);
  
  if(centrality<fCentralityMin || centrality>fCentralityMax){ 
    return;
  }

  ///----> Get 0-10 index of centrality:
  Int_t cent10bin = -99, iCent = -99;

  if(centrality<5.0) {
    cent10bin  = 0; 
  }
  else if(centrality>=5.0 && centrality<10){
    cent10bin  = 1;
  }
  else if(centrality>=10.0) {
    cent10bin = abs(centrality/10.0)+1;
  }

  iCent = cent10bin;


  //Centrality array index for NUA correcion
  Int_t cForNUA = 0;  

  if(centrality<5.0) {
    cForNUA = 0;
  }
  else if(centrality>=5.0 && centrality<10){
    cForNUA = 1; // 1=5-10,
  }
  else if(centrality>=10.0 && centrality<20) {
    cForNUA = 2; // 2 = 10-20,
  }
  else if(centrality>=20 && centrality<40){ 
    cForNUA = 3; // 3=20-40
  }
  else if(centrality>=40){
    cForNUA = 4; // 4=40-90
  }









  

  Int_t ntracks = fAOD->GetNumberOfTracks();
  if(ntracks < 4) return;                        // Minimum 4 tracks per event. 

  fHistEventCount->Fill(stepCount); //4
  stepCount++;


  

  //////----> Get Magnetic field and RunNo.---------
  // Float_t fMagField = fAOD->GetMagneticField();
  // const Int_t QAindex = (fMagField > 0) ? 1 : 0;
  Int_t runNumber = fAOD->GetRunNumber();
  //------------------------------------------------

  if(fListTRKCorr) GetMCCorrectionHist(runNumber);

  if(fListNUACorr){
    GetNUACorrectionHist(runNumber,0);
    GetNUACorrectionHist(runNumber,1);
    GetNUACorrectionHist(runNumber,2);
    GetNUACorrectionHist(runNumber,3);  
  }





  
  Float_t fMultTPCFull = 0;  // TPC mult estimate
  Float_t fMultGlobal  = 0;  // global track multiplicity
  
  //Float_t fMultwRawFB  = 0;  // Uncorrected Multiplicity
  //Float_t fMultCorrFB  = 0;  // Corrected Multiplicity  
  //Int_t   gMultEtaNeg  = 0;
  //Int_t   gMultEtaPos  = 0;
  //Int_t   gMultEtaAll  = 0;

  Float_t trkPt=0,trkPhi=0,trkEta=0;
  Float_t trkChi2=0,trkdEdx=0,trkWgt=1.0;
  Int_t   trkChrg=0, trkTpcNC=0;
  ////PID variables:
  Double_t nSigTOFpion, nSigTPCpion;
  Double_t nSigTOFkaon, nSigTPCkaon;
  Double_t nSigTOFprot, nSigTPCprot;  
  //Bool_t   bTOFmatch= kFALSE;
  Bool_t   isItPion = kFALSE, isItKaon= kFALSE, isItProt= kFALSE;
	  
  ////User's variable:
  Double_t fSumTPCQn2xNeg=0, fSumTPCQn2yNeg=0, fSumTPCQn2xPos=0, fSumTPCQn2yPos=0;
  Double_t fSumWgtEtaNeg=0, fSumWgtEtaPos=0;
  Double_t fNumOfPos = 0;
  Double_t fNumOfNeg = 0;
  Double_t ptWgtMC = 1.0, WgtNUA = 1.0;
  Int_t ptBinMC = 1, iBinNUA = 1;
  
  Float_t fWgtEvent = 1.0; //Event Weight if Any.

  /// TO be passed as Argument:
  Double_t fEtaGapNeg = -0.100;
  Double_t fEtaGapPos =  0.100;
  Double_t gPsiN = 2.0;

  

  
  ////////-----> Starting 1st track Loop -----------
  
  for(Int_t iTrack = 0; iTrack < ntracks; iTrack++) { 

    AliAODTrack* AODtrack = dynamic_cast <AliAODTrack*> (fVevent->GetTrack(iTrack));
    if(!AODtrack) continue;
    
    if(AODtrack->TestFilterBit(fFilterBit)) {  //// Only use FB tracks. 

      trkPt    = AODtrack->Pt();
      trkPhi   = AODtrack->Phi();
      trkEta   = AODtrack->Eta();
      trkChrg  = AODtrack->Charge();
      trkChi2  = AODtrack->Chi2perNDF();
      trkTpcNC = AODtrack->GetTPCNcls();

      fHistEtaPtBeforCut->Fill(trkEta, trkPt);
      fHistEtaPhiBeforCut->Fill(trkPhi,trkEta);
      
      /// This Next function is called After Filter bit is validated!! (Otherwise code breaks!)
      trkdEdx  = AODtrack->GetDetPid()->GetTPCsignal();  

      //Apply track cuts here:
      if((trkPt <= fMaxPtCut) && (trkPt >= fMinPtCut) && (trkEta <= fMaxEtaCut) && (trkEta >= fMinEtaCut) && (trkdEdx >= fdEdxMin) && (trkTpcNC >= fTPCclustMin) && (trkChi2 >= fTrkChi2Min) && (trkChi2 <= 4.0) && TMath::Abs(trkChrg)) {

	//dcaXY  = track->DCA();
	//dcaZ   = track->ZAtDCA();
        
	fHistPtDistMBwithCut->Fill(trkPt);

	//---------->  Here I do All my track level analysis:
    



	  

	//------> Get NUA weights for EP <----------
	WgtNUA = 1.0;
	ptWgtMC = 1.0;
	
	if(trkChrg>0){

	  if(fHCorrectMCposChrg){
	    ptBinMC = fHCorrectMCposChrg->FindBin(trkPt);    //Charge independent MC correction atm.
	    ptWgtMC = 1.0/fHCorrectMCposChrg->GetBinContent(ptBinMC);
	  }
	  
	  if(fHCorrectNUAposChrg[cForNUA]){
	    iBinNUA = fHCorrectNUAposChrg[cForNUA]->FindBin(pVtxZ,trkPhi,trkEta);
	    WgtNUA  = fHCorrectNUAposChrg[cForNUA]->GetBinContent(iBinNUA);
	  }
	}
	else{
	  
	  if(fHCorrectMCnegChrg){
	    ptBinMC = fHCorrectMCnegChrg->FindBin(trkPt);    //Charge independent MC correction atm.
	    ptWgtMC = 1.0/fHCorrectMCnegChrg->GetBinContent(ptBinMC);
	  }
	  
	  if(fHCorrectNUAnegChrg[cForNUA]){
	    iBinNUA = fHCorrectNUAnegChrg[cForNUA]->FindBin(pVtxZ,trkPhi,trkEta);
	    WgtNUA  = fHCorrectNUAnegChrg[cForNUA]->GetBinContent(iBinNUA);  
	  }
	}

	//RefMultRawFB++;
	//RefMultCorrFB += ptWgtMC;
	
	trkWgt = WgtNUA*ptWgtMC;

	
	//if(iTrack%10==0){
	//std::cout<<" pT = "<<trkPt<<"\t MCWgt = "<<ptWgtMC<<"\t Eta = "<<trkEta<<"\t NUAwgt = "<<WgtNUA<<"\t TotalWgt = "<<trkWgt<<endl;
	//}
	



	
	if(trkEta < fEtaGapNeg){
	  fSumTPCQn2xNeg += trkWgt*TMath::Cos(gPsiN*trkPhi);
	  fSumTPCQn2yNeg += trkWgt*TMath::Sin(gPsiN*trkPhi);
	  fSumWgtEtaNeg  += trkWgt;
	}
	else if(trkEta > fEtaGapPos){
	  fSumTPCQn2xPos += trkWgt*TMath::Cos(gPsiN*trkPhi);
	  fSumTPCQn2yPos += trkWgt*TMath::Sin(gPsiN*trkPhi);
	  fSumWgtEtaPos  += trkWgt;
	}

	if(trkChrg > 0){	  
	  fNumOfPos += trkWgt;
	}
	else{
	  fNumOfNeg += trkWgt;
	}


	
	
	//<---------- User track analysis Done---------------

      }// when all trackCuts applied      
    }//-------> when FB is validated




    
    ///------- For Pile-UP removal Purpose only-----
    if(AODtrack->TestFilterBit(128))      fMultTPCFull++; // A. Dobrin TPC vs ESD PileUp Cut.
    if(!AODtrack->TestFilterBit(16) || AODtrack->Chi2perNDF() < 0.1) continue;
    Double_t    bval[2] = {-99., -99.};
    Double_t    bCov[3] = {-99., -99., -99.};
    AliAODTrack copy(*AODtrack);
    if(copy.PropagateToDCA(fVevent->GetPrimaryVertex(), fVevent->GetMagneticField(), 100., bval, bCov) && TMath::Abs(bval[0]) < 0.3 && TMath::Abs(bval[1]) < 0.3){
      fMultGlobal++;
    }///MultGlobal Condition
    
  }///------> 1st Track loop Ends here.<--------




  if(iCent > 9) return;

  fHistEventCount->Fill(stepCount); //5
  stepCount++;



  //// Decide If Pile-Up cut to be applied.
  //fHistEventCount->Fill(stepCount); //6
  //stepCount++;

  
  
  Float_t fAchrgNet = (fNumOfPos - fNumOfNeg)/(fNumOfPos + fNumOfNeg); // Efficiency & NUA Corrected!

  fHistAChrgVsCent->Fill(centrality, fAchrgNet, fWgtEvent);
  // Do Event plane defination, Fill Resolution Etc.
  // Double_t Psi2EtaPos =  Formula for PsiN:




















  
  ////////-----> Starting 2nd track Loop -----------
  /// Track loop variable (not used in previous loop)
  Double_t sumQxTPCneg=0, sumQyTPCneg=0, sumQxTPCpos=0, sumQyTPCpos=0;
  Double_t sumWgtneg=0, sumWgtpos=0;
  Double_t uqRe=0, uqIm=0;

  Double_t trkWgtPion=1.0, trkWgtKaon=1.0, trkWgtProt=1.0;
  Double_t WgtNUAPion=1.0, WgtNUAKaon=1.0, WgtNUAProt=1.0;
  Double_t ptWgtMCPion=1.0, ptWgtMCKaon=1.0, ptWgtMCProt=1.0;



  
  for(Int_t iTrack = 0; iTrack < ntracks; iTrack++) { 

    AliAODTrack* AODtrack = dynamic_cast <AliAODTrack*> (fVevent->GetTrack(iTrack));
    if(!AODtrack) continue;
    
    if(AODtrack->TestFilterBit(fFilterBit)) {  //// Only use FB tracks. 

      trkPt    = AODtrack->Pt();
      trkPhi   = AODtrack->Phi();
      trkEta   = AODtrack->Eta();
      trkChrg  = AODtrack->Charge();
      trkChi2  = AODtrack->Chi2perNDF();
      trkTpcNC = AODtrack->GetTPCNcls();

      fHistEtaPtBeforCut->Fill(trkEta, trkPt);
      fHistEtaPhiBeforCut->Fill(trkPhi,trkEta);
      
      /// This Next function is called After Filter bit is validated!! (Otherwise code breaks!)
      trkdEdx  = AODtrack->GetDetPid()->GetTPCsignal();  

      //Apply track cuts here:
      if((trkPt <= fMaxPtCut) && (trkPt >= fMinPtCut) && (trkEta <= fMaxEtaCut) && (trkEta >= fMinEtaCut) && (trkdEdx >= fdEdxMin) && (trkTpcNC >= fTPCclustMin) && (trkChi2 >= fTrkChi2Min) && (trkChi2 <= 4.0) && TMath::Abs(trkChrg)) {

	//dcaXY  = track->DCA();
	//dcaZ   = track->ZAtDCA();
        
	fHistPtDistMBwithCut->Fill(trkPt);

	//---------->  Here I do All my track level analysis:

	
	isItPion = kFALSE;
	isItKaon = kFALSE;
	isItProt = kFALSE;
     
	///=========> Get TPC/TOF nSigma for PID
	nSigTPCpion = fPIDResponse->NumberOfSigmasTPC(AODtrack, AliPID::kPion);
	nSigTPCkaon = fPIDResponse->NumberOfSigmasTPC(AODtrack, AliPID::kKaon);
	nSigTPCprot = fPIDResponse->NumberOfSigmasTPC(AODtrack, AliPID::kProton);
    
	nSigTOFpion = fPIDResponse->NumberOfSigmasTOF(AODtrack, AliPID::kPion);
	nSigTOFkaon = fPIDResponse->NumberOfSigmasTOF(AODtrack, AliPID::kKaon);
	nSigTOFprot = fPIDResponse->NumberOfSigmasTOF(AODtrack, AliPID::kProton);
      
	//----- Pion
	if(trkPt<=0.6 && TMath::Abs(nSigTPCpion)<=3.0){
	  isItPion = kTRUE;
	}
	else if(trkPt>0.6 && trkPt<=5.0 && TMath::Abs(nSigTPCpion)<=3.0 && TMath::Abs(nSigTOFpion)<=3.0 ){
	  isItPion = kTRUE;
	}
	//----- Kaon
	if(trkPt<=0.45 && TMath::Abs(nSigTPCkaon)<=3.0){
	  isItKaon = kTRUE;
	}
	else if(trkPt>0.45 && trkPt<=5.0 && TMath::Abs(nSigTPCkaon)<=3.0 && TMath::Abs(nSigTOFkaon)<=2.5){
	  isItKaon = kTRUE;
	}
	//----- Proton 
	if(trkPt<=0.8 && TMath::Abs(nSigTPCprot)<=3.0){
	  isItProt = kTRUE;
	  if(trkChrg>0 && trkPt<0.4) isItProt = kFALSE;  //Proton below 0.4 GeV has beam Pipe Contamination
	}
	else if(trkPt>0.8 && trkPt<=5.0 && TMath::Abs(nSigTPCprot)<=3.0 && TMath::Abs(nSigTOFprot)<=3.0){  
	  isItProt = kTRUE;
	}
     
    	//-------- PID selection is done ---------

	ptWgtMC = 1.0; WgtNUA = 1.0;
	trkWgtPion = 1.0; trkWgtKaon = 1.0; trkWgtProt = 1.0;
	WgtNUAPion = 1.0; WgtNUAKaon = 1.0; WgtNUAProt = 1.0; 
	ptWgtMCPion = 1.0; ptWgtMCKaon = 1.0; ptWgtMCProt = 1.0;
	

	
	if(trkChrg>0){
	  
	  ///Tracking:
	  if(fHCorrectMCposChrg){
	    ptBinMC = fHCorrectMCposChrg->FindBin(trkPt);    
	    ptWgtMC = 1.0/fHCorrectMCposChrg->GetBinContent(ptBinMC);
	  }
	  if(isItPion && fHCorrectMCposPion){
	    ptBinMC     = fHCorrectMCposPion->FindBin(trkPt);    
	    ptWgtMCPion = 1.0/fHCorrectMCposPion->GetBinContent(ptBinMC);
	  }
	  if(isItKaon && fHCorrectMCposKaon){
	    ptBinMC     = fHCorrectMCposKaon->FindBin(trkPt);    
	    ptWgtMCKaon = 1.0/fHCorrectMCposKaon->GetBinContent(ptBinMC);
	  }
	  if(isItProt && fHCorrectMCposProt){
	    ptBinMC     = fHCorrectMCposProt->FindBin(trkPt);    
	    ptWgtMCProt = 1.0/fHCorrectMCposProt->GetBinContent(ptBinMC);
	  }

	  ///NUA:
	  if(fHCorrectNUAposChrg[cForNUA]){
	    iBinNUA = fHCorrectNUAposChrg[cForNUA]->FindBin(pVtxZ,trkPhi,trkEta);
	    WgtNUA  = fHCorrectNUAposChrg[cForNUA]->GetBinContent(iBinNUA);
	  }
	  if(isItPion && fHCorrectNUAposPion[cForNUA]){
	    iBinNUA     = fHCorrectNUAposPion[cForNUA]->FindBin(pVtxZ,trkPhi,trkEta);
	    WgtNUAPion  = fHCorrectNUAposPion[cForNUA]->GetBinContent(iBinNUA);
	  }
	  if(isItKaon && fHCorrectNUAposKaon[cForNUA]){
	    iBinNUA     = fHCorrectNUAposKaon[cForNUA]->FindBin(pVtxZ,trkPhi,trkEta);
	    WgtNUAKaon  = fHCorrectNUAposKaon[cForNUA]->GetBinContent(iBinNUA);
	  }
	  if(isItProt && fHCorrectNUAposProt[cForNUA]){
	    iBinNUA     = fHCorrectNUAposProt[cForNUA]->FindBin(pVtxZ,trkPhi,trkEta);
	    WgtNUAProt  = fHCorrectNUAposProt[cForNUA]->GetBinContent(iBinNUA);
	  }       	  
	}
	else{
	  ///Tracking:
	  if(fHCorrectMCnegChrg){
	    ptBinMC = fHCorrectMCnegChrg->FindBin(trkPt);    
	    ptWgtMC = 1.0/fHCorrectMCnegChrg->GetBinContent(ptBinMC);
	  }
	  if(isItPion && fHCorrectMCnegPion){
	    ptBinMC     = fHCorrectMCnegPion->FindBin(trkPt);    
	    ptWgtMCPion = 1.0/fHCorrectMCnegPion->GetBinContent(ptBinMC);
	  }
	  if(isItKaon && fHCorrectMCnegKaon){
	    ptBinMC     = fHCorrectMCnegKaon->FindBin(trkPt);    
	    ptWgtMCKaon = 1.0/fHCorrectMCnegKaon->GetBinContent(ptBinMC);
	  }
	  if(isItProt && fHCorrectMCnegProt){
	    ptBinMC     = fHCorrectMCnegProt->FindBin(trkPt);    
	    ptWgtMCProt = 1.0/fHCorrectMCnegProt->GetBinContent(ptBinMC);
	  }
	  
	  ////NUA:
	  if(fHCorrectNUAnegChrg[cForNUA]){
	    iBinNUA = fHCorrectNUAnegChrg[cForNUA]->FindBin(pVtxZ,trkPhi,trkEta);
	    WgtNUA  = fHCorrectNUAnegChrg[cForNUA]->GetBinContent(iBinNUA);  
	  }
	  if(isItPion && fHCorrectNUAnegPion[cForNUA]){
	    iBinNUA     = fHCorrectNUAnegPion[cForNUA]->FindBin(pVtxZ,trkPhi,trkEta);
	    WgtNUAPion  = fHCorrectNUAnegPion[cForNUA]->GetBinContent(iBinNUA);
	  }
	  if(isItKaon && fHCorrectNUAnegKaon[cForNUA]){
	    iBinNUA     = fHCorrectNUAnegKaon[cForNUA]->FindBin(pVtxZ,trkPhi,trkEta);
	    WgtNUAKaon  = fHCorrectNUAnegKaon[cForNUA]->GetBinContent(iBinNUA);
	  }
	  if(isItProt && fHCorrectNUAnegProt[cForNUA]){
	    iBinNUA     = fHCorrectNUAnegProt[cForNUA]->FindBin(pVtxZ,trkPhi,trkEta);
	    WgtNUAProt  = fHCorrectNUAnegProt[cForNUA]->GetBinContent(iBinNUA);
	  }	  
	}


	trkWgt     = WgtNUA*ptWgtMC;
	trkWgtPion = WgtNUAPion*ptWgtMCPion;
	trkWgtKaon = WgtNUAKaon*ptWgtMCKaon;
	trkWgtProt = WgtNUAProt*ptWgtMCProt;


	// if(iTrack%10==0){
	//  std::cout<<" pT = "<<trkPt<<"\t MCPi = "<<ptWgtMCPion<<"\t MCK = "<<ptWgtMCKaon<<"\t MCProt = "<<ptWgtMCProt<<std::endl;
	//  std::cout<<" Eta = "<<trkEta<<"\t NUAPi = "<<WgtNUAPion<<"\t NUAK = "<<WgtNUAKaon<<"\t NUAProt = "<<WgtNUAProt<<std::endl;
	//  std::cout<<" totPi = "<<trkWgtPion<<"\t totK = "<<trkWgtKaon<<"\t totP = "<<trkWgtProt<<"\t totChrg = "<<trkWgt<<std::endl;	 
	// }
	///----------------------------------------------------------------	




    
	uqRe = TMath::Cos(gPsiN*trkPhi);
	uqIm = TMath::Sin(gPsiN*trkPhi);
	
	//----> Remove Track from EP calculation ------
	sumQxTPCneg = fSumTPCQn2xNeg;   
	sumQyTPCneg = fSumTPCQn2yNeg;
	sumQxTPCpos = fSumTPCQn2xPos;
	sumQyTPCpos = fSumTPCQn2yPos;
    
	sumWgtneg = fSumWgtEtaNeg;
	sumWgtpos = fSumWgtEtaPos;

	//// remove AutoCorrelation:
	if(trkEta < fEtaGapNeg){
	  sumQxTPCneg -= trkWgt*uqRe;
	  sumQyTPCneg -= trkWgt*uqIm; 
	  sumWgtneg   -= trkWgt;
	}
	else if(trkEta > fEtaGapPos){
	  sumQxTPCpos -= trkWgt*uqRe;
	  sumQyTPCpos -= trkWgt*uqIm; 
	  sumWgtpos   -= trkWgt;
	}
	//---------------------------------------------



	


	
	if(trkChrg > 0){

	  fHistv2AchChrgPos[0][iCent]->Fill(fAchrgNet,   (uqRe*sumQxTPCneg + uqIm*sumQyTPCneg)/sumWgtneg, trkWgt); //This Trk weigth is for uQ

	  if(isItPion){
	    fHistv2AchPionPos[0][iCent]->Fill(fAchrgNet, (uqRe*sumQxTPCneg + uqIm*sumQyTPCneg)/sumWgtneg, trkWgtPion);
	  }
	  if(isItKaon){
	    fHistv2AchKaonPos[0][iCent]->Fill(fAchrgNet, (uqRe*sumQxTPCneg + uqIm*sumQyTPCneg)/sumWgtneg, trkWgtKaon);
	  }
	  if(isItProt){
	    fHistv2AchProtPos[0][iCent]->Fill(fAchrgNet, (uqRe*sumQxTPCneg + uqIm*sumQyTPCneg)/sumWgtneg, trkWgtProt);	    
	  }
	}
	else{
	  
	  fHistv2AchChrgNeg[0][iCent]->Fill(fAchrgNet,   (uqRe*sumQxTPCneg + uqIm*sumQyTPCneg)/sumWgtneg, trkWgt);

	  if(isItPion){
	    fHistv2AchPionNeg[0][iCent]->Fill(fAchrgNet, (uqRe*sumQxTPCneg + uqIm*sumQyTPCneg)/sumWgtneg, trkWgtPion);
	  }
	  if(isItKaon){
	    fHistv2AchKaonNeg[0][iCent]->Fill(fAchrgNet, (uqRe*sumQxTPCneg + uqIm*sumQyTPCneg)/sumWgtneg, trkWgtKaon);
	  }
	  if(isItProt){
	    fHistv2AchProtNeg[0][iCent]->Fill(fAchrgNet, (uqRe*sumQxTPCneg + uqIm*sumQyTPCneg)/sumWgtneg, trkWgtProt);	    
	  }
	}


	
	
	fHistEtaPhiAfterCut->Fill(trkPhi,trkEta,trkWgt);


	
	//<-------------------Prottay analysis Done-----------------------



      }//with all trackCuts applied      
    }//-------> if FB is validated
    
  }///------> 2nd Track loop Ends here.<--------
 



  

  
  //Last lines in Event loop
  fCentDistAfterCut->Fill(centrality);
  fHistEventCount->Fill(14.5); //final Event count.
  PostData(1,fListHist);
}//---------------- UserExec ----------------------














void AliAnalysisTaskCVE::SetupEventAndTaskConfigInfo(){
  fHistEventCount = new TH1F("fHistEventCount","Event counts",15,0,15);
  fHistEventCount->GetXaxis()->SetBinLabel(1,"Called UserExec()");
  fHistEventCount->GetXaxis()->SetBinLabel(2,"Called Exec()");
  fHistEventCount->GetXaxis()->SetBinLabel(3,"AOD Exist");
  fHistEventCount->GetXaxis()->SetBinLabel(4,"Vz < 10");
  fHistEventCount->GetXaxis()->SetBinLabel(5,Form("%2.0f<Cent<%2.0f",fCentralityMin,fCentralityMax));
  fHistEventCount->GetXaxis()->SetBinLabel(6,"noAODtrack > 2 ");
  fHistEventCount->GetXaxis()->SetBinLabel(7,"TPC vs Global");
  fHistEventCount->GetXaxis()->SetBinLabel(8,"TPC128 vs ESD");
  fHistEventCount->GetXaxis()->SetBinLabel(9,"Cent vs TPC");
  fHistEventCount->GetXaxis()->SetBinLabel(10,"mult eta+/- > 2");
  fHistEventCount->GetXaxis()->SetBinLabel(11,"centCL1 < 90");
  fHistEventCount->GetXaxis()->SetBinLabel(15,"Survived Events");
  fListHist->Add(fHistEventCount);
  //fHistEventCount->Fill(1);
}//----------SetupEventAndTaskConfigInfo-----------




Int_t AliAnalysisTaskCVE::GetCentralityScaled0to10(Double_t fCent){

 Int_t cIndex = 0;

 if(fCent<5.0) {
   cIndex  = 0; 
 }
 else if(fCent>=5.0 && fCent<10){
   cIndex  = 1;
 }
 else if(fCent>=10.0) {
   cIndex = abs(fCent/10.0)+1;
 }
 return cIndex;
 
}//------------GetCentralityScaled0to10------------

void AliAnalysisTaskCVE::GetNUACorrectionHist(Int_t run, Int_t kParticleID)
{

  if(fListNUACorr){

    if(kParticleID==0){ //charge
      for(int i=0;i<5;i++){
	fHCorrectNUAposChrg[i] = (TH3F *) fListNUACorr->FindObject(Form("fHist_NUA_VzPhiEta_Charge_Pos_Cent%d_Run%d",i,run)); 
	fHCorrectNUAnegChrg[i] = (TH3F *) fListNUACorr->FindObject(Form("fHist_NUA_VzPhiEta_Charge_Neg_Cent%d_Run%d",i,run));
      }
    }
    else if(kParticleID==1){ //Pion
      for(int i=0;i<5;i++){
	fHCorrectNUAposPion[i] = (TH3F *) fListNUACorr->FindObject(Form("fHist_NUA_VzPhiEta_Pion_Pos_Cent%d_Run%d",i,run)); 
	fHCorrectNUAnegPion[i] = (TH3F *) fListNUACorr->FindObject(Form("fHist_NUA_VzPhiEta_Pion_Neg_Cent%d_Run%d",i,run));
      }
    }
    else if(kParticleID==2){ //Kaon
      for(int i=0;i<5;i++){
	fHCorrectNUAposKaon[i] = (TH3F *) fListNUACorr->FindObject(Form("fHist_NUA_VzPhiEta_Kaon_Pos_Cent%d_Run%d",i,run)); 
	fHCorrectNUAnegKaon[i] = (TH3F *) fListNUACorr->FindObject(Form("fHist_NUA_VzPhiEta_Kaon_Neg_Cent%d_Run%d",i,run));
      }
    }
    else if(kParticleID==3){ //Proton
      for(int i=0;i<5;i++){
	fHCorrectNUAposProt[i] = (TH3F *) fListNUACorr->FindObject(Form("fHist_NUA_VzPhiEta_Proton_Pos_Cent%d_Run%d",i,run)); 
	fHCorrectNUAnegProt[i] = (TH3F *) fListNUACorr->FindObject(Form("fHist_NUA_VzPhiEta_Proton_Neg_Cent%d_Run%d",i,run));
      }
    }
    else{
      for(int i=0;i<5;i++){
	fHCorrectNUAposChrg[i]=NULL;
	fHCorrectNUAnegChrg[i]=NULL;
	fHCorrectNUAposPion[i]=NULL;
	fHCorrectNUAnegPion[i]=NULL;
	fHCorrectNUAposKaon[i]=NULL;
	fHCorrectNUAnegKaon[i]=NULL;
	fHCorrectNUAposProt[i]=NULL;
	fHCorrectNUAnegProt[i]=NULL;
      }
    }
    
    
    // if(fHCorrectNUAposChrg[3] && fHCorrectNUAnegChrg[3]){
    //   cout<<"\n=========== Info:: Setting up NUA corrections for Charge Particle, run = "<<run<<"============"<<endl; 
    // }
    // if(fHCorrectNUAposPion[3] && fHCorrectNUAposPion[3]){
    //   cout<<"\n=========== Info:: Setting up NUA corrections for Charged Pions, run = "<<run<<"============"<<endl;
    // }
    // if(fHCorrectNUAposKaon[3] && fHCorrectNUAnegKaon[3]){
    //   cout<<"\n=========== Info:: Setting up NUA corrections for Charged Kaons, run = "<<run<<"============"<<endl;
    // }
    // if(fHCorrectNUAposProt[3] && fHCorrectNUAnegProt[3]){
    //   cout<<"\n=========== Info:: Setting up NUA corrections for proton (anti-p), run = "<<run<<"============"<<endl;
    // }  
  }//------> if list Exist

  // else {
  //   printf("\n ******** Warning: No NUA Correction File/List...!! \n Run= %d, Use NUA Wgt = 1.0 ********* \n",run);
  // }
}



////---------- SetUp Tracking Efficiency Correction Map ---------------

void AliAnalysisTaskCVE::GetMCCorrectionHist(Int_t run){

  if(fListTRKCorr) {
    //cout<<"\n =========> Info: Found TList with MC Tracking Corr Histograms <=========== "<<endl;
    fHCorrectMCposChrg =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyChrgPos");
    fHCorrectMCposPion =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyPionPos");
    fHCorrectMCposKaon =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyKaonPos");
    fHCorrectMCposProt =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyProtPos");

    fHCorrectMCnegChrg =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyChrgNeg");
    fHCorrectMCnegPion =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyPionNeg");
    fHCorrectMCnegKaon =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyKaonNeg");
    fHCorrectMCnegProt =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyProtNeg");

    //for(int i=0;i<10;i++) {
    //fFB_Efficiency_Cent[i] = (TH1D *) fListFBHijing->FindObject(Form("eff_unbiased_%d",i));
    //}
  }
  // else if(!fListTRKCorr){
  //   std::cout<<"\n\n !!!!**** Warning : No FB Efficiency Correction, run = "<<run<<"..!!!**** \n using MC TrkWgt = 1.0 \n"<<std::endl;
  // }
}
