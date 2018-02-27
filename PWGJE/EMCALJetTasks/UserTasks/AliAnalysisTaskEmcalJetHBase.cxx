//
// Base class for Jet-Hadron correlation analysis
//

#include "AliAnalysisTaskEmcalJetHBase.h"

#include <TMath.h>

#include <AliLog.h>

#include "AliEmcalJetTask.h"

namespace PWGJE {
namespace EMCALJetTasks {

const std::map<std::string, AliAnalysisTaskEmcalJetHBase::ELeadingHadronBiasType_t> AliAnalysisTaskEmcalJetHBase::fgkLeadingHadronBiasMap = {
  { "kCharged", AliAnalysisTaskEmcalJetHBase::kCharged},
  { "kNeutral", AliAnalysisTaskEmcalJetHBase::kNeutral},
  { "kBoth", AliAnalysisTaskEmcalJetHBase::kBoth}
};

/**
 * Determine leading hadron pt in a jet. This is inspired by AliJetContainer::GetLeadingHadronMomentum(), but
 * that particular function is avoided because the cluster energy retrieved is always the raw E while the
 * cluster energy used in creating the jet would be preferred. One could create a cluster container and go
 * through all of those steps, but there is a simpler approach: the leading charged and neutral momenta
 * are stored in AliEmcalJet while performing jet finding.
 *
 * @param[in] jet Jet from which the leading hadron pt should be extracted
 * @param[in] leadingHadronType Type of leading hadron pt to retrieve
 *
 * @return Value of the leading hadron pt
 */
double AliAnalysisTaskEmcalJetHBase::GetLeadingHadronPt(AliEmcalJet * jet, AliAnalysisTaskEmcalJetHBase::ELeadingHadronBiasType_t leadingHadronType)
{
  double maxTrackPt = 0;
  double maxClusterPt = 0;

  if (leadingHadronType == kCharged || leadingHadronType == kBoth) {
    maxTrackPt = jet->MaxChargedPt();
  }
  if (leadingHadronType == kNeutral || leadingHadronType == kBoth) {
    maxClusterPt = jet->MaxNeutralPt();
  }

  // The max value will be 0 unless it was filled. Thus, it will only be greater if
  // it was requested.
  return (maxTrackPt > maxClusterPt) ? maxTrackPt : maxClusterPt;
}


/**
 * Function to calculate angle between jet and EP in the 1st quadrant (0,Pi/2).
 * Adapted from AliAnalysisTaskEmcalJetHadEPpid.
 *
 * @param jetAngle Phi angle of the jet (could be any particle)
 * @param epAngle Event plane angle
 *
 * @return Angle between jet and EP in the 1st quadrant (0,Pi/2)
 */
double AliAnalysisTaskEmcalJetHBase::RelativeEPAngle(double jetAngle, double epAngle)
{
  double dphi = (epAngle - jetAngle);

  // ran into trouble with a few dEP<-Pi so trying this...
  if( dphi<-1*TMath::Pi() ) {
    dphi = dphi + 1*TMath::Pi();
  } // this assumes we are doing full jets currently

  if( (dphi>0) && (dphi<1*TMath::Pi()/2) ) {
    // Do nothing! we are in quadrant 1
  } else if ( (dphi>1*TMath::Pi()/2) && (dphi<1*TMath::Pi()) ) {
    dphi = 1*TMath::Pi() - dphi;
  } else if ( (dphi<0) && (dphi>-1*TMath::Pi()/2) ) {
    dphi = std::abs(dphi);
  } else if ( (dphi<-1*TMath::Pi()/2) && (dphi>-1*TMath::Pi()) ) {
    dphi = dphi + 1*TMath::Pi();
  }

  // Warn if we are not in the proper range
  if ( dphi < 0 || dphi > TMath::Pi()/2 ) {
    AliWarningGeneralStream("AliAnalysisTaskEmcalJetHBase") << ": dPHI not in range [0, 0.5*Pi]!\n";
  }

  return dphi;   // dphi in [0, Pi/2]
}

} /* namespace EMCALJetTasks */
} /* namespace PWGJE */
