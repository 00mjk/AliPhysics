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
}

void AliFemtoDreamBasePart::ResetMCInfo() {
  this->SetPt(0);
  //a change
  this->SetEta(0);
  this->SetTheta(0);
  this->SetPhi(0);
  this->SetCharge(0);
  this->SetPDGCode(0);
  this->fIsSet=false;
  this->SetUse(false);
  this->fIsReset=true;
}
