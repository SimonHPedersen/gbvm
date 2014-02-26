//$Id: MemoryBlock.cc,v 1.7 2001/05/21 11:42:44 hempe Exp $

//#include "MemoryBlock.hpp"
#include "gc_new.hpp"

MemoryBlock_t::MemoryBlock_t(unsigned int size): carArraySize(size),
						 carsInUse(0),
						 freeList(0) {
  //allocate alligned block
  DB_INDENT;
  carArray = (Car_t*)memalign(BYTES_IN_CAR, size*BYTES_IN_CAR);
  DB_OUT(gc_memblk, "MemoryBlock(): Allocated new memory block at: "
	 << (void*) carArray << " with length: 0x" << hex
	 << (size*BYTES_IN_CAR) << dec << endl);
  freePtr = carArray;
  DB_OUTDENT;
}

MemoryBlock_t::~MemoryBlock_t() {
  DB_INDENT;
  if(carArray && !carsInUse) {
    DB_OUT(gc_memblk, "~MemoryBlock_t(): freeing memory block with size: "
	   << carArraySize << endl);
    free(carArray);
  } else {
    cerr << "~MemoryBlock_t(): error nothing was allocated or cars still"
      " in use\n";
    exit(1);
  }
  DB_OUTDENT;
}

void *MemoryBlock_t::allocCarSpace() {
  DB_INDENT;
  DB_OUT(gc_memblk, "MemoryBlock_t::allocCarSpace(): carsInUse; " << carsInUse
	 << ", max " << carArraySize << endl);

  //try our freelist first
  if(carArraySize <= carsInUse) {
    //no more room in memory block
    DB_OUT(gc_memblk, "MemoryBlock_t::allocCarSpace(): all cars in use\n");
    DB_OUTDENT;
    return 0;
  }
  if(freeList) {
    Car_t *car = freeList;
    freeList = freeList->next();
    carsInUse++;

    //clear space - done in VMObject
    //    memset(car, 0, BYTES_IN_CAR);

    DB_OUTDENT;
    return (void*)car;
  }
  //try a new block then
  if((size_t)freePtr < ((size_t)carArray + size_t(carArraySize)*BYTES_IN_CAR)) {
    DB_OUT(gc_memblk, "MemoryBlock_t::allocCarSpace(): trying to allocate new"
	   " car from same block\n");
    Car_t *car = freePtr;
    freePtr = (Car_t*)(size_t(freePtr) + BYTES_IN_CAR);
    carsInUse++;
    DB_OUT(gc_memblk, "MemoryBlock_t::allocCarSpace(): returning car address: "
	   << (void*)car << endl);

    //clear space - done in VMObject
    //memset(car, 0, BYTES_IN_CAR);

    DB_OUTDENT;
    return (void*)car;
  }
  //should never happen
  cerr << "MemoryBlock_t::allocCarSpace(): strange internal error\n";
  exit(1);
  DB_OUTDENT;
  return 0;
}

int MemoryBlock_t::freeCar(Car_t *car2Free) {
  DB_INDENT;

  //check if car is in this memory block
  if(car2Free < carArray || car2Free >= (carArray + carArraySize*BYTES_IN_CAR)) {
    //not this memory block
    DB_OUT(gc_memblk, "MemoryBlock_t::freeCar(): car not in this block\n");
    DB_OUTDENT;
    return 0;
  }
  //insert empty car in freelist
  delete(car2Free); //does nothing except freeing the remsets
  car2Free->setNext(freeList);
  freeList = car2Free;
  carsInUse--;

  DB_OUT(gc_memblk, "MemoryBlock_t::freeCar(): carsInUse: " << carsInUse
	 << ", max " << carArraySize << endl);

  //check if we are the last block and all our cars have been freed
  if(!(this->next())) {
    if(!carsInUse) {
      DB_OUT(gc_memblk, "MemoryBlock_t::freeCar(): block is last and unused,"
	     " could be freed\n");
      DB_OUTDENT;
      return 2;
    }
  }
  DB_OUT(gc_memblk, "MemoryBlock_t::freeCar(): freed car successfully\n");
  DB_OUTDENT;
  return 1;
}
