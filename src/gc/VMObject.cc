//$Id: VMObject.cc,v 1.44 2001/05/29 16:17:48 ilsoe Exp $
#include <iostream.h>
//#include <stdio.h>

#include "Car.hpp"
#include "VMObject.hpp"

ostream& operator << (ostream& stream, VMObject_t* vmo) {
  if(vmo) {
    vmo->printOn(stream);
  } else {
    stream << "*** tried to print a null-vmo ***\n";
  }
  return stream;
}

ostream& operator << (ostream& stream, VMObject_t& vmo) {
  vmo.printOn(stream);
  return stream;
}

void* VMObject_t::operator new(size_t size, void *freePointer,
			       ObjectDescriptor_t *objDescriptor) {
  DB_INDENT;
  if(!objDescriptor) {
    DB_OUT(gc_vmo, "VMObject_t::operator new: *** object descriptor is null! ***\n");
  }
  ((VMObject_t*)freePointer)->objectDescriptor = objDescriptor;
  DB_OUT(gc_vmo, "Memory allocation subsystem: allocated VMObject_t at size: " << (int)size
	 << " begin: " << hex << (int)freePointer << dec << endl);
  DB_OUTDENT;
  return freePointer;
}

void VMObject_t::operator delete(void* vmo){
  DB_INDENT;
  DB_OUT(gc_vmo, "deleting VMObject_t " << ((VMObject_t*)vmo)->id << endl);
  DB_OUTDENT;
}

VMObject_t::VMObject_t(){
  DB_INDENT;
  init();
  id = vmObjectHashValue++;
  DB_OUT(gc_vmo_id, "VMObject_t::VMObject_t() called, new object at: " << (void*)this << " with id: " << id << endl);
  DB_OUTDENT;
}

VMObject_t::~VMObject_t(){
}

void VMObject_t::init(void) {
  DB_INDENT;
  nextObjectInCarPtr = 0;
  flags = 0;
  generation = 0;
  forwardPtr = (VMObject_t*)0;
  carPtr = 0;
  //oldCarPtr = 0;
  unsigned int refs = objectDescriptor->getObjLengthRefs(); 
  for(unsigned int i = 0; i < refs; i++) {
    data[i] = 0;
  }
  DB_OUTDENT;
}

ObjectDescriptor_t* VMObject_t::getObjectDescriptor(void) {
  return objectDescriptor;
}

void VMObject_t::setNextObjectInCar(VMObject_t* nextObject) {
  nextObjectInCarPtr = nextObject;
}

VMObject_t *VMObject_t::getNextObjectInCar(void) {
  return nextObjectInCarPtr;
}

int VMObject_t::getExtendedSize(){
  return objectDescriptor->getObjLengthBytes();
}

int VMObject_t::getGeneration(){
  return this->generation;
}

int VMObject_t::setReference(int refNumber, void *objRef) {
  DB_INDENT;
  //check if refNumber is in range of array of references
  //- change this when we get proper objects
  DB_OUT(gc_vmo, "VMObject_t::setReference(): object: " << (void*)this << " with id "  
       << id << "ObjectDescriptor:" << objectDescriptor << endl);

  if(objectDescriptor->hasReferenceAt(refNumber)) {
    data[refNumber] = objRef;
    DB_OUT(gc_vmo, "VMObject_t::setReference(): object: " << (void*)this << " setting ref " << refNumber
	 << " to: " << objRef << endl);
    if(objRef) {
      VMObject_t *vmoObjRef = VMO_FROM_VOIDP(objRef);
      DB_OUT(gc_vmo, "VMObject_t::setReference(): object to add remember reference: "
	     << (void*)vmoObjRef << "with id: " << vmoObjRef->getID() 
	     << " with car " << (void*)vmoObjRef->getCarPtr() << endl);
      vmoObjRef->getCarPtr()->addRememberReference(vmoObjRef, this);
    }
  } else {
    cerr << "VMObject_t::setReference(): hvad helved laver du???"
      " Der er ingen refefence paa plads " << refNumber << " i objekt: "
	 << this->getID() << endl;
    cerr << "VMObject_t::setReference(): objectDescriptor was: "
	 << objectDescriptor << endl;
    exit(1);
  }
  DB_OUTDENT;
  return 1;
}
  
