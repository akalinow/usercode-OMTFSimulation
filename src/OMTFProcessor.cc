#include <iostream>
#include <algorithm>
#include <strstream>

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "UserCode/OMTFSimulation/interface/OMTFProcessor.h"
#include "UserCode/OMTFSimulation/interface/OMTFConfiguration.h"
#include "UserCode/OMTFSimulation/interface/GoldenPattern.h"
#include "UserCode/OMTFSimulation/interface/XMLConfigReader.h"
#include "UserCode/OMTFSimulation/interface/OMTFinput.h"
#include "UserCode/OMTFSimulation/interface/OMTFResult.h"

#include "L1Trigger/RPCTrigger/interface/RPCConst.h"

#include "SimDataFormats/Track/interface/SimTrack.h"

///////////////////////////////////////////////
///////////////////////////////////////////////
OMTFProcessor::OMTFProcessor(const edm::ParameterSet & theConfig){


  myResults.assign(OMTFConfiguration::nTestRefHits,OMTFProcessor::resultsMap());

  if ( !theConfig.exists("patternsXMLFiles") ) return;
  std::vector<std::string> fileNames = theConfig.getParameter<std::vector<std::string> >("patternsXMLFiles");

  XMLConfigReader myReader;
  for(auto it: fileNames){
   myReader.setPatternsFile(it);
   configure(&myReader);
  }

  averagePatterns(1);
  averagePatterns(-1);

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

  //if(aGP->key().thePtCode<14 && aGP->key().thePtCode%2==1) return true;
  //if(aGP->key().thePtCode>18 && aGP->key().thePtCode%2==1) return true;
  //if(aGP->key().thePtCode%2==1) return true;

  if(theGPs.find(aGP->key())!=theGPs.end()) return false;
  else theGPs[aGP->key()] = new GoldenPattern(*aGP);

  for(auto & itRegion: myResults) itRegion[aGP->key()] = OMTFResult(); 

  return true;
}
///////////////////////////////////////////////
///////////////////////////////////////////////
void  OMTFProcessor::averagePatterns(int charge){

  Key aKey(1, 6, charge);

  while(theGPs.find(aKey)!=theGPs.end()){

    GoldenPattern *aGP1 = theGPs.find(aKey)->second;
    GoldenPattern *aGP2 = aGP1;
    GoldenPattern *aGP3 = aGP1;
    GoldenPattern *aGP4 = aGP1;

    ++aKey.thePtCode;
    if(aKey.thePtCode<=31 && theGPs.find(aKey)!=theGPs.end()) aGP2 =  theGPs.find(aKey)->second;

    ++aKey.thePtCode;
    if(aKey.thePtCode<=31 && theGPs.find(aKey)!=theGPs.end()) aGP3 =  theGPs.find(aKey)->second;

    ++aKey.thePtCode;
    if(aKey.thePtCode<=31 && theGPs.find(aKey)!=theGPs.end()) aGP4 =  theGPs.find(aKey)->second;

    ++aKey.thePtCode;



    std::cout<<"Klucz: "
	     <<aGP1->key()
	     <<" "<<aGP2->key()
	     <<" "<<aGP3->key()
	     <<" "<<aGP4->key()<<std::endl;


    GoldenPattern::vector2D meanDistPhi  = aGP1->getMeanDistPhi();
    GoldenPattern::vector2D meanDistPhi1  = aGP1->getMeanDistPhi();
    GoldenPattern::vector2D meanDistPhi2  = aGP2->getMeanDistPhi();
    GoldenPattern::vector2D meanDistPhi3  = aGP3->getMeanDistPhi();
    GoldenPattern::vector2D meanDistPhi4  = aGP4->getMeanDistPhi();
    
    for(unsigned int iLayer=0;iLayer<OMTFConfiguration::nLayers;++iLayer){
      for(unsigned int iRefLayer=0;iRefLayer<OMTFConfiguration::nRefLayers;++iRefLayer){
	meanDistPhi[iLayer][iRefLayer]+=meanDistPhi2[iLayer][iRefLayer];
	meanDistPhi[iLayer][iRefLayer]+=meanDistPhi3[iLayer][iRefLayer];
	meanDistPhi[iLayer][iRefLayer]+=meanDistPhi4[iLayer][iRefLayer];

	meanDistPhi[iLayer][iRefLayer]/=4;
      }
    }
    
    aGP1->setMeanDistPhi(meanDistPhi);
    aGP2->setMeanDistPhi(meanDistPhi);
    aGP3->setMeanDistPhi(meanDistPhi);
    aGP4->setMeanDistPhi(meanDistPhi);

    shiftGP(aGP1,meanDistPhi, meanDistPhi1);
    shiftGP(aGP2,meanDistPhi, meanDistPhi2);   
    shiftGP(aGP3,meanDistPhi, meanDistPhi3);   
    shiftGP(aGP4,meanDistPhi, meanDistPhi4);   
  }  
}
///////////////////////////////////////////////
///////////////////////////////////////////////
void OMTFProcessor::shiftGP(GoldenPattern *aGP,
			    const GoldenPattern::vector2D & meanDistPhiNew,
			    const GoldenPattern::vector2D & meanDistPhiOld){

  ///Shift pdfs by differecne between original menaDistPhi, and
  ///the averaged value
  unsigned int nPdfBins =  exp2(OMTFConfiguration::nPdfAddrBits);

  GoldenPattern::vector3D pdfAllRef = aGP->getPdf();

  int indexShift = 0;
  for(unsigned int iLayer=0;iLayer<OMTFConfiguration::nLayers;++iLayer){
    for(unsigned int iRefLayer=0;iRefLayer<OMTFConfiguration::nRefLayers;++iRefLayer){
      indexShift = meanDistPhiOld[iLayer][iRefLayer] - meanDistPhiNew[iLayer][iRefLayer];
      for(unsigned int iPdfBin=0;iPdfBin<nPdfBins;++iPdfBin) pdfAllRef[iLayer][iRefLayer][iPdfBin] = 0;
	for(unsigned int iPdfBin=0;iPdfBin<nPdfBins;++iPdfBin){
	  if((int)(iPdfBin)+indexShift>=0 && iPdfBin+indexShift<nPdfBins)
	    pdfAllRef[iLayer][iRefLayer][iPdfBin+indexShift] = aGP->pdfValue(iLayer, iRefLayer, iPdfBin);
	}
      }
    }
    aGP->setPdf(pdfAllRef);
}
///////////////////////////////////////////////
///////////////////////////////////////////////
const std::map<Key,GoldenPattern*> & OMTFProcessor::getPatterns() const{ return theGPs; }
///////////////////////////////////////////////
///////////////////////////////////////////////
const std::vector<OMTFProcessor::resultsMap> & OMTFProcessor::processInput(unsigned int iProcessor,
									   const OMTFinput & aInput){

  for(auto & itRegion: myResults) for(auto & itKey: itRegion) itKey.second.clear();

  //////////////////////////////////////
  //////////////////////////////////////  
  std::bitset<128> refHitsBits = aInput.getRefHits(iProcessor);
  if(refHitsBits.none()) return myResults;
   
  for(unsigned int iLayer=0;iLayer<OMTFConfiguration::nLayers;++iLayer){
    const OMTFinput::vector1D & layerHits = aInput.getLayerData(iLayer);
    if(!layerHits.size()) continue;
    ///Number of reference hits to be checked. 
    ///Value read from XML configuration
    unsigned int nTestedRefHits = OMTFConfiguration::nTestRefHits;
    for(unsigned int iRefHit=0;iRefHit<OMTFConfiguration::nRefHits;++iRefHit){
      if(!refHitsBits[iRefHit]) continue;
      if(nTestedRefHits--==0) break;
      const RefHitDef & aRefHitDef = OMTFConfiguration::refHitsDefs[iProcessor][iRefHit];
      int phiRef = aInput.getLayerData(OMTFConfiguration::refToLogicNumber[aRefHitDef.iRefLayer])[aRefHitDef.iInput]; 
      unsigned int iRegion = aRefHitDef.iRegion;
      if(OMTFConfiguration::bendingLayers.count(iLayer)) phiRef = 0;
      const OMTFinput::vector1D restrictedLayerHits = restrictInput(iProcessor, iRegion, iLayer,layerHits);
      for(auto itGP: theGPs){
	GoldenPattern::layerResult aLayerResult = itGP.second->process1Layer1RefLayer(aRefHitDef.iRefLayer,iLayer,
										      phiRef,
										      restrictedLayerHits);
	myResults[OMTFConfiguration::nTestRefHits-nTestedRefHits-1][itGP.second->key()].addResult(aRefHitDef.iRefLayer,iLayer,aLayerResult.first,phiRef);	 
      }
    }
  }  
  //////////////////////////////////////
  ////////////////////////////////////// 
  for(auto & itRefHit: myResults) for(auto & itKey: itRefHit) itKey.second.finalise();

  //#ifndef NDEBUG
  std::ostringstream myStr;
  myStr<<"iProcessor: "<<iProcessor<<std::endl;
  myStr<<"Input: ------------"<<std::endl;
  myStr<<aInput<<std::endl;
  
  for(auto itRefHit: myResults){
    myStr<<"--- Reference hit ---"<<std::endl;
    for(auto itKey: itRefHit){      
      if(itKey.second.empty()) continue;
      myStr<<itKey.first<<std::endl;
      myStr<<itKey.second<<std::endl;
    }
    myStr<<"--------------------"<<std::endl;
  }
  //LogDebug("OMTF processor")<<myStr.str();
  edm::LogInfo("OMTF processor")<<myStr.str();
  //#endif
  
  return myResults;
}   
////////////////////////////////////////////
////////////////////////////////////////////
OMTFinput OMTFProcessor::shiftInput(unsigned int iProcessor,
				    const OMTFinput & aInput){

  int minPhi =  OMTFConfiguration::globalPhiStart(iProcessor);

  ///OMTFConfiguration::nPhiBins/2 to shift the minPhi to 0-nBins scale,
  if(minPhi<0) minPhi+=OMTFConfiguration::nPhiBins;

  OMTFinput myCopy = aInput;
  myCopy.shiftMyPhi(minPhi);
  
  return myCopy;
}
////////////////////////////////////////////
////////////////////////////////////////////
OMTFinput::vector1D OMTFProcessor::restrictInput(unsigned int iProcessor,
						 unsigned int iRegion,
						 unsigned int iLayer,
						 const OMTFinput::vector1D & layerHits){

  OMTFinput::vector1D myHits = layerHits;
  unsigned int iStart = OMTFConfiguration::connections[iProcessor][iRegion][iLayer].first;
  unsigned int iEnd = iStart + OMTFConfiguration::connections[iProcessor][iRegion][iLayer].second -1;

  for(unsigned int iHit=0;iHit<14;++iHit){
    if(iHit<iStart || iHit>iEnd) myHits[iHit] = OMTFConfiguration::nPhiBins;    
  }
  return myHits;
}
////////////////////////////////////////////
////////////////////////////////////////////
void OMTFProcessor::fillCounts(unsigned int iProcessor,
			       const OMTFinput & aInput,
			       const SimTrack* aSimMuon){

  int theCharge = (abs(aSimMuon->type()) == 13) ? aSimMuon->type()/-13 : 0;
  unsigned int  iPt =  RPCConst::iptFromPt(aSimMuon->momentum().pt());

  //////////////////////////////////////  
  std::bitset<128> refHitsBits = aInput.getRefHits(iProcessor);
  if(refHitsBits.none()) return;

  std::ostringstream myStr;
  myStr<<"iProcessor: "<<iProcessor<<std::endl;
  myStr<<"Input: ------------"<<std::endl;
  myStr<<aInput<<std::endl;
  edm::LogInfo("OMTF processor")<<myStr.str();
   
  for(unsigned int iLayer=0;iLayer<OMTFConfiguration::nLayers;++iLayer){
    const OMTFinput::vector1D & layerHits = aInput.getLayerData(iLayer);
    if(!layerHits.size()) continue;
    ///Number of reference hits to be checked. 
    ///Value read from XML configuration
    unsigned int nTestedRefHits = OMTFConfiguration::nTestRefHits;
    for(unsigned int iRefHit=0;iRefHit<OMTFConfiguration::nRefHits;++iRefHit){
      if(!refHitsBits[iRefHit]) continue;
      if(nTestedRefHits--==0) break;
      const RefHitDef & aRefHitDef = OMTFConfiguration::refHitsDefs[iProcessor][iRefHit];
      int phiRef = aInput.getLayerData(OMTFConfiguration::refToLogicNumber[aRefHitDef.iRefLayer])[aRefHitDef.iInput]; 
      unsigned int iRegion = aRefHitDef.iRegion;
      if(OMTFConfiguration::bendingLayers.count(iLayer)) phiRef = 0;
      const OMTFinput::vector1D restrictedLayerHits = restrictInput(iProcessor, iRegion, iLayer,layerHits);
      for(auto itGP: theGPs){	
	if(itGP.first.theCharge!=theCharge) continue;
	if(itGP.first.thePtCode!=iPt) continue;
        itGP.second->addCount(aRefHitDef.iRefLayer,iLayer,phiRef,restrictedLayerHits);
      }
    }
  }
}
////////////////////////////////////////////
////////////////////////////////////////////
