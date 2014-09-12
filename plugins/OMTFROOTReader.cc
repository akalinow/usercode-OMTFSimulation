#include <iostream>
#include <iomanip>

#include "UserCode/OMTFSimulation/plugins/OMTFROOTReader.h"

#include "UserCode/OMTFSimulation/interface/OMTFProcessor.h"
#include "UserCode/OMTFSimulation/interface/OMTFinputMaker.h"
#include "UserCode/OMTFSimulation/interface/OMTFConfiguration.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/AnaEff.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/HitSpecObj.h"



#include "TChain.h"

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
OMTFROOTReader::OMTFROOTReader(const edm::ParameterSet & cfg){

  if (cfg.exists("anaEff")) myAnaEff = new   AnaEff(cfg.getParameter<edm::ParameterSet>("anaEff") );

  if (cfg.exists("omtf")){
    myOMTFConfig = new OMTFConfiguration(cfg.getParameter<edm::ParameterSet>("omtf"));
    myOMTF = new OMTFProcessor(cfg.getParameter<edm::ParameterSet>("omtf"));
  }
  myInputMaker = new OMTFinputMaker();

  theConfig = cfg;
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
OMTFROOTReader::~OMTFROOTReader(){

  delete myOMTF;
  delete myOMTFConfig;
  delete myInputMaker;

}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
void OMTFROOTReader::beginJob(){

  myHistos.SetOwner();
  myAnaEff->init(myHistos);

}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
void OMTFROOTReader::analyze(const edm::Event&, const edm::EventSetup& es){

  myInputMaker->initialize(es);

  // define input chain
  TChain chain("tL1Rpc");
  std::vector<std::string> treeFileNames = theConfig.getParameter<std::vector<std::string> >("treeFileNames");
  for (auto it:treeFileNames)  chain.Add(it.c_str());

  // prepare datastructures and branches
  std::vector<std::pair<uint32_t, uint32_t> > *digSpec = 0;
  TBranch *digSpecBranch = 0;

  EventObj * event = 0;
  TrackObj * simu = 0;
  L1ObjColl* l1ObjColl = 0;
  HitSpecObj* hitSpecProp = 0;

  chain.SetBranchAddress("event",&event);
  chain.SetBranchAddress("simu",&simu);
  chain.SetBranchAddress("digSpec",&digSpec,&digSpecBranch);
  chain.SetBranchAddress("l1ObjColl",&l1ObjColl);
  chain.SetBranchAddress("hitSpecProp",&hitSpecProp);

  // number of events
  Int_t nentries= (Int_t) chain.GetEntries();
  ///Test settings
  nentries = 21;
  //nentries = 5E4;
  /////////////////
  std::cout <<" ENTRIES: " << nentries << std::endl;
 
  // main loop
  unsigned int lastRun = 0;
  //for (int ev=0; ev<nentries; ev+=1) {
  for (int ev=3; ev<4; ev+=1) {

    chain.GetEntry(ev);

    if ( (lastRun != (*event).run) || (ev%(nentries/20)==0)) { 
    //if ( (lastRun != (*event).run) || true) { 
      lastRun = (*event).run; 
      std::cout <<"RUN:"    << std::setw(7) << (*event).run
                <<" event:" << std::setw(8) << ev
                <<" done:"  << std::setw(6)<< std::setiosflags(std::ios::fixed) << std::setprecision(2) << ev*100./nentries<<"%";
      std::cout<<std::endl; 
    }

    const OMTFinput *myInput = myInputMaker->getEvent(*digSpec);
    myInput->print(std::cout);

    ///Run OMTF simulation
    ///l1otf5x=thePatternProvider->check(event, es, simu, hitSpecProp, *digSpec,3);   

   L1ObjColl myL1ObjColl = *l1ObjColl;
   //myL1ObjColl.push_back(l1OTFCand, false, 0.); 
   if (myAnaEff) myAnaEff->run(simu, &myL1ObjColl, hitSpecProp);
  }
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
void OMTFROOTReader::endJob(){

}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
