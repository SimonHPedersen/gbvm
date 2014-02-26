//$Id: TrainTable.cc,v 1.22 2001/06/08 10:26:25 ilsoe Exp $

//#include "TrainTable.hpp"
#include "gc_new.hpp"

//used to clear entries
//element_t emptyElement;

/*
 * NB! if train 1 is requested train 0 must be returned, etc.
 * NB! if train 1 is inserted it must placed in buffer[0] !!
 */

// ----- operators -----

ostream& operator << (ostream& stream, TrainTable_t* tt) {
  if(tt) {
    tt->printOn(stream);
  } else {
    stream << "*** tried to print a null-vmo ***\n";
  }
  return stream;
}

ostream& operator << (ostream& stream, TrainTable_t& tt) {
  tt.printOn(stream);
  return stream;
}

// ----- member functions ------

TrainTable_t::TrainTable_t() : first(0), last(0), count(0){

  //clear table
  for(unsigned int i = 0; i < MAX_NUMBER_OF_TRAINS; i++){
     buffer[i].firstCar = 0;
     buffer[i].lastCar  = 0;
  }
}

//adds a new train to this rinbuffer
void TrainTable_t::addTrain(Car_t* firstCar, Car_t* lastCar){
  if(count < (MAX_NUMBER_OF_TRAINS-1)){
    //never fill the last element - we don't want first==last
    buffer[last].firstCar = firstCar;
    buffer[last].lastCar = lastCar;
    last = (last + 1) % MAX_NUMBER_OF_TRAINS;
    count++;
  } else {
    cerr << "TrainTable::addTrain(): ERROR train table is full\n";
    exit(1);
  }
}

//removes the first train
void TrainTable_t::removeTrain(){
  if(count > 0){
    buffer[first].firstCar = NULL;
    buffer[first].lastCar = NULL;
    first = (first + 1) % MAX_NUMBER_OF_TRAINS;
    count--;
  } else {
    cerr << "TrainTable::removeTrain(): ERROR train table was empty \n";
    exit(1);
  }
}

//returns pointer to last car in train 
Car_t* TrainTable_t::getLastCar(unsigned int trainNumber){
  //cout << "glc(): first: " << first << " last: " << last << endl;
  unsigned int reqElement = (trainNumber-1) % MAX_NUMBER_OF_TRAINS;
  if(first <= last){
    if(reqElement < last && reqElement >= first){
      return buffer[reqElement].lastCar;
    } else {
      cerr << "TrainTable_t::getLastCar(): requested not in [first;last]\n";
      exit(1);
    }
  } else {
    if(reqElement >= first || reqElement < last){
      return buffer[reqElement].lastCar;
    } else {
      cerr << "TrainTable_t::getLastCar(): requested not in" 
	   << "[last;0[ or [0;first]\n"; 
      exit(1);
    }
  } 
  return NULL;
}

//returns pointer to first car in train
Car_t* TrainTable_t::getFirstCar(unsigned int trainNumber){
  unsigned int reqElement = (trainNumber-1) % MAX_NUMBER_OF_TRAINS;
  //cout << "gfc(): first: " << first << " last: " << last << "reqElement: " << reqElement << endl;
  if(first <= last){
    if(reqElement < last && reqElement >= first){
      DB_OUTDENT;
      return buffer[reqElement].firstCar;
    } else {
      cerr << "TrainTable_t::getLastCar(): requested not in [first;last]\n";
      exit(1);
    }
  } else {
    if(reqElement >= first || reqElement < last){
      DB_OUTDENT;
      return buffer[reqElement].firstCar;
    } else {
      cerr << "TrainTable_t::getLastCar(): requested not in" 
	   << "[last;0[ or [0;first]\n"; 
      exit(1);
    }
  } 
  return NULL;
}

//returns pointer to first car in train
Car_t* TrainTable_t::getFromCar(){
  return buffer[first].firstCar;
}

//removes the from car. Assumes that it is not the only one
void TrainTable_t::removeFromCar(void) {
  buffer[first].firstCar = buffer[first].firstCar->next();
}

//returns pointer to last car in last train
Car_t* TrainTable_t::getLastTrainLastCar(){
  unsigned int index;
  if(last != first) {
    if(last == 0) {
      index = MAX_NUMBER_OF_TRAINS -1;
    } else {
      index = last - 1;
    }
  } else {
    index = last;
  }

  return buffer[index].lastCar;
}

#ifdef NEW_STACK_RESCUE

//returns pointer to first car in last train
Car_t* TrainTable_t::getLastTrainFirstCar(){
  unsigned int index;
  if(last != first) {
    if(last == 0) {
      index = MAX_NUMBER_OF_TRAINS -1;
    } else {
      index = last - 1;
    }
  } else {
    index = last;
  }

  return buffer[index].firstCar;
}

#endif //NEW_STACK_RESCUE


//sets to last car in train 
void TrainTable_t::setLastCar(unsigned int trainNumber, Car_t* lastCar){
  unsigned int reqElement = (trainNumber-1) % MAX_NUMBER_OF_TRAINS;
  if(first <= last){
    if(reqElement < last && reqElement >= first){
      buffer[reqElement].lastCar = lastCar;
    } else {
      cerr << "TrainTable_t::setLastCar(): requested not in [first;last]\n";
      exit(1);
    }
  } else {
    if(reqElement >= first || reqElement < last){
      buffer[reqElement].lastCar = lastCar;
    } else {
      cerr << "TrainTable_t::setLastCar(): requested not in" 
	   << "[last;0[ or [0;first]\n"; 
      exit(1);
    }
  } 
}

