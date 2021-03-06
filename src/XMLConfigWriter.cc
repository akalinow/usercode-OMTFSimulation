#include "UserCode/OMTFSimulation/interface/XMLConfigWriter.h"
#include "UserCode/OMTFSimulation/interface/GoldenPattern.h"
#include "UserCode/OMTFSimulation/interface/OMTFinput.h"
#include "UserCode/OMTFSimulation/interface/OMTFConfiguration.h"
#include "UserCode/OMTFSimulation/interface/OMTFResult.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1RpcTriggerAnalysisEfficiencyUtilities.h"

#include <iostream>
#include <sstream>
#include <cmath>

#include "xercesc/framework/StdOutFormatTarget.hpp"
#include "xercesc/framework/LocalFileFormatTarget.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/dom/DOM.hpp"
#include "xercesc/dom/DOMException.hpp"
#include "xercesc/dom/DOMImplementation.hpp"
#include "xercesc/sax/HandlerBase.hpp"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/util/XercesDefs.hpp"
#include "xercesc/util/XercesVersion.hpp"
XERCES_CPP_NAMESPACE_USE

#if _XERCES_VERSION <30100
#include "xercesc/dom/DOMWriter.hpp"
#else
 #include "xercesc/dom/DOMLSSerializer.hpp"
 #include "xercesc/dom/DOMLSOutput.hpp"
#endif
//

//////////////////////////////////
// XMLConfigWriter
//////////////////////////////////
inline std::string _toString(XMLCh const* toTranscode) {
std::string tmp(xercesc::XMLString::transcode(toTranscode));
return tmp;
}

inline XMLCh*  _toDOMS(std::string temp) {
  XMLCh* buff = XMLString::transcode(temp.c_str());
  return  buff;
}
////////////////////////////////////
////////////////////////////////////
XMLConfigWriter::XMLConfigWriter(){

  XMLPlatformUtils::Initialize();
  
  ///Initialise XML document
  domImpl = DOMImplementationRegistry::getDOMImplementation(_toDOMS("Range"));   


}
//////////////////////////////////////////////////
//////////////////////////////////////////////////
void XMLConfigWriter::initialiseXMLDocument(const std::string & docName){

  theDoc = domImpl->createDocument(0,_toDOMS(docName.c_str()), 0);
  theTopElement = theDoc->getDocumentElement();    

}
//////////////////////////////////////////////////
//////////////////////////////////////////////////
void XMLConfigWriter::finaliseXMLDocument(const std::string & fName){

  XMLFormatTarget* formTarget = new LocalFileFormatTarget(fName.c_str());

#if _XERCES_VERSION <30100
  xercesc::DOMWriter* domWriter = (dynamic_cast<DOMImplementation*>(domImpl))->createDOMWriter();
  if(domWriter->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true)){
    domWriter->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);
    }
  domWriter->writeNode(formTarget,*theTopElement);
  
#else
  xercesc::DOMLSSerializer*  theSerializer = (dynamic_cast<DOMImplementation*>(domImpl))->createLSSerializer();
  if (theSerializer->getDomConfig()->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true))
    theSerializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
  DOMLSOutput* theOutput = (dynamic_cast<DOMImplementation*>(domImpl))->createLSOutput();
  theOutput->setByteStream(formTarget);
  theSerializer->write(theTopElement, theOutput);        
  theOutput->release();
  theSerializer->release();
