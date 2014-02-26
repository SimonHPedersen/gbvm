// $Id: IntroductorySpace.cc,v 1.37 2001/06/08 10:26:23 ilsoe Exp $

#include "gc_new.hpp"


//----- operators -----

ostream& operator << (ostream& stream, IntroductorySpace_t* is) {
  if(is) {
    is->printOn(stream);
  } else {
    stream << "*** tried to print a null-introductoryspace ***\n";
  }
  return stream;
}

ostream& operator << (ostream& stream, IntroductorySpace_t& is) {
  is.printOn(stream);
  return stream;
}

//------ member functions -------

IntroductorySpace_t::IntroductorySpace_t() {
  //DB_OUT(gc_introSp, "IntroductorySpace(" << bytes << ")\n");
  cout << "IntroductorySpace(" << BYTES_IN_INTROSP << ")\n";
#ifdef DEBUG
  if(BYTES_IN_INTROSP <= 0) {
    cerr << "IntroductorySpace_t(): Error: was given zero or negative bytes\n";
    exit(1);
  }
#endif
  if(!(introSpacePtr = (void**)memalign(BYTES_IN_INTROSP, BYTES_IN_INTROSP))) {
    cerr << "IntroductorySpace_t(): Error: unable to allocate memory for"
      " introductorys\n";
    exit(1);
  }
  cout << "Introductory space at: " << (void*)introSpacePtr << endl;
  remSet = new RememberedSet_t();
  freePtr = introSpacePtr;
  //  introSpaceSize = bytes;
}

IntroductorySpace_t::~IntroductorySpace_t() {
  free(introSpacePtr);
  delete(remSet);
}

/*
bool IntroductorySpace_t::member(void *address) {
  return (introSpacePtr == (void*)((unsigned int)address & INTROSP_MASK));
}
*/

VMObject_t * IntroductorySpace_t::allocVMO(ObjectDescriptor_t *objDesc) {
  DB_INDENT;
#ifdef DEBUG
  if(!objDesc) {
    cerr << "IntroductorySpace_t::allocVMO(): objdesc was null\n";
    exit(1);
  }
#endif

  size_t freeSpace = (size_t)introSpacePtr + BYTES_IN_INTROSP
    - (size_t)freePtr;
  size_t reqSpace = sizeof(VMObject_t) - sizeof(void*)
    + objDesc->getObjLengthBytes();
  if(freeSpace < reqSpace) {
    DB_OUT(gc_introSp, "IntroductorySpace_t::allocVMO(): cannot allocate"
	   " this object in introductoryspace, " << "free space: " <<
	   freeSpace << ", required space: " << reqSpace << endl);
    DB_OUTDENT;
    return 0;
  }
  void *objLoc = freePtr;
  freePtr = (void*)((size_t)freePtr + reqSpace);
  VMObject_t *vmo = new(objLoc, objDesc)VMObject_t();
  DB_OUTDENT;
  return vmo;
}

