#include "UserCode/OMTFSimulation/interface/InternalObj.h"

#include <bitset>

std::ostream & operator<< (std::ostream &out, const InternalObj &o){
  out<<"InternalObj: ";
  out <<" pt: "<<o.pt<<", eta: "<<o.eta<<", phi: "<<o.phi
      <<", q: "<<o.q
      <<" hits: "<<std::bitset<18>(o.hits).to_string()
      <<", bx: "<<o.bx
      <<", disc: "<<o.disc<<" refLayer: "<<o.refLayer;
  
  return out;
}