int VMObject_t::setReference(void *refLocation, void *target) {
  DB_INDENT;
  int refNumber = ((unsigned int)refLocation - ((unsigned int)this + sizeof(VMObject_t) -sizeof(void*)))/sizeof(void*);
  DB_OUTDENT;
  return setReference(refNumber, target);
}

//NOTE: we update ALL oldRefs in object, so we have to go through all entries 
void VMObject_t::changeReference(void *oldRef, void *newRef) {
  DB_INDENT;
  DB_OUT(gc_vmo, "VMObject_t::changeReference(): changing refernce to "
	 << (void*)VMO_FROM_VOIDP(newRef)->getID() << " in object " << id << " from "
	 << (void*)VMO_FROM_VOIDP(oldRef) << " to " << (void*)VMO_FROM_VOIDP(newRef) << " objectDescriptor: " << objectDescriptor << endl);

  unsigned int dataIndex;
  unsigned int *refMask = objectDescriptor->getReferenceMask();
  unsigned int intsInMask = (((objectDescriptor->getObjLengthRefs()))
			     /(sizeof(unsigned int)*8))+1;

  //DB_OUT(gc_vmo, "VMObject_t::changeReference(): 2" << endl);
  for(unsigned int i = 0; i < intsInMask; i++){
    int mask = refMask[i];
    for(unsigned int j = 0; j < sizeof(int)*8 ; j++){
      if((1<<j) & mask){
	//we've got a reference -calculate index in data[]
	dataIndex = i * sizeof(int) * 8 + j;
	//- is it one we want to change?
	if(oldRef == data[dataIndex]) {
	  data[dataIndex] = newRef;
	}
      }
    }
  }
  //DB_OUT(gc_vmo, "VMObject_t::changeReference(): finished" << endl);
  DB_OUTDENT;
}


void VMObject_t::changeRememberReference(VMObject_t* oldRef,
					 VMObject_t* newRef) {
  DB_INDENT;
  DB_OUT(gc_vmo, "VMObject_t::changeRememberReference(): changing object " << id
       << "'s rem ref to reflect obj " << newRef->getID() << "'s new position\n");
  carPtr->changeRememberReference(this, oldRef, newRef);
  DB_OUTDENT;
}


void VMObject_t::clearRememberReference(VMObject_t *ref2Clear) {
  DB_INDENT;
  DB_OUT(gc_vmo, "VMObject_t::clearRememberReference(): carPtr: " 
	 << (void*)carPtr << endl);
  carPtr->clearRememberReference(this, ref2Clear);
  DB_OUTDENT;
}

//Updates remember reference to this object in references after a move of this object
void VMObject_t::updateReferencesRememberSet(VMObject_t* old_vmo) {
  DB_INDENT;
  void         *objRef;
  //  VMObject_t   *vmRefObj;

  unsigned int *refMask = objectDescriptor->getReferenceMask();
  unsigned int refsInObject = objectDescriptor->getObjLengthRefs(); //used for avoiding checks of
                                                                    //non object member references
  unsigned int intsInMask = (refsInObject/(sizeof(int)*8))+1;

  DB_OUT(gc_vmo, "VMObject_t::updateReferencesRememberSet(): started, " << intsInMask
       << " int(s) in mask\n");

  unsigned int bits2Check = sizeof(unsigned int)*8;
  for(unsigned int i = 0; i < intsInMask; i++){
    int mask = refMask[i];
    //    printf("i: %i, %0x\n", i, mask);
    if(mask) {
      if(i == (intsInMask -1)) {
	bits2Check = refsInObject - (intsInMask - 1) * sizeof(unsigned int)*8;
      }
      for(unsigned int j = 0; j < bits2Check; j++){
	if((1<<j) & mask){
	  //we've got a reference, let's find it
	  objRef = data[i * sizeof(int) * 8 + j];
	  //if refObj not in same gen its remembered set must
	  //be updated
	  //If there is a carPtr we have an object in mature generation,
	  //and all references remembered sets must be updated
	  //In the young generation(s) only remembered sets from objects in
	  //other generations should be updated

	  if(objRef) {
	    DB_OUT(gc_vmo, "VMObject_t::updateReferencesRememberSet(): found a reference"
		   ", changing its remembered set\n");
	    VMO_FROM_VOIDP(objRef)->changeRememberReference(old_vmo, this);
	  }
	}
      }
    }
  }
  DB_OUT(gc_vmo, "VMObject_t::updateReferencesRememberSet(): returning\n");
  DB_OUTDENT;
}


