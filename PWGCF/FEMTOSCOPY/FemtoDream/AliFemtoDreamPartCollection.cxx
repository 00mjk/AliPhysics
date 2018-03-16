/*
 * AliFemtoPPbpbLamPartCollection.cxx

 *
 *  Created on: Aug 29, 2017
 *      Author: gu74req
 */
#include <iostream>
#include "AliFemtoDreamPartCollection.h"
#include "AliLog.h"
ClassImp(AliFemtoDreamPartCollection)
AliFemtoDreamPartCollection::AliFemtoDreamPartCollection()
:fResults()
,fNSpecies(0)
,fMinimalBooking(false)
,fZVtxMultBuffer()
,fValuesZVtxBins()
,fValuesMultBins()
{

}
AliFemtoDreamPartCollection::AliFemtoDreamPartCollection(
    AliFemtoDreamCollConfig *conf,bool MinimalBooking)
:fResults(new AliFemtoDreamCorrHists(conf,MinimalBooking))
,fNSpecies(conf->GetNParticles())
,fMinimalBooking(false)
,fZVtxMultBuffer(conf->GetNZVtxBins(),
                 std::vector<AliFemtoDreamZVtxMultContainer>(
                     conf->GetNMultBins(),
                     AliFemtoDreamZVtxMultContainer(conf)))
,fValuesZVtxBins(conf->GetZVtxBins())
,fValuesMultBins(conf->GetMultBins())
{
}

AliFemtoDreamPartCollection::~AliFemtoDreamPartCollection() {
}

void AliFemtoDreamPartCollection::SetEvent(
    std::vector<std::vector<AliFemtoDreamBasePart>> &Particles,
    float ZVtx,float Mult)
{
  if (Particles.size()!=fNSpecies) {
    TString fatalOut=
        Form("Too few Species %d for %d",Particles.size(),fNSpecies);
    AliFatal(fatalOut.Data());
  }
  int bins[2] = {0,0};
  FindBin(ZVtx,Mult,bins);
  if (!(bins[0]==-99||bins[1]==-99)) {
    auto itZVtx=fZVtxMultBuffer.begin();
    itZVtx+=bins[0];
    auto itMult=itZVtx->begin();
    itMult+=bins[1];
    itMult->PairParticlesSE(Particles,fResults,bins[1]);
    itMult->PairParticlesME(Particles,fResults,bins[1]);
    itMult->SetEvent(Particles);
  }
  return;
}

void AliFemtoDreamPartCollection::PrintEvent(int ZVtx,int Mult) {
  auto itZVtx=fZVtxMultBuffer.begin();
  itZVtx+=ZVtx;
  auto itMult=itZVtx->begin();
  itMult+=Mult;
  return;
}

void AliFemtoDreamPartCollection::FindBin(float ZVtxPos,float Multiplicity,
                                          int *returnBins) {
  returnBins[0]=-99;
  returnBins[1]=-99;
  for (auto itBin=fValuesZVtxBins.begin();itBin!=fValuesZVtxBins.end()-1;
      ++itBin) {
    if (*itBin<ZVtxPos && ZVtxPos<=*(itBin+1)) {
      returnBins[0]=itBin-fValuesZVtxBins.begin();
      break;
    }
  }
  int binCounter = fValuesMultBins.size();
  for (std::vector<int>::reverse_iterator itBin=fValuesMultBins.rbegin();
      itBin!=fValuesMultBins.rend();++itBin) {
    binCounter--;
    if (Multiplicity>=*itBin) {
      returnBins[1]=binCounter;
      break;
    }
  }
  return;
}
