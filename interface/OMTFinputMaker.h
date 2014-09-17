#ifndef OMTFinputMaker_H
#define OMTFinputMaker_H

#include <vector>
#include <stdint.h>

//class EventObj;
class MtfCoordinateConverter;
class OMTFinput;

namespace edm{
  class EventSetup;
}

class OMTFinputMaker{

 public:

  typedef std::pair<uint32_t, uint32_t> DigiSpec;
  typedef std::vector< DigiSpec > VDigiSpec;

  OMTFinputMaker();

  ~OMTFinputMaker();

  void initialize(const edm::EventSetup& es);

  const OMTFinput * getEvent(const VDigiSpec & vDigi);

  const OMTFinput * buildInputForProcessor(const VDigiSpec & vDigi, unsigned int iProcessor);

 private:

  ///Check if digis are within a give processor input.
  ///Simply checks sectors range. 
  bool acceptDigi(const DigiSpec & aDigi,
		  unsigned int iProcessor=0);

  ///Give input number for givedn processor, using
  ///the chamber sector number. 
  ///Result is modulo allowed number of hits per chamber
  unsigned int getInputNumber(unsigned int rawId, 
			      unsigned int iProcessor);

  MtfCoordinateConverter *myPhiConverter;
  OMTFinput *myInput;
  
};

#endif
