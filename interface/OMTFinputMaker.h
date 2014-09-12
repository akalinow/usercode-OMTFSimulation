#ifndef OMTFinputMaker_H
#define OMTFinputMaker_H

#include <vector>
#include <stdint.h>

class EventObj;
class MtfCoordinateConverter;
class OMTFinput;

namespace edm{
  class EventSetup;
}

class OMTFinputMaker{

  typedef std::pair<uint32_t, uint32_t> DigiSpec;
  typedef std::vector< DigiSpec > VDigiSpec;

  OMTFinputMaker(const edm::EventSetup& es);

  ~OMTFinputMaker();

  const OMTFinput * getEvent(const VDigiSpec & vDigi);

 private:

  bool acceptDigi(const DigiSpec & aDigi);

  MtfCoordinateConverter *myPhiConverter;
  OMTFinput *myInput;
  
};

#endif
