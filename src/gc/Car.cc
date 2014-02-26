//$Id: Car.cc,v 1.30 2001/05/21 11:42:42 hempe Exp $

//#include "VMObject.hpp"
#include "Car.hpp"


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

void* Car_t::operator new(size_t size, void* address) {
  return address;
}

void Car_t::operator delete(void* objref) {
  ((Car_t*)objref)->matureGenerationRef.freeCar((Car_t*)objref);
  DB_INDENT;
  DB_OUT(gc_car_delete, "Car_t::operator delete: called\n");
  DB_OUTDENT;
}


//----- constructors/destructors -----


Car_t::Car_t(MatureGeneration_t &matureGen, Train_t &train, unsigned int number):
  matureGenerationRef(matureGen),
  trainRef(train)
{
  DB_INDENT;
  if(!(void*)&trainRef){
    cerr << "Car made with train-pointer NULL" << endl;
    exit(1);
  }
  //this->matureGenerationRef   = matureGen;
  this->carNumber             = number;
  //this->trainRef              = train;

  nextCarInTrainPtr           = 0;
  overflowCar                 = 0;
  vmObjectList                = 0;
  //  lastObject                  = 0;
  rememberedSetPtr            = 0;
  freeDataObjectPtr           = (void*)&data[0];
  freeDataRemPtr              = (void*)&data[BYTES_IN_CAR];
  DB_OUT(gc_car ,"Car build at: " << (void*)this << " with train: " << train.getTrainNumber()
	 << " at " << (void*)&train << endl);
  DB_OUTDENT;
}

Car_t::~Car_t() {
  DB_INDENT;
  DB_OUT(gc_car ,"~Car_t(): car " << this << "with car number: " << carNumber << " is being freed\n");
  if(!vmObjectList){
    DB_OUT(gc_car ,"~Car_t(): object list was empty\n");
  }

  for(VMObject_t *listPtr = vmObjectList; listPtr; listPtr = listPtr->getNextObjectInCar()) {
    if(!(listPtr->getForward())) {
      //this is a garbage object!
#ifdef DEBUG
      collected++;
#endif DEBUG
      DB_OUT(gc_car_collect ,"~Car_t(): *garbage collected object: " 
	     << listPtr->getID() << " * allocated: " << allocated 
	     << " collected: " << collected << " objects in system: " 
	     << allocated-collected << endl);
    }
  }
  if(overflowCar) {
    delete overflowCar;
  }
  DB_OUT(gc_car ,"~Car_t(): car " << carNumber << " has been freed\n");
  DB_OUTDENT;
}

//----- methods -----

Train_t & Car_t::getTrainRef(void) {
  return trainRef;
}

unsigned int Car_t::getCarNumber(void) {
  return carNumber;
}

void Car_t::setNextCarPtr(Car_t *nextCar) {
  nextCarInTrainPtr = nextCar;
}

Car_t * Car_t::getNextCarPtr(void) {
  return nextCarInTrainPtr;
}

VMObject_t * Car_t::addNewObject(ObjectDescriptor_t *objDes) {
  DB_INDENT;
  size_t freeSize   = (size_t)freeDataRemPtr - (size_t)freeDataObjectPtr;
  //  DB_OUT(gc_car ,"Car_t::addNewObject(): freeRem: " << freeDataRemPtr << " freeData: "
  //     << freeDataObjectPtr << " freeSize: " << freeSize << endl;
  size_t objectSize = sizeof(VMObject_t) + objDes->getObjLengthBytes() - sizeof(void*);
  //  DB_OUT(gc_car ,"Car_t::addNewObject(): objSize: " << objectSize << endl;

  if(freeSize >= objectSize + NEW_CAR_FREE_THRESHOLD) {
    //ok make room for object
    VMObject_t *newObject = new(freeDataObjectPtr, objDes) VMObject_t();
    if(newObject) {
      freeDataObjectPtr = (void*)((size_t)freeDataObjectPtr + objectSize);

      //insert new object first in list
      newObject->setNextObjectInCar(vmObjectList);
      vmObjectList = newObject;

      newObject->setCarPtr(this);


      //add remembered set node in the new object
      RememberedSetNode_t *newRemSetNode =
	(RememberedSetNode_t*)newRememberedSetItem(sizeof(RememberedSetNode_t));
      newRemSetNode->firstRemObjNodePtr = 0;
      newRemSetNode->lastRemObjNodePtr  = 0;
      newRemSetNode->vmIdObjectPtr      = newObject;
      newRemSetNode->nextRemSetNodePtr  = rememberedSetPtr;
      rememberedSetPtr                  = newRemSetNode;

      DB_OUTDENT;
      return newObject;
    } else {
      DB_OUT(gc_car ,"Car_t::addNewObject(): Could not add new object (fatal internal error)\n");
      exit(1);
    }
  } else {
    //no more space in car
    DB_OUTDENT;
    return 0;
  }
  DB_OUTDENT;
}

