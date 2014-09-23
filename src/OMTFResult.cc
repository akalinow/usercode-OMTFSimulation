#include <iostream>
#include <ostream>

#include "UserCode/OMTFSimulation/interface/OMTFResult.h"
#include "UserCode/OMTFSimulation/interface/OMTFConfiguration.h"


////////////////////////////////////////////
////////////////////////////////////////////
OMTFResult::OMTFResult(){

  clear();

}
////////////////////////////////////////////
////////////////////////////////////////////
void OMTFResult::addResult(unsigned int iRefLayer,
			   unsigned int iLayer,
			   unsigned int val,
			   int iRefPhi){

  refPhi1D[iRefLayer] = iRefPhi;
  results[iLayer][iRefLayer] = val;

}
////////////////////////////////////////////
////////////////////////////////////////////
void OMTFResult::clear(){

  results1D.assign(OMTFConfiguration::nRefLayers,0);
  hits1D.assign(OMTFConfiguration::nRefLayers,0);
  results.assign(OMTFConfiguration::nLayers,results1D);
  refPhi1D.assign(OMTFConfiguration::nRefLayers,1024);
  
}
////////////////////////////////////////////
////////////////////////////////////////////
void OMTFResult::finalise(){

  for(unsigned int iLogicLayer=0;iLogicLayer<results.size();++iLogicLayer){
    unsigned int connectedLayer = OMTFConfiguration::logicToLogic[iLogicLayer];
    for(unsigned int iRefLayer=0;iRefLayer<results[iLogicLayer].size();++iRefLayer){
      ///If connected layer (POS or BEND) has not been fired, ignore this layer also
      unsigned int val = results[connectedLayer][iRefLayer]>0 ? results[iLogicLayer][iRefLayer]: 0;
      results1D[iRefLayer]+=val;
      ///Do not count bending layers in hit count
      if(!OMTFConfiguration::bendingLayers.count(iLogicLayer)) hits1D[iRefLayer]+=(val>0);
    }      
  }
}
////////////////////////////////////////////
////////////////////////////////////////////
std::ostream & OMTFResult::print(std::ostream & out) const{

 for(unsigned int iLogicLayer=0;iLogicLayer<results.size();++iLogicLayer){
    out<<"Logic layer: "<<iLogicLayer<<" results: ";
    for(unsigned int iRefLayer=0;iRefLayer<results[iLogicLayer].size();++iRefLayer){
      out<<results[iLogicLayer][iRefLayer]<<"\t";
    }
    out<<std::endl;
  }

 out<<"      Sum over layers: ";
 for(unsigned int iRefLayer=0;iRefLayer<results1D.size();++iRefLayer){
   out<<results1D[iRefLayer]<<"\t";
 }

 out<<std::endl;

 out<<"       Number of hits: ";
 for(unsigned int iRefLayer=0;iRefLayer<hits1D.size();++iRefLayer){
   out<<hits1D[iRefLayer]<<"\t";
 }

  return out;
}
////////////////////////////////////////////
////////////////////////////////////////////
