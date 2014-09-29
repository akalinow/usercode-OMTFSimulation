#include <cmath>
#include <iostream>

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
#include "FWCore/MessageLogger/interface/MessageLogger.h"

///////////////////////////////////////
///////////////////////////////////////
OMTFinputMaker::OMTFinputMaker(){ 

  myInput = new OMTFinput();

}
///////////////////////////////////////
///////////////////////////////////////
void OMTFinputMaker::initialize(const edm::EventSetup& es){

 myPhiConverter = new MtfCoordinateConverter(es);
  myPhiConverter->setReferencePhi(0);

}
///////////////////////////////////////
///////////////////////////////////////
OMTFinputMaker::~OMTFinputMaker(){ 

  if(myInput) delete myInput;
  if(myPhiConverter) delete myPhiConverter;

}
///////////////////////////////////////
///////////////////////////////////////
bool  OMTFinputMaker::acceptDigi(const DigiSpec & aDigi,
				 unsigned int iProcessor){
  
  int barrelChamberMin = iProcessor*2 + 1;
  int barrelChamberMax = (iProcessor*2 + 2 +1);

  int endcapChamberMin = iProcessor*6 + 1;
  int endcapChamberMax = (iProcessor*6 + 6 +1);

  ///Clean up digis. Remove unconnected detectors
  uint32_t rawId = aDigi.first;   
  DetId detId(rawId);
  if (detId.det() != DetId::Muon) 
    edm::LogError("Critical") << "PROBLEM: hit in unknown Det, detID: "<<detId.det()<<std::endl;
  switch (detId.subdetId()) {
  case MuonSubdetId::RPC: {
    RPCDetId aId(rawId);
    if(aId.region()==0 && barrelChamberMax==13 && aId.sector()==1) return true;
      if(aId.region()==0 && (aId.sector()<barrelChamberMin || aId.sector()>barrelChamberMax)) return false;    
      if(aId.region()!=0 && 
	 ((aId.sector()-1)*6+aId.subsector()<endcapChamberMin || 
	  (aId.sector()-1)*6+aId.subsector()>endcapChamberMax)) return false;  
      if(aId.region()<0 && barrelChamberMax==37 && (aId.sector()-1)*6+aId.subsector()==1) return true;  
      if(aId.region()<0 ||
	 (aId.region()==0 && aId.ring()<2) ||
	 (aId.region()==0 && aId.station()==4)
	 ) return false;
  }
    break;
  case MuonSubdetId::DT: {
    DTphDigiSpec digi(rawId, aDigi.second);
    DTChamberId dt(rawId);
    ///DT sector counts from 0. Other subsystems count from 1
    if(barrelChamberMax==13 && dt.sector()+1==1) return true;
    if(dt.sector()+1<barrelChamberMin || dt.sector()+1>barrelChamberMax) return false;
    ///Select DT digis with hits in inner and outer layers 
    if (digi.bxNum() != 0 || digi.bxCnt() != 0 || digi.ts2() != 0 ||  digi.code()<4) return false;	
    break;
  }
  case MuonSubdetId::CSC: {
    CSCDetId csc(rawId);
    if(endcapChamberMax==37 && csc.chamber()==1) return true;
    if(csc.chamber()<endcapChamberMin || csc.chamber()>endcapChamberMax) return false;
    //if(csc.station()==1 && csc.ring()==4) return false; //Skip ME1/a due to use of ganged strips, causing problems in phi calculation
    ///////////////////
    break;
  }
  }
  return true;
}
///////////////////////////////////////
///////////////////////////////////////
unsigned int OMTFinputMaker::getInputNumber(unsigned int rawId, 
					    unsigned int iProcessor){

  unsigned int iInput = 99;

  int barrelChamberMin = iProcessor*2 + 1;
  int endcapChamberMin = iProcessor*6 + 1;

  DetId detId(rawId);
  if (detId.det() != DetId::Muon) 
    edm::LogError("Critical") << "PROBLEM: hit in unknown Det, detID: "<<detId.det()<<std::endl;
  switch (detId.subdetId()) {
  case MuonSubdetId::RPC: {
    RPCDetId rpc(rawId);        
    if(rpc.region()==0) iInput = (rpc.sector()- barrelChamberMin)*2;
    if(rpc.region()!=0) iInput = ((rpc.sector()-1)*6+rpc.subsector()-endcapChamberMin)*2;
    if(iProcessor==5 && rpc.region()==0 && rpc.sector()==1) iInput = 4;
    if(iProcessor==5 && rpc.region()!=0 && (rpc.sector()-1)*6+rpc.subsector()==1) iInput = 12;
    //std::cout<<rpc<<" iInput: "<<iInput<<" iProcessor: "<<iProcessor<<std::endl;    
    break;
  }
  case MuonSubdetId::DT: {
    DTChamberId dt(rawId);
    iInput = (dt.sector()+1-barrelChamberMin)*2;
    if(iProcessor==5 && dt.sector()+1==1) iInput = 4;
    //std::cout<<dt<<" iInput: "<<iInput<<" iProcessor: "<<iProcessor<<std::endl;
    break;
  }
  case MuonSubdetId::CSC: {
    CSCDetId csc(rawId);
    iInput = (csc.chamber()-endcapChamberMin)*2;
    if(iProcessor==5 && csc.chamber()==1) iInput = 12;
    //std::cout<<csc<<" iInput: "<<iInput<<" iProcessor: "<<iProcessor<<std::endl;
    break;
  }
  }
  return iInput;
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
    unsigned int nGlobalPhi = OMTFConfiguration::nPhiBins;
    unsigned int iInput = 0;
    myInput->addLayerHit(iLayer,iInput,myPhiConverter->convert(digiIt,nGlobalPhi));

    DetId detId(rawId);
    if (detId.det() != DetId::Muon) 
      edm::LogError("Critical") << "PROBLEM: hit in unknown Det, detID: "<<detId.det()<<std::endl;
    switch (detId.subdetId()) {
      case MuonSubdetId::DT: {
        DTphDigiSpec digi(rawId,digiIt.second);
	myInput->addLayerHit(iLayer+1,iInput,digi.phiB());
        break;
      }
      case MuonSubdetId::CSC: {
        CSCDigiSpec digi(rawId,digiIt.second);
	myInput->addLayerHit(iLayer+1,iInput,digi.pattern());
        break;
      }
    };
  }
  return myInput;
}
////////////////////////////////////////////
////////////////////////////////////////////
const OMTFinput * OMTFinputMaker::buildInputForProcessor(const VDigiSpec & vDigi,
							 unsigned int iProcessor){

  myInput->clear();	
  
  ///Prepare inpout for individual processors.
  for (auto digiIt:vDigi) { 
    ///Check it the data fits into given processor input range
    if(!acceptDigi(digiIt, iProcessor)) continue;

    uint32_t rawId = digiIt.first;   
    unsigned int hwNumber = MtfCoordinateConverter::getLayerNumber(rawId);
    unsigned int iLayer = OMTFConfiguration::hwToLogicLayer[hwNumber];
    unsigned int nGlobalPhi = OMTFConfiguration::nPhiBins;
    int iPhi = myPhiConverter->convert(digiIt,nGlobalPhi);
    unsigned int iInput= getInputNumber(rawId, iProcessor);
    myInput->addLayerHit(iLayer,iInput,iPhi);
    DetId detId(rawId);
    if (detId.det() != DetId::Muon) 
      edm::LogError("Critical") << "PROBLEM: hit in unknown Det, detID: "<<detId.det()<<std::endl;
    switch (detId.subdetId()) {
    case MuonSubdetId::DT: {
      DTphDigiSpec digi(rawId,digiIt.second);
      myInput->addLayerHit(iLayer+1,iInput,digi.phiB());
      break;
    }
    case MuonSubdetId::CSC: {
        CSCDigiSpec digi(rawId,digiIt.second);
	myInput->addLayerHit(iLayer+1,iInput,digi.pattern());
        break;
      }
    };
  }
  return myInput;
}
////////////////////////////////////////////
////////////////////////////////////////////


