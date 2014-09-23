#include "UserCode/OMTFSimulation/interface/OMTFLogicConeBuilder.h"
#include "UserCode/OMTFSimulation/interface/OMTFConfiguration.h"
#include "UserCode/OMTFSimulation/interface/OMTFinputMaker.h"
#include "UserCode/OMTFSimulation/interface/OMTFinput.h"
#include "UserCode/OMTFSimulation/interface/MtfCoordinateConverter.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/DTphDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/CSCDigiSpec.h"

#include <iostream>
#include <bitset>


///////////////////////////////////////
///////////////////////////////////////
OMTFLogicConeBuilder::OMTFLogicConeBuilder(){ 

  //myInput = new OMTFinput();

}
///////////////////////////////////////
///////////////////////////////////////
void OMTFLogicConeBuilder::initialize(const edm::EventSetup& es){

  myPhiConverter = new MtfCoordinateConverter(es);
  myPhiConverter->setReferencePhi(0);

}
///////////////////////////////////////
///////////////////////////////////////
OMTFLogicConeBuilder::~OMTFLogicConeBuilder(){ 

  //if(myInput) delete myInput;
  if(myPhiConverter) delete myPhiConverter;

}  
////////////////////////////////////////////
////////////////////////////////////////////
void OMTFLogicConeBuilder::buildInputForProcessor(const OMTFinputMaker::VDigiSpec & vDigi,
						  unsigned int iProcessor){
								      

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
    uint32_t rawId = digiIt.first;   
    unsigned int hwNumber = MtfCoordinateConverter::getLayerNumber(rawId);
    unsigned int iLayer = OMTFConfiguration::hwToLogicLayer[hwNumber];
    unsigned int nGlobalPhi = OMTFConfiguration::nPhiBins;
    int iPhi = myPhiConverter->convert(digiIt,nGlobalPhi);
    if(iPhi<iMinInputPhi || iPhi>iMaxInputPhi) continue;
    unsigned int iInput = 0;
    myInput->addLayerHit(iLayer,iInput,iPhi);

    DetId detId(rawId);
    if (detId.det() != DetId::Muon) 
      std::cout << "PROBLEM: hit in unknown Det, detID: "<<detId.det()<<std::endl;
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
      default: {std::cout <<" Unexpeced subdet case, id ="<<digiIt.first <<std::endl;}
    };
  }
}
////////////////////////////////////////////
////////////////////////////////////////////
