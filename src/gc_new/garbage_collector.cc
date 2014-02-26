//$Id: garbage_collector.cc,v 1.41 2001/06/08 10:26:25 ilsoe Exp $

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>

//#include "garbage_collector.hpp"
#include "gc_new.hpp"

/* our global garbage collector object */
GarbageCollector_t  garbageCollector(1);
IntroductorySpace_t introSpace;
StackSpace_t        stackSpace(1<<16);
TrainGeneration_t   trainGeneration(8);;
TrainTable_t        ttable;

/* --- GARBAGE COLLECTOR --- */

//generations parameter is ignored
GarbageCollector_t::GarbageCollector_t(int generations) {
  prepareForGC_fkt = 0;
#ifdef DISRUPT_STAT
  gctimes = 0; gc_avg_time = 0; gc_worst_time = 0;
#endif //DISRUPT_STAT
}

GarbageCollector_t::~GarbageCollector_t() {
  DB_INDENT;
#ifdef DISRUPT_STAT
  printFinalDisruptStat();
#endif //DISRUPT_STAT
  cout << "deleting garbage collector\n";
  DB_OUTDENT;
}

VMObject_t *GarbageCollector_t::addStackObject(ObjectDescriptor_t *objDesc) {
#ifdef DEBUG
  if(!objDesc) {
    cerr << "GarbageCollector_t::addStackObject(): objDesc was null\n";
    exit(1);
  }
#endif
  return stackSpace.allocStackVMO(objDesc);
}


VMObject_t *GarbageCollector_t::addNewObject(ObjectDescriptor_t *objDesc) {
  DB_INDENT;
#ifdef DEBUG
  if(!objDesc) {
    cerr << "GarbageCollector_t::addNewObject(): objDesc was null\n";
    exit(1);
  }
#endif

#ifdef NO_GC
  VMObject_t *vmo = trainGeneration.addNewObject(objDesc);
#else //NO_GC that is GC
  VMObject_t *vmo = introSpace.allocVMO(objDesc);
  if(!vmo) {
    //intro space is full, allocate in train generation and ask mutator
    //for gc
    DB_OUT(gc_gc, "GarbageCollector_t::addNewObject(): introSpace is full,"
	   " allocating in train generation\n" << endl);
    vmo = trainGeneration.addNewObject(objDesc);
    if(prepareForGC_fkt) {
      (prepareForGC_fkt)();
    }
  }
  //  newBytesAllocated += objDesc->getObjLengthBytes()
  //    + sizeof(VMObject_t) - sizeof(void*);
#endif //NO_GC
  DB_OUTDENT;
  return vmo;
}


void GarbageCollector_t::registerMutator(void (prepareForGC_f) (void)) {
  prepareForGC_fkt = prepareForGC_f;
  //just checking...
  //(prepareForGC_fkt)();
}

void GarbageCollector_t::garbageCollect() {
  DB_INDENT;


#ifndef NO_GC
# ifdef   INTEGRITY_CHECK
#  ifdef   DEBUG
  heapDump();
  checkIntegrity();
#  endif //DEBUG
# endif //INTEGRITY_CHECK

# ifdef   LIVESTAT
  //cout << "* LiveStat before gc *\n";
  doLiveStat();
  //  cout << "* LiveStat before gc - done *\n";
# endif //LIVESTAT

  DB_OUT(gc_notify, "*** Garbage collecting ***\n");
  //  trainGeneration.garbageCollect();
# ifdef   DEBUG
  static int time = 1;
  cout << "*" << time << endl;
  DB_OUT(gc_notify, "*** number " << time << " ***\n");
  ++time;
# endif //DEBUG

  //  static int gctimes = 0;
  //  static float avg = 0, worst = 0;

# ifdef DISRUPT_STAT
  struct tms beforeGCTime, afterGCTime;

  if((clock_t)-1 == times(&beforeGCTime)) {
    cerr << "GarbageCollector_t::garbageCollect(): error getting time\n";
    exit(1);
  }
# endif //DISRUPT_STAT

  doGC();

# ifdef DISRUPT_STAT
  if((clock_t)-1 == times(&afterGCTime)) {
    cerr << "GarbageCollector_t::garbageCollect(): error getting time\n";
    exit(1);
  }

  int tics = afterGCTime.tms_utime - beforeGCTime.tms_utime;
  double secs = ((double)tics)/CLK_TCK;

  cout << "\ngc took: " << tics << " ticks ~ " << secs << " seconds\n";

  if(gc_worst_time < secs) gc_worst_time = secs;

  gc_avg_time = (gc_avg_time * gctimes + secs) / (gctimes + 1);

  ++gctimes;

  cout << "Average is: " << gc_avg_time << " secs, worst is: " << gc_worst_time << endl;
# endif //DISRUPT_STAT


# ifdef   INTEGRITY_CHECK
#  ifdef   DEBUG
  heapDump();
  checkIntegrity();
#  endif //DEBUG
# endif //INTEGRITY_CHECK
  //  newBytesAllocated = 0;
#else  //NO_GC
  DB_OUT(gc_gc, "GarbageCollector::garbageCollect(): Warning, gc is disabled\n");
#endif //NO_GC
#ifdef   LIVESTAT
  //cout << "* LiveStat after gc *\n";
  doLiveStat();
  //  cout << "* LiveStat after gc - done *\n";
#endif //LIVESTAT
  DB_OUT(gc_notify, "*** Garbage collecting done ***\n");
  DB_OUTDENT;
}

