//
// Helper class to calculate Q cumulant in forward & central regions
//
#ifndef AliForwardGenericFramework_H
#define AliForwardGenericFramework_H
/**
 * @file AliForwardGenericFramework.h
 * @author Freja Thoresen <freja.thoresen@cern.ch>
 *
 * @brief
 *
 * @ingroup pwgcf_forward_flow
 */
#include "TObject.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TString.h"
#include "TNtuple.h"
#include "TList.h"
#include "TMath.h"
#include "TRandom.h"
#include "THn.h"
#include "TString.h"
#include "TComplex.h"

#include "AliForwardSettings.h"
#include "AliForwardNUATask.h"

#include <iostream>

/**
 * Class to handle cumulant calculations.
 */
class AliForwardGenericFramework
{
public:
  /*
  * Constructor
  */
  AliForwardGenericFramework();

  /**
   * Destructor
   */
  virtual ~AliForwardGenericFramework(){}


  AliForwardSettings fSettings;

    // Utility class for filling histograms
  /**
   * Do cumulants calculations for current event with
   * centrality cent
   *
   * @param cent Event centrality
   */
  void CumulantsAccumulate(TH2D*& dNdetadphi, double cent,double vertexpos,Bool_t useFMD,Bool_t doRefFlow, Bool_t doDiffFlow);

  void saveEvent(double cent, double vertexpos,UInt_t r, Int_t ptn);

  /**
   * Constants
   */
  enum {
    ktpcOnly = 128,        // TPC only tracks
    kphiAcceptanceBin = 21 // phi acceptance bin in the FMD histogram (dNdetadphi)
  };

  /**
   * Reset histograms
   */
  void reset();

  THnD* fQvector;//!     // Accumulated reference particles
  THnD* fpvector;//!    // Accumulated differential particles
  THnD* fqvector;//!    // Accumulated differential particles

  TComplex Q(Int_t n, Int_t p, Int_t etaBin);
  TComplex p(Int_t n, Int_t p, Int_t etaBin);
  TComplex q(Int_t n, Int_t p, Int_t etaBin);

  // TH1F fAutoRef;
  // TH1F fAutoDiff;

  TComplex Two(Int_t n1, Int_t n2, Int_t eta1, Int_t eta2);
  TComplex TwoDiff(Int_t n1, Int_t n2, Int_t refetabin, Int_t diffetabin);
  TComplex Four(Int_t n1, Int_t n2, Int_t n3, Int_t n4,Int_t eta1, Int_t eta2);
  TComplex FourDiff(Int_t n1, Int_t n2, Int_t n3, Int_t n4, Int_t refetabinA, Int_t refetabinB, Int_t diffetabin,Int_t qetabin);


  void fill(THnD*& cumu, Int_t n, Int_t ptn, Double_t sample,
                        Double_t zvertex,  
                        Double_t eta,     
                        Double_t cent,    
                        Double_t value)
  { 
    if (TMath::IsNaN(value)) return; 
    Double_t values[6] = {Double_t(n-2),Double_t(ptn),sample, zvertex, eta, cent};//kW4FourA
    cumu->Fill(values,value);
    return;
  }

  Double_t applyNUAcentral(Double_t eta, Double_t phi, Double_t zvertex, Double_t weight){
    if (!fSettings.use_primaries_cen) {
      Int_t nuaeta = fSettings.nuacentral->GetXaxis()->FindBin(eta);
      Int_t nuaphi = fSettings.nuacentral->GetYaxis()->FindBin(phi);
      Int_t nuavtz = fSettings.nuacentral->GetZaxis()->FindBin(zvertex);
      Double_t factor = fSettings.nuacentral->GetBinContent(nuaeta,nuaphi,nuavtz+10*fSettings.nua_runnumber);
      if (!(TMath::IsNaN(factor)) & (factor > 0.)) weight = weight*factor;
      else weight = 0.;
    }
    return weight;
  }


