#include <iostream>

#include "interface/OMTFConfiguration.h"
#include "interface/XMLConfigReader.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

unsigned int OMTFConfiguration::nLayers;
unsigned int OMTFConfiguration::nHitsPerLayer;
unsigned int OMTFConfiguration::nRefLayers;
unsigned int OMTFConfiguration::nPdfAddrBits;
unsigned int OMTFConfiguration::nPdfValBits;
unsigned int OMTFConfiguration::nPhiBits;

std::map<int,int> OMTFConfiguration::hwToLogicLayer;
std::map<int,int> OMTFConfiguration::logicToHwLayer;
std::map<int,int> OMTFConfiguration::logicToLogic;
std::vector<int> OMTFConfiguration::refToLogicNumber;
std::set<int> OMTFConfiguration::bendingLayers;
///////////////////////////////////////////////
///////////////////////////////////////////////
OMTFConfiguration::OMTFConfiguration(const edm::ParameterSet & theConfig){

  if ( !theConfig.exists("configXMLFile") ) return;
  std::string fName = theConfig.getParameter<std::string>("configXMLFile");

  XMLConfigReader myReader;
  myReader.setConfigFile(fName);
  configure(&myReader);
  print(std::cout);

}
///////////////////////////////////////////////
///////////////////////////////////////////////
void OMTFConfiguration::configure(XMLConfigReader *aReader){

 aReader->readConfig(this);

}
///////////////////////////////////////////////
///////////////////////////////////////////////
std::ostream & OMTFConfiguration::print(std::ostream & out){

  out<<"nLayers: "<<nLayers
     <<" nHitsPerLayer: "<<nHitsPerLayer
     <<" nRefLayers: "<<nRefLayers
     <<" nPdfAddrBits: "<<nPdfAddrBits
     <<" nPdfValBits: "<<nPdfValBits
     <<std::endl;

  return out;

}
///////////////////////////////////////////////
///////////////////////////////////////////////