#endif
  
  delete formTarget;
}
//////////////////////////////////////////////////
//////////////////////////////////////////////////
xercesc::DOMElement * XMLConfigWriter::writeEventHeader(unsigned int eventId){

  unsigned int eventBx = eventId*2;

  xercesc::DOMElement *aEvent = 0;
  xercesc::DOMElement *aBx = 0;
  std::ostringstream stringStr;

  aEvent = theDoc->createElement(_toDOMS("Event"));

  stringStr.str("");
  stringStr<<eventId;
  aEvent->setAttribute(_toDOMS("iEvent"), _toDOMS(stringStr.str()));
  aBx = theDoc->createElement(_toDOMS("bx"));
  stringStr.str("");
  stringStr<<eventBx;
  aBx->setAttribute(_toDOMS("iBx"), _toDOMS(stringStr.str()));
  aEvent->appendChild(aBx);
   
  theTopElement->appendChild(aEvent);

  return aBx;
}
//////////////////////////////////////////////////
//////////////////////////////////////////////////
xercesc::DOMElement * XMLConfigWriter::writeEventData(xercesc::DOMElement *aTopElement,
						      unsigned int iProcessor,
						      const OMTFinput & aInput){

  std::ostringstream stringStr;

  xercesc::DOMElement *aProcessor = theDoc->createElement(_toDOMS("Processor"));
  stringStr.str("");
  stringStr<<iProcessor;
  aProcessor->setAttribute(_toDOMS("iProcessor"), _toDOMS(stringStr.str()));
  
  xercesc::DOMElement *aLayer, *aHit; 
  for(unsigned int iLayer=0;iLayer<OMTFConfiguration::nLayers;++iLayer){
    const OMTFinput::vector1D & layerData = aInput.getLayerData(iLayer);
    aLayer = theDoc->createElement(_toDOMS("Layer"));
    stringStr.str("");
    stringStr<<iLayer;
    aLayer->setAttribute(_toDOMS("iLayer"), _toDOMS(stringStr.str()));
    int iHit = 0;
    for(auto hitIt:layerData){
      aHit = theDoc->createElement(_toDOMS("Hit"));
      stringStr.str("");
      stringStr<<iHit;
      aHit->setAttribute(_toDOMS("iHit"), _toDOMS(stringStr.str()));
      stringStr.str("");
      stringStr<<hitIt;
      aHit->setAttribute(_toDOMS("iPhi"), _toDOMS(stringStr.str()));
      ++iHit;
      if(hitIt>=(int)OMTFConfiguration::nPhiBins) continue;
      aLayer->appendChild(aHit);

    }
    if(aLayer->getChildNodes()->getLength()) aProcessor->appendChild(aLayer);   
  }

  aTopElement->appendChild(aProcessor);
  return aProcessor;

}
//////////////////////////////////////////////////
//////////////////////////////////////////////////
void  XMLConfigWriter::writeCandidateData(xercesc::DOMElement *aTopElement,
					  unsigned int iRegion,
					  const L1Obj & aCand){

  xercesc::DOMElement* aResult = theDoc->createElement(_toDOMS("Candidate"));
  std::ostringstream stringStr;
  stringStr.str("");
  stringStr<<iRegion;
  aResult->setAttribute(_toDOMS("iRegion"),_toDOMS(stringStr.str()));
  stringStr.str("");
  stringStr<<L1RpcTriggerAnalysisEfficiencyUtilities::PtScale::ptCode(aCand.pt);
  aResult->setAttribute(_toDOMS("ptCode"),_toDOMS(stringStr.str()));
  stringStr.str("");
  stringStr<<aCand.charge;
  aResult->setAttribute(_toDOMS("charge"),_toDOMS(stringStr.str()));
  stringStr.str("");
  stringStr<<aCand.q;
  aResult->setAttribute(_toDOMS("nHits"), _toDOMS(stringStr.str()));
  stringStr.str("");
  stringStr<<aCand.disc;
  aResult->setAttribute(_toDOMS("disc"), _toDOMS(stringStr.str()));
  aTopElement->appendChild(aResult); 
}
//////////////////////////////////////////////////
//////////////////////////////////////////////////
void XMLConfigWriter::writeResultsData(xercesc::DOMElement *aTopElement,
				       unsigned int iRegion,
				       const Key & aKey,
				       const OMTFResult & aResult){

  OMTFResult::vector2D results = aResult.getResults();

  std::ostringstream stringStr;
  ///Write GP key parameters
  xercesc::DOMElement* aGP = theDoc->createElement(_toDOMS("GP"));
  stringStr.str("");
  stringStr<<aKey.thePtCode;
  aGP->setAttribute(_toDOMS("iPt"), _toDOMS(stringStr.str()));
  stringStr.str("");
  stringStr<<aKey.theEtaCode;
  aGP->setAttribute(_toDOMS("iEta"), _toDOMS(stringStr.str()));
  stringStr.str("");
  stringStr<<"0";
  aGP->setAttribute(_toDOMS("iPhi"), _toDOMS(stringStr.str()));
  stringStr.str("");
  stringStr<<aKey.theCharge;
  aGP->setAttribute(_toDOMS("iCharge"), _toDOMS(stringStr.str()));
  /////////////////
  ///Write results details for this GP
  for(unsigned int iRefLayer=0;iRefLayer<OMTFConfiguration::nRefLayers;++iRefLayer){
    xercesc::DOMElement* aRefLayer = theDoc->createElement(_toDOMS("Result"));
    stringStr.str("");
    stringStr<<iRefLayer;
    aRefLayer->setAttribute(_toDOMS("iRefLayer"), _toDOMS(stringStr.str()));
    stringStr.str("");
    stringStr<<iRegion;
    aRefLayer->setAttribute(_toDOMS("iRegion"), _toDOMS(stringStr.str()));
    stringStr.str("");
    stringStr<<OMTFConfiguration::refToLogicNumber[iRefLayer];
    aRefLayer->setAttribute(_toDOMS("iLogicLayer"), _toDOMS(stringStr.str()));
    for(unsigned int iLogicLayer=0;iLogicLayer<OMTFConfiguration::nLayers;++iLogicLayer){
      xercesc::DOMElement* aLayer = theDoc->createElement(_toDOMS("Layer"));
      stringStr.str("");
      stringStr<<iLogicLayer;
      aLayer->setAttribute(_toDOMS("iLayer"), _toDOMS(stringStr.str()));
      stringStr.str("");
      stringStr<<results[iLogicLayer][iRefLayer];
      aLayer->setAttribute(_toDOMS("value"), _toDOMS(stringStr.str()));
      if(results[iLogicLayer][iRefLayer]) aRefLayer->appendChild(aLayer);
    }
    if(aRefLayer->getChildNodes()->getLength()) aGP->appendChild(aRefLayer);
  }
  if(aGP->getChildNodes()->getLength()) aTopElement->appendChild(aGP);   
}
//////////////////////////////////////////////////
//////////////////////////////////////////////////
void XMLConfigWriter::writeGPData(const GoldenPattern & aGP){

  std::ostringstream stringStr;
  xercesc::DOMElement *aLayer=0, *aRefLayer=0, *aPdf=0;

  xercesc::DOMElement* aGPElement = theDoc->createElement(_toDOMS("GP"));
  stringStr.str("");
  stringStr<<aGP.key().thePtCode;
  aGPElement->setAttribute(_toDOMS("iPt"), _toDOMS(stringStr.str()));
  stringStr.str("");
  stringStr<<aGP.key().theEtaCode;
  aGPElement->setAttribute(_toDOMS("iEta"), _toDOMS(stringStr.str()));
  stringStr.str("");
  //stringStr<<aGP.key().thePhiCode;
  stringStr<<"0";
  aGPElement->setAttribute(_toDOMS("iPhi"), _toDOMS(stringStr.str()));
  stringStr.str("");
  stringStr<<aGP.key().theCharge;
  aGPElement->setAttribute(_toDOMS("iCharge"), _toDOMS(stringStr.str()));
  for(unsigned int iLayer = 0;iLayer<OMTFConfiguration::nLayers;++iLayer){
    int nOfPhis = 0;
    aLayer = theDoc->createElement(_toDOMS("Layer"));
    stringStr.str("");
    stringStr<<iLayer;
    aLayer->setAttribute(_toDOMS("iLayer"), _toDOMS(stringStr.str()));
    stringStr.str("");
    stringStr<<nOfPhis;
    aLayer->setAttribute(_toDOMS("nOfPhis"), _toDOMS(stringStr.str()));
    for(unsigned int iRefLayer=0;iRefLayer<OMTFConfiguration::nRefLayers;++iRefLayer){
      aRefLayer = theDoc->createElement(_toDOMS("RefLayer"));
      int meanDistPhi = aGP.meanDistPhiValue(iLayer,iRefLayer);	       
      stringStr.str("");
      stringStr<<meanDistPhi;
      aRefLayer->setAttribute(_toDOMS("meanDistPhi"), _toDOMS(stringStr.str()));
      int selDistPhi = 0;
      stringStr.str("");
      stringStr<<selDistPhi;
      aRefLayer->setAttribute(_toDOMS("selDistPhi"), _toDOMS(stringStr.str()));
      int selDistPhiShift = 0;
      stringStr.str("");
      stringStr<<selDistPhiShift;
      aRefLayer->setAttribute(_toDOMS("selDistPhiShift"), _toDOMS(stringStr.str()));
      int distMsbPhiShift = 0;
      stringStr.str("");
      stringStr<<distMsbPhiShift;
      aRefLayer->setAttribute(_toDOMS("distMsbPhiShift"), _toDOMS(stringStr.str()));
      aLayer->appendChild(aRefLayer);
    }
    for(unsigned int iRefLayer=0;iRefLayer<OMTFConfiguration::nRefLayers;++iRefLayer){
      for(unsigned int iPdf=0;iPdf<exp2(OMTFConfiguration::nPdfAddrBits);++iPdf){
	aPdf = theDoc->createElement(_toDOMS("PDF"));
	stringStr.str("");
	stringStr<<aGP.pdfValue(iLayer,iRefLayer,iPdf);
	aPdf->setAttribute(_toDOMS("value"), _toDOMS(stringStr.str()));
	aLayer->appendChild(aPdf);
      }
    }
    aGPElement->appendChild(aLayer);
  }
  theTopElement->appendChild(aGPElement);
}
//////////////////////////////////////////////////
//////////////////////////////////////////////////
void  XMLConfigWriter::writeConnectionsData(const std::vector<std::vector <OMTFConfiguration::vector2D> > & measurements4D){

 std::ostringstream stringStr;

  for(unsigned int iProcessor=0;iProcessor<6;++iProcessor){
    xercesc::DOMElement* aProcessorElement = theDoc->createElement(_toDOMS("Processor"));
    stringStr.str("");
    stringStr<<iProcessor;
    aProcessorElement->setAttribute(_toDOMS("iProcessor"), _toDOMS(stringStr.str()));
    for(unsigned int iRefLayer=0;iRefLayer<OMTFConfiguration::nRefLayers;++iRefLayer){	
	xercesc::DOMElement* aRefLayerElement = theDoc->createElement(_toDOMS("RefLayer"));
	stringStr.str("");
	stringStr<<iRefLayer;
	aRefLayerElement->setAttribute(_toDOMS("iRefLayer"), _toDOMS(stringStr.str()));	
	stringStr.str("");
	stringStr<<OMTFConfiguration::processorPhiVsRefLayer[iProcessor][iRefLayer];
	aRefLayerElement->setAttribute(_toDOMS("iGlobalPhiStart"), _toDOMS(stringStr.str()));	
	aProcessorElement->appendChild(aRefLayerElement);
      }
    unsigned int iRefHit = 0;
    for(unsigned int iRegion=0;iRegion<6;++iRegion){
      xercesc::DOMElement* aRegionElement = theDoc->createElement(_toDOMS("LogicRegion"));
      stringStr.str("");
      stringStr<<iRegion;
      aRegionElement->setAttribute(_toDOMS("iRegion"), _toDOMS(stringStr.str()));   
      ////
      for(unsigned int iRefLayer=0;iRefLayer<OMTFConfiguration::nRefLayers;++iRefLayer){
	if(iRefLayer>5) continue;
	for(unsigned int iInput=0;iInput<14;++iInput){
	  unsigned int hitCount =  OMTFConfiguration::measurements4Dref[iProcessor][iRegion][iRefLayer][iInput];
	  if(!hitCount) continue;
	  xercesc::DOMElement* aRefHitElement = theDoc->createElement(_toDOMS("RefHit"));
	  stringStr.str("");
	  stringStr<<iRefHit;
	  aRefHitElement->setAttribute(_toDOMS("iRefHit"), _toDOMS(stringStr.str()));
	  stringStr.str("");
	  stringStr<<iRefLayer;
	  aRefHitElement->setAttribute(_toDOMS("iRefLayer"), _toDOMS(stringStr.str()));	  

	  stringStr.str("");
	  stringStr<<iRegion;
	  aRefHitElement->setAttribute(_toDOMS("iRegion"), _toDOMS(stringStr.str()));	  

	  stringStr.str("");
	  stringStr<<iInput;
	  aRefHitElement->setAttribute(_toDOMS("iInput"), _toDOMS(stringStr.str()));

	  unsigned int logicRegionSize = 10/360.0*OMTFConfiguration::nPhiBins;
	  ///iPhiMin and iPhiMax are expressed in 10 bit scale +-511 used in each processor
	  int iPhiMin = OMTFConfiguration::processorPhiVsRefLayer[iProcessor][iRefLayer]-OMTFConfiguration::globalPhiStart(iProcessor)-511;
	  int iPhiMax = iPhiMin+logicRegionSize-1;

	  iPhiMin+=iRegion*logicRegionSize;
	  iPhiMax+=iRegion*logicRegionSize;


	  stringStr.str("");
	  stringStr<<iPhiMin;
	  aRefHitElement->setAttribute(_toDOMS("iPhiMin"), _toDOMS(stringStr.str()));

	  stringStr.str("");
	  stringStr<<iPhiMax;
	  aRefHitElement->setAttribute(_toDOMS("iPhiMax"), _toDOMS(stringStr.str()));

	  aProcessorElement->appendChild(aRefHitElement);
	  ++iRefHit;
	}
      }
      for(;iRegion==5 && iRefHit<80;++iRefHit){

	xercesc::DOMElement* aRefHitElement = theDoc->createElement(_toDOMS("RefHit"));
	stringStr.str("");
	stringStr<<iRefHit;
	aRefHitElement->setAttribute(_toDOMS("iRefHit"), _toDOMS(stringStr.str()));
	stringStr.str("");
	stringStr<<0;
	aRefHitElement->setAttribute(_toDOMS("iRefLayer"), _toDOMS(stringStr.str()));

	stringStr.str("");
	stringStr<<0;
	aRefHitElement->setAttribute(_toDOMS("iRegion"), _toDOMS(stringStr.str()));

	stringStr.str("");
	stringStr<<0;
	aRefHitElement->setAttribute(_toDOMS("iInput"), _toDOMS(stringStr.str()));

	int iPhiMin = 0;
	int iPhiMax = 1;

	stringStr.str("");
	stringStr<<iPhiMin;
	aRefHitElement->setAttribute(_toDOMS("iPhiMin"), _toDOMS(stringStr.str()));

	stringStr.str("");
	stringStr<<iPhiMax;
	aRefHitElement->setAttribute(_toDOMS("iPhiMax"), _toDOMS(stringStr.str()));

	aProcessorElement->appendChild(aRefHitElement);
      }
    
      ////
      for(unsigned int iLogicLayer=0;iLogicLayer<OMTFConfiguration::nLayers;++iLogicLayer){
	xercesc::DOMElement* aLayerElement = theDoc->createElement(_toDOMS("Layer"));
	stringStr.str("");
	stringStr<<iLogicLayer;
	aLayerElement->setAttribute(_toDOMS("iLayer"), _toDOMS(stringStr.str()));
	const OMTFConfiguration::vector1D & myCounts = OMTFConfiguration::measurements4D[iProcessor][iRegion][iLogicLayer];
	unsigned int maxInput = findMaxInput(myCounts);
	unsigned int begin = 0, end = 0;
	if((int)maxInput-2>=0) begin = maxInput-2;
	else begin = maxInput;
	end = maxInput+3;
	stringStr.str("");
	stringStr<<begin;
	aLayerElement->setAttribute(_toDOMS("iFirstInput"), _toDOMS(stringStr.str()));
	stringStr.str("");
	stringStr<<end-begin+1;
	aLayerElement->setAttribute(_toDOMS("nInputs"), _toDOMS(stringStr.str()));
	aRegionElement->appendChild(aLayerElement);
      }
      aProcessorElement->appendChild(aRegionElement);
    }
    theTopElement->appendChild(aProcessorElement);
  }
}
//////////////////////////////////////////////////
//////////////////////////////////////////////////
unsigned int XMLConfigWriter::findMaxInput(const OMTFConfiguration::vector1D & myCounts){

  unsigned int max = 0;
  unsigned int maxInput = 0;
  for(unsigned int iInput=0;iInput<14;++iInput){
    if(myCounts[iInput]>(int)max){
      max = myCounts[iInput];
      maxInput = iInput;
    }
  }
  return maxInput;
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
