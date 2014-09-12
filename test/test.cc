#include <string>
#include <iostream>

#include "interface/XMLConfigReader.h"
#include "interface/XMLConfigWriter.h"
#include "interface/OMTFConfiguration.h"
#include "interface/OMTFProcessor.h"
#include "interface/OMTFinput.h"
#include "interface/OMTFSorter.h"
#include "interface/L1Obj.h"


int  main(){

  XMLConfigReader *myReader = new XMLConfigReader();
  std::string aDir = "/home/akalinow/scratch/CMS/OverlapTrackFinder/Dev4/CMSSW_7_0_1/src/UserCode/L1RpcTriggerAnalysis/data/OMTF/";

  OMTFConfiguration *myConfig = new OMTFConfiguration();
  std::string fName = aDir+"hwToLogicLayer.xml";
  myReader->setConfigFile(fName);
  myConfig->configure(myReader);
  myConfig->print(std::cout);
  
  OMTFProcessor *myOMTF = new OMTFProcessor();
  fName = aDir+"Patterns_chMinus.xml";
  fName = "/home/akalinow/scratch/CMS/OverlapTrackFinder/Dev4/job_4_ana/SingleMu_19_p/Patterns.xml";
  myReader->setPatternsFile(fName);
  myOMTF->configure(myReader);

  OMTFinput * aInput = new OMTFinput();
  OMTFSorter *mySorter = new OMTFSorter();

  aDir = "/home/akalinow/scratch/CMS/OverlapTrackFinder/Dev4/job_4_ana/SingleMu_19_p/";
  fName = aDir+"TestEvents.xml";
  myReader->setEventsFile(fName);

  XMLConfigWriter *myWriter = new XMLConfigWriter();
  fName = "OMTF_Events";
  myWriter->initialiseXMLDocument(fName);
  

  for(unsigned int iEvent=0;iEvent<5;++iEvent){
    aInput->readData(myReader,iEvent); 
    //aInput->print(std::cout);
    
    const OMTFProcessor::resultsMap & myResults = myOMTF->processInput(*aInput);
    L1Obj myCandidate = mySorter->sortResults(myResults);
    std::cout<<myCandidate<<std::endl;

    ///Write to XML
    xercesc::DOMElement *aTopElement = myWriter->writeEventHeader(iEvent);
    myWriter->writeEventData(aTopElement,*aInput);
    for(auto itKey: myResults){
      myWriter->writeResultsData(aTopElement, 
				 itKey.first,itKey.second);
      //std::cout<<itKey.first<<std::endl;
      //itKey.second.print(std::cout);
      //std::cout<<std::endl;
    }
  }

  fName = "test.xml";
  myWriter->finaliseXMLDocument(fName);
 
  ///Write GPs to XML file
  fName = "OMTF";
  myWriter->initialiseXMLDocument(fName);
  const std::map<Key,GoldenPattern*> & myGPmap = myOMTF->getPatterns();
  for(auto itGP: myGPmap){
    //itGP.second->print(std::cout);
    myWriter->writeGPData(*itGP.second);
  }
  fName = "testGP.xml";
  myWriter->finaliseXMLDocument(fName);
 
  delete myReader;
  delete myWriter;
  delete myOMTF;
  delete mySorter;
  delete aInput;
  
  return 0;
}
