//$Id: garbage_collector.hpp,v 1.23 2001/03/12 13:39:43 ilsoe Exp $

class GarbageCollector_t;

#ifndef GARBAGE_COLLECTOR_HPP
#define GARBAGE_COLLECTOR_HPP

#include <limits.h>

#include "globals.hpp"
#include "ObjectDescriptor.hpp"
#include "VMObject.hpp"
#include "Train.hpp"
#include "rememberedset.hpp"
#include "Generation.hpp"


//#define MAX_NUMBER_OF_TRAINS 4

class VMObject_t;

/* global variables */
extern GarbageCollector_t*    garbageCollector;
extern unsigned int vmObjectHashValue;


class GarbageCollector_t {
public:
                              GarbageCollector_t(int generations);
                              ~GarbageCollector_t();
  //  Generation_t*               getGeneration(VMObject_t* vmo);
  //  int                         sameGeneration(VMObject_t* vmo1, VMObject_t* vmo2);
  //  VMObject_t*                 promoteObject(VMObject_t* vmo);
  VMObject_t                 *addNewObject(ObjectDescriptor_t *objDes);
  VMObject_t                 *addRootObject(ObjectDescriptor_t *objDes);
  void                        registerMutator(void (prepareForGC_fkt) (void));
  void                        garbageCollect(void);
#ifdef DEBUG
  void                        heapDump(void);
  void                        checkIntegrity(void);
#endif
private:
  void                        (*prepareForGC_fkt)(void);
                                //function pointer to the mutator
  int                         newBytesAllocated;
                                //keeps track of how many bytes we have allocated
                                //since last gc
  unsigned int                generations;
  Generation_t                *generation[10];
};

#endif