//Called by MatureGeneration gc on all objects in car
VMObject_t* VMObject_t::moveObject(){
  DB_INDENT;
  if(forwardPtr){
    DB_OUTDENT;
    return forwardPtr;
  } else {

    //first try to find an external (to train) referencing object
    VMObject_t* referencingObj = getARefObjOutsideTrain();
    Car_t *refCar;
    Train_t *refTrain;
    unsigned int forced = 0;
    if(referencingObj){
      if(referencingObj == (VMObject_t*)1) {
	//carefull! this is not a pointer, but the object is referenced externally
	//only from root set
	DB_OUT(gc_vmo, "VMObject_t::moveObject(): *moving object " << id << " * at " << (void*)this
	       << " to new train\n");
	//add a new train
	refCar   = 0; //this means that the dest. train decides which car to put the obj in
	refTrain = carPtr->matureGenerationRef.addTrain();
      } else {
	//we got a reference outside root-set - move object there and update references to it
	refCar = referencingObj->getCarPtr();
	refTrain = &(refCar->getTrainRef());
	//experimental, if ok, then we do not need forced anymore :(
	forced = 1;
      }
    } else {
      referencingObj = getARefObjOutsideCar();
      if(referencingObj){
	forced = 1;
	refCar   = referencingObj->getCarPtr();
	refTrain = &(refCar->getTrainRef());
      } else {
	//this object does not want to move now (not referenced directly from outside
	//its train or car)
	DB_OUTDENT;
	return 0;
      }
    }
    //here refCar and refTrain must have been determined (refCar can be 0)

    if(refCar) {
      DB_OUT(gc_vmo, "VMObject_t::moveObject(): *moving object " << id << " * at " << (void*)this
	   << " to car: " << refCar->getCarNumber() << " in train " << refTrain->getTrainNumber()
	   << endl);
    } else {
      DB_OUT(gc_vmo, "VMObject_t::moveObject(): *moving object " << id << " * at " << (void*)this
	   << " to car: " << (void*)refCar << " and train " << refTrain->getTrainNumber()
	   << " (refeneced from root-set)\n");
    }
    VMObject_t *newObject = refTrain->moveObject(this, carPtr->getObjectsRememberSetPtr(this),
						 refCar, forced);
    //make sure we try to move referenced objects to the location where the object
    //was placed
    refCar = newObject->getCarPtr();
    DB_OUT(gc_vmo, "VMObject_t::moveObject(): *moving referenced objects* forced=" << forced << "\n");
    moveReferencedObjects(refCar, forced); //always forced instead???
    DB_OUT(gc_vmo, "VMObject_t::moveObject(): done moving referenced objects, returning\n");
    DB_OUTDENT;
    return forwardPtr;
  }
  DB_OUTDENT;
}