VMObject_t* Car_t::addObject(VMObject_t *vmo, RememberedSetNode_t *remSetNodePtr) {
  DB_INDENT;
  //is there room for object in car, and do we want more objects in it?
  size_t freeSize   = (size_t)freeDataRemPtr - (size_t)freeDataObjectPtr;
  size_t objectSize = (sizeof(VMObject_t)
		       + vmo->getObjectDescriptor()->getObjLengthBytes()
		       - sizeof(void*));

  if(freeSize >= objectSize + NEW_CAR_FREE_THRESHOLD) {
    memcpy((void*)freeDataObjectPtr, (void*)vmo, objectSize);
    VMObject_t *newObject = (VMObject_t*)freeDataObjectPtr;
    vmo->setForward(newObject);
    freeDataObjectPtr = (void*) ((size_t)freeDataObjectPtr + objectSize);

    //insert new object first in list
    newObject->setNextObjectInCar(vmObjectList);
    vmObjectList = newObject;

    newObject->setCarPtr(this);
    
    //add remembered set node in the new object
    RememberedSetNode_t *newRemSetNode =
      (RememberedSetNode_t*)newRememberedSetItem(sizeof(RememberedSetNode_t));
    newRemSetNode->firstRemObjNodePtr = 0;
    newRemSetNode->lastRemObjNodePtr  = 0;
    newRemSetNode->vmIdObjectPtr      = newObject;
    newRemSetNode->nextRemSetNodePtr  = rememberedSetPtr;
    rememberedSetPtr                  = newRemSetNode;

    //update remembered set? update remembered set's references in that case too
    if(remSetNodePtr && (remSetNodePtr->vmIdObjectPtr == vmo)) {
      unsigned int thisTrainNum = trainRef.getTrainNumber(), refTrainNum, refCarNum;
      VMObject_t *refObj;
      Car_t *refCar;
      Train_t *refTrain;

      //iterate remembered set
      for(RememberedObjectNode_t *ron = remSetNodePtr->firstRemObjNodePtr;
	  ron;
	  ron = ron->nextRemObjNodePtr)
	{
	  if(!(refObj = ron->vmRemObjectPtr)) {
	    DB_OUT(gc_car ,"Car_t::addObject(): error 1\n"); exit(1);
	  }
	  if(!(refCar = refObj->getCarPtr())) {
	    DB_OUT(gc_car ,"Car_t::addObject(): error 2\n"); exit(1);
	  }
	  refCarNum = refCar->getCarNumber();
	  if(!(refTrain = &(refCar->getTrainRef()))) {
	    DB_OUT(gc_car ,"Car_t::addObject(): error 3\n"); exit(1);
	  }
	  refTrainNum = refTrain->getTrainNumber();
	  if(thisTrainNum < refTrainNum 
	     || (thisTrainNum == refTrainNum && carNumber <= refCarNum)) {
	    //copy the reference -we only want to keep the references after ourself
	    addRememberReference(thisTrainNum, refObj, rememberedSetPtr, (RememberedObjectNode_t*)
				 newRememberedSetItem(sizeof(RememberedObjectNode_t)));
	  } else {
	    DB_OUT(gc_car ,"Car_t::addObject(): forgetting remember reference from train "
		   << (void*)refTrainNum << " object " << refObj->getID() << " (which is OK)\n");
	  }	
	  //update the reference(s) in refCar's remembered set to point to the new location
	  DB_OUT(gc_car ,"Car_t::addObject(): trying to change reference\n");
	  refObj->changeReference(VOIDP_FROM_VMO(vmo), VOIDP_FROM_VMO(newObject));
	  // NOT!!! refCar->changeRememberReference(refObj, vmo, newObject);
	}
    } else {
      if(remSetNodePtr) {
	DB_OUT(gc_car ,"Car_t::addObject(): remembered set referencing a different object\n");
      } else {
	DB_OUT(gc_car ,"Car_t::addObject(): remembered set empty\n");
      }
    }
    //update the references' remembered set.
    newObject->updateReferencesRememberSet(vmo);
    DB_OUT(gc_car ,"Car_t::addObject(): returning shining new object location: " << (void*)newObject
	 << " for object: " << newObject->getID() << endl);
    DB_OUT(gc_car ,"Car_t::addObject() ** object " << newObject->getID() << " added to car " 
	 << newObject->getCarPtr()->getCarNumber() << " in train " 
	 << newObject->getCarPtr()->getTrainRef().getTrainNumber() << " **\n");
    DB_OUTDENT;
    return newObject;
  } else {
    //no more space in car
    DB_OUTDENT;
    return 0;
  }
}

