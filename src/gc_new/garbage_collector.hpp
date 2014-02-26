//$Id: garbage_collector.hpp,v 1.16 2001/05/28 12:56:24 ilsoe Exp $

#ifndef GARBAGE_COLLECTOR_HPP
#define GARBAGE_COLLECTOR_HPP

#include <sys/times.h>

class GarbageCollector_t;

/* global variables */
class GarbaggeCollector_t;
class IntroductorySpace_t;
class StackSpace_t;
class TrainGeneration_t;
class TrainTable_t;

extern GarbageCollector_t    garbageCollector;
extern IntroductorySpace_t   introSpace;
extern StackSpace_t          stackSpace;
extern TrainGeneration_t     trainGeneration;
extern TrainTable_t          ttable;

#include "gc_new.hpp"

class GarbageCollector_t {
public:
                              GarbageCollector_t(int generations);
                              ~GarbageCollector_t();
  //  Generation_t*               getGeneration(VMObject_t* vmo);
  //  int                         sameGeneration(VMObject_t* vmo1, VMObject_t* vmo2);
  //  VMObject_t*                 promoteObject(VMObject_t* vmo);
  VMObject_t                 *addStackObject(ObjectDescriptor_t *objDes);
  VMObject_t                 *addNewObject(ObjectDescriptor_t *objDes);
  //  VMObject_t                 *addRootObject(ObjectDescriptor_t *objDes);
  void                        registerMutator(void (prepareForGC_fkt) (void));
  void                        garbageCollect(void);
#ifdef LIVESTAT
  void                        doLiveStat(void);
#endif //LIVESTAT
#ifdef DISRUPT_STAT
  void                        printFinalDisruptStat(void);
#endif
#ifdef DEBUG
  void                        heapDump(void);
  void                        checkIntegrity(void);
#endif //DEBUG
private:
  void                        checkTrainSpaceSetup();
                                //makes sure there is a from-car and a
                                //last car in separate trains
  void                        doGC(void);
                                //the actual gc algorithm
  void                        (*prepareForGC_fkt)(void);
                                //function pointer to the mutator
  //  unsigned int                newBytesAllocated;
                                //keeps track of how many bytes we have allocated
                                //since last gc
private:
#ifdef DISRUPT_STAT
  int                         gctimes;
  float                       gc_avg_time;
  float                       gc_worst_time;
#endif //DISRUPT_STAT
};

#endif
