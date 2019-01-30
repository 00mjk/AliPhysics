
//
// Calculate flow in the forward and central regions using the Q cumulants method.
//
// Inputs:
//  - AliAODEvent
//
// Outputs:
//  - AnalysisResults.root or forward_flow.root
//
#include <iostream>
#include <TROOT.h>
#include <TSystem.h>
#include <TInterpreter.h>
#include <TList.h>
#include <THn.h>
#include "AliGenEventHeaderTunedPbPb.h"
#include "TGraph.h"

#include <vector>
#include <numeric>
#include <assert.h>

#include "THn.h"
#include "TMath.h"
#include "TCutG.h"
#include "TParticle.h"

#include "AliStack.h"
#include "AliMCEvent.h"
#include "AliMCParticle.h"

#include "AliGenEventHeaderTunedPbPb.h"
#include "AliAODMCHeader.h"

#include "AliLog.h"
#include "AliForwardSecondariesTask.h"
#include "AliForwardQCumulantRun2.h"
#include "AliForwardGenericFramework.h"

#include "AliAODForwardMult.h"
#include "AliAODCentralMult.h"
#include "AliAODEvent.h"

#include "AliForwardFlowUtil.h"

#include "AliVVZERO.h"
#include "AliAODVertex.h"
#include "AliCentrality.h"

#include "AliESDEvent.h"
#include "AliVTrack.h"
#include "AliESDtrack.h"
#include "AliAODTrack.h"
#include "AliAODTracklets.h"

#include "AliAnalysisFilter.h"
#include "AliMultSelection.h"
#include "AliMultiplicity.h"
#include "AliAnalysisManager.h"
#include "AliInputEventHandler.h"

using namespace std;
ClassImp(AliForwardSecondariesTask)
#if 0
; // For emacs
#endif

//_____________________________________________________________________
AliForwardSecondariesTask::AliForwardSecondariesTask() : AliAnalysisTaskSE(),
  fOutputList(0),    // output list
  fEventList(0),
    fDeltaList(0),
  fRandom(0),
  fSettings(),
  fUtil(),
  fMultTOFLowCut(),
  fMultTOFHighCut(),
  fMultCentLowCut(),
      fdNdeta(0),
    fPiCheck(0),
    fdNdetaOrigin(0),
    fxray(0),
    fNsecondaries(0),
    fNprimaries(0),
    fITS(0),
    fFMD1(0),
    fFMD2(0),
    fFMD3(0),
    fPipe(0),
    fEarlyDecay(0)
  {
  //
  //  Default constructor
  //
  }

  //_____________________________________________________________________
  AliForwardSecondariesTask::AliForwardSecondariesTask(const char* name) : AliAnalysisTaskSE(name),
  fOutputList(0),    // output list
  fEventList(0),
  fDeltaList(0),
  fRandom(0),
  fSettings(),
  fUtil(),
  fMultTOFLowCut(),
  fMultTOFHighCut(),
  fMultCentLowCut(),
      fdNdeta(0),
    fPiCheck(0),
    fdNdetaOrigin(0),
    fxray(0),
    fNsecondaries(0),
    fNprimaries(0),
    fITS(0),
    fFMD1(0),
    fFMD2(0),
    fFMD3(0),
    fPipe(0),
    fEarlyDecay(0)
  {
  //
  //  Constructor
  //
  //  Parameters:
  //   name: Name of task
  //
    DefineOutput(1, TList::Class());
  }

