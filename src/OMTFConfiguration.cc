#include <iostream>

#include "UserCode/OMTFSimulation/interface/OMTFConfiguration.h"
#include "UserCode/OMTFSimulation/interface/XMLConfigReader.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

unsigned int OMTFConfiguration::nLayers;
unsigned int OMTFConfiguration::nHitsPerLayer;
unsigned int OMTFConfiguration::nRefLayers;
unsigned int OMTFConfiguration::nPdfAddrBits;
unsigned int OMTFConfiguration::nPdfValBits;
unsigned int OMTFConfiguration::nPhiBits;
unsigned int OMTFConfiguration::nPhiBins;
unsigned int OMTFConfiguration::nRefHits;

std::map<int,int> OMTFConfiguration::hwToLogicLayer;
std::map<int,int> OMTFConfiguration::logicToHwLayer;
std::map<int,int> OMTFConfiguration::logicToLogic;
std::vector<int> OMTFConfiguration::refToLogicNumber;
std::set<int> OMTFConfiguration::bendingLayers;
std::vector<std::vector<int> > OMTFConfiguration::processorPhiVsRefLayer;
OMTFConfiguration::vector3D_A OMTFConfiguration::connections;

OMTFConfiguration::vector4D OMTFConfiguration::measurements4D;
OMTFConfiguration::vector4D OMTFConfiguration::measurements4Dref;
///////////////////////////////////////////////
///////////////////////////////////////////////
OMTFConfiguration::OMTFConfiguration(const edm::ParameterSet & theConfig){

  if ( !theConfig.exists("configXMLFile") ) return;
  std::string fName = theConfig.getParameter<std::string>("configXMLFile");

  XMLConfigReader myReader;
  myReader.setConfigFile(fName);
  configure(&myReader);
  print(std::cout);

  ///Vector of all inpouts (14)
  std::vector<int> aLayer1D(14,0);

  ///Vector of all layers 
  OMTFConfiguration::vector2D aLayer2D;
  aLayer2D.assign(OMTFConfiguration::nLayers,aLayer1D);

  ///Vector of all logic cones
  OMTFConfiguration::vector3D aLayer3D;
  aLayer3D.assign(6,aLayer2D);

  ///Vector of all processors
  measurements4D.assign(6,aLayer3D);
  measurements4Dref.assign(6,aLayer3D);


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

  for(unsigned int iProcessor = 0;iProcessor<6; ++iProcessor){
    std::cout<<"Processor: "<<iProcessor;
    for(unsigned int iRefLayer=0;iRefLayer<nRefLayers;++iRefLayer){
      std::cout<<" "<<processorPhiVsRefLayer[iProcessor][iRefLayer];
    }
    std::cout<<std::endl;
  }
  
  return out;

}
///////////////////////////////////////////////
///////////////////////////////////////////////
bool OMTFConfiguration::isInConeRange(int iPhiStart,
				unsigned int coneSize,
				int iPhi){

  if(iPhi<0) iPhi+=OMTFConfiguration::nPhiBins;
  if(iPhiStart<0) iPhiStart+=OMTFConfiguration::nPhiBins;

  if(iPhiStart+(int)coneSize<(int)OMTFConfiguration::nPhiBins){
    return iPhiStart<=iPhi && iPhiStart+(int)coneSize>iPhi;
  }
  else if(iPhi>(int)OMTFConfiguration::nPhiBins/2){
    return iPhiStart<=iPhi;
  }
  else if(iPhi<(int)OMTFConfiguration::nPhiBins/2){
    return iPhi<iPhiStart+(int)coneSize-(int)OMTFConfiguration::nPhiBins;
  }
  return false;
}
///////////////////////////////////////////////
///////////////////////////////////////////////
unsigned int OMTFConfiguration::getConeNumber(unsigned int iProcessor,
					  unsigned int iRefLayer,
					  int iPhi){

  if(iPhi>=(int)OMTFConfiguration::nPhiBins) return 99;

  unsigned int logicConeSize = 10/360.0*OMTFConfiguration::nPhiBins;
  

  unsigned int iCone = 0;
  int iPhiStart = OMTFConfiguration::processorPhiVsRefLayer[iProcessor][iRefLayer];
  
  ///FIX ME 2Pi wrapping  
  while(!OMTFConfiguration::isInConeRange(iPhiStart,logicConeSize,iPhi) && iCone<6){
    ++iCone;
    iPhiStart+=logicConeSize;    
  }

  if(iCone>5) iCone = 99;
  return iCone;
}
///////////////////////////////////////////////
///////////////////////////////////////////////
