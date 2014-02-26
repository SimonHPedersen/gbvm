//$Id: Generation.hpp,v 1.24 2001/05/10 14:45:49 ilsoe Exp $

#ifndef _GENERATION_HPP_
#define _GENERATION_HPP_

#include <math.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>

#include "CarTrain.hpp"

class Generation_t;
class TrainGeneration_t;

const unsigned int BLOCK_SIZE_MULTIPLIER = 2;
//const unsigned int BLOCK_SIZE_MULTIPLIER = 4;
const unsigned int ADR_2_CT_SIZE = (1 << (8*sizeof(void*) - CAR_EXP)); //typically 65536

class TrainGeneration_t {
public:
                              TrainGeneration_t(int heapSizeInCars);
                             ~TrainGeneration_t();
  VMObject_t                 *addNewObject(ObjectDescriptor_t *objDes);
  void                        addNewTrainFirst(void);
                                //adds a new train first, with an empty car in it
  void                        addNewTrainLast(void);
                                //adds a new train last, with an empty car in it
  void                        deleteFirstTrain(void);
                                //deletes the first train in every possible way
  void                        deleteFirstTrainsFirstCar(void);
                                //deletes the first car in the first train in
                                //every possible way
  void                        setCarTrain(void *carAddress,
					  unsigned int train,
					  unsigned int car) {
                                //set the car-number and train-number for a given car
                                adr2CarTrainTbl[addr2index(carAddress)]
				  .setTrainAndCar(train, car);
                              }
  void                        setCar(void *carAddress, unsigned int car) {
                                //set the car-number for a given car
                                adr2CarTrainTbl[addr2index(carAddress)].setCar(car);
                              }
  CarTrain_t                  getCarTrain(void *address) {
                                //returns a pointer to car and train for
                                //something at "address"
                                return adr2CarTrainTbl[addr2index(address)];
                              }
private:
  unsigned int                addr2index(void *address) {
                                //used for indexing addr->cartrain table
                                return ((unsigned int)address)>>CAR_EXP;
  }
  void                       *allocCarSpace(void);
                                //allocates space for a new car or exits!

private:
  DirtyCars_t*                dirtyCars;
  unsigned int                lastMemBlkSize;
                                //the size (in cars) of the last added
                                //memory block
  Lst<MemoryBlock_t>          memoryBlocks;

  unsigned int                firstTrain;
  unsigned int                lastTrain;

  CarTrain_t                  adr2CarTrainTbl[ADR_2_CT_SIZE];
                                //table to get car and train numbers
                                //from an address
  friend class GarbageCollector_t;
  friend class IntroductorySpace_t;
  friend class StackSpace_t;
  friend class Car_t;
  friend class TrainTable_t;
};

#endif
