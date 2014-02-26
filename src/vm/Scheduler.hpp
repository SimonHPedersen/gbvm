//$Id: Scheduler.hpp,v 1.12 2001/05/10 12:36:59 hempe Exp $

#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

class Scheduler_t;

#include "Thread.hpp"
#include "vm_globals.hpp"
#include "GBetaEntities.hpp"
#include "ByteCodeLoader.hpp"
#include "ObjectDescriptor.hpp"
#include "VMObject.hpp"
#include "gc_globals.hpp"

extern Scheduler_t *scheduler;

void gcs_prepareForGC(void);

class Scheduler_t {
public:
  void                       *operator new(size_t size, void *location);
  void                       *operator new(size_t size);
                              Scheduler_t(void) {};

  void                        startEngine(char* fileName);
  unsigned int                getPredefOffset(void);
  unsigned int                getMainThreadOffset(void);
  unsigned int                getEmptyPatternOffset(void);
  void                        addNewThread(Thread_t* thr){};
  void                        prepareForGC(void);
                                //interface from the garbage collector:
                                //gc calls this method (through gcs_prepareForGC)
                                //to tell the scheduler it wants to 
                                //garbage collect.
                                //The scheduler must then later
                                //(when threads are at a safe point)
                                //invoke garbageCollect() 
                                //on the garbage collector
                                //NOTE: handles only one thread!
  MainPart*                   getMainPart(char* id);
public:
  Pattern_t                  *getEmptyObjectPattern(void); 
public:
private:
  void                        mainLoop(void);
                                //NOTE: handles only one thread!
private:
  PartObject_t*               predef;
  Thread_t*                   mainThread;
  Lst<MainPart>              *mainParts;
  EmptyPattern_t             *emptyPattern; 
};

#endif
