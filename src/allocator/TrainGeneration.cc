//$Id: TrainGeneration.cc,v 1.3 2001/05/21 11:42:41 hempe Exp $

//#include "Generation.hpp"
#include "allocator.hpp"


/*
#include "ObjectDescriptor.hpp"
#include "garbage_collector.hpp"
#include "Train.hpp"
#include "Car.hpp"
#include "Generation.hpp"
*/

TrainGeneration_t::TrainGeneration_t(int heapSizeInCars):
  lastMemBlkSize(heapSizeInCars)
{
  if(heapSizeInCars <= 0) {
    cerr << "TrainGeneration_t(): constructor was given 0 or negative size\n";
    exit(1);
  }

  //make sure cars can reach us
  Car_t::setTrainGeneration(this);

  //allocate array for address -> train, car
  /*
  if(!(adr2CarTrainTbl = (CarTrain_t*)malloc(sizeof(CarTrain_t[ADR_2_CT_SIZE])))) {
    cerr << "TrainGeneration_t(): unable to allocate address -> train, car"
      " decoding table, quitting\n";
    exit(1);
  }
  */

  //clear array
  memset((void *)adr2CarTrainTbl, 0, sizeof(CarTrain_t[ADR_2_CT_SIZE]));

  //allocate memory blocks
  memoryBlocks.add(new MemoryBlock_t(heapSizeInCars));

  void * carSpace = memoryBlocks.last()->allocCarSpace();

  nextTrainNumber = 1;
  nextCarNumber = 0;

  //allocate allocation car
  allocCar = new(carSpace)Car_t(nextTrainNumber, nextCarNumber++);

  /*
  nextTrainNumber = 0; // UINT_MAX-4; //for debug only should start from 0
  trainList = new Train_t(*this, nextTrainNumber++, MAX_NUMBER_OF_CARS);
  lastTrain = trainList;
  rootTrain = new Train_t(*this, UINT_MAX, UINT_MAX);

  Generation_t::init(heapSize);
  */
}

TrainGeneration_t::~TrainGeneration_t() {
  //free(adr2CarTrainTbl);

  /*
  DB_INDENT;
  DB_OUT(gc_tg, "~TrainGeneration_t(): farveller 1\n");
  delete rootTrain;
  //delete trains in list
  Train_t *nextTrain = trainList, *train;
  while(nextTrain) {
    train = nextTrain;
    nextTrain = nextTrain->getNextTrainPtr();
    delete train;
  }
  DB_OUT(gc_tg, "~TrainGeneration_t(): farveller end\n");
  //super(size);
  DB_OUTDENT;
  */
}


VMObject_t *TrainGeneration_t::addNewObject(ObjectDescriptor_t *objDes) {
  DB_INDENT;
  //  DB_OUT(gc_tg, "TrainGeneration_t::addNewObject()\n");

  VMObject_t *vmo = allocCar->allocateVMO(objDes);
  if(vmo) {
    DB_OUTDENT;
    return vmo;
  }

  //try to add a new car
  void *carSpace;
  if(!(carSpace = memoryBlocks.last()->allocCarSpace())) {
    DB_OUT(gc_tg, "TrainGeneration_t::addNewObject(): failed to allocate"
	   " space for new car\n");
    //try to add a new memory block
    lastMemBlkSize *= BLOCK_SIZE_MULTIPLIER;
    memoryBlocks.add(new MemoryBlock_t(lastMemBlkSize));
    carSpace = (Car_t*)memoryBlocks.last()->allocCarSpace();
  }

  if(!carSpace) {
    cerr << "TrainGeneration_t::addNewObject(): error:"
      " could not allocate car\n";
    exit(1);
  }

  allocCar = new(carSpace)Car_t(nextTrainNumber, nextCarNumber++);

  if(!(vmo = allocCar->allocateVMO(objDes))) {
    cerr << "TrainGeneration_t::addNewObject(): error: object is too big\n";
    exit(1);
  }

  DB_OUTDENT;
  return vmo;
}

VMObject_t *TrainGeneration_t::addRootObject(ObjectDescriptor_t *objDes) {
  //hack, does not handle root objects yet...
  return addNewObject(objDes);
}

