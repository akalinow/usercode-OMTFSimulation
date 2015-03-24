#ifndef InternalObj_H
#define InternalObj_H

#include <ostream>

struct InternalObj{

  float pt, eta, phi;
  float disc;
  int   bx, q, charge;
  int refLayer;
  int hits;

  InternalObj() : pt(-1.),eta(99.),phi(99.),disc(-999), bx(0),q(-1), charge(99), refLayer(-1) {}
  bool isValid() const { return q >= 0;}

  friend std::ostream & operator<< (std::ostream &out, const InternalObj &o);

};
#endif
