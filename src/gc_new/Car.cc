//$Id: Car.cc,v 1.45 2001/06/08 10:26:23 ilsoe Exp $

//#include "Car.hpp"
#include "gc_new.hpp"

//----- operators -----

ostream& operator << (ostream& stream, Car_t* car) {
  if(car) {
    car->printOn(stream);
  } else {
    stream << "*** tried to print a null-car ***\n";
  }
  return stream;
}

ostream& operator << (ostream& stream, Car_t& car) {
  car.printOn(stream);
  return stream;
}

// ----- constructors/destructors -----

Car_t::Car_t():nextCar(0) {
  freePtr = scanPtr = data;
  extRemSet = new RememberedSet_t();
  intRemSet = new RememberedSet_t();
  cout << "Car_t::Car_t default constr run\n";
}

Car_t::Car_t(unsigned int train, unsigned int car):
  nextCar(0) {
  freePtr = scanPtr = data;
  extRemSet = new RememberedSet_t();
  intRemSet = new RememberedSet_t();
  
  trainGeneration.setCarTrain(this, train, car);
  if(car == 0) {
    ttable.addTrain(this, this);
  } else {
    ttable.setLastCar(train, this);
  }
}

Car_t::~Car_t(){
#ifdef DEBUG
  DB_INDENT;
  for(VMObject_t *vmo = (VMObject_t*)data;
      vmo < freePtr;
      vmo = NEXT_VMO(vmo)) {
    if(!vmo->isForward()) {
      DB_OUT(gc_gc, "~Car_t(): * deleted object " << vmo->getID() << " *\n");
    }
  }
  DB_OUTDENT;
#endif //DEBUG
  delete(extRemSet);
  delete(intRemSet);
}

// ----- member functions -----

VMObject_t *Car_t::allocateVMO(ObjectDescriptor_t *objDesc) {
  DB_INDENT;


  //DB_OUT(gc_car, "Car_t::allocateVMO(): available space: " << availSpace()
  //	 << endl);

  size_t objSize = objDesc->getObjLengthBytes() + sizeof(VMObject_t)
    - sizeof(void*);
  //availSpace();
  if(objSize > availSpace()) {
    // not enough space
    DB_OUT(gc_car, "Car_t::allocateVMO(): out of space\n");
    DB_OUTDENT;
    return 0;
  }

  void *newDataPtr = freePtr;
  freePtr = (void*)((size_t)freePtr + (size_t)objSize);
  VMObject_t *vmo = new(newDataPtr, objDesc)VMObject_t();
  DB_OUTDENT;

  return vmo;
}

bool Car_t::member(void *address) {
  return (getCar(address) == this);
  /*
  return ((size_t)address < (size_t)data
	  || (size_t)address >= ((size_t)this + BYTES_IN_CAR)) ?
    false : true;
  */
}

void * Car_t::moveObj(void *object) {
  void * newLoc = copyObj(object);
  if(newLoc) {
    return newLoc;
  }
  //ok, we have to find another destination
  CarTrain_t thisCarTrain = trainGeneration.getCarTrain(this);
  Car_t *lastCar = ttable.getLastCar(thisCarTrain.getTrain());
  if(lastCar != this) {
    newLoc = lastCar->copyObj(object);
    if(newLoc) {
      return newLoc;
    }
  }
  CarTrain_t lastCarInTrain = trainGeneration.getCarTrain(lastCar);
  void *carSpace = trainGeneration.allocCarSpace();
  Car_t *newCar = new(carSpace)Car_t(lastCarInTrain.getTrain(),
				     lastCarInTrain.getCar() + 1);
  lastCar->setNext(newCar);

  newLoc = newCar->copyObj(object);
  if(newLoc) {
    return newLoc;
  } else {
    cerr << "Car::moveObj(): object was too big to fit inside car\n";
    exit(1);
  }
}

#ifdef NEW_STACK_RESCUE
void * Car_t::moveObjHere(void *object) {
  VMObject_t *vmo = VMO_FROM_VOIDP(object);

  void * forward = vmo->getForwardPtr();
  if(forward) {
    DB_OUT(gc_car, "Car::copyObj(): returning forward ptr: " << forward << endl);
    return forward;
  }

  size_t objectSize = vmo->getFullVMOLength();

  if((objectSize + CAR_FILL_THR_BYTES) > availSpace()) {
    //not enough space
    return 0;
  }

  //copy the object
  memcpy(freePtr, vmo, objectSize);
  
  forward = VOIDP_FROM_VMO(freePtr);
  vmo->setForwardPtr(forward);
  freePtr = (void*)(((size_t)freePtr) + objectSize);

  //update dirty cars
  trainGeneration.dirtyCars->insert(this);
  return forward;
}
#endif //NEW_STACK_RESCUE

