#include <iostream>
#include <cmath>

#include "UserCode/OMTFSimulation/interface/GoldenPattern.h"
#include "UserCode/OMTFSimulation/interface/OMTFConfiguration.h"
#include "UserCode/OMTFSimulation/interface/OMTFinput.h"

////////////////////////////////////////////////////
////////////////////////////////////////////////////
GoldenPattern::layerResult GoldenPattern::process1Layer1RefLayer(unsigned int iRefLayer,
								 unsigned int iLayer,
								 const int phiRefHit,
								 const OMTFinput::vector1D & layerHits){

  GoldenPattern::layerResult aResult;

  int phiMean = meanDistPhi[iLayer][iRefLayer];
  int phiDist = exp2(OMTFConfiguration::nPdfAddrBits);
  ///Select hit closest to the mean of probability 
  ///distribution in given layer
  for(auto itHit: layerHits){
    //if(itHit>=(int)OMTFConfiguration::nPhiBins) continue;
    if(abs(itHit-phiMean-phiRefHit)<phiDist) phiDist = itHit-phiMean-phiRefHit;
  }
  ///Shift phidist, so 0 it at the middle of the range
  phiDist+=exp2(OMTFConfiguration::nPdfAddrBits-1);

  ///Check if phiDist is within pdf range
  ///in -64 +63 U2 code
  ///Find more elegant way to check this.
  if(phiDist<0 ||
     phiDist>exp2(OMTFConfiguration::nPdfAddrBits)-1){
    return aResult;
  }

  int pdfVal = pdfAllRef[iLayer][iRefLayer][phiDist];
  if(iRefLayer>14) pdfVal=0;//Skip RPC layers
  return GoldenPattern::layerResult(pdfVal,pdfVal>0);
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
std::ostream & operator << (std::ostream &out, const GoldenPattern & aPattern){

  out <<"GoldenPattern "<< aPattern.theKey <<std::endl;
  out <<"Number of reference layers: "<<aPattern.meanDistPhi[0].size()
      <<", number of measurement layers: "<<aPattern.pdfAllRef.size()
      <<std::endl;

  if(!aPattern.meanDistPhi.size()) return out;
  if(!aPattern.pdfAllRef.size()) return out;

  out<<"Mean dist phi per layer:"<<std::endl;
  for (unsigned int iRefLayer=0;iRefLayer<aPattern.meanDistPhi[0].size();++iRefLayer){
    out<<"Ref layer: "<<iRefLayer<<" (";
    for (unsigned int iLayer=0;iLayer<aPattern.meanDistPhi.size();++iLayer){   
      out<<aPattern.meanDistPhi[iLayer][iRefLayer]<<"\t";
    }
    out<<")"<<std::endl;
  }

  out<<"PDF per layer:"<<std::endl;
  for (unsigned int iRefLayer=0;iRefLayer<aPattern.pdfAllRef[0].size();++iRefLayer){
    out<<"Ref layer: "<<iRefLayer;
    for (unsigned int iLayer=0;iLayer<aPattern.pdfAllRef.size();++iLayer){   
      out<<", measurement layer: "<<iLayer<<std::endl;
      for (unsigned int iPdf=0;iPdf<aPattern.pdfAllRef[iLayer][iRefLayer].size();++iPdf){   
	out<<aPattern.pdfAllRef[iLayer][iRefLayer][iPdf]<<" ";
      }
      out<<std::endl;
    }
  }
  
  return out;
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