//returns pointer to first car in train
void TrainTable_t::setFirstCar(unsigned int trainNumber, Car_t* firstCar){
  unsigned int reqElement = (trainNumber-1) % MAX_NUMBER_OF_TRAINS;
  if(first <= last){
    if(reqElement < last && reqElement >= first){
      buffer[reqElement].firstCar = firstCar;
    } else {
      cerr << "TrainTable_t::getLastCar(): requested not in [first;last]\n";
      exit(1);
    }
  } else {
    if(reqElement >= first || reqElement < last){
      buffer[reqElement].firstCar = firstCar;
    } else {
      cerr << "TrainTable_t::setFirstCar(): requested not in" 
	   << "[last;0[ or [0;first]\n"; 
      exit(1);
    }
  } 
}

void TrainTable_t::renumberTrainsAndCars(void) {
  //we reorder so the first train gets the number used internally + 1
  //This way we don't have to reorder elements in the internal buffer

  if(trainGeneration.lastTrain >= (HALF_INT_MAX -500)) { //magic number - we don't believe more than 500 new trains can be created during a gc
    //do train renumbering...
    unsigned int newTrainNumber = first + 1;
    for(unsigned int i = first; i != last;
	i = (i + 1) % MAX_NUMBER_OF_TRAINS, newTrainNumber++)
      {
	Car_t *tmpCar = buffer[i].firstCar; //first car in train
	unsigned int newCarNumber = 0;
	while(tmpCar){ //iterates train
	  trainGeneration.setCarTrain(tmpCar, newTrainNumber, newCarNumber);
	  newCarNumber++;
	  tmpCar = tmpCar->getNext();
	}
      }
    //update train numbers in train generation
    trainGeneration.firstTrain = first + 1;
    trainGeneration.lastTrain  = newTrainNumber - 1;
  } else if(trainGeneration.getCarTrain(buffer[first].lastCar).getTrain()
	    >= (HALF_INT_MAX) - 500) {
    //do car renumbering in first train
    Car_t *tmpCar = buffer[first].firstCar; //first car in train
    unsigned int newCarNumber = 0;
    while(tmpCar){ //iterates train
      trainGeneration.setCar(tmpCar, newCarNumber);
      newCarNumber++;
      tmpCar = tmpCar->getNext();
    }
  }
}

void TrainTable_t::printOn(ostream& stream){
  DB_INDENT;
  for(unsigned int i = first; i < last; i++){
    Car_t* tmpCar = buffer[i].firstCar;
    CarTrain_t ct = trainGeneration.getCarTrain(tmpCar);
    stream << "----- Train " << ct.getTrain() << endl;
    while(tmpCar){
      stream << endl << tmpCar;
      tmpCar = tmpCar->getNext();
    }
  }
  DB_OUTDENT;
}

#ifdef LIVESTAT

void TrainTable_t::doLiveStat(size_t *live, size_t *dead, size_t *unused) {
#ifdef undefined
  size_t liveBytes = 0, deadBytes = 0, unusedBytes = 0, tmpLiveBytes, tmpDeadBytes, tmpUnusedBytes;
#endif //undefined

  for(unsigned int i = first; i != last; i = (i + 1) % MAX_NUMBER_OF_TRAINS){ //first != last ever!!!
    Car_t* tmpCar = buffer[i].firstCar;
    while(tmpCar){
      tmpCar->doLiveStat(live, dead, unused);
#ifdef undefined
      tmpLiveBytes = tmpCar->liveBytes
	+ tmpCar->liveObjects*(sizeof(VMObject_t) - sizeof(void*));
      liveBytes += tmpLiveBytes;

      tmpDeadBytes = tmpCar->deadBytes
	+ tmpCar->deadObjects*(sizeof(VMObject_t) - sizeof(void*));
      deadBytes += tmpDeadBytes;

      tmpUnusedBytes = BYTES_IN_CAR
	+ sizeof(void*) - sizeof(Car_t) - (tmpLiveBytes + tmpDeadBytes);
      unusedBytes += tmpUnusedBytes;

#ifdef LIVESTAT_VERB
      cout << "-SUB MEM STAT: live: " << tmpLiveBytes << " dead: "
	   << tmpDeadBytes << " unused: " << tmpUnusedBytes << endl;
#endif //LIVESTAT_VERB
#endif //undefined
      tmpCar = tmpCar->getNext();
    }
  }
#ifdef undefined
  cout << "\nMEMORY STAT: live: " << liveBytes << " dead: "
       << deadBytes << " unused: " << unusedBytes << endl;
#endif //undefined
}

#endif //LIVESTAT

#ifdef DEBUG

void TrainTable_t::gatherObjRefs(VMOHash_t * validObjRefs){

  for(unsigned int i = first; i != last; i = (i + 1) % MAX_NUMBER_OF_TRAINS){ //first != last ever!!!
    Car_t* tmpCar = buffer[i].firstCar;
    while(tmpCar){
      tmpCar->gatherObjRefs(validObjRefs);
      tmpCar = tmpCar->getNext();
    }
  }
}


int TrainTable_t::checkObjRefs(VMOHash_t * validObjRefs) {
  unsigned int errors = 0;

  for(unsigned int i = first; i != last; i = (i + 1) % MAX_NUMBER_OF_TRAINS){ //first != last ever!!!
    Car_t* tmpCar = buffer[i].firstCar;
    while(tmpCar){
      errors += tmpCar->checkObjRefs(validObjRefs);
      tmpCar = tmpCar->getNext();
    }
  }
  return errors;
}

bool TrainTable_t::member(void* address){
  //iterates traintable 
  for(unsigned int i = first; i <= last; i++){
    Car_t* tmpCar = buffer[i].firstCar;
    //iterates cars in train
    while(tmpCar){
      if(tmpCar->member(address)){
	return true;
      }
      tmpCar = tmpCar->getNext();
    }
  }
  
  return false;
}

#endif //DEBUG




