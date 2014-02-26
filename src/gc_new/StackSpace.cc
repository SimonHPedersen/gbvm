// $Id: StackSpace.cc,v 1.23 2001/06/08 10:26:24 ilsoe Exp $

#include "gc_new.hpp"

//----- operators -----

ostream& operator << (ostream& stream, StackSpace_t* ss) {
  if(ss) {
    ss->printOn(stream);
  } else {
    stream << "*** tried to print a null-stackspace ***\n";
  }
  return stream;
}

ostream& operator << (ostream& stream, StackSpace_t& ss) {
  ss.printOn(stream);
  return stream;
}

//------ member functions ------

StackSpace_t::StackSpace_t(size_t bytes) {
#ifdef DEBUG
  if(bytes <= 0) {
    cerr << "StackSpace_t(): Error: was given zero or negative bytes\n";
    exit(1);
  }
#endif
  if(!(stackSpacePtr = (void**)malloc(bytes))) {
    cerr << "StackSpace_t(): Error: unable to allocate memory for"
      " stacks\n";
    exit(1);
  }
  cout << "StackSpace(): allocated stackspace at: " << stackSpacePtr << endl;

  freePtr = stackSpacePtr;
  stackSpaceSize = bytes;
}

StackSpace_t::~StackSpace_t() {
  free(stackSpacePtr);
}

#ifdef undefined
bool StackSpace_t::member(void *address) {
  if(address < stackSpacePtr) {
    return false;
  }
  if(((size_t)address) >= ((size_t)stackSpacePtr + stackSpaceSize)) {
    return false;
  }
  return true;
}
#endif undefined

VMObject_t * StackSpace_t::allocStackVMO(ObjectDescriptor_t *objDesc) {
#ifdef DEBUG
  if(!objDesc) {
    cerr << "StackSpace_t::allocStackSpace(): objdesc was null\n";
    exit(1);
  }
#endif

  size_t freeSpace = (size_t)stackSpacePtr + stackSpaceSize - (size_t)freePtr;
  size_t reqSpace = sizeof(VMObject_t) - sizeof(void*)
    + objDesc->getObjLengthBytes();
  if(freeSpace < reqSpace) {
    cerr << "StackSpace_t::allocStackSpace(): cannot allocate more"
      " stackspace, please fix if this is a problem ;)\n";
    exit(1);
  }
  void *objSpace = freePtr;
  freePtr = (void*)((size_t)freePtr + reqSpace);

  DB_OUT(gc_stackSp, "StackSpace::allocStackSpace(): new object at: "
	 << objSpace << " new freePtr: " << freePtr << endl);

  return new(objSpace, objDesc)VMObject_t();
}

