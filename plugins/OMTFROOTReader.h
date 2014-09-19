#ifndef OMTF_OMTFROOTReader_H
#define OMTF_OMTFROOTReader_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "UserCode/OMTFSimulation/interface/OMTFConfiguration.h"

#include "TObjArray.h"

class OMTFProcessor;
class OMTFConfiguration;
class OMTFinputMaker;
class OMTFSorter;

class AnaEff;
class AnaSiMuDistribution;
class XMLConfigWriter;

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

  void analyseConnections();
  unsigned int findMaxInput(const OMTFConfiguration::vector1D & myCounts);

  TObjArray myHistos;
  edm::ParameterSet theConfig;

  ////
  XMLConfigWriter *myWriter; 

  ///OMTF objects
  OMTFConfiguration *myOMTFConfig;
  OMTFinputMaker *myInputMaker;
  OMTFSorter *mySorter;
  OMTFProcessor *myOMTF;

  ///Analysis objects
  AnaEff        *myAnaEff;
  AnaSiMuDistribution *myAnaSiMu;


}; 

#endif
