#ifndef OMTF_OMTFConfiguration_H
#define OMTF_OMTFConfiguration_H

#include <map>
#include <set>
#include <vector>
#include <ostream>


class XMLConfigReader;

namespace edm{
  class ParameterSet;
}

class OMTFConfiguration{

 public:

  OMTFConfiguration(const edm::ParameterSet & cfg);

  void configure(XMLConfigReader *aReader);

  std::ostream & print(std::ostream & out);

  static unsigned int nLayers;
  static unsigned int nHitsPerLayer;
  static unsigned int nRefLayers;
  static unsigned int nPhiBits;
  static unsigned int nPdfAddrBits;
  static unsigned int nPdfValBits;
  static unsigned int nPhiBins;
    
  static std::map<int,int> hwToLogicLayer;
  static std::map<int,int> logicToHwLayer;
  static std::map<int,int> logicToLogic;
  static std::set<int> bendingLayers;
  static std::vector<int> refToLogicNumber;

  ///Starting iPhi for each processor and each referecne layer    
  ///First index: processor number
  ///Second index: referecne layer number
  static std::vector<std::vector<int> > processorPhiVsRefLayer;

  ///Temporary hack to pass data from deep in class
  typedef std::vector<int> vector1D;
  typedef std::vector<vector1D > vector2D;

  static std::vector<std::vector <vector2D> >measurements4D;

};


#endif
 
