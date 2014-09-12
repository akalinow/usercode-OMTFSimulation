#include <cassert>
#include <iostream>

#include "interface/OMTFSorter.h"

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
std::array<unsigned int,3> OMTFSorter::sortSingleResult(const OMTFResult & aResult){

  OMTFResult::vector1D pdfValsVec = aResult.getSummaryVals();
  OMTFResult::vector1D nHitsVec = aResult.getSummaryHits();
  OMTFResult::vector1D refPhiVec = aResult.getRefPhis();

  assert(pdfValsVec.size()==nHitsVec.size());

  unsigned int nHitsMax = 0;
  unsigned int pdfValMax = 0;
  unsigned int refPhi = 1024;

  std::array<unsigned int,3> sortedResult;
  sortedResult[0] = nHitsMax;
  sortedResult[1] = pdfValMax;
  sortedResult[2] = refPhi;

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

  sortedResult[0] = nHitsMax;
  sortedResult[1] = pdfValMax;
  sortedResult[2] = refPhi;
  return sortedResult;
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
L1Obj OMTFSorter::sortResults(const OMTFProcessor::resultsMap & aResultsMap){

  unsigned int pdfValMax = 0;
  unsigned int nHitsMax = 0;
  unsigned int refPhi = 1024;
  Key bestKey;
  for(auto itKey: aResultsMap){   
    std::array<unsigned int,3> val = sortSingleResult(itKey.second);
    if(val[0]>nHitsMax){
      nHitsMax = val[0];
      pdfValMax = val[1];
      refPhi = val[2];
      bestKey = itKey.first;
    }
    else if(val[0]==nHitsMax && val[1]>pdfValMax){
      pdfValMax = val[1];
      refPhi = val[2];
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