void * Car_t::copyObj(void *object) {
  VMObject_t *vmo = VMO_FROM_VOIDP(object);

  void * forward = vmo->getForwardPtr();
  if(forward) {
    DB_OUT(gc_car, "Car::copyObj(): returning forward ptr: " << forward << endl);
    return forward;
  }

  size_t objectSize = vmo->getFullVMOLength();

  if(objectSize > availSpace()) {
    //not enough space
    return 0;
  }

  //copy the object
  memcpy(freePtr, vmo, objectSize);
  
  forward = VOIDP_FROM_VMO(freePtr);
  vmo->setForwardPtr(forward);
  freePtr = (void*)(((size_t)freePtr) + objectSize);

  //update dirty cars
  trainGeneration.dirtyCars->insert(this);
  return forward;
}

void Car_t::doGCScan(void) {
  //NOTE: THIS CODE IS VERY SIMILAR TO THE CODE IN StackSpace::moveObjects()

  DB_INDENT;
  Car_t *fromCar = ttable.getFromCar();

  //VMObject_t *scan = (VMObject_t*)stackSpacePtr;
  void **scanObj;
  for(;scanPtr < freePtr; scanPtr = NEXT_VMO((VMObject_t*)scanPtr)) { //scan stack objects
    scanObj = (void**)VOIDP_FROM_VMO((VMObject_t*)scanPtr);

    DB_OUT(gc_car_gcsc, "Car_t::doGCScan(): scanPtr at: " << scanPtr
	   << " freePtr at: " << freePtr << endl);
    ObjectDescriptor_t *objDesc = ((VMObject_t*)scanPtr)->getObjectDescriptor();
    unsigned int *refMask = objDesc->getReferenceMask();
    unsigned int refsInObject = objDesc->getObjLengthRefs();
    unsigned int intsInMask = (refsInObject/(sizeof(unsigned int)*8))+1;
    int bits2Check = sizeof(unsigned int)*8;
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
	  if((1<<j) & mask){
	    //we've got a reference, let's find it
	    objRefPtr = &scanObj[i * sizeof(int) * 8 + j];
	    if(*objRefPtr) {
	      //ok, we've got a reference
	      if(introSpace.member(*objRefPtr) || fromCar->member(*objRefPtr)) {
		DB_OUT(gc_car_gcsc, "Car_t::doGCScan(): found "
		       "interesting ref from scanned obj: " << objRefPtr
		       << endl);
		//*objRefPtr = moveObj(*objRefPtr);
		void *newLoc = moveObj(*objRefPtr);
		setVMReferenceGC(objRefPtr, objRefPtr, newLoc);
	      } else {
		DB_OUT(gc_car_gcsc, "Car_t::doGCScan(): found "
		       "ref to nonmoved object: " << objRefPtr
		       << endl);
		if(!stackSpace.member(*objRefPtr)) {
		  DB_OUT(gc_car_gcsc, "Car_t::doGCScan(): was not from stackspace***\n");
		  updateRemRefGC(objRefPtr, *objRefPtr);
		} else {
		  DB_OUT(gc_car_gcsc, "Car_t::doGCScan(): was from stackspace***\n");
		}
		  
	      }
	    }
	  }
	}
      }
    }
  }
  DB_OUT(gc_car_gcsc, "Car_t::doGCScan(): leaving car\n");

#ifdef DEBUG
  if(scanPtr != freePtr) {
    cerr << "Car_t::doGCScan(): ERROR: scanPtr and freePtr different"
      " after scan\n";
    exit(1);
  }
#endif //debug
  DB_OUTDENT;
}

void Car_t::printOn(ostream& stream) {
  DB_INDENT;
  DB_INS_INDT(stream);
  stream << "--- Car " << trainGeneration.getCarTrain(((void*)this)).getCar() 
	 << " at " << (void*)this << " with train " 
	 << trainGeneration.getCarTrain(((void*)this)).getTrain(); 
  dumpBlock(data, (size_t)((size_t)freePtr - (size_t)data), stream);
  stream << endl;
#ifdef DEBUG
  if(DB_ENABLED(gc_remset_ext_dump)) {
    DB_OUT(gc_remset_ext_dump, "Train-external RememberedSet: " << endl); 
    extRemSet->dumpRemSet(data, (size_t)freePtr - (size_t)data, stream);
  }
  if(DB_ENABLED(gc_remset_int_dump)) {
    DB_OUT(gc_remset_int_dump, "Train-internal RememberedSet: " << endl);
    intRemSet->dumpRemSet(data, (size_t)freePtr - (size_t)data, stream);
  }
#endif //DEBUG
  DB_OUTDENT;
}

