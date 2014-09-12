#ifndef OMTF_XMLConfigReader_H
#define OMTF_XMLConfigReader_H

#include <string>
#include <vector>

#include "xercesc/util/XercesDefs.hpp"

class GoldenPattern;
class OMTFConfiguration;

namespace XERCES_CPP_NAMESPACE{

class DOMElement;
class XercesDOMParser;

}

class XMLConfigReader{

 public:

  XMLConfigReader();

  void readConfig(const std::string fName);

  void setConfigFile(const std::string & fName) {configFile = fName;}

  void setPatternsFile(const std::string & fName) {patternsFile = fName;}

  void setEventsFile(const std::string & fName) {eventsFile = fName;}

  std::vector<GoldenPattern*> readPatterns();

  void readConfig(OMTFConfiguration *aConfig);

  //FIXME: output type should be taken from somewhere 
  std::vector<std::vector<int> > readEvent(unsigned int iEvent=0);

 private:

  std::string configFile; //XML file with general configuration
  std::string patternsFile; //XML file with GoldenPatterns
  std::string eventsFile;   //XML file with events

  GoldenPattern * buildGP(xercesc::DOMElement* aGPElement); 
  
  

  xercesc::XercesDOMParser *parser;

};


//////////////////////////////////
//////////////////////////////////
#endif