void IntroductorySpace_t::moveObjects() {
  DB_INDENT;
  Car_t *destCar, *fromCar  = ttable.getFromCar();
  CarTrain_t fromCarTrain = trainGeneration.getCarTrain(fromCar);
  unsigned int fromTrainNum = fromCarTrain.getTrain();
#ifdef FIRST_TRAIN_REFED_TO_2ND_TRAIN
  Car_t *secTrLCar = ttable.getLastCar(fromTrainNum + 1);
#endif //FIRST_TRAIN_REFED_TO_2ND_TRAIN

  for(remSetIt_t i = remSet->getIterator(), endIt = remSet->getEndIterator(); i != endIt; ++i) {
    if(member(**i)) { //the reference should still point to us
      // *i  is the value, that is the address of the reference
      //     (pointer to the slot which contains the pointer to the object to move)
      // **i is the reference (pointer to the object to move)

      destCar = Car_t::getCar(*i); //object should be moved to where it's referenced from
      if(trainGeneration.getCarTrain(destCar).getTrain() == fromTrainNum) {
	DB_OUT(gc_introSp, "IntroductorySpace::moveObjects(): caught attempt to move new objects into first train\n");
#ifdef FIRST_TRAIN_REFED_TO_2ND_TRAIN
	destCar = secTrLCar;
#else  //FIRST_TRAIN_REFED_TO_2ND_TRAIN
	destCar = ttable.getLastTrainLastCar();
#endif //FIRST_TRAIN_REFED_TO_2ND_TRAIN
      }
      
      void * newLoc = destCar->moveObj(**i); //this handles forwardPtrs and dirtycars
      
      //note: the first parameter here should have been the reference to the object, but
      //in this version of the gc, we don't use the source pointer...
      setVMReferenceGC(*i , *i, newLoc);
      //also handles remembered set. We HAVE to do this, since we don't know if it is a forward
      // pointer and the object thus have been moved to an older car in which case the remembered
      //set has to be updated
      
#ifdef UNDEFINED
      //------------ alternative version, perhaps quicker ---------------
      //does not yet handle first train problem
      
      void* fwdPtr = VMO_FROM_VOIDP(**i)->getForwardPtr();
      if(fwdPtr) {
	setVMReferenceGC(*i, *i, fwdPtr);
      } else {
	destCar = Car_t::getCar(*i); //object should be moved to where it's referenced from
	void * newLoc = destCar->moveObj(**i); //this handles forwardPtrs and dirtycars
	**i = newLoc;
      }
#endif //UNDEFINED
    }
  }
    
  DB_OUTDENT;
}

void IntroductorySpace_t::reset(void) {
  DB_INDENT;
#ifdef DEBUG
  for(VMObject_t *vmo = (VMObject_t*)introSpacePtr;
      vmo < freePtr;
      vmo = NEXT_VMO(vmo)) {
    if(vmo->isForward()) {
      DB_OUT(gc_gc, "IntroductorySapce::reset() * deleted object " << vmo->getID() << " *\n");
    }
  }
  
#endif //DEBUG

  freePtr = introSpacePtr;
  /* VMObjects should clear their space themselves - no need to do that twice

    //clear space, assuming BYTES_IN_INTROSP % sizeof(void*) == 0
    void * endSpacePtr = (void*)((size_t)freePtr + BYTES_IN_INTROSP);
    for(void **clearPtr = (void**)freePtr; clearPtr < endSpacePtr;
        clearPtr = (void**)((size_t)clearPtr + sizeof(void*))) {
      *clearPtr = 0;
    }
  */

  remSet->clear();

#ifdef DEBUG
  if(!(remSet->empty())) {
    cerr << "IntroductorySpace_t::reset(): remembered set was not clered\n";
    exit(1);
  }
#endif //DEBUG
  DB_OUTDENT;
}

void IntroductorySpace_t::printOn(ostream& stream){
  DB_INDENT;
  DB_INS_INDT(stream);
  stream << "--- IntroductorySpace at " << (void*)introSpacePtr;
  dumpBlock(introSpacePtr, (size_t)((size_t)freePtr - (size_t)introSpacePtr), stream);
#ifdef DEBUG
  if(DB_ENABLED(gc_remset_intro_dump)) {
    DB_OUT(gc_remset_intro_dump, "Introductory Space RememberedSet: " << endl); 
    remSet->dumpRemSet(introSpacePtr, (size_t)freePtr - (size_t)introSpacePtr, stream);
  }
#endif //DEBUG
  DB_OUTDENT;
}


