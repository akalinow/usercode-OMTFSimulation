#include <iostream>

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "UserCode/OMTFSimulation/plugins/OMTFPatternMaker.h"
#include "UserCode/OMTFSimulation/interface/OMTFProcessor.h"
#include "UserCode/OMTFSimulation/interface/OMTFinputMaker.h"
#include "UserCode/OMTFSimulation/interface/OMTFinput.h"
#include "UserCode/OMTFSimulation/interface/OMTFConfiguration.h"
#include "UserCode/OMTFSimulation/interface/OMTFConfigMaker.h"
#include "UserCode/OMTFSimulation/interface/XMLConfigWriter.h"

#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"

#include "Math/VectorUtil.h"

using namespace L1TMuon;

OMTFPatternMaker::OMTFPatternMaker(const edm::ParameterSet& cfg):
  theConfig(cfg),
  trigPrimSrc(cfg.getParameter<edm::InputTag>("TriggerPrimitiveSrc")),
  g4SimTrackSrc(cfg.getParameter<edm::InputTag>("g4SimTrackSrc")){

  inputToken = consumes<TriggerPrimitiveCollection>(trigPrimSrc);
  consumes<edm::SimTrackContainer>(g4SimTrackSrc);
  
  if(!theConfig.exists("omtf")){
    edm::LogError("OMTFPatternMaker")<<"omtf configuration not found in cfg.py";
  }
  
  myInputMaker = new OMTFinputMaker();
  
  myWriter = new XMLConfigWriter();
  std::string fName = "OMTF_Events";
  myWriter->initialiseXMLDocument(fName);

  makeGoldenPatterns = theConfig.getParameter<bool>("makeGoldenPatterns");
  makeConnectionsMaps = theConfig.getParameter<bool>("makeConnectionsMaps");

  myOMTFConfig = 0;
}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
OMTFPatternMaker::~OMTFPatternMaker(){

  delete myOMTFConfig;
  delete myOMTFConfigMaker;
  delete myOMTF;

}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
void OMTFPatternMaker::beginJob(){

  if(theConfig.exists("omtf")){
    myOMTFConfig = new OMTFConfiguration(theConfig.getParameter<edm::ParameterSet>("omtf"));
    myOMTFConfigMaker = new OMTFConfigMaker(theConfig.getParameter<edm::ParameterSet>("omtf"));
    myOMTF = new OMTFProcessor(theConfig.getParameter<edm::ParameterSet>("omtf"));
  }

  ///Clear existing GoldenPatterns
  const std::map<Key,GoldenPattern*> & theGPs = myOMTF->getPatterns();
  for(auto itGP: theGPs) itGP.second->reset();

}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////  
void OMTFPatternMaker::endJob(){

  if(makeGoldenPatterns && !makeConnectionsMaps){
    std::string fName = "OMTF";
    myWriter->initialiseXMLDocument(fName);
    const std::map<Key,GoldenPattern*> & myGPmap = myOMTF->getPatterns();
    for(auto itGP: myGPmap){
      if(!itGP.second->hasCounts()) continue;
      itGP.second->normalise();
      if(itGP.first.thePtCode==16 && itGP.first.theCharge==1){
      std::cout<<*itGP.second<<std::endl;
      }
      myWriter->writeGPData(*itGP.second);
    }
    fName = "GPs.xml";
    myWriter->finaliseXMLDocument(fName);
  }

  if(makeConnectionsMaps && !makeGoldenPatterns){
    std::string fName = "Connections.xml";  
    myWriter->writeConnectionsData(OMTFConfiguration::measurements4D);
    myWriter->finaliseXMLDocument(fName);
    myOMTFConfigMaker->printPhiMap(std::cout);
    myOMTFConfigMaker->printConnections(std::cout,0,0);
  }
}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////  
void OMTFPatternMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& evSetup){

 ///Get the simulated muon parameters
  const SimTrack* aSimMuon = findSimMuon(iEvent,evSetup);
  if(!aSimMuon){
    edm::LogError("OMTFPatternMaker")<<"No SimMuon found in the event!";
    return;
  }
  
  myInputMaker->initialize(evSetup);

  edm::Handle<TriggerPrimitiveCollection> trigPrimitives;
  iEvent.getByToken(inputToken, trigPrimitives);

  ///Filter digis by dropping digis from selected (by cfg.py) subsystem
  const L1TMuon::TriggerPrimitiveCollection filteredDigis = filterDigis(*trigPrimitives);

  ///Loop over all processors, each covering 60 deg in phi
  for(unsigned int iProcessor=0;iProcessor<6;++iProcessor){
    
    edm::LogInfo("OMTF ROOTReader")<<"iProcessor: "<<iProcessor;
    
    const OMTFinput *myInput = myInputMaker->buildInputForProcessor(filteredDigis,iProcessor);
       
    ///Input data with phi ranges shifted for each processor, so it fits 10 bits range
    const OMTFinput myShiftedInput =  myOMTF->shiftInput(iProcessor,*myInput);	
    
    ///Phi maps should be made with original, global phi values.
    ///Connections maps are rtun on large samples, so the rest
    ///of algoritm is not executed.
    if(makeConnectionsMaps) myOMTFConfigMaker->makeConnetionsMap(iProcessor,*myInput);
  
    if(makeGoldenPatterns){
	myOMTF->fillCounts(iProcessor,myShiftedInput, aSimMuon);
    }
  }
}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////  
const L1TMuon::TriggerPrimitiveCollection OMTFPatternMaker::filterDigis(const L1TMuon::TriggerPrimitiveCollection & vDigi){

  if(!theConfig.getParameter<bool>("dropRPCPrimitives") &&
     !theConfig.getParameter<bool>("dropDTPrimitives") &&
     !theConfig.getParameter<bool>("dropCSCPrimitives")) return vDigi;
  
  L1TMuon::TriggerPrimitiveCollection filteredDigis;
  for(auto it:vDigi){
    switch (it.subsystem()) {
    case L1TMuon::TriggerPrimitive::kRPC: {
      if(!theConfig.getParameter<bool>("dropRPCPrimitives")) filteredDigis.push_back(it);
      break;
    }
    case L1TMuon::TriggerPrimitive::kDT: {
      if(!theConfig.getParameter<bool>("dropDTPrimitives")) filteredDigis.push_back(it);
      break;
    }
    case L1TMuon::TriggerPrimitive::kCSC: {
      if(!theConfig.getParameter<bool>("dropCSCPrimitives")) filteredDigis.push_back(it);
      break;
    }
    case L1TMuon::TriggerPrimitive::kNSubsystems: {break;} 
    }
  }
  return filteredDigis;
}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////  
const SimTrack * OMTFPatternMaker::findSimMuon(const edm::Event &ev, const edm::EventSetup &es, const SimTrack * previous){

  const SimTrack * result = 0;
  edm::Handle<edm::SimTrackContainer> simTks;
  ev.getByLabel("g4SimHits",simTks);

  for (std::vector<SimTrack>::const_iterator it=simTks->begin(); it< simTks->end(); it++) {
    const SimTrack & aTrack = *it;
    if ( !(aTrack.type() == 13 || aTrack.type() == -13) )continue;
    if(previous && ROOT::Math::VectorUtil::DeltaR(aTrack.momentum(),previous->momentum())<0.07) continue;
    if ( !result || aTrack.momentum().pt() > result->momentum().pt()) result = &aTrack;
  }
  return result;
}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////  
