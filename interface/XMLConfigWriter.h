#ifndef OMTF_XMLConfigWriter_H
#define OMTF_XMLConfigWriter_H

#include <string>
#include <vector>

#include "xercesc/util/XercesDefs.hpp"

#include "interface/OMTFConfiguration.h"

class GoldenPattern;
class OMTFConfiguration;
class OMTFinput;
class OMTFResult;
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

  void writeEventData(xercesc::DOMElement *aTopElement,
		      const OMTFinput & aInput);

  void writeResultsData(xercesc::DOMElement *aTopElement,
			const Key& aKey,
			const OMTFResult & aResult);

  void writeGPData(const GoldenPattern & aGP);

  void writeConnectionsData(const std::vector<std::vector <OMTFConfiguration::vector2D> > & measurements4D);

  unsigned int findMaxInput(const OMTFConfiguration::vector1D & myCounts);

 private:

  xercesc::DOMImplementation* domImpl;
  xercesc::DOMElement* theTopElement;
  xercesc::DOMDocument* theDoc;

};


//////////////////////////////////
//////////////////////////////////
#endif
