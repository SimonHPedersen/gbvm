//$Id: DirtyCars.hpp,v 1.5 2001/03/27 13:24:09 hempe Exp $

#ifndef _DIRTYCARS_HPP_
#define _DIRTYCARS_HPP_

class DirtyCars_t;

#include <hash_set>

template <>
class hash<Car_t*>{
public:
  size_t operator()(Car_t* const c1) const{
    hash<int> h;
    return h((int)c1);
  }
};

class EqCar{
public:
  bool operator()(Car_t* const c1, Car_t* const c2) const
  {
    return (c1 == c2);
  }
};

typedef hash_set<Car_t*, hash<Car_t*>, EqCar> CarHashSet_t;

typedef hash_set<Car_t*, hash<Car_t*>, EqCar>::const_iterator dirtyCarsIt_t;

class DirtyCars_t{
public:
                              DirtyCars_t(){}
                              ~DirtyCars_t();
  Car_t*                      getFirst(void);
  Car_t*                      lookup(Car_t* c);
  void                        insert(Car_t* c);
  void                        remove(Car_t* c);
  dirtyCarsIt_t               getIterator(void);
  dirtyCarsIt_t               getEndIterator(void);
  void                        clear(void);
  bool                        empty(void);
  void                        printOn(ostream& stream);
private:
  CarHashSet_t                dirtyCars;
};


ostream& operator << (ostream& stream, DirtyCars_t* rs);
ostream& operator << (ostream& stream, DirtyCars_t& rs);

#endif

