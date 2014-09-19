#include <iostream>

#include "interface/OMTFProcessor.h"
#include "interface/OMTFConfiguration.h"
#include "interface/GoldenPattern.h"
#include "interface/XMLConfigReader.h"
#include "interface/OMTFinput.h"
#include "interface/OMTFResult.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

///////////////////////////////////////////////
///////////////////////////////////////////////
OMTFProcessor::OMTFProcessor(const edm::ParameterSet & theConfig){

  if ( !theConfig.exists("patternsXMLFiles") ) return;
  std::vector<std::string> fileNames = theConfig.getParameter<std::vector<std::string> >("patternsXMLFiles");

  XMLConfigReader myReader;
  for(auto it: fileNames){
   myReader.setPatternsFile(it);
   configure(&myReader);
  }
}
///////////////////////////////////////////////
///////////////////////////////////////////////
OMTFProcessor::~OMTFProcessor(){

   for(auto it: theGPs) delete it.second;

}
///////////////////////////////////////////////
///////////////////////////////////////////////
bool OMTFProcessor::configure(XMLConfigReader *aReader){

  const std::vector<GoldenPattern *> & aGPs = aReader->readPatterns();
  for(auto it: aGPs){    
    if(!addGP(it)) return false;
  }

  return true;
}
///////////////////////////////////////////////
///////////////////////////////////////////////
bool OMTFProcessor::addGP(GoldenPattern *aGP){

  if(theGPs.find(aGP->key())!=theGPs.end()) return false;
  else theGPs[aGP->key()] = new GoldenPattern(*aGP);

  myResults[aGP->key()] = OMTFResult(); 

  return true;
}
///////////////////////////////////////////////
///////////////////////////////////////////////
const std::map<Key,GoldenPattern*> & OMTFProcessor::getPatterns() const{

  return theGPs;

}
///////////////////////////////////////////////
///////////////////////////////////////////////
OMTFProcessor::resultsMap OMTFProcessor::processInput(unsigned int iProcessor,
						      const OMTFinput & aInput){

  for(auto & itKey: myResults) itKey.second.clear();

  for(unsigned int iRefLayer=0;iRefLayer<OMTFConfiguration::nRefLayers;++iRefLayer){
    const OMTFinput::vector1D & refLayerHits = aInput.getLayerData(OMTFConfiguration::refToLogicNumber[iRefLayer]);	
    if(!refLayerHits.size()) continue;
    for(unsigned int iLayer=0;iLayer<OMTFConfiguration::nLayers;++iLayer){
      const OMTFinput::vector1D & layerHits = aInput.getLayerData(iLayer);
      if(!layerHits.size()) continue;
      for(auto itRefHit: refLayerHits){	
	  int phiRef = itRefHit;
	  unsigned int iCone = getConeNumber(iProcessor,iRefLayer,phiRef);
	  if(iCone>5) continue;
	  fillInputRange(iProcessor,iCone,aInput);
	  //continue;//////////////////////////////////////////////////////////////////
	  if(phiRef>=(int)OMTFConfiguration::nPhiBins) continue;
	  if(OMTFConfiguration::bendingLayers.count(iLayer)) phiRef = 0;
	  const OMTFinput::vector1D restricedLayerHits = restrictInput(iProcessor, iCone, iLayer,layerHits);
	  for(auto itGP: theGPs){
	    GoldenPattern::layerResult aLayerResult = itGP.second->process1Layer1RefLayer(iRefLayer,iLayer,
											  phiRef,
											  restricedLayerHits);
	    //restricedLayerHits);
	    myResults[itGP.second->key()].addResult(iRefLayer,iLayer,aLayerResult.first,phiRef);	 
	  }
      }      
    }
  }

  for(auto & itKey: myResults) itKey.second.finalise();

  return myResults;
}   
///////////////////////////////////////////////
///////////////////////////////////////////////
void OMTFProcessor::fillInputRange(unsigned int iProcessor,
				   unsigned int iCone,
				   const OMTFinput & aInput){

  for(unsigned int iLogicLayer=0;iLogicLayer<OMTFConfiguration::nLayers;++iLogicLayer){
    for(unsigned int iHit=0;iHit<14;++iHit){
      bool isHit = aInput.getLayerData(iLogicLayer)[iHit]<(int)OMTFConfiguration::nPhiBins;
      OMTFConfiguration::measurements4D[iProcessor][iCone][iLogicLayer][iHit]+=isHit;
    }
  }
}
///////////////////////////////////////////////
///////////////////////////////////////////////
bool OMTFProcessor::isInConeRange(int iPhiStart,
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
unsigned int OMTFProcessor::getConeNumber(unsigned int iProcessor,
					  unsigned int iRefLayer,
					  int iPhi){

  if(iPhi>=(int)OMTFConfiguration::nPhiBins) return 99;

  unsigned int logicConeSize = 10/360.0*OMTFConfiguration::nPhiBins;
  

  unsigned int iCone = 0;
  int iPhiStart = OMTFConfiguration::processorPhiVsRefLayer[iProcessor][iRefLayer];
  
  ///FIX ME 2Pi wrapping  
  while(!isInConeRange(iPhiStart,logicConeSize,iPhi) && iCone<6){
    ++iCone;
    iPhiStart+=logicConeSize;    
  }

  if(iCone>5) iCone = 99;
  return iCone;
}
////////////////////////////////////////////
////////////////////////////////////////////
OMTFinput::vector1D OMTFProcessor::restrictInput(unsigned int iProcessor,
						 unsigned int iCone,
						 unsigned int iLayer,
						 const OMTFinput::vector1D & layerHits){

  OMTFinput::vector1D myHits = layerHits;
  unsigned int iStart = OMTFConfiguration::connections[iProcessor][iCone][iLayer].first;
  unsigned int iEnd = iStart + OMTFConfiguration::connections[iProcessor][iCone][iLayer].second -1;

  for(unsigned int iHit=0;iHit<14;++iHit){
    if(iHit<iStart || iHit>iEnd) myHits[iHit] = OMTFConfiguration::nPhiBins;    
  }
  return myHits;
}
////////////////////////////////////////////
////////////////////////////////////////////