void Car_t::moveExtObjects(void) {
  //NOTE: only suitable for being used on the from-car!!!
  DB_INDENT;
  Car_t * destCar;
#ifdef DEBUG
  if(this != ttable.getFromCar()) {
    cerr << "Car_t::moveExtObjects(): tried to run moveObjects on car which"
      " was not first car in first train\n";
    exit(1);
  }
#endif //DEBUG
  //External remember references
  for(remSetIt_t i = extRemSet->getIterator(), endIt = extRemSet->getEndIterator(); i != endIt; ++i) {
    DB_OUT(gc_car, "Car::moveExtObjects(): external reference\n");
    if(member(**i)) {  //the reference should still point to us
      // *i  is the value, that is the address of the reference
      //     (pointer to the object which contains the pointer to the object to move)
      // **i is the reference (pointer to the object to move)
      
      DB_OUT(gc_car, "Car::moveExtObjects(): found valid external remember reference to:"
	   << **i << endl);
      
      destCar = Car_t::getCar(*i); //object should be moved to where it's referenced from
      //      if(trainGeneration.getCarTrain(destCar).getTrain() == fromCarTrain.getTrain()) {
      //	DB_OUT(gc_introSp, "IntroductorySpace::moveExtObjects(): caught attempt to move new objects into first train\n");
      //	destCar = ttable.getLastTrainLastCar();
      //      }
      
      void * newLoc = destCar->moveObj(**i); //this handles forwardPtrs and dirtycars
      
      //note: the first parameter here should have been the reference to the object, but
      //in this version of the gc, we don't use the source pointer...
      setVMReferenceGC(*i , *i, newLoc);
      //also handles remembered set. We HAVE to do this, since we don't know if it is a forward
      // pointer and the object thus have been moved to an older car in which case the remembered
      //set has to be updated
    }
  }
  DB_OUTDENT
}

void Car_t::moveIntObjects(void) {
  //NOTE: only suitable for being used on the from-car!!!
  DB_INDENT;
  Car_t * destCar;
#ifdef DEBUG
  if(this != ttable.getFromCar()) {
    cerr << "Car_t::moveIntObjects(): tried to run moveObjects on car which"
      " was not first car in first train\n";
    exit(1);
  }
#endif //DEBUG
  //Internal remember references
  for(remSetIt_t i = intRemSet->getIterator(), endIt = intRemSet->getEndIterator(); i != endIt; ++i) {
    DB_OUT(gc_car, "Car::moveIntObjects(): internal reference\n");
    if(member(**i)) {  //the reference should still point to us
      DB_OUT(gc_car, "Car::moveIntObjects(): found valid internal remember reference to:"
	     << **i << endl);
      destCar = Car_t::getCar(*i); //object should be moved to where it's referenced from
      void * newLoc = destCar->moveObj(**i); //this handles forwardPtrs and dirtycars
      setVMReferenceGC(*i , *i, newLoc);
    }
  }
  DB_OUT(gc_car, "Car::moveIntObjects(): returning\n");
  DB_OUTDENT
}


bool Car_t::isExtRefdTrain() {
  if(extRemSet->empty()){
    return false;
  } else {
    remSetIt_t it = extRemSet->getIterator();
    for( ; it != extRemSet->getEndIterator() ; ++it){
      if(member(**it)){
	return true;
      }
    }
  }
  return false;
}

/*
void Car_t::addExtRememberReference(void** refAdr){
  extRemSet->insert(refAdr);
}


void Car_t::addIntRememberReference(void** refAdr){
  intRemSet->insert(refAdr);
}
*/

#ifdef DEBUG

bool Car_t::hasExtRememberReference(void** refAdr) {
  return extRemSet->lookup(refAdr);
}

bool Car_t::hasIntRememberReference(void** refAdr) {
  return intRemSet->lookup(refAdr);
}

