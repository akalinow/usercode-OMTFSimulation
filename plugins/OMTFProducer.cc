#include <iostream>

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"

#include "UserCode/OMTFSimulation/plugins/OMTFProducer.h"
#include "UserCode/OMTFSimulation/interface/OMTFProcessor.h"
#include "UserCode/OMTFSimulation/interface/OMTFinputMaker.h"
#include "UserCode/OMTFSimulation/interface/OMTFinput.h"
#include "UserCode/OMTFSimulation/interface/OMTFSorter.h"
#include "UserCode/OMTFSimulation/interface/OMTFConfiguration.h"
#include "UserCode/OMTFSimulation/interface/OMTFConfigMaker.h"
#include "UserCode/OMTFSimulation/interface/XMLConfigWriter.h"

using namespace L1TMuon;

OMTFProducer::OMTFProducer(const edm::ParameterSet& cfg):
  theConfig(cfg),
  trigPrimSrc(cfg.getParameter<edm::InputTag>("TriggerPrimitiveSrc")){

  produces<std::vector<L1MuRegionalCand> >("OMTF");

  inputToken = consumes<TriggerPrimitiveCollection>(trigPrimSrc);
  
  if(!theConfig.exists("omtf")){
    edm::LogError("OMTFProducer")<<"omtf configuration not found in cfg.py";
  }
  
  myInputMaker = new OMTFinputMaker();
  mySorter = new OMTFSorter();
  
  myWriter = new XMLConfigWriter();
  std::string fName = "OMTF_Events";
  myWriter->initialiseXMLDocument(fName);

  dumpResultToXML = theConfig.getParameter<bool>("dumpResultToXML");
  dumpGPToXML = theConfig.getParameter<bool>("dumpGPToXML");
  makeConnectionsMaps = theConfig.getParameter<bool>("makeConnectionsMaps");

  myOMTFConfig = 0;
}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
OMTFProducer::~OMTFProducer(){

  delete myOMTFConfig;
  delete myOMTFConfigMaker;
  delete myOMTF;

}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
void OMTFProducer::beginJob(){

  if(theConfig.exists("omtf")){
    myOMTFConfig = new OMTFConfiguration(theConfig.getParameter<edm::ParameterSet>("omtf"));
    myOMTFConfigMaker = new OMTFConfigMaker(theConfig.getParameter<edm::ParameterSet>("omtf"));
    myOMTF = new OMTFProcessor(theConfig.getParameter<edm::ParameterSet>("omtf"));
  }
}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////  
void OMTFProducer::produce(edm::Event& iEvent, const edm::EventSetup& aESetup){

  edm::Handle<TriggerPrimitiveCollection> trigPrimitives;
  iEvent.getByToken(inputToken, trigPrimitives);

  std::auto_ptr<std::vector<L1MuRegionalCand> > myCands(new std::vector<L1MuRegionalCand>);

  if(dumpResultToXML) aTopElement = myWriter->writeEventHeader(iEvent.id().event());

  ///Loop over all processors, each covering 60 deg in phi
  for(unsigned int iProcessor=0;iProcessor<6;++iProcessor){
    
    edm::LogInfo("OMTF ROOTReader")<<"iProcessor: "<<iProcessor;
    
    const OMTFinput *myInput = myInputMaker->buildInputForProcessor(*trigPrimitives,iProcessor);
    
    ///Input data with phi ranges shifted for each processor, so it fits 10 bits range
    const OMTFinput myShiftedInput =  myOMTF->shiftInput(iProcessor,*myInput);	
    
    ///Phi maps should be made with original, global phi values.
    if(makeConnectionsMaps) myOMTFConfigMaker->makeConnetionsMap(iProcessor,*myInput);
    /////////////////////////
    
    ///Results for each GP in each logic region of given processor
    const std::vector<OMTFProcessor::resultsMap> & myResults = myOMTF->processInput(iProcessor,myShiftedInput);
    
    L1MuRegionalCand myOTFCandidate = mySorter->sortProcessor(myResults);
    ////Swith from internal processor 10bit scale to global one
    int procOffset = OMTFConfiguration::globalPhiStart(iProcessor);
    if(procOffset<0) procOffset+=OMTFConfiguration::nPhiBins;
    myOTFCandidate.setPhiValue(myOTFCandidate.phiValue()+OMTFConfiguration::globalPhiStart(iProcessor)+511);
    //////////////////
    if(myOTFCandidate.pt_packed()) myCands->push_back(myOTFCandidate); 
    
    ///Write to XML
    if(dumpResultToXML){
      xercesc::DOMElement * aProcElement = myWriter->writeEventData(aTopElement,iProcessor,myShiftedInput);
      for(unsigned int iRegion=0;iRegion<6;++iRegion){
	///Dump only regions, where a candidate was found
	InternalObj myCand = mySorter->sortRegionResults(myResults[iRegion]);
	if(myCand.pt){
	  myWriter->writeCandidateData(aProcElement,iRegion,myCand);
	  for(auto & itKey: myResults[iRegion]) myWriter->writeResultsData(aProcElement, 
									   iRegion,
									   itKey.first,itKey.second);    
	}
      }
    }
  }
  iEvent.put(myCands, "OMTF");
}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////  
