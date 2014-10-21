#ifndef OMTF_OMTFinput_H
#define OMTF_OMTFinput_H

#include <vector>
#include <ostream>
#include <bitset>

class XMLConfigReader;

class OMTFinput{

 public:

  typedef std::vector<int> vector1D;
  typedef std::vector<vector1D> vector2D;

  OMTFinput();

  ///Add hit to given layer.
  ///iInput marks input number (max 14 per layer)
  bool addLayerHit(unsigned int iLayer,
		   unsigned int iInput,
		   int iPhi);

  ///Reset vectors with data.
  void clear();

  ///Read data from a XML file
  void readData(XMLConfigReader *aReader, 
		unsigned int iEvent=0);

  ///Apply shift to all data
  void shiftMyPhi(int phiShift);

  const OMTFinput::vector1D & getLayerData(unsigned int iLayer) const;

  std::bitset<80> getRefHits(unsigned int iProcessor) const;

  friend std::ostream & operator << (std::ostream &out, const OMTFinput & aInput);

 private:

  ///Measurements in logic layers
  ///First index: layer number
  ///Second index: measurement number within layer
  vector2D measurements; 

  ///Bitset of reference hit flags
  ///If there is a reference hit matching definition for
  ///i-th ref hit index==i is set to true
  //std::bitset<80> refHits;

};


#endif
