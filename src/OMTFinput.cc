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
std::bitset<80> OMTFinput::getRefHits(unsigned int iProcessor) const{
 
  std::bitset<80> refHits;

  unsigned int iRefHit = 0;
  for(auto iRefHitDef:OMTFConfiguration::refHitsDefs[iProcessor]){  
    int iPhi = getLayerData(OMTFConfiguration::refToLogicNumber[iRefHitDef.iRefLayer])[iRefHitDef.iInput];    
    if(iPhi<(int)OMTFConfiguration::nPhiBins) refHits.set(iRefHit, iRefHitDef.fitsRange(iPhi));
    iRefHit++;
  }
  return refHits;
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
	if(measurements[iLogicLayer][iHit]<-511 ||
	   measurements[iLogicLayer][iHit]>511) measurements[iLogicLayer][iHit] = (int)OMTFConfiguration::nPhiBins;	   
      }
    }
  }
}
///////////////////////////////////////////////////
///////////////////////////////////////////////////
std::ostream & operator << (std::ostream &out, const OMTFinput & aInput){
  
for(unsigned int iLogicLayer=0;iLogicLayer<aInput.measurements.size();++iLogicLayer){
    out<<"Logic layer: "<<iLogicLayer<<" Hits: ";
    for(unsigned int iHit=0;iHit<aInput.measurements[iLogicLayer].size();++iHit){
      out<<aInput.measurements[iLogicLayer][iHit]<<"\t";
    }
    out<<std::endl;
  }
  return out;


}
///////////////////////////////////////////////////
///////////////////////////////////////////////////
