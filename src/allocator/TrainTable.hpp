//$Id: TrainTable.hpp,v 1.1 2001/03/16 08:26:16 ilsoe Exp $

#include <iostream.h>
#include "Car.hpp"


#define MAX_NUMBER_OF_TRAINS 5

struct element_t{
  Car_t* firstCar;
  Car_t* lastCar;
  //  void* scan;
};

/* class TrainTable:
 * Purpose given a train-number this table should be able to return:
 * getLastCar(trainNumber) -> returns pointer to last car in train
 * getFirstCar(trainNumber -> returns pointer to first car in train
 * (getScanPointer(trainNumber) -> returns scan pointer of last car
 */

class TrainTable_t{
public:
                              TrainTable_t(void);
  void                        addTrain(Car_t* firstCar, Car_t* lastCar);
                                //adds a new train to this rinbuffer
  void                        removeTrain();
                                //removes the first train
  Car_t*                      getLastCar(unsigned int trainNumber);
                                //returns pointer to last car in train
  Car_t*                      getFirstCar(unsigned int trainNumber);
				//returns pointer to first car in train
//void*                       getScanPointer(unsigned int trainNumber);
				//returns scan pointer of last car
  void                        setLastCar(unsigned int trainNumber, Car_t* lastCar);
                                //returns pointer to last car in train
  void                        setFirstCar(unsigned int trainNumber, Car_t* firstCar);
				//returns pointer to first car in train
private:
  unsigned int                first; //first element
  unsigned int                last;  //next free element cell (last element + 1 % MAX)
  unsigned int                count; //number of elements
  element_t                   buffer[MAX_NUMBER_OF_TRAINS];
};