void VMObject_t::moveReferencedObjects(Car_t* destCar, int forced) {
  DB_INDENT;
  //move referenced objects recursively
  void        *objRef;
  VMObject_t  *vmRefObj;

  DB_OUT(gc_vmo_mro_verb, "VMObject_t::moveReferencedObjects(): 1 called\n");

  unsigned int *refMask = objectDescriptor->getReferenceMask();
  DB_OUT(gc_vmo_mro_verb, "VMObject_t::moveReferencedObjects(): 2 got reference mask pointer\n");
  unsigned int refsInObject = objectDescriptor->getObjLengthRefs(); //used for avoiding checks of
                                                           //non object member references
  unsigned int intsInMask = (refsInObject/(sizeof(unsigned int)*8))+1;
  DB_OUT(gc_vmo_mro_verb, "VMObject_t::moveReferencedObjects(): 3 found " << intsInMask
	 << " ints in mask (" << refsInObject << " refs in object)\n");

  unsigned int bits2Check = sizeof(unsigned int)*8;
  for(unsigned int i = 0; i < intsInMask; i++){
    int mask = refMask[i];
    if(mask) {
      if(i == (intsInMask -1)) {
	bits2Check = refsInObject - (intsInMask - 1) * sizeof(unsigned int)*8;
      }
      DB_OUT(gc_vmo_mro_verb, "VMObject_t::moveReferencedObjects(): traversing mask "
	     << i << " bits2check " << bits2Check << endl);
      for(unsigned int j = 0; j < bits2Check; j++){
	if((1<<j) & mask){
	  //we've got a reference, let's find it
	  objRef = data[i * sizeof(int) * 8 + j];
	  if(objRef) {
	    //move only the objects from the old car
	    DB_OUT(gc_vmo_mro_verb, "VMObject_t::moveReferencedObjects():"
		   " found reference not null" << endl);
	    vmRefObj = VMO_FROM_VOIDP(objRef);
	    if(vmRefObj->getCarPtr() == carPtr && !(vmRefObj->getForward())) {
	      DB_OUT(gc_vmo, "VMObject_t::moveReferencedObjects(): *moving referenced object "
		     << vmRefObj->getID() << " * at (vm) " << (void*)vmRefObj << " from car: "
		     << (void*)carPtr << " to car: " << (void*)destCar << endl);
	      destCar->getTrainRef().
		moveObject(vmRefObj,
			   vmRefObj->getCarPtr()->getObjectsRememberSetPtr(vmRefObj),
			   destCar, forced);
	      vmRefObj->moveReferencedObjects(destCar, forced);
	    }
	    
	  } //we allow null-references to be ignored
	}
      }
    }
  }
  DB_OUT(gc_vmo_mro_verb, "VMObject_t::moveReferencedObjects(): returning" << endl);
  DB_OUTDENT;

}



void VMObject_t::setCarPtr(Car_t* car){
  DB_INDENT;
  //  oldCarPtr = carPtr; //save old car
  carPtr = car;
  DB_OUTDENT;
}

Car_t* VMObject_t::getCarPtr(){
  return carPtr; 
}

void VMObject_t::setForward(VMObject_t* forward_ptr) {
  forwardPtr = forward_ptr;
}

VMObject_t* VMObject_t::getForward(void) {
  return forwardPtr;
}

//private methods

//return external reference
VMObject_t* VMObject_t::getARefObjOutsideTrain(){
  DB_INDENT;
  DB_OUT(gc_vmo, "VMObject_t::getARefObjOutsideTrain() car: " << (void*)carPtr << " object: " 
	 << (void*)this << endl);
  Train_t      &myTrain       = carPtr->getTrainRef();
  unsigned int  thisTrainNum  = myTrain.getTrainNumber();
  unsigned int  refTrainNum   = 0;
  unsigned int  extReferenced = 0;

  //find our remembered set
  RememberedSetNode_t *rsn;
  if(!(rsn = carPtr->getObjectsRememberSetPtr(this))) {
    return 0;
  }
  //iterate rememberedset - try to get an external referencee which is not in root set
  RememberedObjectNode_t *ron;
  VMObject_t             *remObj;

  for(ron = rsn->firstRemObjNodePtr;
      (ron
       && ((refTrainNum =
	    (remObj = ron->vmRemObjectPtr)->getCarPtr()->getTrainRef().getTrainNumber())
	   != thisTrainNum)
       );
      ron = ron->nextRemObjNodePtr)
    {
      DB_OUT(gc_vmo, "VMObject_t::getARefObjOutsideTrain(): ref from outside train (number "
	   << refTrainNum << ")\n");
      
      if(refTrainNum != UINT_MAX) {
	DB_OUT(gc_vmo, "VMObject_t::getARefObjOutsideTrain(): ref is not from root set, returning\n");
	DB_OUTDENT;
	return remObj;
      } else {
	extReferenced = 1;
	DB_OUT(gc_vmo, "VMObject_t::getARefObjOutsideTrain(): ref is from root set, "
	  "remembered but continuing\n");
      }
    }

  if(extReferenced) {
    DB_OUT(gc_vmo, "VMObject_t::getARefObjOutsideTrain(): referenced from root set, returning 1\n");
    DB_OUTDENT;
    return (VMObject_t*)(void*)1;
  } else {
    DB_OUT(gc_vmo, "VMObject_t::getARefObjOutsideTrain(): no ref from outside, returning 0\n");
    DB_OUTDENT;
    return 0;
  }
  DB_OUTDENT;
}

