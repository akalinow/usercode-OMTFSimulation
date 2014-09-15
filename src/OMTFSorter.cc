#include <cassert>
#include <iostream>

#include "interface/OMTFSorter.h"

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
std::tuple<unsigned int,unsigned int, int> OMTFSorter::sortSingleResult(const OMTFResult & aResult){

  OMTFResult::vector1D pdfValsVec = aResult.getSummaryVals();
  OMTFResult::vector1D nHitsVec = aResult.getSummaryHits();
  OMTFResult::vector1D refPhiVec = aResult.getRefPhis();

  assert(pdfValsVec.size()==nHitsVec.size());

  unsigned int nHitsMax = 0;
  unsigned int pdfValMax = 0;
  int refPhi = 1024;

  std::tuple<unsigned int,unsigned int, int>  sortedResult;
  std::get<0>(sortedResult) = nHitsMax;
  std::get<1>(sortedResult) = pdfValMax;
  std::get<2>(sortedResult) = refPhi;

  ///Find a result with biggest number of hits
  for(auto itHits: nHitsVec){
    if(itHits>nHitsMax) nHitsMax = itHits;
  }

  if(!nHitsMax) return sortedResult;

  for(unsigned int ipdfVal=0;ipdfVal<pdfValsVec.size();++ipdfVal){
    if(nHitsVec[ipdfVal] == nHitsMax){
      if(pdfValsVec[ipdfVal]>pdfValMax){
	pdfValMax = pdfValsVec[ipdfVal]; 
	refPhi = refPhiVec[ipdfVal]; 
      }
    }
  }

  std::get<0>(sortedResult) = nHitsMax;
  std::get<1>(sortedResult) = pdfValMax;
  std::get<2>(sortedResult) = refPhi;
  return sortedResult;
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
L1Obj OMTFSorter::sortResults(const OMTFProcessor::resultsMap & aResultsMap){

  unsigned int pdfValMax = 0;
  unsigned int nHitsMax = 0;
  int refPhi = 9999;
  Key bestKey;
  for(auto itKey: aResultsMap){   
    std::tuple<unsigned int,unsigned int, int> val = sortSingleResult(itKey.second);
    ///Accept only candidates with >2 hits
    if(std::get<0>(val)<3) continue;
    if( std::get<0>(val)>nHitsMax){
      nHitsMax = std::get<0>(val);
      pdfValMax = std::get<1>(val);
      refPhi = std::get<2>(val);
      bestKey = itKey.first;
    }
    else if(std::get<0>(val)==nHitsMax &&  std::get<1>(val)>pdfValMax){
      pdfValMax = std::get<1>(val);
      refPhi = std::get<2>(val);
      bestKey = itKey.first;
    }
  }  

  L1Obj candidate;
  candidate.pt = bestKey.thePtCode;
  candidate.eta = bestKey.theEtaCode;
  candidate.phi = refPhi;
  candidate.charge = bestKey.theCharge;
  candidate.q   = nHitsMax;
  candidate.disc = pdfValMax;
  candidate.type = L1Obj::OTF;

  return candidate;

}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
