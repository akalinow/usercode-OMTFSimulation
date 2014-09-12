#include "interface/L1Obj.h"

//ClassImp(L1Obj)

////////////////////////////////////////////////////
///////////////////////////////////////////////////
ostream & operator<< (ostream &out, const L1Obj &o)
{
  out<<"L1Obj: ";
  switch (o.type) {
    case L1Obj::RPCb:     { out <<"RPCb    "; break; }
    case L1Obj::RPCf:     { out <<"RPCf    "; break; }
    case L1Obj::DT:       { out <<"DT      "; break; }
    case L1Obj::CSC:      { out <<"CSC     "; break; }
    case L1Obj::GMT:      { out <<"GMT     "; break; }
    case L1Obj::RPCb_emu: { out <<"RPCb_emu"; break; }
    case L1Obj::RPCf_emu: { out <<"RPCf_emu"; break; }
    case L1Obj::GMT_emu:  { out <<"GMT_emu "; break; }
    case L1Obj::OTF:      { out <<"OTF     "; break; }
    case L1Obj::NONE   :  { out <<"NONE    "; break; }
    default: out <<"Unknown";
  };
  out <<" pt: "<<o.pt<<", eta: "<<o.eta<<", phi: "<<o.phi
      <<", q: "<<o.q<<", disc: "<<o.disc
      <<", bx: "<<o.bx;
  return out;
}
////////////////////////////////////////////////////
///////////////////////////////////////////////////