//return internal reference
VMObject_t* VMObject_t::getARefObjOutsideCar(){
  DB_INDENT;
  //iterate rememberedset - try to get an internal referencee

  RememberedSetNode_t *rsn = carPtr->getObjectsRememberSetPtr(this);

  for(RememberedObjectNode_t *ron = rsn->firstRemObjNodePtr; ron; ron = ron->nextRemObjNodePtr) {
    if(ron->vmRemObjectPtr->getCarPtr() != carPtr) {
      DB_OUTDENT;
      return ron->vmRemObjectPtr;
    }
  }
  DB_OUTDENT;
  return 0;
}

void VMObject_t::setID(unsigned int id) {
  this->id = id;
}

unsigned int VMObject_t::getID(void) {
  return id;
}

void VMObject_t::printVMO(void) {
  DB_INDENT;
  DB_OUT(gc_vmo, "object id: " << id);
  DB_OUTDENT;
}

void *VMObject_t::getDataPtr(void) {
  return data;
}

int VMObject_t::isInternallyReferenced() {
  DB_INDENT;
  DB_OUT(gc_vmo, "VMObject_t::******** isInternallyReferenced() is not implemented *********\n");
  DB_OUTDENT;
  return 1;
}

void VMObject_t::printOn(ostream& stream) {
  DB_INDENT;
  DB_INS_INDT(stream);
  stream << "- VMObject " << id << " at " << (void*)this << " with car at " << (void*)carPtr;
#ifdef DEBUG
  DB_INDENT;
  //print references...
  unsigned int *refMask = objectDescriptor->getReferenceMask();
  unsigned int refsInObject = objectDescriptor->getObjLengthRefs();
  unsigned int intsInMask = (refsInObject/(sizeof(unsigned int)*8))+1;
  int bits2Check = sizeof(unsigned int)*8;
  void *objRef;

  for(unsigned int i = 0; i < intsInMask; i++){
    int mask = refMask[i];
    if(mask) {
      if(i == (intsInMask -1)) {
	bits2Check = refsInObject - (intsInMask - 1) * sizeof(unsigned int)*8;
      }
      for(int j = 0; j < bits2Check; j++){
	if((1<<j) & mask){
	  //we've got a reference, let's find it
	  objRef = data[i * sizeof(int) * 8 + j];
	  if(objRef) {
	    //ok, now let's print it
	    //	    DB_OUT(gc_vmo_dump_refs, endl);
	    //	    DB_OUT(gc_vmo_dump_refs, "-> " << (void*)VMO_FROM_VOIDP(objRef));
	    DB_OUT(gc_vmo_dump_refs, endl);
	    DB_OUT(gc_vmo_dump_refs, "-> " << VMO_FROM_VOIDP(objRef)->getID() << ", "
		   << (void*)VMO_FROM_VOIDP(objRef));
	      /*
		//move only the objects from the old car
		DB_OUT(gc_vmo_mro_verb, "VMObject_t::moveReferencedObjects(): found reference not null" << endl);
		vmRefObj = VMO_FROM_VOIDP(objRef);
		if(vmRefObj->getCarPtr() == carPtr && !(vmRefObj->getForward())) {
		DB_OUT(gc_vmo, "VMObject_t::moveReferencedObjects(): *moving referenced object "
		<< vmRefObj->getID() << " * at (vm) " << (void*)vmRefObj << " from car: "
		<< (void*)carPtr << " to car: " << (void*)destCar << endl);
		destCar->getTrainRef().moveObject(vmRefObj,
		vmRefObj->getCarPtr()->getObjectsRememberSetPtr(vmRefObj),
		destCar, forced);
		vmRefObj->moveReferencedObjects(destCar, forced);
		}
	      */	    
	  } //we allow null-references to be ignored
	}
      }
    }
  }  
  DB_OUTDENT;
#endif
  DB_OUTDENT;
}