//_____________________________________________________________________
  void AliForwardSecondariesTask::UserCreateOutputObjects()
  {

  //
  //  Create output objects
  //
  std::cout << "AliForwardSecondariesTask::UserCreateOutputObjects()" << std::endl;

  this->fOutputList = new TList();
  this->fOutputList->SetOwner(kTRUE);

    TRandom fRandom = TRandom();              // random integer to use for creation of samples (used for error bars).
                                        // Needs to be created here, otherwise it will draw the same random number.

    fDeltaList = new TList();
    fDeltaList->SetName("Delta");

    Int_t bins_phi_eta[5] = {fSettings.fnoSamples, fSettings.fNZvtxBins, 51, fSettings.fCentBins, 200} ;
    Double_t xmin_phi_eta[5] = {0,fSettings.fZVtxAcceptanceLowEdge, -TMath::Pi(), 0, -4};
    Double_t xmax_phi_eta[5] = {10,fSettings.fZVtxAcceptanceUpEdge, TMath::Pi(), 100, 6}; //
    Int_t dimensions = 5;

    Int_t bins_eta_phi[5] = {fSettings.fnoSamples, fSettings.fNZvtxBins, 201, fSettings.fCentBins, 20} ;
    Double_t xmin_eta_phi[5] = {0,fSettings.fZVtxAcceptanceLowEdge, -4, 0, 0.0};
    Double_t xmax_eta_phi[5] = {10,fSettings.fZVtxAcceptanceUpEdge, 6, 100, 2*TMath::Pi()}; //

    Int_t bins_phi[5] = {fSettings.fnoSamples, fSettings.fNZvtxBins, 51, fSettings.fCentBins, 20} ;
    Double_t xmin_phi[5] = {0,fSettings.fZVtxAcceptanceLowEdge, -TMath::Pi(), 0.0, 0.0};
    Double_t xmax_phi[5] = {10,fSettings.fZVtxAcceptanceUpEdge, TMath::Pi(), 100, 2*TMath::Pi()}; //

    Int_t bins_eta[5] = {fSettings.fnoSamples, fSettings.fNZvtxBins, 201, fSettings.fCentBins, 200} ;
    Double_t xmin_eta[5] = {0,fSettings.fZVtxAcceptanceLowEdge, -4, 0, -4};
    Double_t xmax_eta[5] = {10,fSettings.fZVtxAcceptanceUpEdge, 6, 100, 6}; //

    fDeltaList->Add(new THnD("delta_phi_eta", "delta_phi_eta",dimensions,bins_phi_eta, xmin_phi_eta, xmax_phi_eta)); // (samples, vertex,phi_mother - phi_tr ,centrality,eta_mother,eta_tr,eta_p)
    fDeltaList->Add(new THnD("delta_eta_phi", "delta_eta_phi",dimensions,bins_eta_phi, xmin_eta_phi, xmax_eta_phi));// (samples, vertex,eta_mother - eta_tr ,centrality,phi_mother,phi_tr,phi_p)
    fDeltaList->Add(new THnD("delta_phi", "delta_phi",dimensions,bins_phi, xmin_phi, xmax_phi)); // (samples, vertex,phi_mother - phi_tr ,centrality,eta_mother,eta_tr,eta_p)
    fDeltaList->Add(new THnD("delta_eta", "delta_eta",dimensions,bins_eta, xmin_eta, xmax_eta));// (samples, vertex,eta_mother - eta_tr ,centrality,phi_mother,phi_tr,phi_p)

    static_cast<THnD*>(fDeltaList->FindObject("delta_phi_eta"))->GetAxis(0)->SetName("samples");
    static_cast<THnD*>(fDeltaList->FindObject("delta_phi_eta"))->GetAxis(1)->SetName("vertex");
    static_cast<THnD*>(fDeltaList->FindObject("delta_phi_eta"))->GetAxis(2)->SetName("phi_mother - phi_tr");
    static_cast<THnD*>(fDeltaList->FindObject("delta_phi_eta"))->GetAxis(3)->SetName("centrality");
    static_cast<THnD*>(fDeltaList->FindObject("delta_phi_eta"))->GetAxis(4)->SetName("eta");

    static_cast<THnD*>(fDeltaList->FindObject("delta_eta_phi"))->GetAxis(0)->SetName("samples");
    static_cast<THnD*>(fDeltaList->FindObject("delta_eta_phi"))->GetAxis(1)->SetName("vertex");
    static_cast<THnD*>(fDeltaList->FindObject("delta_eta_phi"))->GetAxis(2)->SetName("eta_mother - eta_tr");
    static_cast<THnD*>(fDeltaList->FindObject("delta_eta_phi"))->GetAxis(3)->SetName("centrality");
    static_cast<THnD*>(fDeltaList->FindObject("delta_eta_phi"))->GetAxis(4)->SetName("phi");

    static_cast<THnD*>(fDeltaList->FindObject("delta_phi"))->GetAxis(0)->SetName("samples");
    static_cast<THnD*>(fDeltaList->FindObject("delta_phi"))->GetAxis(1)->SetName("vertex");
    static_cast<THnD*>(fDeltaList->FindObject("delta_phi"))->GetAxis(2)->SetName("phi_mother - phi_tr");
    static_cast<THnD*>(fDeltaList->FindObject("delta_phi"))->GetAxis(3)->SetName("centrality");
    static_cast<THnD*>(fDeltaList->FindObject("delta_phi"))->GetAxis(4)->SetName("phi");

    static_cast<THnD*>(fDeltaList->FindObject("delta_eta"))->GetAxis(0)->SetName("samples");
    static_cast<THnD*>(fDeltaList->FindObject("delta_eta"))->GetAxis(1)->SetName("vertex");
    static_cast<THnD*>(fDeltaList->FindObject("delta_eta"))->GetAxis(2)->SetName("eta_mother - eta_tr");
    static_cast<THnD*>(fDeltaList->FindObject("delta_eta"))->GetAxis(3)->SetName("centrality");
    static_cast<THnD*>(fDeltaList->FindObject("delta_eta"))->GetAxis(4)->SetName("eta");

      //static_cast<THnD*>(static_cast<TList*>(fStdQCList->At(0))->FindObject(Form("cumuRef_v%d", n)))->GetAxis(0)->SetName("samples");

    fEventList = new TList();

    fEventList->Add(new TH1D("Centrality","Centrality",10,0,100));
    fEventList->Add(new TH1D("Vertex","Vertex",fSettings.fNZvtxBins,fSettings.fZVtxAcceptanceLowEdge,fSettings.fZVtxAcceptanceUpEdge));
    fEventList->Add(new TH2F("hOutliers","Maximum #sigma from mean N_{ch} pr. bin",
     20, 0., 100., 500, 0., 5.)); //((fFlags & kMC) ? 15. : 5. // Sigma <M> histogram
    fEventList->Add(new TH1D("FMDHits","FMDHits",100,0,10));
    fEventList->SetName("EventInfo");

    Int_t bins_prim[4] = {fSettings.fnoSamples, fSettings.fNZvtxBins, fSettings.fCentBins, 200} ;
    Double_t xmin_prim[4] = {0,fSettings.fZVtxAcceptanceLowEdge, 0, -4};
    Double_t xmax_prim[4] = {10,fSettings.fZVtxAcceptanceUpEdge, 100, 6}; //
    Int_t dimensions_prim = 4;
    fDeltaList->Add(new THnD("fnoPrim", "fnoPrim", dimensions_prim, bins_prim, xmin_prim, xmax_prim)); //(samples,vertex, phi, cent, eta)
    static_cast<THnD*>(fDeltaList->FindObject("fnoPrim"))->GetAxis(0)->SetName("samples");
    static_cast<THnD*>(fDeltaList->FindObject("fnoPrim"))->GetAxis(1)->SetName("vertex");
    static_cast<THnD*>(fDeltaList->FindObject("fnoPrim"))->GetAxis(2)->SetName("centrality");
    static_cast<THnD*>(fDeltaList->FindObject("fnoPrim"))->GetAxis(3)->SetName("eta_mother");

    fOutputList->Add(fEventList);
    fOutputList->Add(fDeltaList);

    // do analysis
    PostData(1, fOutputList);
  }


