//$Id: Train.cc,v 1.17 2001/05/29 16:17:48 ilsoe Exp $

#include "Train.hpp"

ostream& operator << (ostream& stream, Train_t* train) {
  if(train) {
    train->printOn(stream);
  } else {
    stream << "*** tried to print a null-train ***\n";
  }
  return stream;
}

ostream& operator << (ostream& stream, Train_t& train) {
  train.printOn(stream);
  return stream;
}

Train_t::Train_t(MatureGeneration_t& matureGenRef, unsigned int number, unsigned int maxCars) : 
  matureGenerationRef(matureGenRef)
{
  DB_INDENT;
  trainNumber = number;
  maxCars     = maxCars;
  nextCarNumber = 0;
  nextTrainPtr = 0;
  numberOfCars = 1;
  void* newCarAddress = matureGenerationRef.getCarAddress();
  DB_OUT(gc_trn, "Train_t::Train_t(): train " << number << " new car address at: "
	 << newCarAddress << endl);
  carList = new(newCarAddress) Car_t(matureGenerationRef, *this, nextCarNumber++);
  lastCar = carList;
  DB_OUTDENT;
}

Train_t::~Train_t(){
  //iterate and delete cars
  Car_t *car, *tmpCar;
  car = carList;
  while(car){
    tmpCar = car;
    car = car->getNextCarPtr();
    delete tmpCar;
  }
}


VMObject_t *Train_t::addNewObject(ObjectDescriptor_t *objDes) {
  DB_INDENT;
  //this function in particular must be able to build new trains
  VMObject_t *newObj;
  if((newObj = lastCar->addNewObject(objDes))) {
    DB_OUT(gc_trn, "Train_t::addNewObject(): train " << trainNumber << " added object\n");
    DB_OUTDENT;
    return newObj;
  } else {
    if(addCar()) {
      DB_OUT(gc_trn, "Train_t::addNewObject(): train " << trainNumber << " added new car\n");
      if((newObj = lastCar->addNewObject(objDes))) {
	DB_OUTDENT;
	return newObj;
      } else {
	//daelme et stort objekt
	DB_OUT(gc_trn, "Train_t::addNewObject(): objects bigger than cars not supported, "
	       "contact your program supplier if the problem continues\n");
	DB_OUTDENT;
	exit(1);
      }
    } else {
      //no room for more cars in this train
      Train_t *newTrain;
      newTrain = matureGenerationRef.addTrain();
      if((newObj = newTrain->addNewObject(objDes))) {
	DB_OUTDENT;
	return newObj;
      } else {
	//daelme et stort objekt
	DB_OUT(gc_trn, "Train_t::addNewObject(): objects bigger than cars not supported, "
	       "contact your program supplier if the problem continues\n");
	DB_OUTDENT;
	exit(1);
      }
    }
  }
  DB_OUTDENT;
}

int Train_t::isExternallyReferenced(){
  DB_INDENT;
  if(!carList) {
    //train is empty
    DB_OUT(gc_trn, "Train_t::isExternallyReferenced(): carlist is empty\n");
    DB_OUTDENT;
    return 0;
  }
  Car_t* car = carList;
  while(car){
    if(car->isReferencedOutsideTrain()){
      DB_OUTDENT;
      return 1;
    }
    car = car->getNextCarPtr();
  }
  DB_OUTDENT;
  return 0;
}

