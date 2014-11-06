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
  ///Then select best candidate amongs found for each logic region
  InternalObj sortProcessorResults(const std::vector<OMTFProcessor::resultsMap> & procResults);

  ///Sort all processor results. 
  ///First for each region cone find a best candidate using sortRegionResults() 
  ///Then select best candidate amongs found for each logic region
  L1MuRegionalCand sortProcessor(const std::vector<OMTFProcessor::resultsMap> & procResults);

  ///Sort results for signle logic region.
  ///Select candidate with highed number of hit layers
  ///Then select a candidate with largest likelihood value
  InternalObj sortRegionResults(const OMTFProcessor::resultsMap & aResultsMap);

 private:

  ///Find a candidate with best parameters for given GoldenPattern
  ///Sorting is made amongs candidates with different reference layers
  ///The output tuple contains (nHitsMax, pdfValMax, refPhi, refLayer) 
  std::tuple<unsigned int,unsigned int, int, int> sortSingleResult(const OMTFResult & aResult);

};

#endif