//_____________________________________________________________________
void AliForwardSecondariesTask::UserExec(Option_t *)
{
  //
  //  Parameters:
  //   option: Not used
  //
  AliMCEvent* fAOD = this->MCEvent();
  AliStack* stack = fAOD->Stack();
  if(!fAOD) {
    std::cout << "no aod" << std::endl;
    return;
  }
  if (!stack) {
    std::cout << "no stack" << std::endl;
    return;
  }

  // Disregard events without reconstructed vertex
  Float_t event_vtx_z = fAOD->GetPrimaryVertex()->GetZ();
  if (!(TMath::Abs(event_vtx_z) > 0)) {
    return;
  }

  AliMultSelection *MultSelection = (AliMultSelection*)fInputEvent->FindListObject("MultSelection");
  Double_t v0cent = MultSelection->GetMultiplicityPercentile("V0M");

  Int_t nTracks   = fAOD->GetNumberOfTracks();
  Int_t nPrim     = stack->GetNprimary();

  static_cast<TH1D*>(fEventList->FindObject("Centrality"))->Fill(v0cent);
  static_cast<TH1D*>(fEventList->FindObject("Vertex"))->Fill(event_vtx_z);
  THnD* delta_eta = static_cast<THnD*>(fDeltaList->FindObject("delta_eta")); // (samples, vertex,phi_mother - phi_tr ,centrality,eta_mother,eta_tr,eta_p)
  THnD* delta_phi = static_cast<THnD*>(fDeltaList->FindObject("delta_phi")); // (samples, vertex,phi_mother - phi_tr ,centrality,eta_mother,eta_tr,eta_p)
  THnD* delta_phi_eta = static_cast<THnD*>(fDeltaList->FindObject("delta_phi_eta")); // (samples, vertex,phi_mother - phi_tr ,centrality,eta_mother,eta_tr,eta_p)
  THnD* delta_eta_phi = static_cast<THnD*>(fDeltaList->FindObject("delta_eta_phi")); // (samples, vertex,phi_mother - phi_tr ,centrality,eta_mother,eta_tr,eta_p)
  THnD* fnoPrim = static_cast<THnD*>(fDeltaList->FindObject("fnoPrim"));//->Fill(event_vtx_z,event_vtx_z,event_vtx_z);

  bool useEvent = kTRUE;

  if (useEvent){
  Double_t randomInt = static_cast<Double_t>(fRandom.Integer(fSettings.fnoSamples));
  std::vector< Int_t > listOfMothers;

  for (Int_t iTr = 0; iTr < nTracks; iTr++) {
    AliMCParticle* p = static_cast< AliMCParticle* >(this->MCEvent()->GetTrack(iTr));

    // Ignore things that do not make a signal in the FMD
    AliTrackReference* tr = fUtil.IsHitFMD(p);
    if (tr && p->Charge() != 0){

      AliMCParticle* mother = GetMother(p);
      //if (!mother) continue;
      if (!mother) mother = p;

      Double_t eta_mother = mother->Eta();
      Double_t phi_mother = mother->Phi();

      Double_t *etaPhi = new Double_t[2];
      this->GetTrackRefEtaPhi(p, etaPhi);

      Double_t phi_tr = etaPhi[1];
      Double_t eta_tr = etaPhi[0];

      // (samples, vertex,phi_mother - phi_tr ,centrality,eta_mother,eta_tr)
      Double_t phi[5] = {randomInt,event_vtx_z, WrapPi(phi_mother - phi_tr), v0cent, eta_tr};
      Double_t eta[5] = {randomInt,event_vtx_z, (eta_mother - eta_tr), v0cent, phi_tr};

      //if (!(fabs(eta_tr - eta_mother) < 0.1)) continue;
      delta_phi_eta->Fill(phi,1);
      delta_eta_phi->Fill(eta,1);

      phi[4] = phi_tr;
      eta[4] = eta_tr;

      delta_phi->Fill(phi,1);
      delta_eta->Fill(eta,1);

      Double_t x_prim[4] =  {randomInt,event_vtx_z,v0cent,eta_tr};
      Bool_t isNewPrimary = AddMotherIfFirstTimeSeen(mother,listOfMothers);
      if (!isNewPrimary){
        listOfMothers.push_back(mother->GetLabel());
        fnoPrim->Fill(x_prim,1);
      }
    }
  }

  PostData(1, fOutputList);
} // End of useEvent
  return;
}