//is the car referenced from outside itself?
VMObject_t* Car_t::isReferencedOutsideCar(void) {
  DB_INDENT;
  RememberedObjectNode_t *ron;

  for(RememberedSetNode_t *rsn = rememberedSetPtr; rsn; rsn = rsn->nextRemSetNodePtr) {
    for(ron = rsn->firstRemObjNodePtr; ron; ron = ron->nextRemObjNodePtr) {
      if(ron->vmRemObjectPtr->getCarPtr() != this) {
	DB_OUTDENT;
	return ron->vmRemObjectPtr; 
      }
    }
  }
  DB_OUTDENT;
  return 0;
}

//is the car referenced from outside itself?
VMObject_t* Car_t::isReferencedOutsideTrain(void) {
  DB_INDENT;
  RememberedObjectNode_t *ron;
  VMObject_t *remObj;
  unsigned int thisTrainNum  = trainRef.getTrainNumber();
  unsigned int refTrainNum   = 0;
  unsigned int extReferenced = 0;
  //iterate all RemberedSetNodes and check if the first remsetobjnode in each for external train
  for(RememberedSetNode_t *rsn = rememberedSetPtr; rsn; rsn = rsn->nextRemSetNodePtr) {
    DB_OUT(gc_car ,"Car_t::isReferencedOutsideTrain(): ron " << rsn->firstRemObjNodePtr << endl);

    //check this object's remembered set
    for(ron = rsn->firstRemObjNodePtr;
	(ron
	 && ((refTrainNum = (remObj = ron->vmRemObjectPtr)->getCarPtr()->getTrainRef().getTrainNumber())
	     != thisTrainNum)
	 );
	ron = ron->nextRemObjNodePtr)
      {
	DB_OUT(gc_car ,"Car_t::isReferencedOutsideTrain(): ref from outside train (number "
	     << refTrainNum << ")\n");

	if(refTrainNum != UINT_MAX) {
	  DB_OUT(gc_car ,"Car_t::isReferencedOutsideTrain(): ref is not from root set, returning\n");
	  DB_OUTDENT;
	  return remObj;
	} else {
	  extReferenced = 1;
	  DB_OUT(gc_car ,"Car_t::isReferencedOutsideTrain(): ref is from root set, remembered but continuing\n");
	}
      }

    /*
    if((ron = rsn->firstRemObjNodePtr) &&
       ((refTrainNum = (remObj = ron->vmRemObjectPtr)->getCarPtr()->getTrainRef().getTrainNumber())
	!= thisTrainNum)) {
      //if there is an outside-train-reference in an objects remembered set, then there will
      //be one of these first in the list since it is sorted with external train refs first.
      //That is: we only need to check the first rem-ref for each object.
      //NOTE: I'M NOT SURE WHETHER THIS COMPARING REFERENCES IN THE ABOVE LINE WORKS!
      DB_OUT(gc_car ,"Car_t::isReferencedOutsideTrain(): ref from outside train (number "
	   << refTrainNum << ")\n";
      if(refTrainNum != UINT_MAX) {
	return remObj;
      } else {
	extReferenced = 1;
	//this should search in the rest of the objects remembered set until a reference not from
	//root set is found - lets rewrite it instead       
      }
    } else {
      if(ron && remObj) {
      DB_OUT(gc_car ,"Car_t::isReferencedOutsideTrain(): (no) this trainnr: " << thisTrainNum
	   << " remset trainnr: " << remObj->getCarPtr()->getTrainRef().getTrainNumber() << endl;
      } else {
	DB_OUT(gc_car ,"Car_t::isReferencedOutsideTrain(): ???\n";
      }
    }
    */

  }
  if(extReferenced) {
    DB_OUT(gc_car ,"Car_t::isReferencedOutsideTrain(): referenced from root set, returning 1\n");
    DB_OUTDENT;
    return (VMObject_t*)(void*)1;
  } else {
    DB_OUT(gc_car ,"Car_t::isReferencedOutsideTrain(): no ref from outside, returning 0\n");
    DB_OUTDENT;
    return 0;
  }
  /*
  //go through all objects in the car
  for(int i = 0; i < freeIndex; i++) {
    //check if the object has a reference outside the car
    VMObject_t* vmo = 0;
    if((vmo = vmObject[i].getARefObjOutsideTrain())) {
      DB_OUT(gc_car ,"Car_t::isReferencedOutsideTrain returning object: " << vmo << "\n";
      return vmo;
    }
  }
  //no valid references were found
  return 0;
  */
}