//Garbage collection with the hot, new and famous train algorithm
void TrainGeneration_t::garbageCollect() {
  DB_INDENT;
  DB_OUT(gc_tg, "*** MG: garbage collecting not implemented ***\n");
  DB_OUTDENT;

  /*
  DB_INDENT;
  DB_OUT(gc_tg, "*** MG: garbage collecting ***\n");

#ifdef DEBUG
  DB_OUT(gc_tg_heapdump, "MG: dumping heap\n");
  Train_t *tmpTrain = trainList;
  while(tmpTrain) {
    DB_OUT0(gc_tg_heapdump, tmpTrain);
    tmpTrain = tmpTrain->getNextTrainPtr();
  }
#endif

  //avoid segfault if there are no trains
  if(!trainList) {
    DB_OUT(gc_tg, "TrainGeneration_t::garbageCollect(): tried to garbage collect when there"
	   " no trains\n");
    DB_OUTDENT;
    return;
  }
  //check first train for outside references
  if(!trainList->isExternallyReferenced()){
    DB_OUT(gc_tg, "TrainGeneration_t::garbageCollect(): first train not externally referenced\n");
    Train_t* oldFirstTrain = trainList;
    //make train list point to next train in list
    trainList = trainList->getNextTrainPtr();
    delete oldFirstTrain;
    DB_OUT(gc_tg, "TrainGeneration_t::garbageCollect(): removed garbage-train\n");
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
      DB_OUT(gc_tg, "TrainGeneration_t::garbageCollect(): removed empty garbage-train\n");
    }
  }

#ifdef DEBUG
  DB_OUT(gc_tg_heapdump, "MG: after gc, dumping heap\n");
  tmpTrain = trainList;
  while(tmpTrain) {
    DB_OUT0(gc_tg_heapdump, tmpTrain);
    tmpTrain = tmpTrain->getNextTrainPtr();
  }
#endif

  DB_OUTDENT;
  */
}

/*
Train_t *TrainGeneration_t::addTrain() {
  DB_INDENT;
  if((nextTrainNumber + 1) == UINT_MAX) {
    //we should renumber trains now
    DB_OUT(gc_tg, "TrainGeneration_t::addTrain(): ***** WARNING: TRAIN NUMBERS WRAPPING AROUND"
	   " ***** this will probably crash!\n");
    renumberTrains();
  }

  Train_t* newTrain = new Train_t(*this, nextTrainNumber++, MAX_NUMBER_OF_CARS);
  lastTrain->setNextTrainPtr(newTrain);
  DB_OUTDENT;
  return lastTrain = newTrain;
}

void TrainGeneration_t::renumberTrains(void) {
  nextTrainNumber = 0;
  for(Train_t *tmpTrain = trainList; tmpTrain; tmpTrain = tmpTrain->getNextTrainPtr()) {
    tmpTrain->setTrainNumber(nextTrainNumber++);
  }
}

Car_t *TrainGeneration_t::getCarAddress() {
  DB_INDENT;
  Car_t *carSpace = (Car_t*)malloc(sizeof(Car_t));
  if(carSpace) {
    DB_OUTDENT;
    return carSpace;
  } else {
    DB_OUT(gc_tg, "TrainGeneration_t::getCarAddress(): could not get mem for car.\n");
    DB_OUTDENT;
    return 0;
  }
  return 0;
}

void TrainGeneration_t::freeCar(Car_t *carPtr) {
  free(carPtr);
}
*/

#ifdef DEBUG
void TrainGeneration_t::heapDump(void){
  /*
  DB_OUT(gc_tg_heapdump, "MG: dumping heap\n");
  Train_t *tmpTrain = trainList;
  while(tmpTrain) {
    DB_OUT0(gc_tg_heapdump, tmpTrain);
    tmpTrain = tmpTrain->getNextTrainPtr();
  }
  */
}

VMObject_t **TrainGeneration_t::gatherObjRefs(VMObject_t **vmoRefArray) {
  /*
  vmoRefArray = rootTrain->gatherObjRefs(vmoRefArray);
  Train_t *train = trainList;
  while (train) {
    vmoRefArray = train->gatherObjRefs(vmoRefArray);
    train = train->getNextTrainPtr();
  }
  return vmoRefArray;
  */
  return 0;
}

int TrainGeneration_t::checkObjRefs(VMObject_t **vmoRefArray, unsigned int length) {
  /*
  int retval = 0;
  retval += rootTrain->checkObjRefs(vmoRefArray, length);
  Train_t *train = trainList;
  while (train) {
    retval += train->checkObjRefs(vmoRefArray, length);
    train = train->getNextTrainPtr();
  }
  return retval;
  */
  return 0;
}

#endif

CarTrain_t *TrainGeneration_t::getCarTrain(void *address) {
  return &adr2CarTrainTbl[addr2index(address)];
}


unsigned int TrainGeneration_t::addr2index(void *address) {
  return ((unsigned int)address)>>CAR_EXP;
}
