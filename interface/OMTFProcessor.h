#ifndef OMTF_OMTFProcessor_H
#define OMTF_OMTFProcessor_H

#include <map>

#include "UserCode/OMTFSimulation/interface/GoldenPattern.h"
#include "UserCode/OMTFSimulation/interface/OMTFResult.h"

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
  OMTFProcessor::resultsMap processInput(unsigned int iProcessor,
					 const OMTFinput & aInput);

  ///Return map of GoldenPatterns
  const std::map<Key,GoldenPattern*> & getPatterns() const;

 private:

  ///Add GoldenPattern to pattern map.
  ///If GP key already exists in map, a new entry is ignored
  bool addGP(GoldenPattern *aGP);

  ///Find number of logic cone within a given processor.
  ///Number is calculated assuming 10 deg wide logic cones
  unsigned int getConeNumber(unsigned int iProcessor,
			     unsigned int iRefLayer,
			     int iPhi);

  ///Check if given referecne hit is
  ///in phi range for some logic cone.
  ///Care is needed arounf +Pi and +2Pi points
  bool isInConeRange(int iPhiStart,
		     unsigned int coneSize,
		     int iPhi);

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
  OMTFProcessor::resultsMap myResults;



};


#endif
