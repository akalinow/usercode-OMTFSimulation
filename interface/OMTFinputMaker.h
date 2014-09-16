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

  bool acceptDigi(const DigiSpec & aDigi,
		  unsigned int iProcessor=0);

  MtfCoordinateConverter *myPhiConverter;
  OMTFinput *myInput;
  
};

#endif
