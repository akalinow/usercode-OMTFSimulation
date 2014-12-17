#include <cassert>
#include <iostream>
#include <strstream>

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"

#include "UserCode/OMTFSimulation/interface/OMTFConfiguration.h"
#include "UserCode/OMTFSimulation/interface/OMTFSorter.h"

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
std::tuple<unsigned int,unsigned int, int, int> OMTFSorter::sortSingleResult(const OMTFResult & aResult){

  OMTFResult::vector1D pdfValsVec = aResult.getSummaryVals();
  OMTFResult::vector1D nHitsVec = aResult.getSummaryHits();
  OMTFResult::vector1D refPhiVec = aResult.getRefPhis();

  assert(pdfValsVec.size()==nHitsVec.size());

  unsigned int nHitsMax = 0;
  unsigned int pdfValMax = 0;
  int refPhi = 1024;
  int refLayer = -1;

  std::tuple<unsigned int,unsigned int, int, int>  sortedResult;
  std::get<0>(sortedResult) = nHitsMax;
  std::get<1>(sortedResult) = pdfValMax;
  std::get<2>(sortedResult) = refPhi;
  std::get<3>(sortedResult) = refLayer;

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
	refLayer = ipdfVal;
      }
    }
  }

  std::get<0>(sortedResult) = nHitsMax;
  std::get<1>(sortedResult) = pdfValMax;
  std::get<2>(sortedResult) = refPhi;
  std::get<3>(sortedResult) = refLayer;
  return sortedResult;
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
InternalObj OMTFSorter::sortRegionResults(const OMTFProcessor::resultsMap & aResultsMap,
					  int charge){

  unsigned int pdfValMax = 0;
  unsigned int nHitsMax = 0;  
  int refPhi = 9999;
  int refLayer = -1;
  Key bestKey;
  for(auto itKey: aResultsMap){   
    //if(itKey.first.theCharge!=charge) continue;
    std::tuple<unsigned int,unsigned int, int, int > val = sortSingleResult(itKey.second);
    ///Accept only candidates with >2 hits
    if(std::get<0>(val)<3) continue;
    ///Accept candidates with good likelihood value
    //if(std::get<1>(val)/std::get<0>(val)<30) continue;
    //if(std::get<3>(val)>5) continue;
    ///
    if( std::get<0>(val)>nHitsMax){
      nHitsMax = std::get<0>(val);
      pdfValMax = std::get<1>(val);
      refPhi = std::get<2>(val);
      refLayer = std::get<3>(val);
      bestKey = itKey.first;
    }
    else if(std::get<0>(val)==nHitsMax &&  std::get<1>(val)>pdfValMax){
      pdfValMax = std::get<1>(val);
      refPhi = std::get<2>(val);
      refLayer = std::get<3>(val);
      bestKey = itKey.first;
    }
  }  

  InternalObj candidate;
  candidate.pt =  bestKey.thePtCode;
  candidate.eta = bestKey.theEtaCode; 
  candidate.phi = refPhi;
  candidate.charge = bestKey.theCharge;
  candidate.q   = nHitsMax;
  candidate.disc = pdfValMax;
  candidate.refLayer = refLayer;

  return candidate;

}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
InternalObj OMTFSorter::sortProcessorResults(const std::vector<OMTFProcessor::resultsMap> & procResults,
					     int charge){

  InternalObj candidate;
  std::vector<InternalObj> regionCands;

  for(auto itRegion: procResults) regionCands.push_back(sortRegionResults(itRegion,charge));

  for(auto itCand: regionCands){
    if(itCand.q>candidate.q) candidate = itCand;
    if(itCand.q==candidate.q && itCand.disc>candidate.disc) candidate = itCand;
  }

  std::ostringstream myStr;
  for(unsigned int iRegion=0;iRegion<regionCands.size();++iRegion) myStr<<"Logic Region: "<<iRegion<<" "<<regionCands[iRegion]<<std::endl;
  myStr<<"Selected Candidate with charge: "<<charge<<" "<<candidate<<std::endl;
  edm::LogInfo("OMTF Sorter")<<myStr.str();

  return candidate;
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
L1MuRegionalCand OMTFSorter::sortProcessor(const std::vector<OMTFProcessor::resultsMap> & procResults,
					   int charge){

  InternalObj myCand = sortProcessorResults(procResults, charge);

  L1MuRegionalCand candidate;
  candidate.setPhiValue(myCand.phi);
  candidate.setPtPacked(myCand.pt);
  //candidate.setQualityPacked(3);//FIX ME
  candidate.setBx(1000*myCand.disc+100*myCand.refLayer+myCand.q);//FIX ME
  candidate.setChargeValue(myCand.charge);

  return candidate;
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