VMObject_t* Train_t::moveObject(VMObject_t* vmo, RememberedSetNode_t* remSetNode,
				Car_t* car, int forced){
  DB_INDENT;
  //first check if there is not a request for a specific car
  if(!car) {
    //default behaviour is to add to last car if no car is specified
    car = lastCar;
  }
  // else {
  //?? Should this method check if the car* belongs to this train?
  //}

  VMObject_t* destination;

  //first try to add object to car
  if(!(destination = car->addObject(vmo, remSetNode))){
    //then try to add object to this train
    destination = addObject(vmo, remSetNode, forced);
    if(!destination){
      if(forced) {
	DB_OUT(gc_trn, "Train_t::moveObject(): forced failed! (internal error) bailing out\n");
	exit(1);
      } else {
	//add a new train
	Train_t* newTrain = matureGenerationRef.addTrain();
	if (remSetNode) {
	  DB_OUT(gc_trn, "Train_t::moveObject(); remsetnode: " << remSetNode->vmIdObjectPtr << endl);
	} else {
	  DB_OUT(gc_trn, "Train_t::moveObject(); remsetnode: 0\n");
	}
	destination = newTrain->addObject(vmo, remSetNode, forced);
	if(!destination) {
	  DB_OUT(gc_trn, "Train_t::moveObject(): unable to find a new location for object"
		 " - very strange! - bailing out\n");
	  exit(1);
	}
      }
    }
  }
  DB_OUT(gc_trn, "Train_t::moveObject(): returning new object location\n");
  DB_OUTDENT;
  return destination;
}

void Train_t::freeFirstCar(){
  Car_t* firstCar = carList;
  carList = carList->getNextCarPtr();
  numberOfCars--;
  delete firstCar;
}

Car_t* Train_t::getFirstCar(){
  return carList;
}

unsigned int Train_t::getTrainNumber(){
  return trainNumber;
}

void Train_t::setTrainNumber(unsigned int trainNumber) {
  this->trainNumber = trainNumber;
}

void Train_t::setNextTrainPtr(Train_t* train){
  nextTrainPtr = train;
}

Train_t* Train_t::getNextTrainPtr(){
  return nextTrainPtr;
}


VMObject_t* Train_t::addObject(VMObject_t* vmo, RememberedSetNode_t* remSetNode, int forced){
  VMObject_t* destination;
  //check that the train has a car, if not try to add one
  if(!carList){
    addCar();
    carList = lastCar;
  }

  //add object to last car if no room then add a car
  if(!(destination = lastCar->addObject(vmo, remSetNode))){
    if(forced) {
      //we want the object in THIS train
      addCarForced();
      return lastCar->addObject(vmo, remSetNode);
    } else {
      //only add car if there are not too many cars in the train
      if(addCar()){
	return lastCar->addObject(vmo, remSetNode);
      } else {
	return 0;
      }
    }
  } else {
    return destination;
  }
}

unsigned int Train_t::addCar(){
  //check if there is space in train for a new car
  if(numberOfCars < (maxCars-1)){
    addCarForced();
    return 1;
  } else {	
    return 0;
  }
}

void Train_t::addCarForced(){
  void* carAddress = matureGenerationRef.getCarAddress();
  Car_t* newCar = new(carAddress) Car_t(matureGenerationRef, *this, nextCarNumber++);
  lastCar->setNextCarPtr(newCar);
  lastCar = newCar;
  numberOfCars++;
}

void Train_t::printOn(ostream& stream) {
  DB_INDENT;
  DB_INS_INDT(stream);
  stream << "----- Train " << trainNumber << " at " << (void*)this << " next train at "
	 << (void*)nextTrainPtr;
  Car_t *currentCar = carList;
  while(currentCar) {
    stream << endl << currentCar;
    currentCar = currentCar->getNextCarPtr();
  }
  stream << endl;
  DB_OUTDENT;
}

#ifdef DEBUG

VMObject_t **Train_t::gatherObjRefs(VMObject_t **vmoRefArray) {
  Car_t *car = carList;
  while(car) {
    vmoRefArray = car->gatherObjRefs(vmoRefArray);
    car = car->getNextCarPtr();
  }
  return vmoRefArray;
}

int Train_t::checkObjRefs(VMObject_t **vmoRefArray, unsigned int length) {
  Car_t *car = carList;
  int retval = 0;
  while(car) {
    retval += car->checkObjRefs(vmoRefArray, length);
    car = car->getNextCarPtr();
  }
  return retval;
}

#endif
