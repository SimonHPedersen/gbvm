//$Id: Generation.hpp,v 1.3 2001/04/09 08:49:58 ilsoe Exp $

#ifndef _GENERATION_HPP_
#define _GENERATION_HPP_

#include <math.h>
#include <stdlib.h>
//#include <stdio.h>
#include <iostream.h>
#include <string.h>

class CarTrain_t;
class Generation_t;
class TrainGeneration_t;

const unsigned int BLOCK_SIZE_MULTIPLIER = 4;

#include "Car.hpp"

const unsigned int ADR_2_CT_SIZE = (1 << (32 - CAR_EXP)); //(unsigned int)pow(2, 32 - CAR_EXP);

//#include "Train.hpp"
#include "Lists.hpp"
#include "ObjectDescriptor.hpp"
#include "VMObject.hpp"
#include "MemoryBlock.hpp"





class CarTrain_t {
public:
                              CarTrain_t(void): train(0), car(0) {};
  void                        setTrain(unsigned int _train) {train = _train;}
  unsigned int                getTrain(void) {return train;}
  void                        setCar(unsigned int _car) {car = _car;}
  unsigned int                getCar(void) {return car;}
  
private:
  unsigned int                train;
  unsigned int                car;
};





// this class is now unused!
class Generation_t {
public:
  virtual                     ~Generation_t() {};

  virtual VMObject_t         *addNewObject(ObjectDescriptor_t *objDes) = 0;
  virtual VMObject_t         *addRootObject(ObjectDescriptor_t *objDes) = 0;
  virtual void                garbageCollect() = 0; //pure virtual
#ifdef DEBUG
  virtual void                heapDump(void) = 0;
  virtual VMObject_t        **gatherObjRefs(VMObject_t **vmoRefArray) = 0;
  virtual int                 checkObjRefs(VMObject_t **vmoRefArray, unsigned int length) = 0;
#endif
protected:
  VMObject_t                 *rootObject;
  void                       *heapPointer;                
                                //beginning of heap - not to be changed
  void                       *rootSpacePointer;           
                                //beginning of root space
  void                       *freePointer;                
                                //start of free heap space
  unsigned int                heapSize;                      
                                //total size of heap
};





class TrainGeneration_t/*:public Generation_t*/ {
public:
                              TrainGeneration_t(int heapSizeInCars);
  //TrainGeneration_t(TrainGeneration_t*);
  /*virtual*/                    ~TrainGeneration_t();
  /*virtual*/ VMObject_t         *addNewObject(ObjectDescriptor_t *objDes);
  /*virtual*/ VMObject_t         *addRootObject(ObjectDescriptor_t *objDes);
  /*virtual*/ void                garbageCollect(void);
  /*  Train_t                    *addTrain(void);
  void                        renumberTrains(void);
  Car_t                      *getCarAddress(void);
  void                        freeCar(Car_t* carPtr);
  */
#ifdef DEBUG
  /*virtual*/ void                heapDump(void);
  /*virtual*/ VMObject_t        **gatherObjRefs(VMObject_t **vmoRefArray);
  /*virtual*/ int                 checkObjRefs(VMObject_t **vmoRefArray, unsigned int length);
#endif

  inline CarTrain_t          *getCarTrain(void *address);
                                //returns a pointer to car and train for
                                //something at "address"
private:
  inline unsigned int         addr2index(void *address);


private:
  unsigned int                lastMemBlkSize;
                                //the size (in cars) of the last added
                                //memory block
  Lst<MemoryBlock_t>          memoryBlocks;

  //a quick hack...
  Car_t                      *allocCar;
  unsigned int                nextTrainNumber;
  unsigned int                nextCarNumber;
  /*  Train_t                    *trainList;
  Train_t                    *lastTrain;
  Train_t                    *rootTrain;
  unsigned int                nextTrainNumber; */
                                //to enumerate trains

  CarTrain_t                  adr2CarTrainTbl[ADR_2_CT_SIZE];
                                //table to get car and train numbers
                                //from an address
};

#endif