int Car_t::addRememberReference(VMObject_t *obj, VMObject_t *remPtr) {
  DB_INDENT;
  DB_OUT(gc_car ,"Car_t::addRememberReference1(): Car number: " << carNumber 
	 << "called trainRef" << (void*)&trainRef << "typid(this): " 
	 << typeid(this).name() << endl);
#ifdef DEBUG
  garbageCollector->heapDump();
#endif
  addRememberReference(trainRef.getTrainNumber(), remPtr, getObjectsRememberSetPtr(obj),
		       (RememberedObjectNode_t*)newRememberedSetItem(sizeof(RememberedObjectNode_t)));

  //DB_OUT(gc_car ,"Car_t::addRememberReference(VMObject_t *obj, VMObject_t *remPtr): NOT YET IMPLEMENTED\n";
  DB_OUTDENT;
  return 0;
}

void Car_t::addRememberReference(unsigned int thisTrainNum, VMObject_t *remPtr,
				 RememberedSetNode_t *rsn, //remember set for 
				 RememberedObjectNode_t *node2Insert) {
  DB_INDENT;
  DB_OUT(gc_car ,"Car_t::addRememberReference2(): called\n");
  node2Insert->vmRemObjectPtr = remPtr;

  //is there object in remset?
  if(rsn->firstRemObjNodePtr) {
    DB_OUT(gc_car ,"Car_t::addRememberReference2(): object in remset\n");
    //insert first or last?
    //SEGMENTATION FAULT IN THIS LINE WHEN RUNNING GBVM M. testinput.gbc
    if(thisTrainNum == remPtr->getCarPtr()->getTrainRef().getTrainNumber()){

       // rsn->firstRemObjNodePtr->vmRemObjectPtr->getCarPtr()->getTrainRef().getTrainNumber()) {
      DB_OUT(gc_car ,"Car_t::addRememberReference2(): insert last\n");
      //insert last
      rsn->lastRemObjNodePtr->nextRemObjNodePtr = node2Insert;
      rsn->lastRemObjNodePtr = node2Insert;
      node2Insert->nextRemObjNodePtr = 0;
    } else {
      DB_OUT(gc_car ,"Car_t::addRememberReference2(): insert first\n");
      //insert first
      node2Insert->nextRemObjNodePtr = rsn->firstRemObjNodePtr;
      rsn->firstRemObjNodePtr = node2Insert;
    }
  } else {
    //first insertion
    DB_OUT(gc_car ,"Car_t::addRememberReference2(): first insertion\n");
    node2Insert->nextRemObjNodePtr = 0;
    rsn->firstRemObjNodePtr = rsn->lastRemObjNodePtr = node2Insert;
  }
  DB_OUTDENT;
}