void GarbageCollector_t::checkTrainSpaceSetup() {
  DB_INDENT;
  DB_OUT(gc_gc, "GarbageCollector::checkTrainSpaceSetup(): checking if "
	 "there is a need to renumber trains and/or cars\n");
  ttable.renumberTrainsAndCars();

  DB_OUT(gc_gc, "GarbageCollector::checkTrainSpaceSetup(): checking if "
	 "there is a from-car and a last car\n");
  
  //check if the train generation is in a state with an existing fromCar
  //and an existing lastCar in a different train
  Car_t *fromCar = ttable.getFromCar();
  Car_t *lastCar;
  if(!fromCar) {
    trainGeneration.addNewTrainFirst();
    trainGeneration.addNewTrainLast();
    
    //last car must be different from from car
  } else if((lastCar = ttable.getLastTrainLastCar())
	    == fromCar) {
    trainGeneration.addNewTrainLast();
    
    //last car and from car must be in different trains
  } else if(trainGeneration.getCarTrain(fromCar).getTrain() ==
	    trainGeneration.getCarTrain(lastCar).getTrain()) {
    trainGeneration.addNewTrainLast();
  }
  DB_OUTDENT;
}


void GarbageCollector_t::doGC(void){
  DB_INDENT;
  
  DirtyCars_t *dirtyCars = trainGeneration.dirtyCars;
  
#ifdef DEBUG
  if(!dirtyCars->empty()) {
    cerr << "GarbageCollector::doGC(): dirty cars was not empty\n";
    exit(1);
  }
#endif //DEBUG
  
  checkTrainSpaceSetup();

  //***** the actual train algorithm *****

  DB_OUT(gc_gc, "GarbageCollector::doGC(): moving externally referenced "
	 "objects in introductory space\n");

#ifdef INTRO_FIRST
  // old version where introspace is garbage collected before from car
  introSpace.moveObjects();

  //cout << "GarbageCollector::doGC(): dumping dirty cars 3: " << dirtyCars << endl;
  
  DB_OUT(gc_gc, "GarbageCollector::doGC(): moving objects referenced from "
	 "from car\n");

  Car_t *firstCar = ttable.getFromCar();
  firstCar->moveExtObjects();

#else //INTRO_FIRST

  /* new version */
  DB_OUT(gc_gc, "GarbageCollector::doGC(): moving objects referenced from "
	 "from car\n");

  Car_t *firstCar = ttable.getFromCar();
  firstCar->moveExtObjects();

  //scan all the moved objects
  while(!(dirtyCars->empty())) {
    Car_t *dirtyCar = dirtyCars->getFirst();
    DB_OUT(gc_gc, "GarbageCollectort::doGC(): scanning dirty car at: "
	   << (void*)dirtyCar << endl);
    dirtyCar->doGCScan();
    dirtyCars->remove(dirtyCar);
  }
  
  introSpace.moveObjects();

#endif //INTRO_FIRST


  //move the immediate reachable objects from the stacks
  DB_OUT(gc_gc, "GarbageCollector::doGC(): moving objects referenced from "
	 "stack space\n");
  int extRefed = stackSpace.moveObjects();
  
  //cout << "GarbageCollector::doGC(): dumping dirty cars 3: " << dirtyCars << endl;
  
  DB_OUT(gc_gc, "GarbageCollector::doGC(): scanning dirty cars pass one\n");
  
  //scan all the moved objects
  while(!(dirtyCars->empty())) {
    Car_t *dirtyCar = dirtyCars->getFirst();
    DB_OUT(gc_gc, "GarbageCollectort::doGC(): scanning dirty car at: "
	   << (void*)dirtyCar << endl);
    dirtyCar->doGCScan();
    dirtyCars->remove(dirtyCar);
  }
  DB_OUT(gc_gc, "GarbageCollector::doGC(): scanning dirty cars pass one "
	 "- done\n");
  
  //check if the first train is externally referenced
  if(!extRefed) {
    DB_OUT(gc_gc, "GarbageCollector::doGC(): train not ext refed from stack, "
	   "checking external remembered sets\n");
    Car_t *currentCar = firstCar->next();
    for(; currentCar; currentCar = currentCar->next()) {
      DB_OUT(gc_gc, "garbageCollector::doGC(): current car: " << currentCar << endl);
      if(currentCar->isExtRefdTrain()) {
	DB_OUT(gc_gc, "garbageCollector::doGC(): *** car: " << currentCar <<
	       " is externally referenced\n");
	extRefed = 1;
	break;
      }
    }
  }  
  if(!extRefed) {
    //train not ext refed
    //delete all cars
    DB_OUT(gc_gc, "GarbageCollector::doGC(): train is not externally "
	   "referenced, deleting\n");
    trainGeneration.deleteFirstTrain();
  } else {
    //move int refed objects from first car
    firstCar->moveIntObjects();
    while(!(dirtyCars->empty())) {
      Car_t *dirtyCar = dirtyCars->getFirst();
      dirtyCar->doGCScan();
      dirtyCars->remove(dirtyCar);
    }
    //reclaim firstCar
    DB_OUT(gc_gc, "GarbageCollector::doGC(): reclaiming first car\n");
    trainGeneration.deleteFirstTrainsFirstCar();
  }
  //reset introductory space / clear remset and contents
  introSpace.reset();

  DB_OUT(gc_gc, "GarbageCollector::doGC(): done for now\n");

#ifdef undefined
  //reclaim/reset introductory space

#endif //undefined
  DB_OUTDENT;
}