Double_t AliForwardSecondariesTask::WrapPi(Double_t phi){
  if (phi >= TMath::Pi()){
    phi = phi - 2*TMath::TwoPi();
  }
  if (phi < -TMath::Pi()){
    phi = phi + 2*TMath::TwoPi();
  }
  return phi;
}

Double_t AliForwardSecondariesTask::GetTrackReferenceEta(AliTrackReference* tr) {
  return -1.*TMath::Log(TMath::Tan(tr->Theta()/2));
}

Bool_t AliForwardSecondariesTask::AddMotherIfFirstTimeSeen(AliMCParticle* p, std::vector<Int_t> v){

  //Checking if v contains elements (is empty):
  if(v.empty()){
     return false;
  }
  Int_t x = p->GetLabel();
  if(std::find(v.begin(), v.end(), x) != v.end()) {
      /* v contains x */
    return true;
  } else {
      /* v does not contain x */
    return false;
  }

}



/// Modulo for float numbers
///
/// \param x nominator
/// \param y denominator
///
/// \return Rest of the rounded down division
Double_t AliForwardSecondariesTask::Mod(Double_t x, Double_t y) {
  if (0 == y)
    return x;
  return x - y * floor(x/y);
}

/// Wrap angle around 0 and 2pi
Double_t AliForwardSecondariesTask::Wrap02pi(Double_t angle) {
  const Double_t two_pi = TMath::Pi();
  Double_t lower_edge = -TMath::Pi();
  Double_t interval = two_pi;
  if (lower_edge <= angle && angle < two_pi) {
    return angle;
  }
  return Mod(angle - lower_edge, interval) + lower_edge;
}


