#include <iostream>
#include <cmath>

#include "interface/GoldenPattern.h"
#include "interface/OMTFConfiguration.h"
#include "interface/OMTFinput.h"

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
  return GoldenPattern::layerResult(pdfVal,pdfVal>0);
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
std::ostream & GoldenPattern::print(std::ostream &out) const{

 out <<"GoldenPattern "<< theKey <<std::endl;
 out <<"Number of reference layers: "<<meanDistPhi[0].size()
     <<", number of measurement layers: "<<pdfAllRef.size()
     <<std::endl;

 if(!meanDistPhi.size()) return out;
 if(!pdfAllRef.size()) return out;

 out<<"Mean dist phi per layer:"<<std::endl;
 for (unsigned int iRefLayer=0;iRefLayer<meanDistPhi[0].size();++iRefLayer){
   out<<"Ref layer: "<<iRefLayer<<" (";
   for (unsigned int iLayer=0;iLayer<meanDistPhi.size();++iLayer){   
     out<<meanDistPhi[iLayer][iRefLayer]<<"\t";
   }
   out<<")"<<std::endl;
 }

 out<<"PDF per layer:"<<std::endl;
 for (unsigned int iRefLayer=0;iRefLayer<pdfAllRef[0].size();++iRefLayer){
   out<<"Ref layer: "<<iRefLayer;
   for (unsigned int iLayer=0;iLayer<pdfAllRef.size();++iLayer){   
     out<<", measurement layer: "<<iLayer<<std::endl;
     for (unsigned int iPdf=0;iPdf<pdfAllRef[iLayer][iRefLayer].size();++iPdf){   
       out<<pdfAllRef[iLayer][iRefLayer][iPdf]<<" ";
     }
     out<<std::endl;
   }
 }
   
 return out;
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
