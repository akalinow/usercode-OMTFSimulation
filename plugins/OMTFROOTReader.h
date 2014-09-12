#ifndef OMTF_OMTFROOTReader_H
#define OMTF_OMTFROOTReader_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "TObjArray.h"

class OMTFProcessor;
class OMTFConfiguration;
class OMTFinputMaker;
class AnaEff;

namespace edm { 
  class Event; 
  class EventSetup; 
}

class OMTFROOTReader: public edm::EDAnalyzer {

public:

  OMTFROOTReader(const edm::ParameterSet & cfg);
  virtual ~OMTFROOTReader();
  virtual void beginJob();
  virtual void beginRun(const edm::Run&,  const edm::EventSetup& es){};
  virtual void analyze(const edm::Event&, const edm::EventSetup& es);
  virtual void endJob();

private:

  TObjArray myHistos;
  edm::ParameterSet theConfig;

  OMTFConfiguration *myOMTFConfig;
  OMTFinputMaker *myInputMaker;
  OMTFProcessor *myOMTF;
  AnaEff        *myAnaEff;


}; 

#endif
