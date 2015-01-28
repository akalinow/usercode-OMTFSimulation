#ifndef OMTF_OMTFProcessor_H
#define OMTF_OMTFProcessor_H

#include <map>

#include "UserCode/OMTFSimulation/interface/GoldenPattern.h"
#include "UserCode/OMTFSimulation/interface/OMTFResult.h"

class OMTFConfiguration;
class XMLConfigReader;
class OMTFinput;

class SimTrack;

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

  ///Process input data from a single event
  ///Input data is represented by hits in logic layers expressed in local coordinates
  ///Vector index: logic region number
  ///Map key: GoldenPattern key
  const std::vector<OMTFProcessor::resultsMap> & processInput(unsigned int iProcessor,
							      const OMTFinput & aInput);

  ///Return map of GoldenPatterns
  const std::map<Key,GoldenPattern*> & getPatterns() const;

  ///Shift phi values in input to fit the 11 bits
  ///range. For each processor the global phi beggining-511 
  ///is added, so it starts at -551
  OMTFinput shiftInput(unsigned int iProcessor,
		       const OMTFinput & aInput);

  ///Fill counts for a GoldenPattern of this
  ///processor unit. Pattern key is selcted according 
  ///to the SimTrack parameters.
  void fillCounts(unsigned int iProcessor,
		  const OMTFinput & aInput,
		  const SimTrack* aSimMuon);
  
 private:

  ///Add GoldenPattern to pattern map.
  ///If GP key already exists in map, a new entry is ignored
  bool addGP(GoldenPattern *aGP);

  ///Average patterns. Use same meanDistPhi for two
  ///patterns neighboring in pt code
  void averagePatterns();

  ///Fill map of used inputs.
  ///FIXME: using hack from OMTFConfiguration
  void fillInputRange(unsigned int iProcessor,
		      unsigned int iCone,
		      const OMTFinput & aInput);

  void fillInputRange(unsigned int iProcessor,
		      unsigned int iCone,
		      unsigned int iRefLayer,
		      unsigned int iHit);
    
  ///Remove hits whis are outside input range
  ///for given processor and cone
  OMTFinput::vector1D restrictInput(unsigned int iProcessor,
				    unsigned int iCone,
				    unsigned int iLayer,
				    const OMTFinput::vector1D & layerHits);

  ///Map holding Golden Patterns
  std::map<Key,GoldenPattern*> theGPs;

  ///Map holding results on current event data
  ///for each GP
  std::vector<OMTFProcessor::resultsMap> myResults;

};


#endif