void Car_t::clearRememberReference(VMObject_t *obj, VMObject_t *remRef) {
  DB_INDENT;
  RememberedSetNode_t *rsn = getObjectsRememberSetPtr(obj);
  if(!rsn) {
    DB_OUTDENT;
    return;
  }
  
  RememberedObjectNode_t *ronPrev = 0, *ron;
  for(ron = rsn->firstRemObjNodePtr; ron; ronPrev = ron, ron = ron->nextRemObjNodePtr) {
    if(ron->vmRemObjectPtr == remRef) {
      if(ronPrev) {
	ronPrev->nextRemObjNodePtr = ron->nextRemObjNodePtr;
	DB_OUT(gc_car ,"Car_t::clearRememberReference(): removed remset node\n");
	DB_OUTDENT;
	return;
      } else {
	rsn->firstRemObjNodePtr = ron->nextRemObjNodePtr;
	DB_OUT(gc_car ,"Car_t::clearRememberReference(): removed first remset node\n");
	DB_OUTDENT;
	return;
      }
    }
  }
  DB_OUT(gc_car ,"Car_t::clearRememberReference(): could not find remset node to remove\n\n");
  DB_OUTDENT;
  return;
}

void Car_t::changeRememberReference(VMObject_t* vmo, VMObject_t* oldPtr, VMObject_t* newPtr) {
  DB_INDENT;
  //get vmo's remembered set
  RememberedSetNode_t *rsn;

  rsn = getObjectsRememberSetPtr(vmo);
  if(!rsn) {
    cerr << "Car_t::changeRememberReference(): rsn was 0 for vmobject: " << vmo->getID() 
	 << " (at " << vmo << "), quitting\n";
    exit(1);
  }

  if(rsn->vmIdObjectPtr == vmo) {
    //found it! Now we should determine if this reference is external to train or not
    int externalRef = (&trainRef != &(newPtr->getCarPtr()->getTrainRef()));

    //DB_OUT(gc_car ,"Car_t::changeRememberReference(): External ref? " << externalRef << endl;

    //find ALL occurrences of oldPtr, change them to newPtr, and relocate them in the list
    //depending on whether they are external or not
    int foundAny = 0;
    RememberedObjectNode_t *ronPrev = 0, *ron;
    for(ron = rsn->firstRemObjNodePtr;	ron; ronPrev = ron, ron = ron->nextRemObjNodePtr) {
      if(ron->vmRemObjectPtr == oldPtr) {
	DB_OUT(gc_car ,"Car_t::changeRememberReference(): changing remember reference\n");
	foundAny = 1;
	ron->vmRemObjectPtr = newPtr;
	//move remnode according to externalRef
	if(externalRef) {
	  //move to front of rem-set for vmo
	  DB_OUT(gc_car ,"Car_t::changeRememberReference(): ext ref\n");
	  if(ronPrev) {
	    DB_OUT(gc_car ,"Car_t::changeRememberReference(): prev ref\n");
	    ronPrev->nextRemObjNodePtr = ron->nextRemObjNodePtr;
	    ron->nextRemObjNodePtr     = rsn->firstRemObjNodePtr;
	    rsn->firstRemObjNodePtr    = ron;
	    if(rsn->lastRemObjNodePtr == ron) {
	      rsn->lastRemObjNodePtr   = ronPrev;
	    }
	    //make sure the loop continues the right place if ron has been moved to front,
	    //otherwise the loop would start all over...
	    ron = ronPrev;
	  } //no ronPrev? then we must have been the first in the list -we do not need to move then!
	} // else do not move node
      }
    }
    if(!foundAny) {
      DB_OUT(gc_car ,"Car_t::changeRememberReference(): inserting reference\n");
      //insert reference now!
      //DB_OUT(gc_car ,"didnt find any reference!";
      ron = (RememberedObjectNode_t*)newRememberedSetItem(sizeof(RememberedObjectNode_t));
      ron->vmRemObjectPtr = newPtr;
      //insert new node according to externalRef
      if(externalRef){
	DB_OUT(gc_car ,"Car_t::changeRememberReference(): inserting reference in front\n");
	//insert in front
	ron->nextRemObjNodePtr = rsn->firstRemObjNodePtr;
	rsn->firstRemObjNodePtr = ron;
	if(!(rsn->lastRemObjNodePtr)) {
	  //ok, we're the last too
	  rsn->lastRemObjNodePtr = ron;
	}
      } else {
	DB_OUT(gc_car ,"Car_t::changeRememberReference(): inserting reference last \n");
	//insert last
	ron->nextRemObjNodePtr = 0;
	if(!(rsn->firstRemObjNodePtr)) {
	  //is this the first remember object? then both the first and last ptr must be updated
	  rsn->firstRemObjNodePtr = rsn->lastRemObjNodePtr = ron;
	} else {
	  //there are objects in list
	  //the last should point to the new objnode, and the last-node pointer sould point to
	  //new objnode
	  rsn->lastRemObjNodePtr->nextRemObjNodePtr = ron;
	  rsn->lastRemObjNodePtr = ron;
	}
      }
    }
  } else {
    //Could not find this object in remembered set - strange, probably an error
    //DB_OUT(gc_car ,"Car_t::changeRememberReference(): strange error (probably) you should be worried! Perhaps"
    //      " there is an object without remembered set?\n";
  }
  //DB_OUT(gc_car ,"Car_t::changeRememberReference(): returning\n";
  DB_OUTDENT;
}

