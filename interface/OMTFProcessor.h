#ifndef OMTF_OMTFProcessor_H
#define OMTF_OMTFProcessor_H

#include <map>

#include "interface/GoldenPattern.h"
#include "interface/OMTFResult.h"

class XMLConfigReader;
class OMTFinput;

namespace edm{
class ParameterSet;
}

class OMTFProcessor{

 public:

  typedef std::map<Key,OMTFResult> resultsMap;

  OMTFProcessor(const edm::ParameterSet & cfg);

  ~OMTFProcessor();
  
  ///Fill GP map with patterns
  bool configure(XMLConfigReader *aReader);

  ///Process input datafrom asingle event
  ///Input data i reprecented by hits in logic layers
  OMTFProcessor::resultsMap processInput(OMTFinput & aInput);

  ///Return map of GoldenPatterns
  const std::map<Key,GoldenPattern*> & getPatterns() const;

 private:

  ///Add GoldenPattern to pattern map.
  ///If GP key already exists in map, a new entry is ignored
  bool addGP(GoldenPattern *aGP);

  ///Map holding Golden Patterns
  std::map<Key,GoldenPattern*> theGPs;

  ///Map holding results on current event data
  ///for each GP
  OMTFProcessor::resultsMap myResults;

};


#endif
