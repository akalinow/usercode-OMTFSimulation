#include "UserCode/OMTFSimulation/interface/InternalObj.h"

std::ostream & operator<< (std::ostream &out, const InternalObj &o){
  out<<"InternalObj: ";
  out <<" pt: "<<o.pt<<", eta: "<<o.eta<<", phi: "<<o.phi
      <<", q: "<<o.q<<", bx: "<<o.bx<<", charge: "<<o.charge
      <<", disc: "<<o.disc<<" refLayer: "<<o.refLayer;
  
  return out;
}

