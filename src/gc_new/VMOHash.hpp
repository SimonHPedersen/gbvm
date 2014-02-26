//$Id: VMOHash.hpp,v 1.2 2001/04/25 17:26:57 ilsoe Exp $

#ifndef _VMOHASH_HPP_
#define _VMOHASH_HPP_

class VMOHash_t;

#include <hash_set>

//#include "VMObject.hpp"

template <>
class hash<VMObject_t*>{
public:
  size_t operator()(VMObject_t* const c1) const{
    hash<int> h;
    return h((int)c1);
  }
};

class EqVmo{
public:
  bool operator()(VMObject_t* const c1, VMObject_t* const c2) const
  {
    return (c1 == c2);
  }
};

typedef hash_set<VMObject_t*, hash<VMObject_t*>, EqVmo> VMOHashSet_t;

typedef hash_set<VMObject_t*, hash<VMObject_t*>, EqVmo>::const_iterator VMOHashIt_t;

class VMOHash_t{
public:
                              VMOHash_t(){}
                              ~VMOHash_t();
  VMObject_t*                 getFirst(void);
  VMObject_t*                 lookup(VMObject_t* c);
  void                        insert(VMObject_t* c);
  void                        remove(VMObject_t* c);
  VMOHashIt_t                 getIterator(void);
  VMOHashIt_t                 getEndIterator(void);
  void                        clear(void);
  bool                        empty(void);
  void                        printOn(ostream& stream);
private:
  VMOHashSet_t                vmohash;
};


ostream& operator << (ostream& stream, VMOHash_t* rs);
ostream& operator << (ostream& stream, VMOHash_t& rs);

#endif