AliMCParticle* AliForwardSecondariesTask::GetMother(AliMCParticle* p) {
  // Recurses until the mother IsPhysicalPrimary
  // Return NULL if no mother was found
  AliMCEvent* event = this->MCEvent();
  // GetLabel() is the index on the Stack!
  Bool_t isPP = event->Stack()->IsPhysicalPrimary(p->GetLabel());
  //Bool_t isPP = this->IsRedefinedPhysicalPrimary(p);
  // Return this particle if it is stable
  if (isPP) {
    return p;
  }
  else {
    // No stable particle found and no mother left !?
    if (p->GetMother() < 0) {
      return 0x0;
    }
    AliMCParticle* ancestor = dynamic_cast< AliMCParticle* >(event->GetTrack(p->GetMother()));
    return GetMother(ancestor);
  }
}

Bool_t AliForwardSecondariesTask::hasParticleMaterialInteractionInAncestors(AliMCParticle* p){
  AliMCEvent* event = this->MCEvent();

  Bool_t pIsFromMat = event->Stack()->IsSecondaryFromMaterial(p->GetLabel());
  // If `p` is from material, we don't need to look further
  if (pIsFromMat) {
    return true;
  }
  // `p` has no mother and is not from material interaction
  if ((p->GetMother() < 0)) {
    return false;
  }
  AliMCParticle* ancestor = dynamic_cast< AliMCParticle* >(event->GetTrack(p->GetMother()));
  // Return the ancestor if `p` is from material but `ancestor` is not
  // Recurse if non of the above patterns matched
  return hasParticleMaterialInteractionInAncestors(ancestor);
}


AliMCParticle* AliForwardSecondariesTask::GetChargedMother(AliMCParticle* p) {
  AliMCParticle *mother = this->GetMother(p);
  if (!mother || mother->Charge() == 0) {
    return 0x0;
  }
  return mother;
}

AliMCParticle* AliForwardSecondariesTask::GetNeutralMother(AliMCParticle* p) {
  AliMCParticle *mother = this->GetMother(p);
  if (!mother || mother->Charge() != 0) {
    return 0x0;
  }
  return mother;
}