RememberedSetNode_t* Car_t::getCarsRememberSetPtr(void) {
  return rememberedSetPtr;
}

RememberedSetNode_t* Car_t::getObjectsRememberSetPtr(VMObject_t *vmo) {
  DB_INDENT;
  RememberedSetNode_t *rsn;
  DB_OUT(gc_car, "Car_t::getObjectRememberSetPtr(): 1");
  for(rsn = rememberedSetPtr; rsn && (rsn->vmIdObjectPtr != vmo); rsn = rsn->nextRemSetNodePtr);
  if(rsn) {
    DB_OUT(gc_car ,"Car_t::getObjectsRememberSetPtr(): returning rsn for object: "
	 << rsn->vmIdObjectPtr->getID() << " at " << (void*)rsn->vmIdObjectPtr << endl);
  }
  DB_OUT(gc_car, "Car_t::getObjectRememberSetPtr(): returning rsn: " << rsn);
  DB_OUTDENT;
  return rsn;
}


void* Car_t::newRememberedSetItem(size_t objectSize) {
  DB_INDENT;
  if(!overflowCar) {
    size_t freeSize = (size_t)freeDataRemPtr - (size_t)freeDataObjectPtr;
    if(freeSize >= objectSize) {
      //there is space enough here
      freeDataRemPtr = (void*) ((size_t)freeDataRemPtr - (size_t)objectSize);
      DB_OUTDENT;
      return (RememberedSetNode_t*)freeDataRemPtr;
    } else {
      //allocate an overflowCar now!
      DB_OUT(gc_car ,"Car_t::newRememberedSetNode(): *allocating overflow car*\n");
      overflowCar = new(matureGenerationRef.getCarAddress())
	Car_t(matureGenerationRef, trainRef, 0);
    }
  }
  //if we reached this place we have to use an overflow car.
  DB_OUTDENT;
  return overflowCar->newRememberedSetItem(objectSize);
}

