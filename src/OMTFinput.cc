#include <cassert>
#include "interface/OMTFinput.h"
#include "interface/OMTFConfiguration.h"
#include "interface/XMLConfigReader.h"

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
		 int iPhi){

  assert(iLayer<OMTFConfiguration::nLayers);
  if(measurements[iLayer].size()<6) measurements[iLayer].push_back(iPhi);
  else return false;

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

  vector1D aLayer1D;
  measurements.assign(OMTFConfiguration::nLayers,aLayer1D);

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
