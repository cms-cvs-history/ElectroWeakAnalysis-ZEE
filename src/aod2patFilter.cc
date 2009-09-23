#ifndef aod2patFilter_H
#define aod2patFilter_H

/******************************************************************************
 *
 * Implementation Notes:
 *
 *   this is a filter that creates pat::Electrons without the need of
 *   running the PAT sequence
 *
 *   it is meant to be an interface of Wenu and Zee CandidateFilters
 *   for the October 2009 exercise
 *   it does make sense to implement the trigger requirement here
 *   but it will not be implemented in order to keep compatibolity with the
 *   old code
 *
 *
 * contact:
 * Nikolaos.Rompotis@Cern.ch
 *
 * Nikolaos Rompotis
 * Imperial College London
 *
 * 21 Sept 2009
 *
 *****************************************************************************/



// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
//
#include <vector>
#include <iostream>
#include "FWCore/ParameterSet/interface/InputTag.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Framework/interface/TriggerNames.h"
#include "DataFormats/METReco/interface/METCollection.h"
#include "DataFormats/METReco/interface/CaloMETCollection.h"
//
#include "TString.h"
#include "TMath.h"
#include "DataFormats/PatCandidates/interface/MET.h"


class aod2patFilter : public edm::EDFilter {
   public:
      explicit aod2patFilter(const edm::ParameterSet&);
      ~aod2patFilter();

   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual bool filter(edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
  //bool isInFiducial(double eta);
      
      // ----------member data ---------------------------
  //double ETCut_;
  //double METCut_;
  //double ETCut2ndEle_;
  //edm::InputTag triggerCollectionTag_;
  //edm::InputTag triggerEventTag_;
  //std::string hltpath_;
  //edm::InputTag hltpathFilter_;
  edm::InputTag electronCollectionTag_;
  edm::InputTag metCollectionTag_;

  //double BarrelMaxEta_;
  //double EndCapMaxEta_;
  //double EndCapMinEta_;
  //bool electronMatched2HLT_;
  //double electronMatched2HLT_DR_;
  //bool vetoSecondElectronEvents_;
};
#endif


aod2patFilter::aod2patFilter(const edm::ParameterSet& iConfig)
{

  electronCollectionTag_=iConfig.getUntrackedParameter<edm::InputTag>
    ("electronCollectionTag");
  metCollectionTag_=iConfig.getUntrackedParameter<edm::InputTag>
    ("metCollectionTag");


  produces< pat::ElectronCollection > 
    ("patElectrons").setBranchAlias("patElectrons");

  produces< pat::METCollection>("patCaloMets").setBranchAlias("patCaloMets");
  //produces< pat::METCollection>("patPfMets").setBranchAlias("patPfMets");
  //produces< pat::METCollection>("patTcMets").setBranchAlias("patTcMets");
  //produces< pat::METCollection>("patT1cMets").setBranchAlias("patT1cMets");

}

aod2patFilter::~aod2patFilter()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


bool
aod2patFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace std;
  using namespace pat;
  // *************************************************************************
  // ELECTRONS
  // *************************************************************************
  edm::Handle<reco::GsfElectronCollection> gsfElectrons;
  iEvent.getByLabel(electronCollectionTag_, gsfElectrons);
  if (!gsfElectrons.isValid()) {
    std::cout <<"aod2patFilter: Could not get electron collection with label: "
	      <<electronCollectionTag_ << std::endl;
    return false;
  }
  const reco::GsfElectronCollection *pElecs = gsfElectrons.product();
  // calculate your electrons
  auto_ptr<pat::ElectronCollection> patElectrons(new pat::ElectronCollection);
  for (reco::GsfElectronCollection::const_iterator elec = pElecs->begin();
       elec != pElecs->end(); ++elec) {
    reco::GsfElectron mygsfelec = *elec;
    pat::Electron myElectron(mygsfelec);
    // now set the isolations from the Gsf electron
    myElectron.setTrackIso(elec->dr03TkSumPt());
    myElectron.setECalIso(elec->dr04EcalRecHitSumEt());
    myElectron.setHCalIso(elec->dr04HcalTowerSumEt());

    patElectrons->push_back(myElectron);
  }
  // *************************************************************************
  // METs
  // *************************************************************************
  edm::Handle<reco::CaloMETCollection> calomets;
  iEvent.getByLabel(metCollectionTag_, calomets);
  if (! calomets.isValid()) {
    std::cout << "aod2patFilter: Could not get met collection with label: "
	      << metCollectionTag_ << std::endl;
    return false;
  }
  const  reco::CaloMETCollection *mycalomets =  calomets.product();
  auto_ptr<pat::METCollection> patCaloMets(new pat::METCollection);
  for (reco::CaloMETCollection::const_iterator met = mycalomets->begin();
       met != mycalomets->end(); ++ met ) {
    pat::MET mymet(*met);
    patCaloMets->push_back(mymet);
  }

  //
  // put everything in the event
  //
  iEvent.put( patElectrons, "patElectrons");
  iEvent.put( patCaloMets, "patCaloMets");
  //

  return true;

}

// ------------ method called once each job just before starting event loop  -
void 
aod2patFilter::beginJob(const edm::EventSetup&)
{
}

// ------------ method called once each job just after ending the event loop  -
void 
aod2patFilter::endJob() {
}


//define this as a plug-in
DEFINE_FWK_MODULE(aod2patFilter);
