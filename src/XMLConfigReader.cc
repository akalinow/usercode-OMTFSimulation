#include <iostream>
#include <cmath>

#include "UserCode/OMTFSimulation/interface/XMLConfigReader.h"
#include "UserCode/OMTFSimulation/interface/GoldenPattern.h"
#include "UserCode/OMTFSimulation/interface/OMTFinput.h"
#include "UserCode/OMTFSimulation/interface/OMTFConfiguration.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

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
XERCES_CPP_NAMESPACE_USE


//////////////////////////////////
// XMLConfigReader
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
XMLConfigReader::XMLConfigReader(){

  XMLPlatformUtils::Initialize();
  
  ///Initialise XML parser  
  parser = new XercesDOMParser(); 
  parser->setValidationScheme(XercesDOMParser::Val_Auto);
  parser->setDoNamespaces(false);

}
//////////////////////////////////////////////////
//////////////////////////////////////////////////
std::vector<GoldenPattern*> XMLConfigReader::readPatterns(){

  std::vector<GoldenPattern*> aGPs;
  parser->parse(patternsFile.c_str()); 
  xercesc::DOMDocument* doc = parser->getDocument();
  assert(doc);

  unsigned int nElem = doc->getElementsByTagName(_toDOMS("GP"))->getLength();
  if(nElem<1){
    edm::LogError("critical")<<"Problem parsing XML file "<<patternsFile<<std::endl;
    edm::LogError("critical")<<"No GoldenPattern items: GP found"<<std::endl;
    return aGPs;
  }

  DOMNode *aNode = 0;
  DOMElement* aGPElement = 0;
  for(unsigned int iItem=0;iItem<nElem;++iItem){
    aNode = doc->getElementsByTagName(_toDOMS("GP"))->item(iItem);
    aGPElement = static_cast<DOMElement *>(aNode); 
    aGPs.push_back(buildGP(aGPElement));    
  }
  delete doc;

  return aGPs;
}
//////////////////////////////////////////////////
//////////////////////////////////////////////////
GoldenPattern * XMLConfigReader::buildGP(DOMElement* aGPElement){

  unsigned int iPt = std::atoi(_toString(aGPElement->getAttribute(_toDOMS("iPt"))).c_str());
  int iEta = std::atoi(_toString(aGPElement->getAttribute(_toDOMS("iEta"))).c_str());
  int iCharge = std::atoi(_toString(aGPElement->getAttribute(_toDOMS("iCharge"))).c_str());
  int val = 0;
  unsigned int nLayers = aGPElement->getElementsByTagName(_toDOMS("Layer"))->getLength();
  assert(nLayers==OMTFConfiguration::nLayers);
  DOMNode *aNode = 0;
  DOMElement* aLayerElement = 0;
  DOMElement* aItemElement = 0;
  GoldenPattern::vector2D meanDistPhi2D(nLayers);
  GoldenPattern::vector1D pdf1D(exp2(OMTFConfiguration::nPdfAddrBits));
  GoldenPattern::vector3D pdf3D(OMTFConfiguration::nLayers);
  GoldenPattern::vector2D pdf2D(OMTFConfiguration::nRefLayers);
  ///Loop over layers
  for(unsigned int iLayer=0;iLayer<nLayers;++iLayer){
    aNode = aGPElement->getElementsByTagName(_toDOMS("Layer"))->item(iLayer);
    aLayerElement = static_cast<DOMElement *>(aNode); 
    ///MeanDistPhi vector
    unsigned int nItems = aLayerElement->getElementsByTagName(_toDOMS("RefLayer"))->getLength();
    assert(nItems==OMTFConfiguration::nRefLayers);
    GoldenPattern::vector1D meanDistPhi1D(nItems);
    for(unsigned int iItem=0;iItem<nItems;++iItem){
      aNode = aLayerElement->getElementsByTagName(_toDOMS("RefLayer"))->item(iItem);
      aItemElement = static_cast<DOMElement *>(aNode); 
      val = std::atoi(_toString(aItemElement->getAttribute(_toDOMS("meanDistPhi"))).c_str());
      meanDistPhi1D[iItem] = val;
    }
    meanDistPhi2D[iLayer] = meanDistPhi1D;
    ///PDF vector
    nItems = aLayerElement->getElementsByTagName(_toDOMS("PDF"))->getLength();
    assert(nItems==OMTFConfiguration::nRefLayers*exp2(OMTFConfiguration::nPdfAddrBits));
    for(unsigned int iRefLayer=0;iRefLayer<OMTFConfiguration::nRefLayers;++iRefLayer){
      pdf1D.assign(exp2(OMTFConfiguration::nPdfAddrBits),0);
      for(unsigned int iPdf=0;iPdf<exp2(OMTFConfiguration::nPdfAddrBits);++iPdf){
	aNode = aLayerElement->getElementsByTagName(_toDOMS("PDF"))->item(iRefLayer*exp2(OMTFConfiguration::nPdfAddrBits)+iPdf);
	aItemElement = static_cast<DOMElement *>(aNode); 
	val = std::atoi(_toString(aItemElement->getAttribute(_toDOMS("value"))).c_str());
	pdf1D[iPdf] = val;
      }
      pdf2D[iRefLayer] = pdf1D;
    }
    pdf3D[iLayer] = pdf2D;
  }

  Key aKey(iEta,iPt,iCharge);
  GoldenPattern *aGP = new GoldenPattern(aKey);
  aGP->setMeanDistPhi(meanDistPhi2D);
  aGP->setPdf(pdf3D);

  return aGP;
}
//////////////////////////////////////////////////
//////////////////////////////////////////////////
std::vector<std::vector<int> > XMLConfigReader::readEvent(unsigned int iEvent){

  parser->parse(eventsFile.c_str()); 
  xercesc::DOMDocument* doc = parser->getDocument();
  assert(doc);

  OMTFinput::vector1D input1D;
  OMTFinput::vector2D input2D(OMTFConfiguration::nLayers);

  unsigned int nElem = doc->getElementsByTagName(_toDOMS("OMTF_Events"))->getLength();
  assert(nElem==1);
 
  DOMNode *aNode = doc->getElementsByTagName(_toDOMS("OMTF_Events"))->item(0);
  DOMElement* aOMTFElement = static_cast<DOMElement *>(aNode); 
  DOMElement* aEventElement = 0;
  DOMElement* aBxElement = 0;
  DOMElement* aLayerElement = 0;
  DOMElement* aHitElement = 0;
  unsigned int aLogicLayer = OMTFConfiguration::nLayers+1;
  int val = 0;

  nElem = aOMTFElement->getElementsByTagName(_toDOMS("Event"))->getLength();
   if(nElem<iEvent){
    edm::LogError("critical")<<"Problem parsing XML file "<<eventsFile<<std::endl;
    edm::LogError("critical")<<"not enough events found: "<<nElem<<std::endl;
    assert(nElem>=iEvent);
  }
 
  aNode = aOMTFElement->getElementsByTagName(_toDOMS("Event"))->item(iEvent);
  aEventElement = static_cast<DOMElement *>(aNode); 
  
  unsigned int nBX = aEventElement->getElementsByTagName(_toDOMS("bx"))->getLength();
  assert(nBX>0);
  aNode = aEventElement->getElementsByTagName(_toDOMS("bx"))->item(0);
  aBxElement = static_cast<DOMElement *>(aNode); 
  
  unsigned int nLayersHit = aBxElement->getElementsByTagName(_toDOMS("Layer"))->getLength();    
  assert(nLayersHit<=OMTFConfiguration::nLayers);
  
  input1D.clear();    
  input2D.assign(OMTFConfiguration::nLayers,input1D);
  
  for(unsigned int iLayer=0;iLayer<nLayersHit;++iLayer){
    aNode = aBxElement->getElementsByTagName(_toDOMS("Layer"))->item(iLayer);
    aLayerElement = static_cast<DOMElement *>(aNode); 
    aLogicLayer = std::atoi(_toString(aLayerElement->getAttribute(_toDOMS("iLayer"))).c_str());
    nElem = aLayerElement->getElementsByTagName(_toDOMS("Hit"))->getLength();     
    input1D.clear();
    for(unsigned int iHit=0;iHit<nElem;++iHit){
      aNode = aLayerElement->getElementsByTagName(_toDOMS("Hit"))->item(iHit);
      aHitElement = static_cast<DOMElement *>(aNode); 
      val = std::atoi(_toString(aHitElement->getAttribute(_toDOMS("iPhi"))).c_str());
      input1D.push_back(val);
    }
    input2D[aLogicLayer] = input1D;
  }
  delete doc;
  return input2D;
}
//////////////////////////////////////////////////
//////////////////////////////////////////////////
void XMLConfigReader::readConfig(OMTFConfiguration *aConfig){
  
  parser->parse(configFile.c_str()); 
  xercesc::DOMDocument* doc = parser->getDocument();
  assert(doc);
  unsigned int nElem = doc->getElementsByTagName(_toDOMS("OMTF"))->getLength();
  if(nElem!=1){
    edm::LogError("critical")<<"Problem parsing XML file "<<configFile<<std::endl;
    assert(nElem==1);
  }
  DOMNode *aNode = doc->getElementsByTagName(_toDOMS("OMTF"))->item(0);
  DOMElement* aOMTFElement = static_cast<DOMElement *>(aNode);  

  ///Addresing bits numbers
  nElem = aOMTFElement->getElementsByTagName(_toDOMS("GlobalData"))->getLength();
  assert(nElem==1);
  aNode = aOMTFElement->getElementsByTagName(_toDOMS("GlobalData"))->item(0);
  DOMElement* aElement = static_cast<DOMElement *>(aNode); 

  float minPdfVal = std::atof(_toString(aElement->getAttribute(_toDOMS("minPdfVal"))).c_str()); 
  unsigned int nPdfAddrBits = std::atoi(_toString(aElement->getAttribute(_toDOMS("nPdfAddrBits"))).c_str()); 
  unsigned int nPdfValBits =  std::atoi(_toString(aElement->getAttribute(_toDOMS("nPdfValBits"))).c_str()); 
  unsigned int nHitsPerLayer =  std::atoi(_toString(aElement->getAttribute(_toDOMS("nHitsPerLayer"))).c_str()); 
  unsigned int nPhiBits =  std::atoi(_toString(aElement->getAttribute(_toDOMS("nPhiBits"))).c_str()); 
  unsigned int nPhiBins =  std::atoi(_toString(aElement->getAttribute(_toDOMS("nPhiBins"))).c_str()); 
  unsigned int nRefHits =  std::atoi(_toString(aElement->getAttribute(_toDOMS("nRefHits"))).c_str()); 
  unsigned int nTestRefHits =  std::atoi(_toString(aElement->getAttribute(_toDOMS("nTestRefHits"))).c_str()); 
  OMTFConfiguration::minPdfVal = minPdfVal;
  OMTFConfiguration::nPdfAddrBits = nPdfAddrBits;
  OMTFConfiguration::nPdfValBits = nPdfValBits;
  OMTFConfiguration::nHitsPerLayer = nHitsPerLayer;
  OMTFConfiguration::nPhiBits = nPhiBits;
  OMTFConfiguration::nPhiBins = nPhiBins;
  OMTFConfiguration::nRefHits = nRefHits;
  OMTFConfiguration::nTestRefHits = nTestRefHits;

  ///hw <-> logic numbering map
  unsigned int nLogicLayers = 0;
  nElem = aOMTFElement->getElementsByTagName(_toDOMS("LayerMap"))->getLength();
  DOMElement* aLayerElement = 0;
  for(uint i=0;i<nElem;++i){
    aNode = aOMTFElement->getElementsByTagName(_toDOMS("LayerMap"))->item(i);
    aLayerElement = static_cast<DOMElement *>(aNode); 
    unsigned int hwNumer = std::atoi(_toString(aLayerElement->getAttribute(_toDOMS("hwNumber"))).c_str());
    unsigned int logicNumber = std::atoi(_toString(aLayerElement->getAttribute(_toDOMS("logicNumber"))).c_str());
    unsigned int isBendingLayer = std::atoi(_toString(aLayerElement->getAttribute(_toDOMS("bendingLayer"))).c_str());
    unsigned int iConnectedLayer = std::atoi(_toString(aLayerElement->getAttribute(_toDOMS("connectedToLayer"))).c_str());
    aConfig->hwToLogicLayer[hwNumer] = logicNumber;
    aConfig->logicToHwLayer[logicNumber] = hwNumer;    
    aConfig->logicToLogic[logicNumber] = iConnectedLayer;    
    if(isBendingLayer)     aConfig->bendingLayers.insert(logicNumber);    
    if(nLogicLayers<logicNumber) nLogicLayers = logicNumber;
  }
  ++nLogicLayers;//logic number in XML starts from 0.
  OMTFConfiguration::nLayers = nLogicLayers;

  ///ref<->logic numberig map
  unsigned int nRefLayers = 0;
  nElem = aOMTFElement->getElementsByTagName(_toDOMS("RefLayerMap"))->getLength();
  aConfig->refToLogicNumber.resize(nElem);
  DOMElement* aRefLayerElement = 0;
  for(uint i=0;i<nElem;++i){
    aNode = aOMTFElement->getElementsByTagName(_toDOMS("RefLayerMap"))->item(i);
    aRefLayerElement = static_cast<DOMElement *>(aNode); 
    unsigned int refLayer = std::atoi(_toString(aRefLayerElement->getAttribute(_toDOMS("refLayer"))).c_str());
    unsigned int logicNumber = std::atoi(_toString(aRefLayerElement->getAttribute(_toDOMS("logicNumber"))).c_str());
    aConfig->refToLogicNumber[refLayer] = logicNumber;
    if(nRefLayers<logicNumber) nRefLayers = refLayer;
  }
  ++nRefLayers;//ref number in XML starts from 0.
  OMTFConfiguration::nRefLayers = nRefLayers;

  ///processors initial phi for each reference layer
  std::vector<int> vector1D(OMTFConfiguration::nRefLayers,OMTFConfiguration::nPhiBins);
  OMTFConfiguration::processorPhiVsRefLayer.assign(6,vector1D);

  ///connections tables for each processor each logic cone
  ///Vector of all layers 
  OMTFConfiguration::vector1D_A aLayer1D(OMTFConfiguration::nLayers);
  ///Vector of all logic cones
  OMTFConfiguration::vector2D_A aLayer2D;
  aLayer2D.assign(6,aLayer1D);
  ///Vector of all processors
  OMTFConfiguration::connections.assign(6,aLayer2D);

  ///Starting phis of each region
  ///Vector of all regions in one processor
  std::vector<std::pair<int,int> > aRefHit1D(6,std::pair<int,int>(9999,9999));
  ///Vector of all reflayers
  std::vector<std::vector<std::pair<int,int> > > aRefHit2D;
  aRefHit2D.assign(8,aRefHit1D);
  ///Vector of all processors
  OMTFConfiguration::regionPhisVsRefLayerVsProcessor.assign(6,aRefHit2D);

  //Vector of ref hit definitions
  std::vector<RefHitDef> aRefHitsDefs(OMTFConfiguration::nRefHits);
  ///Vector of all processros
  OMTFConfiguration::refHitsDefs.assign(6,aRefHitsDefs);

  nElem = aOMTFElement->getElementsByTagName(_toDOMS("Processor"))->getLength();
  assert(nElem==6);
  DOMElement* aProcessorElement = 0;
  for(uint i=0;i<nElem;++i){
    aNode = aOMTFElement->getElementsByTagName(_toDOMS("Processor"))->item(i);
    aProcessorElement = static_cast<DOMElement *>(aNode); 
    unsigned int iProcessor = std::atoi(_toString(aProcessorElement->getAttribute(_toDOMS("iProcessor"))).c_str());
    unsigned int nElem1 = aProcessorElement->getElementsByTagName(_toDOMS("RefLayer"))->getLength();
    assert(nElem1==OMTFConfiguration::nRefLayers);
    DOMElement* aRefLayerElement = 0;
    for(uint ii=0;ii<nElem1;++ii){
      aNode = aProcessorElement->getElementsByTagName(_toDOMS("RefLayer"))->item(ii);
      aRefLayerElement = static_cast<DOMElement *>(aNode); 
      unsigned int iRefLayer = std::atoi(_toString(aRefLayerElement->getAttribute(_toDOMS("iRefLayer"))).c_str());
      int iPhi = std::atoi(_toString(aRefLayerElement->getAttribute(_toDOMS("iGlobalPhiStart"))).c_str());
      OMTFConfiguration::processorPhiVsRefLayer[iProcessor][iRefLayer] = iPhi;
    }
    ///////////
    nElem1 = aProcessorElement->getElementsByTagName(_toDOMS("RefHit"))->getLength();
    assert(nElem1==OMTFConfiguration::nRefHits);
    DOMElement* aRefHitElement = 0;
    for(uint ii=0;ii<nElem1;++ii){
      aNode = aProcessorElement->getElementsByTagName(_toDOMS("RefHit"))->item(ii);
      aRefHitElement = static_cast<DOMElement *>(aNode); 
      unsigned int iRefHit = std::atoi(_toString(aRefHitElement->getAttribute(_toDOMS("iRefHit"))).c_str());
      int iPhiMin = std::atoi(_toString(aRefHitElement->getAttribute(_toDOMS("iPhiMin"))).c_str());
      int iPhiMax = std::atoi(_toString(aRefHitElement->getAttribute(_toDOMS("iPhiMax"))).c_str());
      unsigned int iInput = std::atoi(_toString(aRefHitElement->getAttribute(_toDOMS("iInput"))).c_str());
      unsigned int iRegion = std::atoi(_toString(aRefHitElement->getAttribute(_toDOMS("iRegion"))).c_str());
      unsigned int iRefLayer = std::atoi(_toString(aRefHitElement->getAttribute(_toDOMS("iRefLayer"))).c_str());
      OMTFConfiguration::regionPhisVsRefLayerVsProcessor[iProcessor][iRefLayer][iRegion] = std::pair<int,int>(iPhiMin,iPhiMax);
      OMTFConfiguration::refHitsDefs[iProcessor][iRefHit] = RefHitDef(iInput,iPhiMin,iPhiMax,iRegion,iRefLayer);
    }
    ///////////
    unsigned int nElem2 = aProcessorElement->getElementsByTagName(_toDOMS("LogicRegion"))->getLength();
    assert(nElem2==6); //FIXME: hardcoded
    DOMElement* aRegionElement = 0;
    for(uint ii=0;ii<nElem2;++ii){
      aNode = aProcessorElement->getElementsByTagName(_toDOMS("LogicRegion"))->item(ii);
      aRegionElement = static_cast<DOMElement *>(aNode); 
      unsigned int iRegion = std::atoi(_toString(aRegionElement->getAttribute(_toDOMS("iRegion"))).c_str());
      unsigned int nElem3 = aRegionElement->getElementsByTagName(_toDOMS("Layer"))->getLength();
      assert(nElem3==OMTFConfiguration::nLayers); 
      DOMElement* aLayerElement = 0;
      for(uint iii=0;iii<nElem3;++iii){
	aNode = aRegionElement->getElementsByTagName(_toDOMS("Layer"))->item(ii);
	aLayerElement = static_cast<DOMElement *>(aNode); 
	unsigned int iLayer = std::atoi(_toString(aLayerElement->getAttribute(_toDOMS("iLayer"))).c_str());
	unsigned int iFirstInput = std::atoi(_toString(aLayerElement->getAttribute(_toDOMS("iFirstInput"))).c_str());
	unsigned int nInputs = std::atoi(_toString(aLayerElement->getAttribute(_toDOMS("nInputs"))).c_str());
	OMTFConfiguration::connections[iProcessor][iRegion][iLayer] = std::pair<unsigned int, unsigned int>(iFirstInput,nInputs);
      }
    }   
  }
  delete doc;
}
//////////////////////////////////////////////////
//////////////////////////////////////////////////