#ifdef LIVESTAT
void IntroductorySpace_t::doLiveStat(size_t *live, size_t *dead, size_t *unused) {
  size_t liveObjects = 0, deadObjects = 0, liveBytes = 0, deadBytes = 0;

  for(VMObject_t *vmoIndex = (VMObject_t*)introSpacePtr;
      vmoIndex < freePtr; vmoIndex = NEXT_VMO(vmoIndex)) {
    ObjectDescriptor_t *objDesc = vmoIndex->getObjectDescriptor();
    if(vmoIndex->isForward()) {
      //a live object
      vmoIndex->setForwardBit(false);
      ++liveObjects;
      liveBytes += objDesc->getObjLengthBytes();
    } else {
      //a dead object
      ++deadObjects;
      deadBytes += objDesc->getObjLengthBytes();
    }
  }
#ifdef LIVESTAT_VERB
  cout << "Livestat for: introductory-space" << endl
       << "  Live/dead objects: " << liveObjects << "/" << deadObjects << endl
       << "  Live/dead bytes  : " << liveBytes << "/" << deadBytes << endl
       << "   -in vmo bytes   : " << liveBytes + liveObjects*(sizeof(VMObject_t) - sizeof(void*))
       << "/" << deadBytes + deadObjects*(sizeof(VMObject_t) - sizeof(void*)) << endl;
#endif //LIVESTAT_VERB

  size_t liveBytesH = liveBytes + liveObjects*(sizeof(VMObject_t) - sizeof(void*)); //includes headers
  size_t deadBytesH = deadBytes + deadObjects*(sizeof(VMObject_t) - sizeof(void*)); //includes headers
  *live   += liveBytesH;
  *dead   += deadBytesH;
  *unused += BYTES_IN_INTROSP - (liveBytesH + deadBytesH);
}

#endif //LIVESTAT


#ifdef DEBUG

bool IntroductorySpace_t::hasRememberReference(void** refAdr) {
  return remSet->lookup(refAdr);
}

void IntroductorySpace_t::gatherObjRefs(VMOHash_t *vmoRefs) {
  DB_INDENT;
  VMObject_t *vmo = (VMObject_t*)introSpacePtr;
  while(vmo < freePtr) {
    vmoRefs->insert(vmo);
    vmo = NEXT_VMO(vmo);
  }
  DB_OUTDENT;
}

int IntroductorySpace_t::checkObjRefs(VMOHash_t *vmoRefs) {
  DB_INDENT;
  int retval = 0;
  VMObject_t *vmo = (VMObject_t*)introSpacePtr;
  ObjectDescriptor_t *objDesc;

  //check all objects in car
  while(vmo < freePtr) {
    if(!(objDesc = vmo->getObjectDescriptor())) {
      cerr << "IntroductorySpace_t::checkObjRefs(): found object " << vmo->getID() << 
	", vmo at: " << (void*)vmo << " without ObjDesc!" << endl;
      cerr << "Train was: " /*<< trainRef.getTrainNumber()*/ << " and car was: "
	   << /*carNumber <<*/ endl;
      retval++;
      DB_OUTDENT;
      return retval;
    }
    if(void * forward = vmo->getForwardPtr()) {
      cerr << "IntroductorySpace::checkObjRefs(): found object " << vmo->getID() << 
	", vmo at: " << (void*)vmo << " with forward pointer set to:" <<
	forward << endl;
      retval++;
    }
    unsigned int numberOfRefs = objDesc->getObjLengthRefs();
    //check each reference
    for(unsigned int refInVMO = 0; refInVMO < numberOfRefs; refInVMO++) {
      if(objDesc->hasReferenceAt(refInVMO) && vmo->data[refInVMO]) {
	VMObject_t *vmoRef = VMO_FROM_VOIDP(vmo->data[refInVMO]);
	//check if reference is in list
	if(!(vmoRefs->lookup(vmoRef))) {
	  cerr << "IntroductorySpace_t::checkObjRefs(): found object " << vmo->getID() <<
	    ", vmo at: " << (void*)vmo << " with illegal object pointer" << endl;
	  cerr << "Object descriptor: " << endl << vmo->getObjectDescriptor() << endl;
	  //	  cerr << "Train was: " /*<< trainRef.getTrainNumber()*/ << " and car was: "
	  //	       << /*carNumber <<*/ endl;
	  cerr << "Bad reference was number: " << refInVMO << " with value: "
	       << (void*)(vmo->data[refInVMO]) << endl;
	  retval++;
	}
      }
    }
    vmo = NEXT_VMO(vmo);
  }
  DB_OUTDENT;
  return retval;
}


#endif //DEBUG
