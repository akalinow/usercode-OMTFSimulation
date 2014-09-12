#ifndef OMTF_OMTFSorter_H
#define OMTF_OMTFSorter_H

#include <array>

#include "interface/OMTFResult.h"
#include "interface/OMTFProcessor.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"

class OMTFSorter{

 public:

  L1Obj sortResults(const OMTFProcessor::resultsMap & aResultsMap);

 private:

   std::array<unsigned int,3> sortSingleResult(const OMTFResult & aResult);

};

#endif
