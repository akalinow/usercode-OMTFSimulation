#include <cmath>
#include <vector>
#include <iostream>

#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"
#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"

#include "UserCode/OMTFSimulation/interface/OMTFinputMaker.h"
#include "UserCode/OMTFSimulation/interface/OMTFinput.h"
#include "UserCode/OMTFSimulation/interface/OMTFConfiguration.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

///////////////////////////////////////
///////////////////////////////////////
OMTFinputMaker::OMTFinputMaker(){ 

  myInput = new OMTFinput();

}
///////////////////////////////////////
///////////////////////////////////////
void OMTFinputMaker::initialize(const edm::EventSetup& es){ }
///////////////////////////////////////
///////////////////////////////////////
OMTFinputMaker::~OMTFinputMaker(){ 

  if(myInput) delete myInput;

}
///////////////////////////////////////
///////////////////////////////////////
bool  OMTFinputMaker::acceptDigi(uint32_t rawId,
				 unsigned int iProcessor){
  
  int barrelChamberMin = iProcessor*2 + 1;
  int barrelChamberMax = (iProcessor*2 + 2 +1);

  int endcapChamberMin = iProcessor*6 + 1;
  int endcapChamberMax = (iProcessor*6 + 6 +1);

  ///Clean up digis. Remove unconnected detectors
  DetId detId(rawId);
  if (detId.det() != DetId::Muon) 
    edm::LogError("Critical OMTFinputMaker") << "PROBLEM: hit in unknown Det, detID: "<<detId.det()<<std::endl;
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
    DTChamberId dt(rawId);
    ///DT sector counts from 0. Other subsystems count from 1
    if(barrelChamberMax==13 && dt.sector()+1==1) return true;
    if(dt.sector()+1<barrelChamberMin || dt.sector()+1>barrelChamberMax) return false;
   	
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
bool OMTFinputMaker::filterDigiQuality(const L1TMuon::TriggerPrimitive & aDigi) const{

  switch (aDigi.subsystem()) {
  case L1TMuon::TriggerPrimitive::kDT: {
    if (aDigi.getDTData().bx!= 0 || aDigi.getDTData().BxCntCode!= 0 || aDigi.getDTData().Ts2TagCode!= 0 || aDigi.getDTData().qualityCode<4) return false;  
    break;
  }
  case L1TMuon::TriggerPrimitive::kCSC: {}
  case L1TMuon::TriggerPrimitive::kRPC: {}
  case L1TMuon::TriggerPrimitive::kNSubsystems: {}
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
    edm::LogError("Critical OMTFinputMaker") << "PROBLEM: hit in unknown Det, detID: "<<detId.det()<<std::endl;
  switch (detId.subdetId()) {
  case MuonSubdetId::RPC: {
    RPCDetId rpc(rawId);        
    if(rpc.region()==0) iInput = (rpc.sector()- barrelChamberMin)*2;
    if(rpc.region()!=0) iInput = ((rpc.sector()-1)*6+rpc.subsector()-endcapChamberMin)*2;
    if(iProcessor==5 && rpc.region()==0 && rpc.sector()==1) iInput = 4;
    if(iProcessor==5 && rpc.region()!=0 && (rpc.sector()-1)*6+rpc.subsector()==1) iInput = 12;
    break;
  }
  case MuonSubdetId::DT: {
    DTChamberId dt(rawId);
    iInput = (dt.sector()+1-barrelChamberMin)*2;
    if(iProcessor==5 && dt.sector()+1==1) iInput = 4;
    break;
  }
  case MuonSubdetId::CSC: {
    CSCDetId csc(rawId);
    iInput = (csc.chamber()-endcapChamberMin)*2;
    if(iProcessor==5 && csc.chamber()==1) iInput = 12;
    break;
  }
  }
  return iInput;
}
////////////////////////////////////////////
///Helper function for sorting the RPC primitives by strip number
bool rpcPrimitiveCmp(const L1TMuon::TriggerPrimitive *a,
		     const L1TMuon::TriggerPrimitive *b) { return a->getStrip()<b->getStrip(); };
////////////////////////////////////////////
const OMTFinput * OMTFinputMaker::buildInputForProcessor(const L1TMuon::TriggerPrimitiveCollection & vDigi,
							 unsigned int iProcessor){
  myInput->clear();	

  std::map<unsigned int, std::vector<const L1TMuon::TriggerPrimitive *> > detMap;
  
  ///Prepare inpout for individual processors.
  for (const auto &digiIt:vDigi) { 
    ///Check it the data fits into given processor input range
    if(!acceptDigi(digiIt.rawId(), iProcessor)) continue;
    if(!filterDigiQuality(digiIt)) continue;

    unsigned int hwNumber = OMTFConfiguration::getLayerNumber(digiIt.rawId());
    unsigned int iLayer = OMTFConfiguration::hwToLogicLayer[hwNumber];
    unsigned int nGlobalPhi = OMTFConfiguration::nPhiBins;
    int iPhi =  digiIt.getCMSGlobalPhi()/(2.0*M_PI)*nGlobalPhi;
    unsigned int iInput= getInputNumber(digiIt.rawId(), iProcessor);
    if(digiIt.subsystem()!=L1TMuon::TriggerPrimitive::kRPC) myInput->addLayerHit(iLayer,iInput,iPhi);
    
    switch (digiIt.subsystem()) {
    case L1TMuon::TriggerPrimitive::kDT: {
      myInput->addLayerHit(iLayer+1,iInput,digiIt.getDTData().bendingAngle);
      break;
    }
    case L1TMuon::TriggerPrimitive::kCSC: {
      myInput->addLayerHit(iLayer+1,iInput,digiIt.getCSCData().pattern);
        break;
    }
    case L1TMuon::TriggerPrimitive::kRPC: {
      if(detMap.find(digiIt.rawId())==detMap.end()) detMap[digiIt.rawId()] = std::vector<const L1TMuon::TriggerPrimitive *>(0);
      detMap[digiIt.rawId()].push_back(&(digiIt));
      break;
    }
    case L1TMuon::TriggerPrimitive::kNSubsystems: {}
    };    
  }

  std::ostringstream myStr;
  ///Decluster hits in each RPC detId
  typedef std::tuple<unsigned int,const L1TMuon::TriggerPrimitive *, const L1TMuon::TriggerPrimitive *> halfDigi;
  std::vector<halfDigi> result;

  for(auto detIt:detMap) {   
    myStr<<"det: "<<detIt.first<<" size: "<<detIt.second.size()<<std::endl;
    std::sort(detIt.second.begin(), detIt.second.end(),rpcPrimitiveCmp);    
    for(auto stripIt: detIt.second) {
      myStr<<"strip: "<<stripIt->getStrip()<<std::endl;
      if (result.empty() || std::get<0>(result.back()) != detIt.first) result.push_back(halfDigi(detIt.first,stripIt,stripIt));
      else if (stripIt->getStrip() - std::get<2>(result.back())->getStrip() == 1) std::get<2>(result.back()) = stripIt;
      else if (stripIt->getStrip() - std::get<2>(result.back())->getStrip() > 1) result.push_back(halfDigi(detIt.first,stripIt,stripIt));
    }
  }
  for(auto halfDigiIt:result) myStr<<"halfDigi: "<<std::get<1>(halfDigiIt)->getStrip()<<" "<<std::get<2>(halfDigiIt)->getStrip()<<std::endl;
  edm::LogInfo("OMTFInputMaker")<<myStr.str();
  
  return myInput;
}
////////////////////////////////////////////
////////////////////////////////////////////
void OMTFinputMaker::declusterRPC(const L1TMuon::TriggerPrimitiveCollection & vDigi){

  std::map<unsigned int, std::vector<int> > detMap;

  ///Sort RPC digis by a detId
  for (auto digiIt:vDigi) {
    if(digiIt.subsystem()==L1TMuon::TriggerPrimitive::kRPC) {
      if(detMap.find(digiIt.rawId())==detMap.end()) detMap[digiIt.rawId()] = std::vector<int>(0); 
	 detMap[digiIt.rawId()].push_back(digiIt.getStrip());
	 }
    }


  std::ostringstream myStr;


    typedef std::tuple<unsigned int,int,int> halfDigi;
    std::vector<halfDigi> result;
    ///Decluster hits in each detId
    for(auto &detIt:detMap) {
      myStr<<"det: "<<detIt.first<<std::endl;
      std::sort(detIt.second.begin(), detIt.second.end());
      for(const auto &stripIt:detIt.second) {
	myStr<<"strip: "<<stripIt<<std::endl;
	if (result.empty() || std::get<0>(result.back()) != detIt.first) result.push_back(halfDigi(detIt.first,stripIt,stripIt));
	else if (stripIt - std::get<2>(result.back()) == 1) std::get<2>(result.back()) = stripIt;
	else if (stripIt - std::get<2>(result.back()) > 1) result.push_back(halfDigi(detIt.first,stripIt,stripIt));
      }
    } 
      for(auto halfDigiIt:result) myStr<<"halfDigi: "<<std::get<1>(halfDigiIt)<<" "<<std::get<2>(halfDigiIt)<<std::endl;
    

    edm::LogInfo("OMTFInputMaker")<<myStr.str();
}
////////////////////////////////////////////
////////////////////////////////////////////
