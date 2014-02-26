//$Id: Train.hpp,v 1.14 2001/03/12 13:39:42 ilsoe Exp $
#ifndef GARBAGE_TRAIN_H
#define GARBAGE_TRAIN_H

#define MAX_NUMBER_OF_CARS 2

#include <stdio.h>
#include <iostream.h>
#include <stdlib.h>

class Train_t;

#include "ObjectDescriptor.hpp"
#include "Car.hpp"
#include "VMObject.hpp"
#include "garbage_collector.hpp"
#include "Generation.hpp"
#include "rememberedset.hpp"

ostream& operator << (ostream& stream, Train_t* train);
ostream& operator << (ostream& stream, Train_t& train);

class Train_t {
public:
                              Train_t(MatureGeneration_t& matureGenRef, unsigned int number,
				      unsigned int maxCars);
                              ~Train_t();
  VMObject_t                 *addNewObject(ObjectDescriptor_t *objDes);
                                //allocates memory in a car somewhere for a new object

  int                         isExternallyReferenced(); 
                                //is the train referenced from outside the train itself?

  VMObject_t *                moveObject(VMObject_t * vmo, 
					 RememberedSetNode_t* remSetNode, 
					 Car_t* car,
					 int forced); 
                                //add object to train in car if not possible
                                //then add to the last car if not possible
                                //then add to a new car to the train and if not possible
                                //add a new train and add the object to this

  //  VMObject_t *                moveObjectForced(VMObject_t * vmo, 
  //					       RememberedSetNode_t* remSetNode, 
  //					       Car_t* car); 
                                //add object to train in car if not possible
                                //then add to the last car and if not possible
                                //then add a new car and add the object to this

  //void                      freeTrain();   
                               //free all cars in train and free the train == destructor

  void                        freeFirstCar();  
                               //free the first car

  Car_t                      *getFirstCar();
  Car_t                      *getLastCar();
  unsigned int                getTrainNumber();
  void                        setTrainNumber(unsigned int trainNumber);
                                //Only used when train numbers wrap around and we need
                                //to start over
  void                        setNextTrainPtr(Train_t* train);
  Train_t                    *getNextTrainPtr();

  void                        printOn(ostream& dest);
                                //print object on stream including all cars

#ifdef DEBUG
  VMObject_t                **gatherObjRefs(VMObject_t **vmoRefArray);
  int                         checkObjRefs(VMObject_t **vmoRefArray, unsigned int length);
#endif
private:
  VMObject_t                 *addObject(VMObject_t * vmo, 
					RememberedSetNode_t* remSetNode,
					int forced); 
                                //add object last car in to train

  unsigned int                addCar();
                                //add new car to train if possible

  void                        addCarForced();
                                //add new car to train

  Train_t                    *nextTrainPtr;
  unsigned int                trainNumber;
  unsigned int                nextCarNumber; 
                                //to enumerate the cars in this train 
                                //(not used to manage carlist)
  unsigned int               maxCars;
                                //the maximum number of cars in the train

  Car_t                      *carList;
  Car_t                      *lastCar;
  unsigned int                numberOfCars;
  MatureGeneration_t&         matureGenerationRef;
};

#endif

