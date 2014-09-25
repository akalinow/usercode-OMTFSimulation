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
  static unsigned int nRefHits;
    
  static std::map<int,int> hwToLogicLayer;
  static std::map<int,int> logicToHwLayer;
  static std::map<int,int> logicToLogic;
  static std::set<int> bendingLayers;
  static std::vector<int> refToLogicNumber;

  ///Starting iPhi for each processor and each referecne layer    
  ///Global phi scale is used
  ///First index: processor number
  ///Second index: referecne layer number
  static std::vector<std::vector<int> > processorPhiVsRefLayer;

  ///Begin and end local phi for each processor and each referecne layer    
  ///First index: processor number
  ///Second index: reference layer number
  ///Third index: region
  ///pair.first: starting phi of region (inclusive)
  ///pair.second: ending phi of region (inclusive)
  static std::vector<std::vector<std::vector<std::pair<int,int> > > >regionPhisVsRefLayerVsProcessor;

  ///Map of connections
  typedef std::vector< std::pair<unsigned int, unsigned int> > vector1D_A;
  typedef std::vector<vector1D_A > vector2D_A;
  typedef std::vector<vector2D_A > vector3D_A;
  static vector3D_A connections;

  ///Temporary hack to pass data from deep in class
  typedef std::vector<int> vector1D;
  typedef std::vector<vector1D > vector2D;
  typedef std::vector<vector2D > vector3D;
  typedef std::vector<vector3D > vector4D;
  static vector4D measurements4D;
  static vector4D measurements4Dref;


  ///Find number of logic region within a given processor.
  ///Number is calculated assuming 10 deg wide logic regions
  ///Global phi scale is assumed at input.
  static unsigned int getRegionNumber(unsigned int iProcessor,
				    unsigned int iRefLayer,
				    int iPhi);

  ///Find logic region number using shifted, 10 bit
  ///phi values, and commection maps
  static unsigned int getRegionNumberFromMap(unsigned int iProcessor,
					     unsigned int iRefLayer,
					     int iPhi);
  
  ///Check if given referecne hit is
  ///in phi range for some logic cone.
  ///Care is needed arounf +Pi and +2Pi points
  static bool isInRegionRange(int iPhiStart,
			    unsigned int coneSize,
			    int iPhi);


  ///Return global phi for beggining of given processor
  ///Uses minim phi over all reference layers.
  static int globalPhiStart(unsigned int iProcessor);

};


#endif
 
