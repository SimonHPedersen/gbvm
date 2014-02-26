//$Id: MemoryBlock.hpp,v 1.4 2001/04/25 17:26:57 ilsoe Exp $

#ifndef _MEMORY_BLOCK_HPP_
#define _MEMORY_BLOCK_HPP_

class MemoryBlock_t;

#include <stdlib.h>
//#include "Lists.hpp"
//#include "Car.hpp"

class MemoryBlock_t: public LstElm<MemoryBlock_t> {
public:
                              MemoryBlock_t(unsigned int size);
                                //size is the number of cars to store in
                                //the memory block
                              ~MemoryBlock_t();
  void                       *allocCarSpace();
                                //returns a car, 0 if unavailable
  int                         freeCar(Car_t *car2Free);
                                //frees a car returns 1 on success
  bool                        member(void *address) {
                               return (address < carArray || address >= freePtr) ?
				 false : true;
                              }
    
private:
  unsigned int                carArraySize;
                                //number of cars in this block
  unsigned int                carsInUse;
                                //number of used blocks
  Car_t                      *carArray;
                                //pointer to the actual data
  Car_t                      *freePtr;
                                //pointer into block where the next block is free
  Car_t                      *freeList;
                                //list of cars which have been freed
};


#endif
