//$Id: mature_generation.cc,v 1.20 2001/03/12 13:39:44 ilsoe Exp $

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>

#include "ObjectDescriptor.hpp"
#include "garbage_collector.hpp"
#include "Train.hpp"
#include "Car.hpp"
#include "Generation.hpp"


MatureGeneration_t::MatureGeneration_t(int heapSize) {
  nextTrainNumber = 0; // UINT_MAX-4; //for debug only should start from 0
  trainList = new Train_t(*this, nextTrainNumber++, MAX_NUMBER_OF_CARS);
  lastTrain = trainList;
  rootTrain = new Train_t(*this, UINT_MAX, UINT_MAX);

  Generation_t::init(heapSize);
}

MatureGeneration_t::~MatureGeneration_t() {
  DB_INDENT;
  DB_OUT(gc_mg, "~MatureGeneration_t(): farveller 1\n");
  delete rootTrain;
  //delete trains in list
  Train_t *nextTrain = trainList, *train;
  while(nextTrain) {
    train = nextTrain;
    nextTrain = nextTrain->getNextTrainPtr();
    delete train;
  }
  DB_OUT(gc_mg, "~MatureGeneration_t(): farveller end\n");
  //super(size);
  DB_OUTDENT;
}


VMObject_t *MatureGeneration_t::addNewObject(ObjectDescriptor_t *objDes) {
  DB_INDENT;
  DB_OUT(gc_mg, "MatureGeneration_t::addNewObject()\n");
  DB_OUTDENT;
  return lastTrain->addNewObject(objDes);
}

VMObject_t *MatureGeneration_t::addRootObject(ObjectDescriptor_t *objDes) {
  return rootTrain->addNewObject(objDes);
}

//Garbage collection with the hot, new and famous train algorithm
void MatureGeneration_t::garbageCollect() {
  DB_INDENT;
  DB_OUT(gc_mg, "*** MG: garbage collecting ***\n");

#ifdef DEBUG
  DB_OUT(gc_mg_heapdump, "MG: dumping heap\n");
  Train_t *tmpTrain = trainList;
  while(tmpTrain) {
    DB_OUT0(gc_mg_heapdump, tmpTrain);
    tmpTrain = tmpTrain->getNextTrainPtr();
  }
#endif

  //avoid segfault if there are no trains
  if(!trainList) {
    DB_OUT(gc_mg, "MatureGeneration_t::garbageCollect(): tried to garbage collect when there"
	   " no trains\n");
    DB_OUTDENT;
    return;
  }
  //check first train for outside references
  if(!trainList->isExternallyReferenced()){
    DB_OUT(gc_mg, "MatureGeneration_t::garbageCollect(): first train not externally referenced\n");
    Train_t* oldFirstTrain = trainList;
    //make train list point to next train in list
    trainList = trainList->getNextTrainPtr();
    delete oldFirstTrain;
    DB_OUT(gc_mg, "MatureGeneration_t::garbageCollect(): removed garbage-train\n");
  } else {
    Car_t* firstCar = trainList->getFirstCar();
    //check first car for outside references
    if(firstCar->isReferencedOutsideCar()){
      
      RememberedSetNode_t* carRemSet;
      VMObject_t* vmo;

      //iterate cars remembered set - first move only objects that are referenced outside train
      for(carRemSet = firstCar->getCarsRememberSetPtr();
	  carRemSet != NULL;
	  carRemSet = carRemSet->nextRemSetNodePtr){
	vmo = carRemSet->vmIdObjectPtr;
	//this is a bit bad, as this is also done in moveObject
	if(vmo->getARefObjOutsideTrain()) {
	  vmo->moveObject();
	}
      }
      //iterate cars remembered set - the rest of the referenced objects will be moved now
      for(carRemSet = firstCar->getCarsRememberSetPtr();
	  carRemSet != NULL;
	  carRemSet = carRemSet->nextRemSetNodePtr){
	vmo = carRemSet->vmIdObjectPtr;
	vmo->moveObject();
      }
    }
    //delete car
    trainList->freeFirstCar();      
    // ***NEW*** CHECK IF THE TRAIN IS NOW EMPTY AND FREE IT IF SO
    if(!trainList->getFirstCar()) {
      Train_t* oldFirstTrain = trainList;
      trainList = trainList->getNextTrainPtr();
      delete oldFirstTrain;
      DB_OUT(gc_mg, "MatureGeneration_t::garbageCollect(): removed empty garbage-train\n");
    }
  }

#ifdef DEBUG
  DB_OUT(gc_mg_heapdump, "MG: after gc, dumping heap\n");
  tmpTrain = trainList;
  while(tmpTrain) {
    DB_OUT0(gc_mg_heapdump, tmpTrain);
    tmpTrain = tmpTrain->getNextTrainPtr();
  }
#endif

  DB_OUTDENT;
}

Train_t *MatureGeneration_t::addTrain() {
  DB_INDENT;
  if((nextTrainNumber + 1) == UINT_MAX) {
    //we should renumber trains now
    DB_OUT(gc_mg, "MatureGeneration_t::addTrain(): ***** WARNING: TRAIN NUMBERS WRAPPING AROUND"
	   " ***** this will probably crash!\n");
    renumberTrains();
  }

  Train_t* newTrain = new Train_t(*this, nextTrainNumber++, MAX_NUMBER_OF_CARS);
  lastTrain->setNextTrainPtr(newTrain);
  DB_OUTDENT;
  return lastTrain = newTrain;
}

void MatureGeneration_t::renumberTrains(void) {
  nextTrainNumber = 0;
  for(Train_t *tmpTrain = trainList; tmpTrain; tmpTrain = tmpTrain->getNextTrainPtr()) {
    tmpTrain->setTrainNumber(nextTrainNumber++);
  }
}

Car_t *MatureGeneration_t::getCarAddress() {
  DB_INDENT;
  Car_t *carSpace = (Car_t*)malloc(sizeof(Car_t));
  if(carSpace) {
    DB_OUTDENT;
    return carSpace;
  } else {
    DB_OUT(gc_mg, "MatureGeneration_t::getCarAddress(): could not get mem for car.\n");
    DB_OUTDENT;
    return 0;
  }
}

void MatureGeneration_t::freeCar(Car_t *carPtr) {
  free(carPtr);
}

#ifdef DEBUG
void MatureGeneration_t::heapDump(void){
  DB_OUT(gc_mg_heapdump, "MG: dumping heap\n");
  Train_t *tmpTrain = trainList;
  while(tmpTrain) {
    DB_OUT0(gc_mg_heapdump, tmpTrain);
    tmpTrain = tmpTrain->getNextTrainPtr();
  }
}

VMObject_t **MatureGeneration_t::gatherObjRefs(VMObject_t **vmoRefArray) {
  vmoRefArray = rootTrain->gatherObjRefs(vmoRefArray);
  Train_t *train = trainList;
  while (train) {
    vmoRefArray = train->gatherObjRefs(vmoRefArray);
    train = train->getNextTrainPtr();
  }
  return vmoRefArray;
}

int MatureGeneration_t::checkObjRefs(VMObject_t **vmoRefArray, unsigned int length) {
  int retval = 0;
  retval += rootTrain->checkObjRefs(vmoRefArray, length);
  Train_t *train = trainList;
  while (train) {
    retval += train->checkObjRefs(vmoRefArray, length);
    train = train->getNextTrainPtr();
  }
  return retval;
}


#endif
