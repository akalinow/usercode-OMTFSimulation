#ifndef OMTFPatternMaker_H
#define OMTFPatternMaker_H

#include "xercesc/util/XercesDefs.hpp"

#include "DataFormats/L1TMuon/interface/L1TMuonTriggerPrimitive.h"
#include "DataFormats/L1TMuon/interface/L1TMuonTriggerPrimitiveFwd.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

class OMTFProcessor;
class OMTFConfiguration;
class OMTFConfigMaker;
class OMTFinputMaker;

class SimTrack;

class XMLConfigWriter;

namespace XERCES_CPP_NAMESPACE{
  class DOMElement;
  class DOMDocument;
  class DOMImplementation;
}

class OMTFPatternMaker : public edm::EDAnalyzer {
public:

  OMTFPatternMaker(const edm::ParameterSet & cfg);

  virtual ~OMTFPatternMaker();

  virtual void beginJob();

  virtual void endJob();
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);  

private:

  const SimTrack *findSimMuon(const edm::Event &ev, const edm::EventSetup &es, const SimTrack *previous=0);

  edm::ParameterSet theConfig;
  edm::InputTag trigPrimSrc, g4SimTrackSrc;
  edm::EDGetTokenT<L1TMuon::TriggerPrimitiveCollection> inputToken;

  const L1TMuon::TriggerPrimitiveCollection filterDigis(const L1TMuon::TriggerPrimitiveCollection & vDigi);

  bool makeConnectionsMaps, makeGoldenPatterns;

  ///OMTF objects
  OMTFConfiguration *myOMTFConfig;
  OMTFinputMaker *myInputMaker;
  OMTFProcessor *myOMTF;
  ///
  xercesc::DOMElement *aTopElement;
  OMTFConfigMaker *myOMTFConfigMaker;
  XMLConfigWriter *myWriter;

}; 

#endif
