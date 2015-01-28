#include <iostream>
#include <cmath>

#include "FWCore/MessageLogger/interface/MessageLogger.h"

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
    if(itHit>=(int)OMTFConfiguration::nPhiBins) continue;
    if(abs(itHit-phiMean-phiRefHit)<phiDist) phiDist = itHit-phiMean-phiRefHit;
  }

  ///Shift phidist, so 0 is at the middle of the range
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
void GoldenPattern::addCount(unsigned int iRefLayer,
			     unsigned int iLayer,
			     const int phiRefHit,
			     const OMTFinput::vector1D & layerHits){

  int nHitsInLayer = 0;
  int phiDist = exp2(OMTFConfiguration::nPdfAddrBits);

  for(auto itHit: layerHits){
    if(itHit>=(int)OMTFConfiguration::nPhiBins) continue;
    if(abs(itHit-phiRefHit)<phiDist) phiDist = itHit-phiRefHit;
    ++nHitsInLayer;
  }
  ///For making the patterns take events with a single hit in each layer
  if(nHitsInLayer>1 || nHitsInLayer==0) return;
  
  ///Shift phidist, so 0 is at the middle of the range
  int phiDistShift=phiDist+exp2(OMTFConfiguration::nPdfAddrBits-1);
  
  ///Check if phiDist is within pdf range
  ///in -64 +63 U2 code
  ///Find more elegant way to check this.
  if(phiDistShift<0 ||
     phiDistShift>exp2(OMTFConfiguration::nPdfAddrBits)-1){
    return;
  }

  ///Shift phiDist so it is in +-Pi range
  if(phiDist>=(int)OMTFConfiguration::nPhiBins/2) phiDist-=(int)OMTFConfiguration::nPhiBins;
  if(phiDist<=-(int)OMTFConfiguration::nPhiBins/2) phiDist+=(int)OMTFConfiguration::nPhiBins;

  meanDistPhi[iLayer][iRefLayer]+=phiDist;
  ++meanDistPhiCounts[iLayer][iRefLayer];
  ++pdfAllRef[iLayer][iRefLayer][phiDistShift];
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

  if(aPattern.meanDistPhiCounts.size()){
    out<<"Counts number per layer:"<<std::endl;
    for (unsigned int iRefLayer=0;iRefLayer<aPattern.meanDistPhi[0].size();++iRefLayer){
      out<<"Ref layer: "<<iRefLayer<<" (";
      for (unsigned int iLayer=0;iLayer<aPattern.meanDistPhi.size();++iLayer){   
	out<<aPattern.meanDistPhiCounts[iLayer][iRefLayer]<<"\t";
      }
      out<<")"<<std::endl;
    }
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
void GoldenPattern::reset(){

  GoldenPattern::vector1D meanDistPhi1D(OMTFConfiguration::nRefLayers);
  GoldenPattern::vector2D meanDistPhi2D(OMTFConfiguration::nLayers);
  meanDistPhi2D.assign(OMTFConfiguration::nLayers, meanDistPhi1D);
  meanDistPhi = meanDistPhi2D;
  meanDistPhiCounts = meanDistPhi2D;

  ///For making the patterns use extended pdf width in phi
  //OMTFConfiguration::nPdfAddrBits = 12;

  GoldenPattern::vector1D pdf1D(exp2(OMTFConfiguration::nPdfAddrBits));
  GoldenPattern::vector2D pdf2D(OMTFConfiguration::nRefLayers);
  GoldenPattern::vector3D pdf3D(OMTFConfiguration::nLayers);

  pdf2D.assign(OMTFConfiguration::nRefLayers,pdf1D);
  pdfAllRef.assign(OMTFConfiguration::nLayers,pdf2D);

}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
void GoldenPattern::normalise(){

  ///Mean dist phi
  for (unsigned int iRefLayer=0;iRefLayer<meanDistPhi[0].size();++iRefLayer){
    for (unsigned int iLayer=0;iLayer<meanDistPhi.size();++iLayer){   
      if(meanDistPhiCounts.size() && meanDistPhiCounts[iLayer][iRefLayer]) 
	meanDistPhi[iLayer][iRefLayer]/=meanDistPhiCounts[iLayer][iRefLayer];
    }
  }
  
  const float minPlog =  log(OMTFConfiguration::minPdfVal);
  const float nBitsVal = OMTFConfiguration::nPdfValBits;
  
  ///Probabilities. Normalise and change from float to integer values
  float pVal;
  int digitisedVal, truncatedValue;
  for (unsigned int iRefLayer=0;iRefLayer<pdfAllRef[0].size();++iRefLayer){
    for (unsigned int iLayer=0;iLayer<pdfAllRef.size();++iLayer){   
      for (unsigned int iPdf=0;iPdf<pdfAllRef[iLayer][iRefLayer].size();++iPdf){   
	if(!meanDistPhiCounts[OMTFConfiguration::refToLogicNumber[iRefLayer]][iRefLayer] ||
	   !pdfAllRef[iLayer][iRefLayer][iPdf]){
	  continue;
	}
	pVal = log((float)pdfAllRef[iLayer][iRefLayer][iPdf]/meanDistPhiCounts[OMTFConfiguration::refToLogicNumber[iRefLayer]][iRefLayer]);
	if(pVal<minPlog) continue;
	///Digitisation
	///Values remapped 0->std::pow(2,nBitsVal)
	///          minPlog->0 
	digitisedVal = (std::pow(2,nBitsVal)-1) - (pVal/minPlog)*(std::pow(2,nBitsVal)-1);
	///Make sure digitised value is saved using nBitsVal bits
	truncatedValue  = 0 | (digitisedVal  & ((int)pow(2,nBitsVal)-1));
	pdfAllRef[iLayer][iRefLayer][iPdf] = truncatedValue;
      }
    }
  } 
     
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
bool GoldenPattern::hasCounts(){

 for (unsigned int iRefLayer=0;iRefLayer<meanDistPhi[0].size();++iRefLayer){
    for (unsigned int iLayer=0;iLayer<meanDistPhi.size();++iLayer){   
      if(meanDistPhiCounts.size() && meanDistPhiCounts[iLayer][iRefLayer]) return true;
  }
 }
 return false;
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
