#include <cassert>
#include <iostream>

#include "UserCode/OMTFSimulation/interface/OMTFinput.h"
#include "UserCode/OMTFSimulation/interface/OMTFConfiguration.h"
#include "UserCode/OMTFSimulation/interface/XMLConfigReader.h"

///////////////////////////////////////////////////
///////////////////////////////////////////////////
OMTFinput::OMTFinput(){

  clear();

}
///////////////////////////////////////////////////
///////////////////////////////////////////////////
const OMTFinput::vector1D & OMTFinput::getLayerData(unsigned int iLayer) const{ 
  assert(iLayer<measurements.size());
  return measurements[iLayer];
}
///////////////////////////////////////////////////
///////////////////////////////////////////////////
bool OMTFinput::addLayerHit(unsigned int iLayer,
			    unsigned int iInput,
			    int iPhi){

  assert(iLayer<OMTFConfiguration::nLayers);
  assert(iInput<14);

  if(measurements[iLayer][iInput]!=(int)OMTFConfiguration::nPhiBins) ++iInput;
  
  if(iInput>13) return false;
  measurements[iLayer][iInput] = iPhi;

  return true;				      
}
///////////////////////////////////////////////////
///////////////////////////////////////////////////
void OMTFinput::readData(XMLConfigReader *aReader, 
			 unsigned int iEvent){

  measurements = aReader->readEvent(iEvent);
  
}
///////////////////////////////////////////////////
///////////////////////////////////////////////////
void OMTFinput::clear(){

  vector1D aLayer1D(14,OMTFConfiguration::nPhiBins);
  measurements.assign(OMTFConfiguration::nLayers,aLayer1D);

}
///////////////////////////////////////////////////
///////////////////////////////////////////////////
void  OMTFinput::shiftMyPhi(int phiShift){

for(unsigned int iLogicLayer=0;iLogicLayer<measurements.size();++iLogicLayer){
    for(unsigned int iHit=0;iHit<measurements[iLogicLayer].size();++iHit){
      if(!OMTFConfiguration::bendingLayers.count(iLogicLayer) &&
	 measurements[iLogicLayer][iHit]<(int)OMTFConfiguration::nPhiBins){
	if(measurements[iLogicLayer][iHit]<0) measurements[iLogicLayer][iHit]+=OMTFConfiguration::nPhiBins;
	measurements[iLogicLayer][iHit]-=phiShift;
	if(measurements[iLogicLayer][iHit]<0) measurements[iLogicLayer][iHit]+=OMTFConfiguration::nPhiBins;
	measurements[iLogicLayer][iHit]+=-511;
      }
    }
  }
}
///////////////////////////////////////////////////
///////////////////////////////////////////////////
std::ostream & OMTFinput::print(std::ostream & out) const{

  for(unsigned int iLogicLayer=0;iLogicLayer<measurements.size();++iLogicLayer){
    out<<"Logic layer: "<<iLogicLayer<<" Hits: ";
    for(unsigned int iHit=0;iHit<measurements[iLogicLayer].size();++iHit){
      out<<measurements[iLogicLayer][iHit]<<"\t";
    }
    out<<std::endl;
  }
  return out;
}
///////////////////////////////////////////////////
///////////////////////////////////////////////////
