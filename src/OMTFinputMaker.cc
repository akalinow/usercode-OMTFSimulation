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
  
  int barrelChamberMin = iProcessor;
  int barrelChamberMax = iProcessor*2 + 2;
  
  int endcapChamberMin = iProcessor;
  int endcapChamberMax = iProcessor*7 + 7;
  

  ///Clean up digis. Remove unconnected detectors
  uint32_t rawId = aDigi.first;   
  DetId detId(rawId);
  if (detId.det() != DetId::Muon) 
    std::cout << "PROBLEM: hit in unknown Det, detID: "<<detId.det()<<std::endl;
  switch (detId.subdetId()) {
  case MuonSubdetId::RPC: {
    RPCDetId aId(rawId);    
    
    if(aId.region()==0 && (aId.sector()<barrelChamberMin || aId.sector()>barrelChamberMax)) return false;    
    if(aId.region()!=0 && 
       ((aId.sector()-1)*6+aId.subsector()<endcapChamberMin || 
	(aId.sector()-1)*6+aId.subsector()>endcapChamberMax)) return false;
    
    if(aId.region()<0 ||
       (aId.region()==0 && aId.ring()<2) ||
       (aId.region()==0 && aId.station()==4)
       ) return false;
  }
    break;
  case MuonSubdetId::DT: {
    DTphDigiSpec digi(rawId, aDigi.second);
    DTChamberId dt(rawId);
    if(dt.sector()<barrelChamberMin || dt.sector()>barrelChamberMax) return false;
    ///Select DT digis with hits in inner and outer layers 
    if (digi.bxNum() != 0 || digi.bxCnt() != 0 || digi.ts2() != 0 ||  digi.code()<4) return false;	
    break;
  }
  case MuonSubdetId::CSC: {
    CSCDetId csc(rawId);
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
const OMTFinput * OMTFinputMaker::getEvent(const VDigiSpec & vDigi){

  myInput->clear();

  for (auto digiIt:vDigi) {
    if(!acceptDigi(digiIt)) continue;
 
    uint32_t rawId = digiIt.first;
    
    unsigned int hwNumber = MtfCoordinateConverter::getLayerNumber(rawId);
    unsigned int iLayer = OMTFConfiguration::hwToLogicLayer[hwNumber];
    unsigned int nGlobalPhi = OMTFConfiguration::nPhiBins;

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
////////////////////////////////////////////
////////////////////////////////////////////
const OMTFinput * OMTFinputMaker::buildInputForProcessor(const VDigiSpec & vDigi,
							 unsigned int iProcessor){

  /*
  for (auto it:vDigi){
    DetId detId(it.first);
    switch (detId.subdetId()) {
    case MuonSubdetId::RPC: { std::cout << std::endl <<RPCDetId(it.first)<<" "<<RPCDigiSpec(it.first, it.second);  break; }
    case MuonSubdetId::DT:  { std::cout << std::endl <<DTChamberId(it.first)<<" "<<DTphDigiSpec(it.first, it.second); break; }
    case MuonSubdetId::CSC: { std::cout << std::endl <<CSCDetId(it.first)<<" "<<CSCDigiSpec(it.first, it.second);  break; }
    };
    std::cout<<std::endl;
  }
  */

  myInput->clear();								      
  ///Single OMTFProcessor covers 60 deg + 10 overlap
  ///with 4096 bins for 2Pi this is 682 bins + 113 overlap
  unsigned int nBinsPer2Pi = OMTFConfiguration::nPhiBins;
  unsigned int nBinsPerProcessor = 682; //=60 deg
  unsigned int nBinsPerOverlap = 113/2; //=5 deg, half of overlap on each side
  int iPhi = (iProcessor+0.5)*nBinsPerProcessor;
  ///Phi range for reference hits
  //int iMinRefPhi = iProcessor*nBinsPerProcessor;
  //int iMaxRefPhi = (iProcessor+1)*nBinsPerProcessor;
  ///Phi range for input
  int iMinInputPhi = iProcessor*nBinsPerProcessor-nBinsPerOverlap;
  int iMaxInputPhi = (iProcessor+1)*nBinsPerProcessor+nBinsPerOverlap;
  if(iMinInputPhi<0) iMinInputPhi+=nBinsPer2Pi;
  if(iMinInputPhi>=(int)nBinsPer2Pi) iMinInputPhi-=nBinsPer2Pi;
  ///
  if(iPhi>nBinsPer2Pi/2.0) iPhi-=nBinsPer2Pi;
  if(iMinInputPhi>nBinsPer2Pi/2.0) iMinInputPhi-=nBinsPer2Pi;
  if(iMaxInputPhi>nBinsPer2Pi/2.0) iMaxInputPhi-=nBinsPer2Pi;
  ///
  
  OMTFinput *myInput = new OMTFinput();

  ///Prepare inpout for individual processors.
  for (auto digiIt:vDigi) { 
    if(!acceptDigi(digiIt, iProcessor)) continue;

    uint32_t rawId = digiIt.first;   
    unsigned int hwNumber = MtfCoordinateConverter::getLayerNumber(rawId);
    unsigned int iLayer = OMTFConfiguration::hwToLogicLayer[hwNumber];
    unsigned int nGlobalPhi = OMTFConfiguration::nPhiBins;
    int iPhi = myPhiConverter->convert(digiIt,nGlobalPhi);
    if(iPhi<iMinInputPhi || iPhi>iMaxInputPhi) continue;

    myInput->addLayerHit(iLayer,iPhi);

    DetId detId(rawId);
    if (detId.det() != DetId::Muon) 
      std::cout << "PROBLEM: hit in unknown Det, detID: "<<detId.det()<<std::endl;
    switch (detId.subdetId()) {
      case MuonSubdetId::DT: {
        DTphDigiSpec digi(rawId,digiIt.second);
	myInput->addLayerHit(iLayer+1,digi.phiB());
        break;
      }
      case MuonSubdetId::CSC: {
        CSCDigiSpec digi(rawId,digiIt.second);
	myInput->addLayerHit(iLayer+1,digi.pattern());
        break;
      }
    };
  }
  return myInput;
}
////////////////////////////////////////////
////////////////////////////////////////////
