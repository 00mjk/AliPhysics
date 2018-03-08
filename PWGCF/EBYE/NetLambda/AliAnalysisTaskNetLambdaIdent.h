#ifndef AliAnalysisTaskNetLambdaIdent_h
#define AliAnalysisTaskNetLambdaIdent_h

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// Analysis task for net-lambda fluctuations analysis
// Author: Alice Ohlson (alice.ohlson@cern.ch)

#include "AliAnalysisTaskSE.h"
//#include "AliNetLambdaHelper.h"
class TList;
class AliESDEvent;
class AliESDtrack;
class AliAnalysisUtils;
class AliPIDResponse;
class TTree;
class TH1;
class TH2;
class TH3;
class TH3F;
class AliLightV0;
#include "AliEventCuts.h"

class AliAnalysisTaskNetLambdaIdent : public AliAnalysisTaskSE {
 public:
  AliAnalysisTaskNetLambdaIdent(const char* name="AliAnalysisTaskNetLambdaIdent");
  virtual ~AliAnalysisTaskNetLambdaIdent(){};
  virtual void UserCreateOutputObjects();
  virtual void UserExec(Option_t *option);

  void SetCentCut(Float_t cut){centcut = cut;};
  void SetPtMinLambda(Float_t pt){ptminlambda = pt;};
  void SetEtaCutLambda(Float_t eta){etacutlambda = eta;};
  void SetCosPACut(Float_t cut){cospacut = cut;};
  void SetMinimumRadius(Float_t cut){minradius = cut;};
  void SetCrossedRowsCut(Float_t rows, Float_t ratio){ncrossedrows = rows; crossedrowsclustercut = ratio;};
  
  void SetIsMC(Bool_t val){fIsMC = val;};
  Bool_t GetIsMC(){return fIsMC;};
  void SetIsAOD(Bool_t val){fIsAOD = val;};
  Bool_t GetIsAOD(){return fIsAOD;};
  void SetEventSelection(UInt_t val) {fEvSel = val;}
  void MakeK0STree(Bool_t val){fK0STree = val;  Printf("************************* setting fK0STree = %i *************************",fK0STree);};
  void MakeLambdaTree(Bool_t val){fLambdaTree = val;};

 protected:
  AliAnalysisTaskNetLambdaIdent(const  AliAnalysisTaskNetLambdaIdent &task);
  AliAnalysisTaskNetLambdaIdent& operator=(const  AliAnalysisTaskNetLambdaIdent &task);
  
  AliESDEvent* fESD;
  AliAODEvent* fAOD;
  //AliAnalysisUtils* fUtils;   //! analysis utils to detect pileup
  AliPIDResponse* fPIDResponse; // points to class for PID
  AliEventCuts fEventCuts;      /// Event cuts
  TList* fListOfHistos;         //! list of output histograms
  TTree* fTree;                 //! output tree

  TH1I*  hEventStatistics;      //! cut-by-cut counter of events
  
  TH1F* hGenPt;
  TH1F* hGenPhi;
  TH1F* hGenEta;
  TH1F* hTrackPt;
  TH1F* hTrackPhi;
  TH1F* hTrackEta;
  TH2F* hNSigmaProton;
  TH2F* hArmPod;
      
  TH1F*  hLambdaPtGen;
  TH1F*  hAntiLambdaPtGen;
  TH1F*  hLambdaPtReco;
  TH1F*  hAntiLambdaPtReco;
  
  TH2F*  hInvMassLambda;
  TH2F*  hInvMassAntiLambda;
  TH2F*  hInvMassLambdaOnTheFly;
  TH2F*  hInvMassAntiLambdaOnTheFly;
  TH2F*  hInvMassLambdaReco;
  TH2F*  hInvMassAntiLambdaReco;
  TH2F*  hInvMassLambdaSecFromMaterial;
  TH2F*  hInvMassAntiLambdaSecFromMaterial;
  TH2F*  hInvMassLambdaSecFromWeakDecay;
  TH2F*  hInvMassAntiLambdaSecFromWeakDecay;
  TH2F*  hInvMassLike;
  //TH3F*  hInvMassPtPidLambda;
  //TH3F*  hInvMassPtPidAntiLambda;

