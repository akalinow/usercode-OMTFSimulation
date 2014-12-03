#ifndef OMTF_XMLConfigWriter_H
#define OMTF_XMLConfigWriter_H

#include <string>
#include <vector>

#include "xercesc/util/XercesDefs.hpp"

#include "UserCode/OMTFSimulation/interface/OMTFConfiguration.h"

class GoldenPattern;
class OMTFConfiguration;
class OMTFinput;
class OMTFResult;
class InternalObj;
struct Key;

namespace XERCES_CPP_NAMESPACE{
  class DOMElement;
  class DOMDocument;
  class DOMImplementation;
}

class XMLConfigWriter{

 public:

  XMLConfigWriter();

  void initialiseXMLDocument(const std::string & docName);

  void finaliseXMLDocument(const std::string & fName);

  xercesc::DOMElement * writeEventHeader(unsigned int eventId);

  xercesc::DOMElement * writeEventData(xercesc::DOMElement *aTopElement,
				       unsigned int iProcessor,
				       const OMTFinput & aInput);

  void writeCandidateData(xercesc::DOMElement *aTopElement,
			  unsigned int iRegion,
			  const InternalObj & aCand);

  void writeResultsData(xercesc::DOMElement *aTopElement,
			unsigned int iRegion,
			const Key& aKey,
			const OMTFResult & aResult);

  void writeGPData(const GoldenPattern & aGP);

  void writeConnectionsData(const std::vector<std::vector <OMTFConfiguration::vector2D> > & measurements4D);

  unsigned int findMaxInput(const OMTFConfiguration::vector1D & myCounts);

 private:

  ///Utility function to set which layers should be ignored
  bool removeLayers(unsigned int iLayer);

  xercesc::DOMImplementation* domImpl;
  xercesc::DOMElement* theTopElement;
  xercesc::DOMDocument* theDoc;

};


//////////////////////////////////
//////////////////////////////////
#endif
