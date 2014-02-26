//$Id: TrainTable.hpp,v 1.20 2001/06/08 10:26:25 ilsoe Exp $

#ifndef _TRAINTABLE_HPP_
#define _TRAINTABLE_HPP_

class TrainTableElement_t;
class TrainTable_t;

#include <iostream.h>
//#include "Car.hpp"


//#define MAX_NUMBER_OF_TRAINS 1024 //not always enough!!!
#define MAX_NUMBER_OF_TRAINS 4096

class TrainTableElement_t{
public:
  Car_t* firstCar;
  Car_t* lastCar;
  //  void* scan;
};

ostream& operator << (ostream& stream, TrainTable_t* tt);
ostream& operator << (ostream& stream, TrainTable_t& tt);

/* class TrainTable:
 * Purpose given a train-number this table should be able to return:
 * getLastCar(trainNumber) -> returns pointer to last car in train
 * getFirstCar(trainNumber -> returns pointer to first car in train
 * (getScanPointer(trainNumber) -> returns scan pointer of last car
 * NB! if train 1 is requested train 0 must be returned, etc.
 * NB! if train 1 is inserted it must placed in buffer[0] !!
 */

class TrainTable_t{
public:
                              TrainTable_t(void);
  void                        addTrain(Car_t* firstCar, Car_t* lastCar);
                                //adds a new train to this ringbuffer
  void                        removeTrain();
                                //removes the first train
  Car_t*                      getLastCar(unsigned int trainNumber);
                                //returns pointer to last car in train
  Car_t*                      getFirstCar(unsigned int trainNumber);
				//returns pointer to first car in train
  Car_t*                      getFromCar(void);
                                //returns first car of first train
                                //i.e., conceptually the from car
  void                        removeFromCar(void);
                                //removes the from car. Assumes that it is
                                //not the only one
  Car_t*                      getLastTrainLastCar(void);
#ifdef NEW_STACK_RESCUE
  Car_t*                      getLastTrainFirstCar(void);
#endif //NEW_STACK_RESCUE
//void*                       getScanPointer(unsigned int trainNumber);
				//returns scan pointer of last car
  void                        setLastCar(unsigned int trainNumber, Car_t* lastCar);
                                //sets pointer to last car in train
  void                        setFirstCar(unsigned int trainNumber, Car_t* firstCar);
				//returns pointer to first car in train
  void                        renumberTrainsAndCars(void);
                                //resets train and car numbers if necessary
  void                        printOn(ostream& stream);
#ifdef LIVESTAT
  void                        doLiveStat(size_t *live, size_t *dead, size_t *unused);
#endif //LIVESTAT

#ifdef DEBUG
  void                        gatherObjRefs(VMOHash_t *vmoRefs);
  int                         checkObjRefs (VMOHash_t *vmoRefs);
  bool                        member(void* address);
                                //returns true if address is member of a 
                                //valid car - a car allocated by trainGeneration
#endif //DEBUG

private:
  unsigned int                first; //first element
  unsigned int                last;  //next free element cell (last element + 1 % MAX)
  unsigned int                count; //number of elements
  TrainTableElement_t         buffer[MAX_NUMBER_OF_TRAINS];
};

#endif