  TH3F* hXiPlus;
  TH3F* hXiMinus;
  TH3F* hXiZero;
  TH3F* hXiZeroAnti;

  // kinematic cuts
  Float_t centcut;
  Float_t ptminlambda;
  Float_t etacutlambda;
  Float_t cospacut;
  Float_t minradius;
  Float_t ncrossedrows;
  Float_t crossedrowsclustercut;

  Float_t fCent;
  Float_t fNSpdTracklets;
  Double_t fVtxZ;
  Int_t fRunNumber;
  TClonesArray *fAcceptV0;
  TClonesArray *fGenLambda;
  TClonesArray *fGenCascade;

  Bool_t fIsMC;
  Bool_t fIsAOD;
  UInt_t fEvSel;
  Bool_t fK0STree;
  Bool_t fLambdaTree;
  
  const Float_t massPi = 0.139570;
  const Float_t massP = 0.938272;
  const Float_t massLambda = 1.115683;

  //AliMCEvent*              fMcEvent;    //! MC event
  //AliInputEventHandler*    fMcHandler;  //! MCEventHandler 
 
  ClassDef(AliAnalysisTaskNetLambdaIdent,1);
};

//_____________________________________________________________________________
class AliLightV0 : public TObject
{
 public:
 AliLightV0() : pt(-999), eta(-999), phi(-999), invmassL(-999), invmassAL(-999),
    cospt(-999), decayr(-999), decayl(-999), dcav0(-999), dcadaughters(-999), mcstatus(0),
    ppt(-999), peta(-999), pphi(-999), pnsigmapr(-999), pdca(-999), npt(-999), neta(-999), nphi(-999), nnsigmapr(-999), ndca(-999), cascpt(-999), casceta(-999) {};
 AliLightV0(Float_t ptin, Float_t etain, Float_t phiin) : pt(ptin), eta(etain), phi(phiin), invmassL(-999), invmassAL(-999),
    cospt(-999), decayr(-999), decayl(-999), dcav0(-999), dcadaughters(-999), mcstatus(0),
    ppt(-999), peta(-999), pphi(-999), pnsigmapr(-999), pdca(-999), npt(-999), neta(-999), nphi(-999), nnsigmapr(-999), ndca(-999), cascpt(-999), casceta(-999) {};
 AliLightV0(Float_t ptin, Float_t etain, Float_t phiin, Float_t invmassLin, Float_t invmassALin,
	    Float_t cosptin, Float_t decayrin, Float_t decaylin, Float_t dcav0in) : pt(ptin), eta(etain), phi(phiin), invmassL(invmassLin),
    invmassAL(invmassALin), cospt(cosptin), decayr(decayrin), decayl(decaylin), dcav0(dcav0in), dcadaughters(-999), mcstatus(0),
    ppt(-999), peta(-999), pphi(-999), pnsigmapr(-999), pdca(-999), npt(-999), neta(-999), nphi(-999), nnsigmapr(-999), ndca(-999), cascpt(-999), casceta(-999) {};
  virtual ~AliLightV0(){};
  void SetPt(Float_t val){pt = val;};
  void SetEta(Float_t val){eta = val;};
  void SetPhi(Float_t val){phi = val;};
  void SetInvMassLambda(Float_t val){invmassL = val;};
  void SetInvMassAntiLambda(Float_t val){invmassAL = val;};
  void SetCosPointingAngle(Float_t val){cospt = val;};
  void SetDecayR(Float_t val){decayr = val;};
  void SetDecayL(Float_t val){decayl = val;};
  void SetDCAV0(Float_t val){dcav0 = val;};
  void SetDCADaughters(Float_t val){dcadaughters = val;};
  void SetMcStatus(Int_t val){mcstatus = val;};
  void SetPosDaughter(Float_t pt, Float_t eta, Float_t phi, Float_t nsigma, Float_t dca){ppt = pt; peta = eta; pphi = phi; pnsigmapr = nsigma; pdca = dca;};
  void SetNegDaughter(Float_t pt, Float_t eta, Float_t phi, Float_t nsigma, Float_t dca){npt = pt; neta = eta; nphi = phi; nnsigmapr = nsigma; ndca = dca;};
  void SetCascadePtEta(Float_t pt, Float_t eta){cascpt = pt; casceta = eta;};
  