void Car_t::printOn(ostream& stream) {
  DB_INDENT;
  DB_INS_INDT(stream);
  stream << "--- Car " << carNumber << " at " << (void*)this << " with train " << trainRef.getTrainNumber();
  VMObject_t *currentVMO = vmObjectList;
  while(currentVMO) {
    stream << endl << currentVMO;
    currentVMO = currentVMO->getNextObjectInCar();
  }
#ifdef DEBUG
  //print remembered set
  DB_INDENT;
  for(RememberedSetNode_t *rsn = rememberedSetPtr; rsn; rsn = rsn->nextRemSetNodePtr) {
    if(rsn->vmIdObjectPtr) {
      DB_OUT(gc_remset_dump, endl);
      DB_OUT(gc_remset_dump, "Remembered set for object " << rsn->vmIdObjectPtr->getID() << ":");
      DB_INDENT;
      for(RememberedObjectNode_t *ron = rsn->firstRemObjNodePtr; ron; ron = ron = ron->nextRemObjNodePtr) {
	DB_OUT(gc_remset_dump, endl);
	DB_OUT(gc_remset_dump, "-> " << ron->vmRemObjectPtr->getID() << ", " <<
	       (void*)ron->vmRemObjectPtr);
      }
      //      DB_OUT(gc_remset_dump, endl);
      DB_OUTDENT;
    }
  }
  DB_OUTDENT;
#endif
  //print overflow car
  if(overflowCar) {
    stream << endl;
    DB_INS_INDT(stream);
    stream << " -- overflow car ->\n";
    stream << overflowCar;
  }
  DB_OUTDENT;
}

#ifdef DEBUG


VMObject_t **Car_t::gatherObjRefs(VMObject_t **vmoRefArray) {
  DB_INDENT;
  VMObject_t *vmo = (VMObject_t*)&data[0];
  while(vmo < freeDataObjectPtr) {
    //DB_OUT(gc_integrity, "VMObject at: " << (void*)vmo << endl);
    *vmoRefArray = vmo; /* + sizeof(VMObject_t) - sizeof(void*); //ref to internal object...*/
    vmoRefArray++;
    //get next object
    vmo = NEXT_VMO(vmo);
  }
  DB_OUTDENT;
  return vmoRefArray;
}

int Car_t::checkObjRefs(VMObject_t **vmoRefArray, unsigned int length) {
  DB_INDENT;
  int retval = 0;
  VMObject_t *vmo = (VMObject_t*)&data[0];
  ObjectDescriptor_t *objDesc;

  //check all objects in car
  while(vmo < freeDataObjectPtr) {
    if(!(objDesc = vmo->getObjectDescriptor())) {
      cerr << "Car_t::checkObjRefs(): found object " << vmo->getID() << 
	", vmo at: " << (void*)vmo << " without ObjDesc!" << endl;
      cerr << "Train was: " << trainRef.getTrainNumber() << " and car was: "
	   << carNumber << endl;
      retval++;
      DB_OUTDENT;
      return retval;
    }
    unsigned int numberOfRefs = objDesc->getObjLengthRefs();
    //check each reference
    for(unsigned int refInVMO = 0; refInVMO < numberOfRefs; refInVMO++) {
      if(objDesc->hasReferenceAt(refInVMO) && vmo->data[refInVMO]) {
	VMObject_t *vmoRef = VMO_FROM_VOIDP(vmo->data[refInVMO]);
	//check if reference is in list
	int found = 0;
	for(unsigned int i = 0; i < length && !found; i++) {
	  if(vmoRef == vmoRefArray[i]) {
	    found = 1;
	  }
	}
	if(!found) {
	  cerr << "Car_t::checkObjRefs(): found object " << vmo->getID() <<
	    ", vmo at: " << (void*)vmo << " with illegal object pointer" << endl;
	  cerr << "Object descriptor: " << endl << vmo->getObjectDescriptor() << endl;
	  cerr << "Train was: " << trainRef.getTrainNumber() << " and car was: "
	       << carNumber << endl;
	  cerr << "Bad reference was number: " << refInVMO << " with value: "
	       << (void*)(vmo->data[refInVMO]) << endl;
	  cerr << "Object with illegal pointer has typeid: "
	       << typeid(VOIDP_FROM_VMO(vmo)).name() << endl;
	  retval++;
	}
      }
    }
    vmo = NEXT_VMO(vmo);
  }
  DB_OUTDENT;
  return retval;
}

#endif
