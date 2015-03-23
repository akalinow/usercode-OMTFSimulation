#include <cassert>
#include <iostream>
#include <strstream>
#include <algorithm>

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
InternalObj OMTFSorter::sortRefHitResults(const OMTFProcessor::resultsMap & aResultsMap,
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

  InternalObj candidate(bestKey.thePtCode, bestKey.theEtaCode, refPhi,
			pdfValMax, 0, nHitsMax,
			bestKey.theCharge, refLayer);


  /////TEST AVERAGE PT///////
  /*
  pdfValMax = 0;
  for(auto itKey: aResultsMap){
    if(itKey.first.thePtCode>candidate.pt) continue;
    std::tuple<unsigned int,unsigned int, int, int > val = sortSingleResult(itKey.second);
    if(std::get<0>(val)==nHitsMax && std::get<1>(val)!=candidate.disc &&  std::get<1>(val)>pdfValMax){
      pdfValMax = std::get<1>(val);
      refPhi = std::get<2>(val);
      refLayer = std::get<3>(val);
      bestKey = itKey.first;
    }
  }
  InternalObj candidatePrevious;
  candidatePrevious.pt =  bestKey.thePtCode;
  candidatePrevious.eta = bestKey.theEtaCode; 
  candidatePrevious.phi = refPhi;
  candidatePrevious.charge = bestKey.theCharge;
  candidatePrevious.q   = nHitsMax;
  candidatePrevious.disc = pdfValMax;
  candidatePrevious.refLayer = refLayer;

  pdfValMax = 0;
  for(auto itKey: aResultsMap){   
    if(itKey.first.thePtCode<candidate.pt) continue;
    std::tuple<unsigned int,unsigned int, int, int > val = sortSingleResult(itKey.second);
    if(std::get<0>(val)==nHitsMax && std::get<1>(val)!=candidate.disc && std::get<1>(val)>pdfValMax){
      pdfValMax = std::get<1>(val);
      refPhi = std::get<2>(val);
      refLayer = std::get<3>(val);
      bestKey = itKey.first;
    }
  }
  InternalObj candidateNext;
  candidateNext.pt =  bestKey.thePtCode;
  candidateNext.eta = bestKey.theEtaCode; 
  candidateNext.phi = refPhi;
  candidateNext.charge = bestKey.theCharge;
  candidateNext.q   = nHitsMax;
  candidateNext.disc = pdfValMax;
  candidateNext.refLayer = refLayer;

  if(candidate.pt){
    float weightedPtCode = candidatePrevious.pt*candidatePrevious.disc + 
                           candidate.pt*candidate.disc + 
                           candidateNext.pt*candidateNext.disc;
    weightedPtCode/= candidatePrevious.disc + candidate.disc + candidateNext.disc;
    candidate.pt = (int)weightedPtCode;
  } 
*/
  ////////////////////////////// 

  return candidate;

}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
InternalObj OMTFSorter::sortProcessorResults(const std::vector<OMTFProcessor::resultsMap> & procResults,
					     int charge){

  InternalObj candidate;
  std::vector<InternalObj> refHitCands;

  for(auto itRefHit: procResults) refHitCands.push_back(sortRefHitResults(itRefHit,charge));

  // Sort candidates with decreased goodness...
  /*
  std::sort( refHitCands.begin(), refHitCands.end(), 
	     [](const InternalObj &o1, const InternalObj &o2){
	       if(o1.q > o2.q) return true;
	       else if(o1.q==o2.q && o1.disc > o2.disc) return true;
	       else return false;
	     } );
  */
  //where goodness definied in < operator of InternalObj
  std::sort( refHitCands.begin(), refHitCands.end() );
  // and then take the best one
  if(!refHitCands.empty()) candidate = refHitCands[0];

  //Clean candidate list by removing dupicates bazing on Phi distance 
  //Assumed that list is ordered
  std::vector<InternalObj> refHitCleanCands;
  for(std::vector<InternalObj>::iterator it1 = refHitCands.begin();
      it1 != refHitCands.end(); ++it1){
    bool isGhost=false;
    for(std::vector<InternalObj>::iterator it2 = refHitCleanCands.begin();
	it2 != refHitCleanCands.end(); ++it2){
      if(std::abs(it1->phi - it2->phi)<10/360.0*OMTFConfiguration::nPhiBins){//consider shrink veto window 10->5 deg
	isGhost=true;
	break;
      }
    }
    if(!isGhost) refHitCleanCands.push_back(*it1);
  }
  refHitCleanCands.resize( refHitCands.size() );//preserve number of candidates adding empty ones

  //if(candidate.q>0){
  if(true){
    std::cout<<"before cleaning\n";
    for(unsigned int ii=0; ii<refHitCands.size(); ++ii)
      std::cout<<"\t"<<ii<<". "<<refHitCands[ii]<<"\n";   
    std::cout<<"after cleaning\n";
    for(unsigned int ii=0; ii<refHitCleanCands.size(); ++ii)
      std::cout<<"\t"<<ii<<". "<<refHitCleanCands[ii]<<"\n";
    std::cout<<std::endl;
  }

  std::ostringstream myStr;
  for(unsigned int iRefHit=0;iRefHit<refHitCands.size();++iRefHit){
    if(refHitCands[iRefHit].q) myStr<<"Ref hit: "<<iRefHit<<" "<<refHitCands[iRefHit]<<std::endl;
  }
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
