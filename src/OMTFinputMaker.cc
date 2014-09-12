#include <cmath>

#include "UserCode/OMTFSimulation/interface/OMTFinputMaker.h"
#include "UserCode/OMTFSimulation/interface/OMTFinput.h"
#include "UserCode/OMTFSimulation/interface/OMTFConfiguration.h"
#include "UserCode/OMTFSimulation/interface/MtfCoordinateConverter.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDetIdUtil.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DTphDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/CSCDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDigiSpec.h"
#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"

#include "FWCore/Framework/interface/EventSetup.h"

///////////////////////////////////////
///////////////////////////////////////
OMTFinputMaker::OMTFinputMaker(const edm::EventSetup& es){ 

  myPhiConverter = new MtfCoordinateConverter(es);
  myPhiConverter->setReferencePhi(0);
  myInput = new OMTFinput();

}
///////////////////////////////////////
///////////////////////////////////////
OMTFinputMaker::~OMTFinputMaker(){ 

  delete myInput;
  delete myPhiConverter;

}
///////////////////////////////////////
///////////////////////////////////////
bool  OMTFinputMaker::acceptDigi(const DigiSpec & aDigi){

  ///Clean up digis. Remove unconnected detectors
  uint32_t rawId = aDigi.first;   
  DetId detId(rawId);
  if (detId.det() != DetId::Muon) 
    std::cout << "PROBLEM: hit in unknown Det, detID: "<<detId.det()<<std::endl;
  switch (detId.subdetId()) {
  case MuonSubdetId::RPC: {
    RPCDetId aId(rawId);    
    if(aId.region()<0 ||
       (aId.region()==0 && aId.ring()<2) ||
       (aId.region()==0 && aId.station()==4)
       ) return false;
  }
    break;
  case MuonSubdetId::DT: {
    DTphDigiSpec digi(rawId, aDigi.second);
    ///Select DT digis with hits in inner and outer layers 
    if (digi.bxNum() != 0 || digi.bxCnt() != 0 || digi.ts2() != 0 ||  digi.code()<4) return false;	
    break;
  }
  case MuonSubdetId::CSC: {
    CSCDetId csc(rawId);
    //if(csc.station()==1 && csc.ring()==4) return false; //Skip ME1/a due to use of ganged strips, causing problems in phi calculation
    ///////////////////
    break;
  }
  }
  return true;
}
///////////////////////////////////////
///////////////////////////////////////
const OMTFinput * OMTFinputMaker::getEvent(const VDigiSpec & vDigi){

  myInput->clear();

  for (auto digiIt:vDigi) {
    if(!acceptDigi(digiIt)) continue;
 
    uint32_t rawId = digiIt.first;
    
    unsigned int hwNumber = MtfCoordinateConverter::getLayerNumber(rawId);
    unsigned int iLayer = OMTFConfiguration::hwToLogicLayer[hwNumber];
    unsigned int nGlobalPhi = 4096;

    DetId detId(rawId);
    if (detId.det() != DetId::Muon) 
      std::cout << "PROBLEM: hit in unknown Det, detID: "<<detId.det()<<std::endl;
    switch (detId.subdetId()) {
    case MuonSubdetId::RPC: {
      RPCDigiSpec digi(rawId,digiIt.second);
      myInput->addLayerHit(iLayer,myPhiConverter->convert(digiIt,nGlobalPhi));
      break;
      }	
      case MuonSubdetId::DT: {
        DTphDigiSpec digi(rawId,digiIt.second);
	myInput->addLayerHit(iLayer,myPhiConverter->convert(digiIt,nGlobalPhi));
	myInput->addLayerHit(iLayer+1,digi.phiB());
        break;
      }
      case MuonSubdetId::CSC: {
        CSCDigiSpec digi(rawId,digiIt.second);
	myInput->addLayerHit(iLayer,myPhiConverter->convert(digiIt,nGlobalPhi));
	myInput->addLayerHit(iLayer+1,digi.pattern());
        break;
      }
      default: {std::cout <<" Unexpeced subdet case, id ="<<digiIt.first <<std::endl; return 0;}
    };
  }
  return myInput;
}
///////////////////////////////////////
///////////////////////////////////////