int StackSpace_t::moveObjects() {
  DB_INDENT;
  int extRefed = 0; //used for checking if there are external references
                    //from the stack to other objects inside the first train
  bool haveAddedCar = false; //only add one new car for each session

  Car_t *fromCar = ttable.getFromCar();

  DB_OUT(gc_stackSp, "StackSpace::moveObjects(): fromcar at: " << (void*)fromCar << endl);

  VMObject_t *scan = (VMObject_t*)stackSpacePtr;
  void **scanObj;

  for(;((void*)scan) < freePtr; scan = NEXT_VMO(scan)) { //scan stack objects
    scanObj = (void**)VOIDP_FROM_VMO(scan);

    //DB_OUT(gc_stackSp, "StackSpace::moveObjects(): scan at: " << (void*)scan
    //	   << " freePtr at: " << freePtr << endl);

    ObjectDescriptor_t *objDesc = scan->getObjectDescriptor();
    unsigned int       *refMask = objDesc->getReferenceMask();
    unsigned int   refsInObject = objDesc->getObjLengthRefs();
    unsigned int     intsInMask = (refsInObject/(sizeof(unsigned int)*8))+1;
    int              bits2Check = sizeof(unsigned int)*8;
    void **objRefPtr; //pointer to the location where the references in
                      //the objects are


    //i counts integers in mask
    //j counts the bits in each mask

    for(unsigned int i = 0; i < intsInMask; i++){
      int mask = refMask[i];
      if(mask) {
	if(i == (intsInMask -1)) {
	  bits2Check = refsInObject - (intsInMask - 1) * sizeof(unsigned int)*8;
	}
	for(int j = 0; j < bits2Check; j++){
	  if((1<<j) & mask){	    //we've got a reference according to the mask
	    objRefPtr = &scanObj[i * sizeof(int) * 8 + j];

	    if(*objRefPtr) {	    //we've got a reference which is set

	      if((introSpace.member(*objRefPtr) || fromCar->member(*objRefPtr))) {
		//The reference refers to an object that is in the area being gc'ed.
		void *fwdPtr = VMO_FROM_VOIDP(*objRefPtr)->getForwardPtr();
		if(fwdPtr) {
		  DB_OUT(gc_stackSp, "StackSpace::moveObjects(): found fwd ptr\n");
		  *objRefPtr = fwdPtr;
		} else {
#ifdef NEW_STACK_RESCUE
		  Car_t *destCar = ttable.getLastTrainLastCar();
		  if(haveAddedCar) {
		    *objRefPtr = destCar->moveObj(*objRefPtr);
		  } else {
		    Car_t *lastTrFCar = ttable.getLastTrainFirstCar();
		    if(destCar == lastTrFCar) {
		      void *tmpObjRefPtr;
		      if((tmpObjRefPtr = destCar->moveObjHere(*objRefPtr))) {
			*objRefPtr = tmpObjRefPtr;
		      } else {
			trainGeneration.addNewTrainLast();
			haveAddedCar = true;
			destCar = ttable.getLastTrainLastCar();
			*objRefPtr = destCar->moveObj(*objRefPtr);
		      }
		    } else {
		      destCar = ttable.getLastTrainLastCar();
		      *objRefPtr = destCar->moveObj(*objRefPtr);		      
		    }
		  }
#else //NEW_STACK_RESCUE
		  if(!haveAddedCar) {  
		    trainGeneration.addNewTrainLast();
		    haveAddedCar = true;
		  }
		  Car_t *destCar = ttable.getLastTrainLastCar();
		  DB_OUT(gc_stackSp, "StackSpace::moveObjects(): found "
			 "interesting ref from stack: " << *objRefPtr <<
			 " destCar will be at: " << (void*)destCar << endl);
		  *objRefPtr = destCar->moveObj(*objRefPtr);
#endif //NEW_STACK_RESCUE
		}
	      } else {
		//Even though the referenced object does not have to be
		//moved, we still check if it is a reference to the first
		//train. If no ext refs are found, we know that we can
		//throw the train away afterwards.
		if(!extRefed) {
		  //check if reference was into the first train
		  for(Car_t *tmpCar = fromCar->next(); tmpCar; tmpCar = tmpCar->next()) {
		    DB_OUT(gc_stackSp, "StackSpace::moveObjects(): searching"
			   " for ref to train in car at: " << (void*)tmpCar << "\n");
		    if(tmpCar->member(*objRefPtr)) {
		      DB_OUT(gc_stackSp, "StackSpace::moveObjects(): "
			     "found ext ref\n");
		      extRefed = 1;
		      break;
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
  DB_OUTDENT;
  return extRefed;
}

void StackSpace_t::printOn(ostream& stream){
  DB_INDENT;
  DB_INS_INDT(stream);
  stream << "--- StackSpace at " << (void*)this;
  dumpBlock(stackSpacePtr, (size_t)((size_t)freePtr - (size_t)stackSpacePtr), stream);
  DB_OUTDENT;
}

#ifdef LIVESTAT
void StackSpace_t::doMarking(void) {
  VMObject_t *vmo = (VMObject_t*)stackSpacePtr;
  while(vmo < freePtr) {
    //DB_OUT(gc_integrity, "VMObject at: " << (void*)vmo << endl);
    vmo->mark();
    //get next object
    vmo = NEXT_VMO(vmo);
  }  
}

void StackSpace_t::doLiveStat(size_t *live, size_t *dead, size_t *unused) {
  size_t liveObjects = 0, deadObjects = 0, liveBytes = 0, deadBytes = 0;
  for(VMObject_t *vmoIndex = (VMObject_t*)stackSpacePtr;
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
  cout << "Livestat for: stack/root-space" << endl
       << "  Live/dead objects: " << liveObjects << "/" << deadObjects << endl
       << "  Live/dead bytes  : " << liveBytes << "/" << deadBytes << endl
       << "   -in vmo bytes   : " << liveBytes + liveObjects*(sizeof(VMObject_t) - sizeof(void*))
       << "/" << deadBytes + deadObjects*(sizeof(VMObject_t) - sizeof(void*)) << endl;
#endif //LIVESTAT_VERB

  size_t liveBytesH = liveBytes + liveObjects*(sizeof(VMObject_t) - sizeof(void*)); //includes headers
  size_t deadBytesH = deadBytes + deadObjects*(sizeof(VMObject_t) - sizeof(void*)); //includes headers
  *live   += liveBytesH;
  *dead   += deadBytesH;
  *unused += stackSpaceSize - (liveBytesH + deadBytesH);
}

#endif //LIVESTAT

#ifdef DEBUG

void StackSpace_t::gatherObjRefs(VMOHash_t *vmoRefs) {
  DB_INDENT;
  VMObject_t *vmo = (VMObject_t*)stackSpacePtr;
  while(vmo < freePtr) {
    //DB_OUT(gc_integrity, "VMObject at: " << (void*)vmo << endl);
    //    *vmoRefArray = vmo; /* + sizeof(VMObject_t) - sizeof(void*); //ref to internal object...*/
    //    vmoRefArray++;
    vmoRefs->insert(vmo);
    //get next object
    vmo = NEXT_VMO(vmo);
  }
  DB_OUTDENT;
}

int StackSpace_t::checkObjRefs(VMOHash_t *vmoRefs) {
  DB_INDENT;
  int retval = 0;
  VMObject_t *vmo = (VMObject_t*)stackSpacePtr;
  ObjectDescriptor_t *objDesc;

  //check all objects in stack space
  while(vmo < freePtr) {
    if(!(objDesc = vmo->getObjectDescriptor())) {
      cerr << "StackSpace_t::checkObjRefs(): found object " << vmo->getID() << 
	", vmo at: " << (void*)vmo << " without ObjDesc!" << endl;
      cerr << "Train was: " /*<< trainRef.getTrainNumber()*/ << " and car was: "
	   << /*carNumber <<*/ endl;
      retval++;
      DB_OUTDENT;
      return retval;
    }
    if(void * forward = vmo->getForwardPtr()) {
      cerr << "StackSpace::checkObjRefs(): found object " << vmo->getID() << 
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
	  cerr << "StackSpace_t::checkObjRefs(): found object " << vmo->getID() <<
	    ", vmo at: " << (void*)vmo << " with illegal object pointer" << endl;
	  cerr << "Object descriptor: " << endl << vmo->getObjectDescriptor() << endl;
	  cerr << "Train was: " /*<< trainRef.getTrainNumber()*/ << " and car was: "
	       << /*carNumber <<*/ endl;
	  cerr << "Bad reference was number: " << refInVMO << " with value: "
	       << (void*)(vmo->data[refInVMO]) << endl;
	  cerr << "Object with illegal pointer has typeid: "
	       << /*typeid(VOIDP_FROM_VMO(vmo)).name() <<*/ endl;
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
