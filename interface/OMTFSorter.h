#ifndef OMTF_OMTFSorter_H
#define OMTF_OMTFSorter_H

#include <tuple>

#include "UserCode/OMTFSimulation/interface/OMTFResult.h"
#include "UserCode/OMTFSimulation/interface/OMTFProcessor.h"

#include "UserCode/OMTFSimulation/interface/InternalObj.h"

class L1MuRegionalCand;

class OMTFSorter{

 public:

  ///Sort all processor results. 
  ///First for each region cone find a best candidate using sortRegionResults() 
  ///Then select best candidate amongs found for each logic region.
  ///The sorting is made for candidates with a given charge
  InternalObj sortProcessorResults(const std::vector<OMTFProcessor::resultsMap> & procResults,
				   int charge=0);
  //
  void sortProcessorResults(const std::vector<OMTFProcessor::resultsMap> & procResults,
			    std::vector<InternalObj> & refHitCleanCands,
			    int charge=0);

  ///Sort all processor results. 
  ///First for each region cone find a best candidate using sortRegionResults() 
  ///Then select best candidate amongs found for each logic region
  L1MuRegionalCand sortProcessor(const std::vector<OMTFProcessor::resultsMap> & procResults,
				 int charge=0);
  //
  void sortProcessor(const std::vector<OMTFProcessor::resultsMap> & procResults,
		     std::vector<L1MuRegionalCand> & sortedCands,
		     int charge=0);

  ///Sort results from a single reference hit.
  ///Select candidate with highest number of hit layers
  ///Then select a candidate with largest likelihood value and given charge
  ///as we allow two candidates with opposite charge from single 10deg region
  InternalObj sortRefHitResults(const OMTFProcessor::resultsMap & aResultsMap, 
				int charge=0);

 private:

  ///Find a candidate with best parameters for given GoldenPattern
  ///Sorting is made amongs candidates with different reference layers
  ///The output tuple contains (nHitsMax, pdfValMax, refPhi, refLayer, hitsWord)
  ///hitsWord codes number of layers hit: hitsWord= sum 2**iLogicLayer, 
  ///where sum runs over layers which were hit  
  std::tuple<unsigned int,unsigned int, int, int, unsigned int> sortSingleResult(const OMTFResult & aResult);

};

#endif