#ifdef LIVESTAT
void GarbageCollector_t::doLiveStat(void) {
  size_t liveIntro = 0, deadIntro = 0, unusedIntro = 0,
         liveTrain = 0, deadTrain = 0, unusedTrain = 0,
         liveStack = 0, deadStack = 0, unusedStack = 0;

  //mark all live objects recursively
  stackSpace.doMarking();

  //run stats
  stackSpace.doLiveStat(&liveStack, &deadStack, &unusedStack);
  introSpace.doLiveStat(&liveIntro, &deadIntro, &unusedIntro);
  ttable.doLiveStat    (&liveTrain, &deadTrain, &unusedTrain);

  //we don't want anything to do with the stack stats, but we
  //clear the fwd ptrs anyway so the integrity check does not get mad

  cout << "\nMEMORY STAT:"
       << " intro: " << liveIntro << " " << deadIntro << " " << unusedIntro
       << " train: " << liveTrain << " " << deadTrain << " " << unusedTrain
       << " stack: " << liveStack << " " << deadStack << " " << unusedStack << endl;
}
#endif //LIVESTAT

#ifdef DISRUPT_STAT
void GarbageCollector_t::printFinalDisruptStat(void) {
  cout << "\ngc's: " << gctimes << " avg. time: " << gc_avg_time
       << " sec, worst: " << gc_worst_time << " sec, total: "
       << gctimes*gc_avg_time << endl;
}
#endif //DISRUPT_STAT

#ifdef DEBUG
void GarbageCollector_t::heapDump(void){
  DB_INDENT;
  DB_OUT(gc_heapdump, "GarbageCollector::heapDump(): *** dumping heap ***\n" << endl);
  DB_OUT(gc_heapdump, stackSpace << endl);
  DB_OUT(gc_heapdump, introSpace << endl);
  DB_OUT(gc_heapdump, ttable << endl);
  DB_OUT(gc_heapdump, "GarbageCollector::heapDump(): *** done dumping heap ***\n" << endl);
  DB_OUTDENT;
}

void GarbageCollector_t::checkIntegrity(void){

  DB_INDENT;
  DB_OUT(gc_integrity, "GarbageCollector::checkIntegrity(): *** checkIntegrity started ***\n");
  int errors = 0;

  VMOHash_t *validObjRefs = new VMOHash_t();

  //build array of valid object references
  DB_OUT(gc_integrity, "GarbageCollector::checkIntegrity: gathering all"
	 " valid vmo's\n");
  introSpace.gatherObjRefs(validObjRefs);
  stackSpace.gatherObjRefs(validObjRefs);
  ttable.gatherObjRefs(validObjRefs);

  //check references
  DB_OUT(gc_integrity, "GarbageCollector::checkIntegrity: checking "
	 "references in all objects\n");
  errors += introSpace.checkObjRefs(validObjRefs);
  errors += stackSpace.checkObjRefs(validObjRefs);
  errors += ttable.checkObjRefs(validObjRefs);

  delete(validObjRefs);

  if(errors) {
    cerr << "GarbageCollector_t::checkIntegrity(): *** found " << errors <<
      " error(s), bailing out! ***\n";
    exit(1);
  }
  DB_OUT(gc_integrity, "GarbageCollector::checkIntegrity: *** no errors found! ***\n");
  DB_OUTDENT;
}

#endif