  Double_t applyNUAforward(TH2D*& dNdetadphi, Bool_t useFMD, Int_t etaBin, Int_t phiBin, Double_t eta, Double_t phi, Double_t zvertex, Double_t weight){

    // holes in the FMD
    if (fSettings.nua_mode & fSettings.kFill){
      if (etaBin >= 125 && etaBin <=137){
        if (phiBin == 17 || phiBin == 18) weight = 1.;
      }
      if (etaBin >= 168 && etaBin <=185){
        if (phiBin == 14) weight = 1.;
      }
    }

    if (fSettings.nua_mode & fSettings.kInterpolate) weight = AliForwardNUATask::InterpolateWeight(dNdetadphi,phiBin,etaBin,weight);
    if (!fSettings.use_primaries_fwd) {
      Int_t nuaeta = fSettings.nuaforward->GetXaxis()->FindBin(eta);
      Int_t nuaphi = fSettings.nuaforward->GetYaxis()->FindBin(phi);
      Int_t nuavtz = fSettings.nuaforward->GetZaxis()->FindBin(zvertex);
      Double_t factor = fSettings.nuaforward->GetBinContent(nuaeta,nuaphi,nuavtz+10*fSettings.nua_runnumber);
      if (!(TMath::IsNaN(factor)) & (factor > 0.)) weight = weight*factor;
      else weight = 0.;
    }
    return weight;
  }

  Double_t applySecondaryCorr(Int_t n, Double_t eta,Double_t zvertex,Double_t cent, Double_t weight){
    Int_t secn = n-1;//fSettings.seccorr_fwd->GetZaxis()->FindBin(n-2);

    if (fSettings.seccorr_fwd){
      Int_t seceta = fSettings.seccorr_fwd->GetZaxis()->FindBin(eta);
      Int_t secvtz = fSettings.seccorr_fwd->GetYaxis()->FindBin(zvertex);
      Double_t factor = fSettings.seccorr_fwd->GetBinContent(secn,secvtz,seceta);
      if (!(TMath::IsNaN(factor)) & (factor > 0.)) weight = weight*factor;
      else weight = 0.;
    }
    if (fSettings.seccorr_cent){
      Int_t seceta = fSettings.seccorr_cent->GetYaxis()->FindBin(eta);
      Int_t seccent = fSettings.seccorr_cent->GetZaxis()->FindBin(cent);
      Double_t factor = fSettings.seccorr_cent->GetBinContent(secn,seceta,seccent);
      if (!(TMath::IsNaN(factor)) & (factor > 0.)) weight = weight*factor;
      else weight = 0.;      
    }
    return weight;
  }

  THnD* cumu_dW2A        ;//!  // multiplicity for all particles in subevent A (note subevent A can also be the entire event)
  THnD* cumu_dW2TwoA     ;//!  // <w2*two>
  THnD* cumu_dW2B        ;//!  // multiplicity for all particles in subevent B (note subevent B can NOT be the entire event)
  THnD* cumu_dW2TwoB     ;//!  // <w2*two>  Int_t kW4          = 3; // <w4>
  THnD* cumu_dW4         ;//! 
  THnD* cumu_dW4Four     ;//! 

  THnD* cumu_dW4FourTwo  ;//! 
  THnD* cumu_dW4ThreeTwo ;//! 
  //THnD* cumu_dW4_mixed   ;//! 
  THnD* cumu_dWTwoTwoN   ;//!  // Numerator of R_{n,n; 2}
  THnD* cumu_dWTwoTwoD   ;//!  // Denominator of R_{n,n; 2}

  THnD* cumu_rW2         ;//!  // multiplicity for all particles in subevent A (note subevent A can also be the entire event)
  THnD* cumu_rW2Two      ;//!  // <w2*two>
  THnD* cumu_rW4         ;//! 
  THnD* cumu_rW4Four     ;//!   

  ClassDef(AliForwardGenericFramework, 1); // object for eta dependent cumulant ananlysis
};

#endif
