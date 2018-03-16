/*
 * AliFemtoDreamCorrHists.cxx
 *
 *  Created on: Sep 12, 2017
 *      Author: gu74req
 */
#include <iostream>
#include <vector>
#include "AliFemtoDreamCorrHists.h"
#include "TMath.h"
ClassImp(AliFemtoDreamCorrHists)
AliFemtoDreamCorrHists::AliFemtoDreamCorrHists()
:fQA(0)
,fResults(0)
,fPairs(0)
,fPairQA(0)
,fMinimalBooking(false)
,fMomentumResolution(false)
,fPhiEtaPlots(false)
,fSameEventDist(0)
,fSameEventMultDist(0)
,fSameEventkTDist(0)
,fSameEventmTDist(0)
,fPairCounterSE(0)
,fMixedEventDist(0)
,fMixedEventMultDist(0)
,fMixedEventkTDist(0)
,fMixedEventmTDist(0)
,fPairCounterME(0)
,fMomResolution(0)
,fRadiiEtaPhiSE(0)
,fRadiiEtaPhiME(0)
,fDoMultBinning(false)
,fDokTBinning(false)
,fDomTBinning(false)
{
}

AliFemtoDreamCorrHists::AliFemtoDreamCorrHists(
    AliFemtoDreamCollConfig *conf,bool MinimalBooking) {
  fMinimalBooking=MinimalBooking;
  fMomentumResolution=conf->GetDoMomResolution();
  fDoMultBinning=conf->GetDoMultBinning();
  fDokTBinning=conf->GetDokTBinning();
  fDomTBinning=conf->GetDomTBinning();
  fPhiEtaPlots=conf->GetDoPhiEtaBinning();
  int multbins=conf->GetNMultBins();
  int nParticles=conf->GetNParticles();
  const int nHists=conf->GetNParticleCombinations();
  std::vector<int> NBinsHist=conf->GetNBinsHist();
  std::vector<int>::iterator itNBins=NBinsHist.begin();
  std::vector<float> kRelMin=conf->GetMinKRel();
  std::vector<float>::iterator itKMin=kRelMin.begin();
  std::vector<float> kRelMax=conf->GetMaxKRel();
  std::vector<float>::iterator itKMax=kRelMax.begin();

  if (nHists!=(int)NBinsHist.size() || nHists!=(int)kRelMin.size() ||
      nHists!=(int)kRelMax.size()) {
    //Todo: Replace by AliFatal!
    std::cout<<"Initialzing the bin sizes, something went horribly wrong!"<<std::endl;
  }
  //The way the histograms are assigned later is going to be for example for
  //4 different particle species X1,X2,X3,X4:
  //    X1  X2  X3  X4
  //X1  1   2   3   4
  //X2      5   6   7
  //X3          8   9
  //X4              10<-----Number of the Histogram=Position in input vector
  //X1 corresponds the first particle array in your vector that you hand over
  //in the AliFemtoDreamPartCollection::SetEvent Method, X2 to the second and
  //so on.
  //in case we only book the most basic things we don't need this
  fResults=new TList();
  fResults->SetName("Results");
  fResults->SetOwner();

  if (!fMinimalBooking) {
    fQA=new TList();
    fQA->SetName("PairQA");
    fQA->SetOwner();

    fPairQA=new TList*[nHists];
    fPairCounterSE=new TH2F*[nHists];
    fPairCounterME=new TH2F*[nHists];
    if (fMomentumResolution) {
      fMomResolution=new TH2F*[nHists];
    } else {
      fMomResolution = 0;
    }
    if (fPhiEtaPlots) {
      fRadiiEtaPhiSE=new TH2F***[nHists];
      fRadiiEtaPhiME=new TH2F***[nHists];
    } else {
      fRadiiEtaPhiSE=0;
      fRadiiEtaPhiME=0;
    }
  } else {
    fQA=0;
    fPairQA=0;
    fPairCounterSE=0;
    fPairCounterME=0;
    fMomResolution=0;
    fMomResolution = 0;
    fRadiiEtaPhiSE=0;
    fRadiiEtaPhiME=0;
  }
  //we always want to do this, regardless of the booking type!
  fPairs=new TList*[nHists];
  fSameEventDist=new TH1F*[nHists];
  fMixedEventDist=new TH1F*[nHists];
  if (fDoMultBinning) {
    fSameEventMultDist=new TH2F*[nHists];
    fMixedEventMultDist=new TH2F*[nHists];
  } else {
    fSameEventMultDist=0;
    fMixedEventMultDist=0;
  }
  if (fDokTBinning) {
    fSameEventkTDist=new TH2F*[nHists];
    fMixedEventkTDist=new TH2F*[nHists];
  } else {
    fSameEventkTDist=0;
    fMixedEventkTDist=0;
  }
  if (fDomTBinning) {
    fSameEventmTDist=new TH2F*[nHists];
    fMixedEventmTDist=new TH2F*[nHists];
  } else {
    fSameEventmTDist=0;
    fMixedEventmTDist=0;
  }
  int Counter=0;
  for (int iPar1 = 0; iPar1 < nParticles; ++iPar1) {
    for (int iPar2 = iPar1; iPar2 < nParticles; ++iPar2) {
      fPairs[Counter]=new TList();
      TString PairFolderName=Form("Particle%d_Particle%d",iPar1,iPar2);
      fPairs[Counter]->SetName(PairFolderName.Data());
      fPairs[Counter]->SetOwner();
      fResults->Add(fPairs[Counter]);

      TString SameEventName=Form("SEDist_Particle%d_Particle%d",iPar1,iPar2);
      fSameEventDist[Counter]=new TH1F(SameEventName.Data(),
                                       SameEventName.Data(),
                                       *itNBins,*itKMin,*itKMax);
      fSameEventDist[Counter]->Sumw2();
      fPairs[Counter]->Add(fSameEventDist[Counter]);

      TString MixedEventName=Form("MEDist_Particle%d_Particle%d",iPar1,iPar2);
      fMixedEventDist[Counter]=new TH1F(MixedEventName.Data(),
                                        MixedEventName.Data(),
                                        *itNBins,*itKMin,*itKMax);
      fMixedEventDist[Counter]->Sumw2();
      fPairs[Counter]->Add(fMixedEventDist[Counter]);

      if (fDoMultBinning) {
        TString SameMultEventName=
            Form("SEMultDist_Particle%d_Particle%d",iPar1,iPar2);
        fSameEventMultDist[Counter]=new TH2F(SameMultEventName.Data(),
                                             SameMultEventName.Data(),
                                             *itNBins,*itKMin,*itKMax,
                                             multbins,1,multbins+1);
        fSameEventMultDist[Counter]->Sumw2();
        fPairs[Counter]->Add(fSameEventMultDist[Counter]);

        TString MixedMultEventName=
            Form("MEMultDist_Particle%d_Particle%d",iPar1,iPar2);
        fMixedEventMultDist[Counter]=new TH2F(MixedMultEventName.Data(),
                                              MixedMultEventName.Data(),
                                              *itNBins,*itKMin,*itKMax,
                                              multbins,1,multbins+1);
        fMixedEventMultDist[Counter]->Sumw2();
        fPairs[Counter]->Add(fMixedEventMultDist[Counter]);

      }

      if (fDokTBinning) {
        TString SamekTEventName=
            Form("SEkTDist_Particle%d_Particle%d",iPar1,iPar2);
        fSameEventkTDist[Counter]=new TH2F(SamekTEventName.Data(),
                                             SamekTEventName.Data(),
                                             *itNBins*1.5,*itKMin,*itKMax*1.5,
                                             *itNBins*1.5,*itKMin,*itKMax*1.5);
        fSameEventkTDist[Counter]->Sumw2();
        fPairs[Counter]->Add(fSameEventkTDist[Counter]);

        TString MixedkTEventName=
            Form("MEkTDist_Particle%d_Particle%d",iPar1,iPar2);
        fMixedEventkTDist[Counter]=new TH2F(MixedkTEventName.Data(),
                                              MixedkTEventName.Data(),
                                              *itNBins*1.5,*itKMin,*itKMax*1.5,
                                              *itNBins*1.5,*itKMin,*itKMax*1.5);
        fMixedEventkTDist[Counter]->Sumw2();
        fPairs[Counter]->Add(fMixedEventkTDist[Counter]);

      }

      if (fDomTBinning) {
        TString SamemTEventName=
            Form("SEmTDist_Particle%d_Particle%d",iPar1,iPar2);
        fSameEventmTDist[Counter]=new TH2F(SamemTEventName.Data(),
                                             SamemTEventName.Data(),
                                             *itNBins*1.5,*itKMin,*itKMax*1.5,
                                             *itNBins*1.5,*itKMin,*itKMax*1.5);
        fSameEventmTDist[Counter]->Sumw2();
        fPairs[Counter]->Add(fSameEventmTDist[Counter]);

        TString MixedmTEventName=
            Form("MEmTDist_Particle%d_Particle%d",iPar1,iPar2);
        fMixedEventmTDist[Counter]=new TH2F(MixedmTEventName.Data(),
                                              MixedmTEventName.Data(),
                                              *itNBins*1.5,*itKMin,*itKMax*1.5,
                                              *itNBins*1.5,*itKMin,*itKMax*1.5);
        fMixedEventmTDist[Counter]->Sumw2();
        fPairs[Counter]->Add(fMixedEventmTDist[Counter]);

      }
      if (!fMinimalBooking) {
        fPairQA[Counter]=new TList();
        TString PairQAName=Form("QA_Particle%d_Particle%d",iPar1,iPar2);
        fPairQA[Counter]->SetName(PairQAName.Data());
        fPairQA[Counter]->SetOwner();
        fQA->Add(fPairQA[Counter]);
        TString PairCounterSEName=
            Form("SEPairs_Particle%d_Particle%d",iPar1,iPar2);
        fPairCounterSE[Counter]=new TH2F(
            PairCounterSEName.Data(),PairCounterSEName.Data(),20,0,20,20,0,20);
        fPairCounterSE[Counter]->Sumw2();
        fPairCounterSE[Counter]->GetXaxis()->SetTitle(Form("Particle%d",iPar1));
        fPairCounterSE[Counter]->GetYaxis()->SetTitle(Form("Particle%d",iPar2));
        fPairQA[Counter]->Add(fPairCounterSE[Counter]);

        TString PairCounterMEName=
            Form("MEPairs_Particle%d_Particle%d",iPar1,iPar2);
        fPairCounterME[Counter]=new TH2F(
            PairCounterMEName.Data(),PairCounterMEName.Data(),20,0,20,20,0,20);
        fPairCounterME[Counter]->Sumw2();
        fPairCounterME[Counter]->GetXaxis()->SetTitle(Form("Particle%d",iPar1));
        fPairCounterME[Counter]->GetYaxis()->SetTitle(Form("Particle%d",iPar2));
        fPairQA[Counter]->Add(fPairCounterME[Counter]);

        if (fMomentumResolution) {
          //take twice the number of bins we use for the CF to be sure, the range is
          //hard coded. This assumed that the input is in GeV!
          //          *itNBins,*itKMin,*itKMax,
          double dNBin=(*itKMax-*itKMin)/(double)(*itNBins);
          dNBin/=2;
          int nBims=(int)(1/dNBin);
          TString MomResoName=
              Form("MomentumResolution_Particle%d_Particle%d",iPar1,iPar2);
          fMomResolution[Counter]=
              new TH2F(MomResoName.Data(),MomResoName.Data(),nBims,0,1,
                       nBims,0,1);
          fMomResolution[Counter]->Sumw2();
          fMomResolution[Counter]->GetXaxis()->SetTitle("k_{Generated}");
          fMomResolution[Counter]->GetYaxis()->SetTitle("k_{Reco}");
          fPairQA[Counter]->Add(fMomResolution[Counter]);
        }
        if (fPhiEtaPlots) {
          fRadiiEtaPhiSE[Counter]=new TH2F**[3];
          fRadiiEtaPhiME[Counter]=new TH2F**[3];
          for (int iDaug=0;iDaug<3;++iDaug) {
          const int nRad=conf->GetNRadii();
          fRadiiEtaPhiSE[Counter][iDaug]=new TH2F*[nRad];
          fRadiiEtaPhiME[Counter][iDaug]=new TH2F*[nRad];
            for (int iRad=0;iRad<nRad;++iRad) {
              TString RadNameSE=Form("SERad_%i_Particle%d_Particle%d_Daug%d",iRad,iPar1,iPar2,iDaug);
              TString RadNameME=Form("MERad_%i_Particle%d_Particle%d_Daug%d",iRad,iPar1,iPar2,iDaug);
              fRadiiEtaPhiSE[Counter][iDaug][iRad]=
                  new TH2F(RadNameSE.Data(),RadNameSE.Data(),400,0,4,400,0,1);
              fRadiiEtaPhiSE[Counter][iDaug][iRad]->GetXaxis()->SetTitle("#Delta#eta");
              fRadiiEtaPhiSE[Counter][iDaug][iRad]->GetYaxis()->SetTitle("#Delta#phi");
              fPairQA[Counter]->Add(fRadiiEtaPhiSE[Counter][iDaug][iRad]);
              fRadiiEtaPhiME[Counter][iDaug][iRad]=
                  new TH2F(RadNameME.Data(),RadNameME.Data(),400,0,4,400,0,1);
              fRadiiEtaPhiME[Counter][iDaug][iRad]->GetXaxis()->SetTitle("#Delta#eta");
              fRadiiEtaPhiME[Counter][iDaug][iRad]->GetYaxis()->SetTitle("#Delta#phi");
              fPairQA[Counter]->Add(fRadiiEtaPhiME[Counter][iDaug][iRad]);
            }
          }
        }
      }

      ++Counter;
      ++itNBins;
      ++itKMin;
      ++itKMax;
    }
  }
}

AliFemtoDreamCorrHists::~AliFemtoDreamCorrHists() {
  if (fPairs) {
    delete [] fPairs;
    delete fPairs;
  }
  if (fSameEventDist) {
    delete [] fSameEventDist;
    delete fSameEventDist;
  }
  if (fMixedEventDist) {
    delete [] fMixedEventDist;
    delete fMixedEventDist;
  }
}

