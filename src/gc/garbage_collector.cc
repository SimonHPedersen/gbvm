//$Id: garbage_collector.cc,v 1.34 2001/05/21 11:42:43 hempe Exp $

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>

#include "garbage_collector.hpp"

/* our global garbage collector object */
GarbageCollector_t* garbageCollector = new GarbageCollector_t(1);
unsigned int vmObjectHashValue = 0;

/* GENERATION SUPERCLASS */

Generation_t::Generation_t(int size) {
  DB_INDENT;
  DB_OUT(gc_gc, "Generation_t(int)");
#ifdef DEBUG
  collected = 0;
  allocated = 0;
#endif
  //init(size);
  DB_OUTDENT;
}

Generation_t::~Generation_t() {
  DB_INDENT;
  DB_OUT(gc_gc, "freeing generation\n");
  free(heapPointer);
}

void Generation_t::init(int size) {
  DB_INDENT;
  DB_OUT(gc_gc, "Generation_t::init(int size)\n");
  heapSize = size;
  heapPointer = malloc(size);
  rootSpacePointer = malloc(sizeof(VMObject_t));
  freePointer = heapPointer;
  DB_OUT(gc_gc, "Generation_t::init(): returned\n");
  DB_OUTDENT;
}


/*
int Generation_t::ownsObject(VMObject_t* vmo) {
  if(vmo >= heapPointer && vmo < (void*)((int)heapPointer + heapSize)) {
    return 1;
  } else {
    return 0;
  }
}


VMObject_t* Generation_t::promoteObject(VMObject_t* vmo) {
  //we never promote objects!
  return 0;
}
*/

/* GARBAGE COLLECTOR */

GarbageCollector_t::GarbageCollector_t(int generations) {
  /* this is how it should have been
  int i;
  this->generations = generations;
  for(i = 0; i < (generations-1); i++) {
    generation[i] = new YoungGeneration_t(4096);
  }
  generation[generations-1] = new MatureGeneration_t(100000);
  */
  this->generations = 1;
  generation[0] = new MatureGeneration_t(0);
  prepareForGC_fkt = 0;
  newBytesAllocated = 0;
}

GarbageCollector_t::~GarbageCollector_t() {
  DB_INDENT;
  for(unsigned int i = 0; i < generations; i++) {
    DB_OUT(gc_gc, i << endl);
    //generation[i]->~Generation();
    delete generation[i];
  }
  DB_OUTDENT;
}


VMObject_t *GarbageCollector_t::addNewObject(ObjectDescriptor_t *objDes) {
  DB_INDENT;
  DB_OUT(gc_gc, "GarbageCollector_t::addNewObject 1" << endl);
#ifdef DEBUG
  allocated++;
#endif
  VMObject_t *vmo = generation[0]->addNewObject(objDes);
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
  return generation[0]->addRootObject(objDes);
}

void GarbageCollector_t::registerMutator(void (prepareForGC_f) (void)) {
  prepareForGC_fkt = prepareForGC_f;
  //just checking...
  (prepareForGC_fkt)();
}

void GarbageCollector_t::garbageCollect() {
  DB_INDENT;
#ifndef NO_GC
# ifdef INTEGRITY_CHECK
  checkIntegrity();
# endif
  DB_OUT(gc_notify, "Garbage collecting\n");
#  ifdef DEBUG
  //for(int i = 0; i < 100; i++) {
#  endif
    generation[0]->garbageCollect();
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
}

#ifdef DEBUG
void GarbageCollector_t::heapDump(void){
  generation[0]->heapDump();
}

void GarbageCollector_t::checkIntegrity(void){
  DB_INDENT;
  int errors = 0;
  VMObject_t **validObjRefsArray = (VMObject_t**)malloc(CHECK_INTEGRITY_MAX_OBJS * sizeof(VMObject_t*));
  DB_OUT(gc_integrity, "GarbageCollector::checkIntegrity: gathering all valid vmo's\n");
  VMObject_t **lastObjRef = generation[0]->gatherObjRefs(validObjRefsArray);
  unsigned int length = (((unsigned int)lastObjRef)-((unsigned int)validObjRefsArray))
    /sizeof(void*);

  DB_OUT(gc_integrity, "GarbageCollector::checkIntegrity: checking references in all"
	 " objects\n");
  errors += generation[0]->checkObjRefs(validObjRefsArray, length);
  free(validObjRefsArray);

  if(errors) {
    cerr << "GarbageCollector_t::checkIntegrity(): found " << errors <<
      " error(s), bailing out!\n";
    exit(1);
  }
  DB_OUT(gc_integrity, "GarbageCollector::checkIntegrity: no errors found!\n");
  DB_OUTDENT;
}

#endif

