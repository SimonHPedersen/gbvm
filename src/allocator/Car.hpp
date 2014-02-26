//$Id: Car.hpp,v 1.2 2001/05/21 11:42:40 hempe Exp $

#ifndef _CAR_HPP_
#define _CAR_HPP_

//#include <math.h>
#include <iostream.h>

class Car_t;

const unsigned int CAR_EXP = 16;
const unsigned int BYTES_IN_CAR = (1 << CAR_EXP);

ostream& operator << (ostream& stream, Car_t* car);
ostream& operator << (ostream& stream, Car_t& car);

//#include "gc_globals.hpp"
#include "ObjectDescriptor.hpp"
#include "VMObject.hpp"
#include "Generation.hpp"

class Car_t {
public:
  void *operator              new(size_t mySize, void *address) { return address; }
  //  void  operator              delete(void* objref_or_what);

                              Car_t():nextCar(0) {
				freePtr = scanPtr = data;
				cout << "Car_t::Car_t default constr run\n";
			      }

                              Car_t(unsigned int train, unsigned int car):nextCar(0) {
				freePtr = scanPtr = data;
#ifdef DEBUG
				if(!tg) {
				  cerr << "Car_t::Car(..): ERROR: TrainGeneration"
				    " not initialized\n";
				  exit(1);
				}
#endif
				CarTrain_t *ct = tg->getCarTrain(this);
				ct->setTrain(train);
				ct->setCar(car);
			      }
  
  Car_t                      *next() { return nextCar; }
  void                        setNext(Car_t *next) {
                                nextCar = next;
                              }
  VMObject_t                 *allocateVMO(ObjectDescriptor_t *objDesc);
  void                        printOn(ostream &stream);
  static void                 setTrainGeneration(TrainGeneration_t *_tg)
                                //sets reference to the train generation
                                { tg = _tg;}
private:
  size_t                      availSpace(void);
  static TrainGeneration_t   *tg;
                                //the generation the cars belong to
private:
  Car_t                      *nextCar;
                                //used both in trains and MemoryBlocks
                                //freelists
  void                       *freePtr;
                                //pointer to the next free memory in car
  void                       *scanPtr;
                                //pointer used during garbage collection
  void                       *data[1];
                                //the actual data, the data proceeds out of the object
};


#endif