Bool_t AliForwardSecondariesTask::IsRedefinedPhysicalPrimary(AliMCParticle* p) {
  AliMCEvent* event = this->MCEvent();
  // Is this a pi0 which was produced as a primary particle?
  if (TMath::Abs(p->PdgCode()) == 111 /*pi0*/ &&
      p->GetLabel() < event->Stack()->GetNprimary()) {
      std::cout << "found a pi0" << std::endl;

    return true;
  }
  // Is it a Physical Primary by the standard definition?
  Bool_t isPPStandardDef = event->Stack()->IsPhysicalPrimary(p->GetLabel());
  AliMCParticle *pi0Candidate = dynamic_cast< AliMCParticle* >(event->GetTrack(p->GetMother()));
  // Check if this is a primary originating from a pi0
  if (isPPStandardDef && pi0Candidate) {
    if (TMath::Abs(pi0Candidate->PdgCode()) == 111/*pi0*/) {
        std::cout << "found wrong pi0Candidate" << std::endl;

      return false;//false; // Don't allow stable particles stemming from pi0!
    }
  }
  return isPPStandardDef;
}

std::vector< AliMCParticle* > AliForwardSecondariesTask::GetDaughters(AliMCParticle* p) {
  AliMCEvent* event = this->MCEvent();
  std::vector< AliMCParticle* > daughters;
  // Find the decays ("edges") leading downstream from this particle ("vertex")
  AliMCParticle* daughterFirst =
    static_cast< AliMCParticle* >(event->GetTrack(p->GetFirstDaughter()));
  // p's mother does not have daughters (p == mother)
  if (!daughterFirst) {
    return daughters;
  }
  AliMCParticle* daughterLast =
    static_cast< AliMCParticle* >(event->GetTrack(p->GetLastDaughter()));
  // We only have one daughter
  if (!daughterLast) {
    daughterLast = daughterFirst;
  }
  // Perform depth-first-search in decay chain for hits on FMD
  for (Int_t iDaughter = daughterFirst->GetLabel(); iDaughter <= daughterLast->GetLabel(); iDaughter++){
    AliMCParticle* daughter  = static_cast< AliMCParticle* >(event->GetTrack(iDaughter));
    daughters.push_back(daughter);
  }
  return daughters;
}

Int_t AliForwardSecondariesTask::ParticleProducedNHitsOnFMD(AliMCParticle* p) {
  // "Explore" the current particle (Graph theory wise)
  Int_t counter = fUtil.IsHitFMD(p) ? 1 : 0;

  // Find the decays ("edges") leading downstream from this particle ("vertex")
  // Perform depth-first-search in decay chain for hits on FMD
  for (auto daughter : this->GetDaughters(p)){
    counter += ParticleProducedNHitsOnFMD(daughter);
  }
  return counter;
}


AliMCParticle* AliForwardSecondariesTask::GetIncidentParticleFromFirstMaterialInteraction(AliMCParticle* p) {
  AliMCEvent* event = this->MCEvent();
  // Is this particle from material, but there is no mother?!
  Bool_t pIsFromMat = event->Stack()->IsSecondaryFromMaterial(p->GetLabel());
  // If `p` is not from material, we don't need to look further
  if (!pIsFromMat) {
    return 0x0;
  }
  // `p` is from material, but has no mother; This should not happen
  if (pIsFromMat && (p->GetMother() < 0)) {
    return 0x0;
  }
  AliMCParticle* ancestor = dynamic_cast< AliMCParticle* >(event->GetTrack(p->GetMother()));
  // Return the ancestor if `p` is from material but `ancestor` is not
  if (pIsFromMat &&
      !event->Stack()->IsSecondaryFromMaterial(ancestor->GetLabel())) {
    return ancestor;
  }
  // Recurse if non of the above patterns matched
  else {
    return GetIncidentParticleFromFirstMaterialInteraction(ancestor);
  }
}

AliMCParticle* AliForwardSecondariesTask::GetFirstNonPrimaryMother(AliMCParticle* p) {
  AliMCEvent* event = this->MCEvent();
  // If `p` is not from material, we don't need to look further
  if (event->Stack()->IsPhysicalPrimary(p->GetLabel())) {
    return 0x0;
  }
  // Are there no more mothers left?
  if (p->GetMother() < 0) {
    return 0x0;
  }
  AliMCParticle* ancestor = dynamic_cast< AliMCParticle* >(event->GetTrack(p->GetMother()));
  // Return p if its ancestor is a primary particle; else recurse
  if (event->Stack()->IsPhysicalPrimary(ancestor->GetLabel())) {
    return p;
  }
  // Recurse if non of the above patterns matched
  else {
    return GetIncidentParticleFromFirstMaterialInteraction(ancestor);
  }
}