  Float_t GetPt(){return pt;};
  Float_t GetEta(){return eta;};
  Float_t GetPhi(){return phi;};
  Float_t GetInvMassLambda(){return invmassL;};
  Float_t GetInvMassAntiLambda(){return invmassAL;};
  Float_t GetCosPointingAngle(){return cospt;};
  Float_t GetDecayR(){return decayr;};
  Float_t GetDecayL(){return decayl;};
  Float_t GetDCAV0(){return dcav0;};
  Float_t GetDCADaughters(){return dcadaughters;};
  Int_t   GetMcStatus(){return mcstatus;};
  void    GetPosDaughter(Float_t& pt, Float_t& eta, Float_t& phi, Float_t& nsigma, Float_t& dca){pt = ppt; eta = peta; phi = pphi; nsigma = pnsigmapr; dca = pdca;};
  void    GetNegDaughter(Float_t& pt, Float_t& eta, Float_t& phi, Float_t& nsigma, Float_t& dca){pt = npt; eta = neta; phi = nphi; nsigma = nnsigmapr; dca = ndca;};
  Float_t GetCascadePt(){return cascpt;};
  Float_t GetCascadeEta(){return casceta;};

 private:
  Float_t   pt;
  Float_t   eta;
  Float_t   phi;
  Float_t   invmassL;
  Float_t   invmassAL;
  Float_t   cospt;
  Float_t   decayr;
  Float_t   decayl;
  Float_t   dcav0;
  Float_t   dcadaughters;
  Int_t     mcstatus;
  Float_t   ppt; // positive daughter properties
  Float_t   peta;
  Float_t   pphi;
  Float_t   pnsigmapr;
  Float_t   pdca;
  Float_t   npt; // negative daughter properties
  Float_t   neta;
  Float_t   nphi;
  Float_t   nnsigmapr;
  Float_t   ndca;
  Float_t   cascpt;
  Float_t   casceta;
  
  ClassDef(AliLightV0, 2);
};

//_____________________________________________________________________________
class AliLightGenV0 : public TObject
{
 public:
 AliLightGenV0() : pt(-999), eta(-999), phi(-999), id(-999), ppt(-999), peta(-999), pphi(-999), npt(-999), neta(-999), nphi(-999) {};
 AliLightGenV0(Float_t ptin, Float_t etain, Float_t phiin, Int_t idin) : pt(ptin), eta(etain), phi(phiin), id(idin), ppt(-999), peta(-999), pphi(-999), npt(-999), neta(-999), nphi(-999) {};
  virtual ~AliLightGenV0(){};
  void SetPt(Float_t val){pt = val;};
  void SetEta(Float_t val){eta = val;};
  void SetPhi(Float_t val){phi = val;};
  void SetId(Int_t val){id = val;}
  void SetPosDaughter(Float_t pt, Float_t eta, Float_t phi){ppt = pt; peta = eta; pphi = phi;};
  void SetNegDaughter(Float_t pt, Float_t eta, Float_t phi){npt = pt; neta = eta; nphi = phi;};
  
  Float_t GetPt(){return pt;};
  Float_t GetEta(){return eta;};
  Float_t GetPhi(){return phi;};
  Int_t   GetId(){return id;};
  void    GetPosDaughter(Float_t& pt, Float_t& eta, Float_t& phi){pt = ppt; eta = peta; phi = pphi;};
  void    GetNegDaughter(Float_t& pt, Float_t& eta, Float_t& phi){pt = npt; eta = neta; phi = nphi;};

 private:
  Float_t   pt;
  Float_t   eta;
  Float_t   phi;
  Int_t     id;
  Float_t   ppt; // positive daughter properties
  Float_t   peta;
  Float_t   pphi;
  Float_t   npt; // negative daughter properties
  Float_t   neta;
  Float_t   nphi;
  
  ClassDef(AliLightGenV0, 1);
};
#endif

