//$Id: garbage_collector.cc,v 1.2 2001/05/21 11:42:42 hempe Exp $

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>

//#include "garbage_collector.hpp"
#include "allocator.hpp"

/* our global garbage collector object */
GarbageCollector_t* garbageCollector = new GarbageCollector_t(1);
//unsigned int vmObjectHashValue = 0;

/* --- GARBAGE COLLECTOR --- */

GarbageCollector_t::GarbageCollector_t(int generations) {
  trainGeneration = new TrainGeneration_t(8);
  prepareForGC_fkt = 0;
  newBytesAllocated = 0;
}

GarbageCollector_t::~GarbageCollector_t() {
  DB_INDENT;
  delete trainGeneration;
  DB_OUTDENT;
}


VMObject_t *GarbageCollector_t::addNewObject(ObjectDescriptor_t *objDes) {
  DB_INDENT;

  DB_OUT(gc_gc, "GarbageCollector_t::addNewObject 1" << endl);
  /*
#ifdef DEBUG
  allocated++;
#endif
  */

  VMObject_t *vmo = trainGeneration->addNewObject(objDes);
#ifndef NO_GC
  newBytesAllocated += objDes->getObjLengthBytes() + sizeof(VMObject_t);
  //would we want to garbage collect now?
  if((newBytesAllocated > (BYTES_IN_CAR/2)) && prepareForGC_fkt) {
    //ask mutator for permission to gc if mutator has registered
    (prepareForGC_fkt)();
  }
  DB_OUT(gc_gc, "GarbageCollector_t::addNewObject 2" << endl);
#endif
  DB_OUTDENT;
  return vmo;

}

VMObject_t *GarbageCollector_t::addRootObject(ObjectDescriptor_t *objDes) {
  return trainGeneration->addRootObject(objDes);
}

void GarbageCollector_t::registerMutator(void (prepareForGC_f) (void)) {
  prepareForGC_fkt = prepareForGC_f;
  //just checking...
  //(prepareForGC_fkt)();
}

void GarbageCollector_t::garbageCollect() {
  /*

  DB_INDENT;
#ifndef NO_GC
# ifdef INTEGRITY_CHECK
  checkIntegrity();
# endif
  DB_OUT(gc_notify, "Garbage collecting\n");
#  ifdef DEBUG
  //for(int i = 0; i < 100; i++) {
#  endif
    trainGeneration->garbageCollect();
# ifdef INTEGRITY_CHECK
    checkIntegrity();
# endif

# ifdef DEBUG
    //}
# endif
  newBytesAllocated = 0;
#else
  DB_OUT(gc_gc, "GarbageCollector::garbageCollect(): Warning, gc is disabled\n");
#endif
  DB_OUTDENT;
  */
}

#ifdef DEBUG
void GarbageCollector_t::heapDump(void){
  trainGeneration->heapDump();
}

void GarbageCollector_t::checkIntegrity(void){
  /*

  DB_INDENT;
  int errors = 0;
  VMObject_t **validObjRefsArray = (VMObject_t**)malloc(CHECK_INTEGRITY_MAX_OBJS * sizeof(VMObject_t*));
  DB_OUT(gc_integrity, "GarbageCollector::checkIntegrity: gathering all valid vmo's\n");
  VMObject_t **lastObjRef = trainGeneration->gatherObjRefs(validObjRefsArray);
  unsigned int length = (((unsigned int)lastObjRef)-((unsigned int)validObjRefsArray))
    /sizeof(void*);

  DB_OUT(gc_integrity, "GarbageCollector::checkIntegrity: checking references in all"
	 " objects\n");
  errors += trainGeneration->checkObjRefs(validObjRefsArray, length);
  free(validObjRefsArray);

  if(errors) {
    cerr << "GarbageCollector_t::checkIntegrity(): found " << errors <<
      " error(s), bailing out!\n";
    exit(1);
  }
  DB_OUT(gc_integrity, "GarbageCollector::checkIntegrity: no errors found!\n");
  DB_OUTDENT;
  */

}

#endif

