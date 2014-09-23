#ifndef OMTF_OMTFLogicConeBuilder_H
#define OMTF_OMTFLogicConeBuilder_H

#include "UserCode/OMTFSimulation/interface/OMTFinputMaker.h"

class MtfCoordinateConverter;
class OMTFinput;

class OMTFLogicConeBuilder{

 public: 

  OMTFLogicConeBuilder();

  ~OMTFLogicConeBuilder();

  void initialize(const edm::EventSetup& es);

  void buildInputForProcessor(const OMTFinputMaker::VDigiSpec & vDigi,
			      unsigned int iProcessor);

 private:

  MtfCoordinateConverter *myPhiConverter;

};
#endif
