/*
 * AliFemtoPPbpbLamBaseParticle.cxx
 *
 *  Created on: Aug 29, 2017
 *      Author: gu74req
 */

#include "AliFemtoDreamBasePart.h"
ClassImp(AliFemtoDreamBasePart)
AliFemtoDreamBasePart::AliFemtoDreamBasePart()
:fIsReset(false)
,fGTI(0)
,fTrackBufferSize(0)
,fP()
,fMCP()
,fPt(0)
,fMCPt(0)
,fP_TPC(0)
,fEta(0)
,fTheta(0)
,fMCTheta(0)
,fPhi(0)
,fPhiAtRadius(0)
,fMCPhi(0)
,fIDTracks(0)
,fCharge(0)
,fCPA(0)
,fOrigin(kUnknown)
,fPDGCode(0)
,fMCPDGCode(0)
,fPDGMotherWeak(0)
,fMotherID(0)
,fIsMC(false)
,fUse(true)
,fIsSet(true)
{
}

AliFemtoDreamBasePart::~AliFemtoDreamBasePart() {
}


void AliFemtoDreamBasePart::SetMCParticle(AliAODMCParticle *mcPart) {
  this->SetPt(mcPart->Pt());
  this->SetMomentum(mcPart->Px(),mcPart->Py(),mcPart->Pz());
  this->SetEta(mcPart->Eta());
  this->SetTheta(mcPart->Theta());
  this->SetPhi(mcPart->Phi());
  this->SetCharge(mcPart->Charge());
  this->SetPDGCode(mcPart->GetPdgCode());
  this->fIsSet=true;
  this->SetUse(true);
  this->fIsReset=false;
  //find the original mother in the mc stack.
  int lastMother=0;
  int motherID=-99;
  while (motherID!=-1) {
    lastMother=motherID;
    motherID=mcPart->GetMother();
  }
  std::cout << lastMoher << std::endl;
  this->SetMotherID(lastMother);
}

void AliFemtoDreamBasePart::ResetMCInfo() {
  this->SetPt(0);
  //a change
  this->SetEta(0);
  this->SetTheta(0);
  this->SetPhi(0);
  this->SetCharge(0);
  this->SetPDGCode(0);
  this->SetMotherID(0);
  this->fIsSet=false;
  this->SetUse(false);
  this->fIsReset=true;
}