void Car_t::gatherObjRefs(VMOHash_t *vmoRefs) {
  DB_INDENT;
  VMObject_t *vmo = (VMObject_t*)&data[0];
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

int Car_t::checkObjRefs(VMOHash_t *vmoRefs) {
  DB_INDENT;
  int retval = 0;
  VMObject_t *vmo = (VMObject_t*)&data[0];
  ObjectDescriptor_t *objDesc;

  //check that car numbers are ok
  CarTrain_t thisCarTrain = trainGeneration.getCarTrain(this);
  if(nextCar) {
    CarTrain_t nextCarTrain = trainGeneration.getCarTrain(nextCar);
    if(thisCarTrain.getTrain() != nextCarTrain.getTrain()) {
      cerr << "Car_t::checkObjRefs(): trains were different for cars in the same train...\n";
      retval++;
    }
    if((thisCarTrain.getCar() + 1) != nextCarTrain.getCar()) {
      cerr << "Car_t::checkObjRefs(): found bad car numbering in train: "
	   << thisCarTrain.getTrain() << " bad cars were at: "
	   << (void*)this << " and " << (void*)nextCar <<endl;
      retval++;
    }
  }

  //check all objects in car
  while(vmo < freePtr) {
    if(!(objDesc = vmo->getObjectDescriptor())) {
      cerr << "Car_t::checkObjRefs(): found object " << vmo->getID() << 
	", vmo at: " << (void*)vmo << " without ObjDesc!" << endl;
      cerr << "Train was: " /*<< trainRef.getTrainNumber()*/ << " and car was: "
	   << /*carNumber <<*/ endl;
      retval++;
      DB_OUTDENT;
      return retval;
    }
    if(void * forward = vmo->getForwardPtr()) {
      cerr << "Car_t::checkObjRefs(): found object " << vmo->getID() << 
	", vmo at: " << (void*)vmo << " with forward pointer set to:" <<
	forward << endl;
      retval++;
    }
    unsigned int numberOfRefs = objDesc->getObjLengthRefs();
    //check each reference
    for(unsigned int refInVMO = 0; refInVMO < numberOfRefs; refInVMO++) {
      if(objDesc->hasReferenceAt(refInVMO) && vmo->data[refInVMO]) {
	VMObject_t *vmoRef = VMO_FROM_VOIDP(vmo->data[refInVMO]);
	void **vmoRefLoc;

	if(vmoRefs->lookup(vmoRef)) {
	  vmoRefLoc = &(vmo->data[refInVMO]);
	  //check remembered sets integrity
	  if(ttable.member(vmoRef)) {
	    Car_t *refCar = Car_t::getCar(vmoRef);
	    /*
	      CarTrain_t *refCarTrain = trainGeneration.getCarTrain(vmoRef);
	      if(*refCarTrain < *thisCarTrain) {
	    */
	    CarTrain_t refCarTrain = trainGeneration.getCarTrain(vmoRef);
	    if(refCarTrain < thisCarTrain) {
	      //there should be some kind of remembered set item
	      if(refCarTrain.getTrain() == thisCarTrain.getTrain()) {
		                           //internal reference should be present
		if(!(refCar->hasIntRememberReference(vmoRefLoc))) {
		  cerr << "Car::checkObjRefs(): ERROR: COULD NOT FIND INTERNAL REM REF "
		       << (void*)vmoRefLoc << "(from object " << vmo->getID() << ") IN CAR: "
		       << (void*)refCar << endl;
		  retval++;
		}                          //external reference should not be present
		if(refCar->hasExtRememberReference(vmoRefLoc)) {
		  cerr << "Car::checkObjRefs(): ERROR: FOUND EXTERNAL REM REF "
		       << (void*)vmoRefLoc << "(from object " << vmo->getID() << ") IN CAR: "
		       << (void*)refCar << endl;
		}
	      } else {                     //internal reference should not be present
		if(refCar->hasIntRememberReference(vmoRefLoc)) {
		  cerr << "Car::checkObjRefs(): ERROR: FOUND INTERNAL REM REF "
		       << (void*)vmoRefLoc << "(from object " << vmo->getID() << ") IN CAR: "
		       << (void*)refCar << endl;
		}                          //external reference should be present
		if(!(refCar->hasExtRememberReference(vmoRefLoc))) {
		  cerr << "Car::checkObjRefs(): ERROR: COULD NOT FIND EXTERNAL REM REF "
		       << (void*)vmoRefLoc << "(from object " << vmo->getID() << ") IN CAR: "
		       << (void*)refCar << endl;
		  retval++;
		}
	      }
	    } else { //no remembered set item should be present

	      //external reference should not be present
	      if(refCar->hasExtRememberReference(vmoRefLoc)) {
		cerr << "Car::checkObjRefs(): ERROR: FOUND EXTERNAL REM REF "
		     << (void*)vmoRefLoc << "(from object " << vmo->getID() << ") IN CAR: "
		     << (void*)refCar << endl;
		retval++;
	      }
	      //internal reference should not be present
	      if(refCar->hasIntRememberReference(vmoRefLoc)) {
		cerr << "Car::checkObjRefs(): ERROR: FOUND INTERNAL REM REF "
		     << (void*)vmoRefLoc << "(from object " << vmo->getID() << ") IN CAR: "
		     << (void*)refCar << endl;
		retval++;
	      }
	    }
	  } else { //vmoRef was not in train-space
	    if(!(introSpace.member(vmoRef))) {
	      if(!(stackSpace.member(vmoRef))) {
		cerr << "Car::checkObjRefs(): ERROR: REF WAS NOT TO EITHER "
		  "TRAINGEN, INTROSPACE, OR STACKSPACE\n" << (void*)vmoRefLoc << "(from "
		  "object " << vmo->getID() << "\n";
		retval++;
	      } else {
		cout << "Car::checkObjRefs(): WARNING: Found ref to stack-space/root-space, from vmobject:\n"
		     << vmo << " with object descriptor:\n" << vmo->getObjectDescriptor()
		     << " reference number was " << refInVMO << " and referenced object was:\n"
		     << vmoRef << " Object Descriptor was:\n" << vmoRef->getObjectDescriptor() << endl;
	      }
	    } else {
	      //remembered set should be present in the intro space
	      if(!(introSpace.hasRememberReference(vmoRefLoc))) {
		cerr << "Car::checkObjRefs(): ERROR: COULD NOT FIND REM REF "
		     << (void*)vmoRefLoc << "(from object " << vmo->getID()
		     << ") IN INTROSPACE\n";
		retval++;
	      }
	    }
	  }
	} else { // reference not found
	  cerr << "Car_t::checkObjRefs(): found object " << vmo->getID() <<
	    ", vmo at: " << (void*)vmo << " with illegal object pointer" << endl;
	  //  cerr << "Object descriptor: " << endl;
	  //  vmo->getObjectDescriptor()->print();
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

#ifdef LIVESTAT

//counts the live/dead objects (live objects have their fwdPtr set
//in a mark-phase. This method clears fwdPtrs, calculates the stats
//prints them if that's wanted and adds them to what its given pointers to.
void Car_t::doLiveStat(size_t *live, size_t *dead, size_t *unused) {
  size_t liveObjects = 0, deadObjects = 0, liveBytes = 0, deadBytes = 0;
  for(VMObject_t *vmoIndex = (VMObject_t*)data; vmoIndex < freePtr; vmoIndex = NEXT_VMO(vmoIndex)) {
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
  CarTrain_t ct = trainGeneration.getCarTrain(this);
  cout << "Livestat for: " << ct.getTrain() << ", " << ct.getCar() << endl
       << "  Live/dead objects: " << liveObjects << "/" << deadObjects << endl
       << "  Live/dead bytes  : " << liveBytes << "/" << deadBytes << endl
       << "   -in vmo bytes   : " << liveBytes + liveObjects*(sizeof(VMObject_t) - sizeof(void*))
       << "/" << deadBytes + deadObjects*(sizeof(VMObject_t) - sizeof(void*)) << endl;
#endif //LIVESTAT_VERB

  size_t liveBytesH = liveBytes + liveObjects*(sizeof(VMObject_t) - sizeof(void*)); //includes headers
  size_t deadBytesH = deadBytes + deadObjects*(sizeof(VMObject_t) - sizeof(void*)); //includes headers
  *live   += liveBytesH;
  *dead   += deadBytesH;
  *unused += (BYTES_IN_CAR + sizeof(void*) - sizeof(Car_t)  //the free bytes in a car without its header
	      - (liveBytesH + deadBytesH));                  //bytes used by dead or alive objects
}

#endif //LIVESTAT

// ----- private member functions -----

size_t Car_t::availSpace(void) {
  return ((size_t)this + BYTES_IN_CAR) - (size_t)freePtr;
}

// ----- public static functions ------

//Car_t* Car_t::getCar(void* v){
//  return (Car_t*)(((int)v)&CAR_MASK);
//}