AliTrackReference* AliForwardSecondariesTask::IsHitITS(AliMCParticle* p) {
  for (Int_t iTrRef = 0; iTrRef < p->GetNumberOfTrackReferences(); iTrRef++) {
    AliTrackReference* ref = p->GetTrackReference(iTrRef);
    // Check hit on ITS
    if (!ref || AliTrackReference::kITS != ref->DetectorId()) {
      continue;
    }
    // We are interested if it produced a signal, not only a hit in the support structure.
    // This is an envelop around the active area
    if (ref->R() > 3.5 && ref->R() < 4.5 && TMath::Abs(ref->Z()) < 14.1) {
      return ref;
    }
  }
  return 0x0;
}


void AliForwardSecondariesTask::GetTrackRefEtaPhi(AliMCParticle* p, Double_t* etaPhi) {
  AliTrackReference* ref = 0x0;
  for (Int_t iTrRef = 0; iTrRef < p->GetNumberOfTrackReferences(); iTrRef++) {
    ref = p->GetTrackReference(iTrRef);
    // Check hit on FMD
    if (ref && AliTrackReference::kFMD == ref->DetectorId()) {
      break;
    }
    else {
      ref = 0x0;
    }
  }
  if (!ref) {
    etaPhi = 0x0;
    return;
  }
  const AliVVertex* vertex = this->MCEvent()->GetPrimaryVertex();
  // Calculate the vector pointing from the vertex to the track reference on the detector
  Double_t x      = ref->X() - vertex->GetX();
  Double_t y      = ref->Y() - vertex->GetY();
  Double_t z      = ref->Z() - vertex->GetZ();
  Double_t rr     = TMath::Sqrt(x * x + y * y);
  Double_t thetaR = TMath::ATan2(rr, z);
  Double_t phiR   = TMath::ATan2(y,x);
  // Correct angles
  if (thetaR < 0) {
    thetaR += 2*TMath::Pi();
  }
  if (phiR < 0) {
    phiR += 2*TMath::Pi();
  }
  etaPhi[0] = -TMath::Log(TMath::Tan(thetaR / 2));
  etaPhi[1] = phiR;
  // cout << x << " " << y << " " << z << endl << endl;
  // cout << etaPhi[0] - p->Eta() << " " << etaPhi[1] - p->Phi() << " "
  //      << this->GetDaughters(p).size() << " "
  //      << p->PdgCode() << " "
  //      << endl;
}

Int_t AliForwardSecondariesTask::GetOriginType(AliMCParticle *p) {
  AliMCEvent* event = this->MCEvent();
  AliStack *stack = event->Stack();
  if (stack->IsPhysicalPrimary(p->GetLabel())) {
    return cOriginType::kPRIMARY;
  }
  Double_t r = TMath::Sqrt(p->Yv() * p->Yv() + p->Xv() * p->Xv());
  if (this->fITS->IsInside(p->Zv(), r)) {
    return cOriginType::kITS;
  }
  if (this->fFMD1->IsInside(p->Zv(), r) ||
      this->fFMD2->IsInside(p->Zv(), r) ||
      this->fFMD3->IsInside(p->Zv(), r)) {
    return cOriginType::kFMD;
  }
  if (this->fPipe->IsInside(p->Zv(), r)) {
    return cOriginType::kPIPE;
  }
  if (this->fEarlyDecay->IsInside(p->Zv(), r)) {
    return cOriginType::kEARLYDECAY;
  }
  return cOriginType::kOTHER;
}



//_____________________________________________________________________
void AliForwardSecondariesTask::Terminate(Option_t */*option*/)
{
  return;
}


//_____________________________________________________________________
//
//
// EOF
