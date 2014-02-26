//$Id: TrainGeneration.cc,v 1.29 2001/05/21 11:42:45 hempe Exp $

#include "gc_new.hpp"

TrainGeneration_t::TrainGeneration_t(int heapSizeInCars):
  lastMemBlkSize(heapSizeInCars)
{
  if(heapSizeInCars <= 0) {
    cerr << "TrainGeneration_t(): constructor was given 0 or negative size\n";
    exit(1);
  }

  //make dirycars object pointer
  dirtyCars = new DirtyCars_t();

  //clear array
  memset((void *)adr2CarTrainTbl, 0, sizeof(CarTrain_t[ADR_2_CT_SIZE]));

  //allocate memory blocks
  memoryBlocks.add(new MemoryBlock_t(heapSizeInCars));

  //THIS HAS TO BE 1, INTROSPACE AND STACKSPACE ARE 0
  firstTrain = lastTrain = 1;
}

TrainGeneration_t::~TrainGeneration_t() {
  delete(dirtyCars);
}


VMObject_t *TrainGeneration_t::addNewObject(ObjectDescriptor_t *objDes) {
  DB_INDENT;
  DB_OUT(gc_tg, "TrainGeneration_t::addNewObject()\n");
  
  Car_t *lastCar = ttable.getLastTrainLastCar();
  if(!lastCar) { //train generation _can_ be empty (like initially)
    addNewTrainFirst();
    lastCar = ttable.getLastTrainLastCar();
  }
  VMObject_t *vmo = lastCar->allocateVMO(objDes);
  if(vmo) {
    DB_OUTDENT;
    return vmo;
  }

  //we have to allocate a new last car

  Car_t *newLastCar = new(allocCarSpace())
    Car_t(lastTrain, (getCarTrain(lastCar).getCar() + 1));
  lastCar->setNext(newLastCar);
  if((vmo = newLastCar->allocateVMO(objDes))) {
    DB_OUTDENT;
    return vmo;
  }
  cerr << "TrainGeneration::addNewObject(): could not allocate space for"
    " new object with object descriptor: " << objDes << endl;
  exit(1);
}


void TrainGeneration_t::addNewTrainFirst() {
  DB_INDENT;
#ifdef DEBUG
  if(ttable.getFromCar()) {
    cerr << "TrainGeneration_t::addNewTrainFirst(): tried to add "
      "a new first train when there was already one, shame on you!\n";
    exit(1);
  }
#endif //DEBUG
  DB_OUT(gc_tg, "TrainGeneration::addNewTrainFirst()\n");
  new(allocCarSpace())Car_t(firstTrain, 0);
  DB_OUTDENT;
}

void TrainGeneration_t::addNewTrainLast() {
  new(allocCarSpace())Car_t(++lastTrain, 0);
}


void TrainGeneration_t::deleteFirstTrain() {
  DB_INDENT;
  firstTrain++;
  Car_t *tmpCar = ttable.getFromCar();
  Car_t *nextCar;
  while(tmpCar) {
    nextCar = tmpCar->next();
    MemoryBlock_t *mblk = memoryBlocks.first();
    while(mblk) { //search for the memblk to free car in
      switch (mblk->freeCar(tmpCar)) {
      case 0:
	mblk = mblk->next();
	break;
      case 1:
	mblk = 0;
	break;
      case 2:
	DB_OUT(gc_tg, "TrainGeneration::deleteFirstTrain(): delete memory"
	       " block here\n");
	mblk = 0;
	break;
      default:
	cerr << "TrainGeneration::deleteFirstTrain(): interal error\n";
      }
    }
    tmpCar = nextCar;
  }
  ttable.removeTrain();
  DB_OUTDENT;
}

void TrainGeneration_t::deleteFirstTrainsFirstCar() {
  DB_INDENT;
  Car_t *tmpCar = ttable.getFromCar();
  Car_t *nextCar;
  if(!(nextCar = tmpCar->next())) {
    //train consists of only one car, kill it!
    DB_OUT(gc_tg, "TrainGeneration::deleteFirstTrainsFirstCar(): "
	   "deleting train consisting of one car\n");
    deleteFirstTrain();
    DB_OUTDENT;
    return;
  }

  ttable.removeFromCar();
  MemoryBlock_t *mblk = memoryBlocks.first();
  while(mblk) { //search for the memblk to free car in
    switch (mblk->freeCar(tmpCar)) {
    case 0:
      mblk = mblk->next();
      break;
    case 1:
      mblk = 0;
      break;
    case 2:
      DB_OUT(gc_tg, "TrainGeneration::deleteFirstTrainsFirstCar(): delete memory"
	     " block here\n");
      mblk = 0;
      break;
    default:
      cerr << "TrainGeneration::deleteFirstTrainsFirstCar(): interal error\n";
    }
  }
  DB_OUTDENT;
}


void* TrainGeneration_t::allocCarSpace() {
  DB_INDENT;
  //try to add a new car
  void *carSpace = 0;
  for(MemoryBlock_t *mblk = memoryBlocks.first(); mblk; mblk = mblk->next()){
    if((carSpace = mblk->allocCarSpace())) {
      DB_OUT(gc_tg, "TrainGeneration_t::allocateCar(): allocated"
	     " space for new car at: " << (void*)carSpace << "\n");
      DB_OUTDENT;
      return carSpace;
    }
  }

  if(!carSpace) {
    //try to add a new memory block
    lastMemBlkSize *= BLOCK_SIZE_MULTIPLIER;
    memoryBlocks.add(new MemoryBlock_t(lastMemBlkSize));
    if(!(carSpace = (Car_t*)memoryBlocks.last()->allocCarSpace())) {
      cerr << "TrainGeneration_t::addNewObject(): error:"
	" could not allocate car\n";
      exit(1);
    } else {
      DB_OUT(gc_tg, "TrainGeneration_t::allocateCar(): allocated"
	     " space for new car in new block\n");      
    }
  }
  DB_OUTDENT;
  return carSpace;
}
