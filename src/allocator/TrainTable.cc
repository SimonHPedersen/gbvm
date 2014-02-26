//$Id: TrainTable.cc,v 1.2 2001/05/21 11:42:41 hempe Exp $

//#include "TrainTable.hpp"
#include "allocator.hpp"

//used to clear entries
element_t emptyElement;

TrainTable_t::TrainTable_t() : first(0), last(0), count(0){
  emptyElement.firstCar=0;
  emptyElement.lastCar=0;

  /*NO NEED FOR INITIALIZATION ???
    for(unsigned int i = 0; i < MAX_NUMBER_OF_CARS; i++){
    buffer[i] = emptyElement;
    }
   */

}

//adds a new train to this rinbuffer
void TrainTable_t::addTrain(Car_t* firstCar, Car_t* lastCar){
  if(count < MAX_NUMBER_OF_TRAINS){
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
    buffer[first]=emptyElement;
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
  unsigned int reqElement = trainNumber % MAX_NUMBER_OF_TRAINS;
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
  unsigned int reqElement = trainNumber % MAX_NUMBER_OF_TRAINS;
  //cout << "gfc(): first: " << first << " last: " << last << "reqElement: " << reqElement << endl;
  if(first <= last){
    if(reqElement < last && reqElement >= first){
      return buffer[reqElement].firstCar;
    } else {
      cerr << "TrainTable_t::getLastCar(): requested not in [first;last]\n";
      exit(1);
    }
  } else {
    if(reqElement >= first || reqElement < last){
      return buffer[reqElement].firstCar;
    } else {
      cerr << "TrainTable_t::getLastCar(): requested not in" 
	   << "[last;0[ or [0;first]\n"; 
      exit(1);
    }
  } 
  return NULL;
}

/* UNSUPPORTED
//returns scan pointer of last car
void* TrainTable_t::getScanPointer(unsigned int trainNumber){

}
*/


//sets to last car in train 
void TrainTable_t::setLastCar(unsigned int trainNumber, Car_t* lastCar){
  unsigned int reqElement = trainNumber % MAX_NUMBER_OF_TRAINS;
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
  unsigned int reqElement = trainNumber % MAX_NUMBER_OF_TRAINS;
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




