#ifndef OMTF_OMTFinput_H
#define OMTF_OMTFinput_H

#include <vector>
#include <ostream>

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

  ///Apply shitf to all data
  void shiftMyPhi(int phiShift);

  const OMTFinput::vector1D & getLayerData(unsigned int iLayer) const;

  std::ostream & print(std::ostream & out) const;

 private:

  ///Measurements in logic layers
  ///First index: layer number
  ///Second index: measurement number within layer
  vector2D measurements; 

};


#endif
