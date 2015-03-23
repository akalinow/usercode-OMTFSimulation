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
				   int charge);
  //
  void sortProcessorResults(const std::vector<OMTFProcessor::resultsMap> & procResults,
			    std::vector<InternalObj> & refHitCleanCands,
			    int charge);

  ///Sort all processor results. 
  ///First for each region cone find a best candidate using sortRegionResults() 
  ///Then select best candidate amongs found for each logic region
  L1MuRegionalCand sortProcessor(const std::vector<OMTFProcessor::resultsMap> & procResults,
				 int charge);
  //
  void sortProcessor(const std::vector<OMTFProcessor::resultsMap> & procResults,
		     std::vector<L1MuRegionalCand> & sortedCands,
		     int charge);

  ///Sort results from a single reference hit.
  ///Select candidate with highest number of hit layers
  ///Then select a candidate with largest likelihood value and given charge
  ///as we allow two candidates with opposite charge from single 10deg region
  InternalObj sortRefHitResults(const OMTFProcessor::resultsMap & aResultsMap, 
				int charge);

 private:

  ///Find a candidate with best parameters for given GoldenPattern
  ///Sorting is made amongs candidates with different reference layers
  ///The output tuple contains (nHitsMax, pdfValMax, refPhi, refLayer) 
  std::tuple<unsigned int,unsigned int, int, int> sortSingleResult(const OMTFResult & aResult);

};

#endif
