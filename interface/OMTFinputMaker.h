#ifndef OMTFinputMaker_H
#define OMTFinputMaker_H

#include <vector>
#include <stdint.h>
#include <memory>

#include "L1Trigger/L1TMuon/interface/GeometryTranslator.h"

#include "DataFormats/L1TMuon/interface/L1TMuonTriggerPrimitive.h"
#include "DataFormats/L1TMuon/interface/L1TMuonTriggerPrimitiveFwd.h"

class MtfCoordinateConverter;
class OMTFinput;

namespace edm{
  class EventSetup;
}

class OMTFinputMaker{

 public:

  OMTFinputMaker();

  ~OMTFinputMaker();

  void initialize(const edm::EventSetup& es);

  ///Method translating trigger digis into input matrix with global phi coordinates
  ///Flavour using a vector of trigger primitives. 
  const OMTFinput * buildInputForProcessor(const L1TMuon::TriggerPrimitiveCollection & vDigi, unsigned int iProcessor);

 private:

  ///Check if digis are within a give processor input.
  ///Simply checks sectors range. 
  bool acceptDigi(uint32_t rawId, unsigned int iProcessor);

  bool filterDigiQuality(const L1TMuon::TriggerPrimitive & aDigi) const;

  ///Give input number for givedn processor, using
  ///the chamber sector number. 
  ///Result is modulo allowed number of hits per chamber
  unsigned int getInputNumber(unsigned int rawId, 
			      unsigned int iProcessor);
  /*
  ///Helper function for sorting the RPC primitives by strip number
  bool rpcPrimitiveCmp(const L1TMuon::TriggerPrimitive *a,
		       const L1TMuon::TriggerPrimitive *b) { return a->getStrip()<b->getStrip(); };
  */
  OMTFinput *myInput;

   
  std::unique_ptr<L1TMuon::GeometryTranslator> geom;
  
};

#endif
