//$Id: Car.hpp,v 1.37 2001/06/08 10:26:23 ilsoe Exp $

#ifndef _CAR_HPP_
#define _CAR_HPP_

//#include <math.h>
#include <iostream.h>

class Car_t;

#ifndef CAR_EXP
// default value is 16
#  define CAR_EXP 16
#endif //CAR_EXP

const unsigned int BYTES_IN_CAR = (1 << CAR_EXP);  //typically 0x00010000
const unsigned int CAR_MASK = ~(BYTES_IN_CAR - 1); //typically 0xffff0000

#ifdef NEW_STACK_RESCUE
# ifndef CAR_FILL_THR
#  define CAR_FILL_THR 0.8
# endif //CAR_FILL_THR
const unsigned int CAR_FILL_THR_BYTES = (unsigned int) CAR_FILL_THR * BYTES_IN_CAR;
#endif //NEW_STACK_RESCUE

ostream& operator << (ostream& stream, Car_t* car);
ostream& operator << (ostream& stream, Car_t& car);

class Car_t {
public:
  void *operator              new(size_t mySize, void *address)
                                { return address; }
  void  operator              delete(void* objref_or_what) {}

                              Car_t();
                              Car_t(unsigned int train, unsigned int car);

                              ~Car_t();

  Car_t                      *next() { return nextCar; }
  void                        setNext(Car_t *next) {
                                nextCar = next;
                              }
  Car_t                      *getNext(void) {
                                return nextCar;
                              }
  VMObject_t                 *allocateVMO(ObjectDescriptor_t *objDesc);
  bool                        member(void *address);
  void                       *moveObj(void *object);
                                //the primary external interface to moving
                                //objects into this car or the last or a
                                //new car
#ifdef NEW_STACK_RESCUE
  void                       *moveObjHere(void *object);
                                //the secondary external interface to
                                //moving objects into this car only.
                                //returns 0 if car is too filled up
#endif //NEW_STACK_RESCUE

private:
  void                       *copyObj(void *object);
                                //takes an object reference, converts it
                                //to a vmo, copies it, and returns the
                                //location of the new object (not VMO)
public:
  void                        doGCScan(void);
                                //scan this car's new objects, that is
                                //the objects between scanPtr and freePtr
  void                        moveExtObjects(void);
                                //scan remembered set for train-external
                                //references, move these objects out of
                                //the car
  void                        moveIntObjects(void);
                                //scan remembered set for train-internal
                                //references, move these objects out of
                                //the car
  void                        printOn(ostream &stream);
  bool                        isExtRefdTrain(void);
                                //must check if the external ref still 
                                //points into the car
  void                        addExtRememberReference(void** refAdr) {
                                extRemSet->insert(refAdr);
                              }
  void                        addIntRememberReference(void** refAdr) {
                                intRemSet->insert(refAdr);
                              }
#ifdef DEBUG
  bool                        hasExtRememberReference(void** refAdr);
  bool                        hasIntRememberReference(void** refAdr);
  void                        gatherObjRefs(VMOHash_t *vmoRefs);
  int                         checkObjRefs (VMOHash_t *vmoRefs);
#endif //DEBUG
#ifdef LIVESTAT
  void                        doLiveStat(size_t *live, size_t *dead, size_t *unused);
                                //sizes in bytes will be added
#endif //LIVESTAT
private:
  size_t                      availSpace(void);
  //  static TrainGeneration_t   *tg;
  //                                //the generation the cars belong to
public:
  static Car_t               *getCar(void* v) {
                                return (Car_t*)(((int)v)&CAR_MASK);
                              }
private:
  Car_t                      *nextCar;
                                //used both in trains and MemoryBlocks
                                //freelists
  void                       *freePtr;
                                //pointer to the next free memory in car
  void                       *scanPtr;
                                //pointer used during garbage collection
  RememberedSet_t            *extRemSet;
  RememberedSet_t            *intRemSet;
#ifdef undefined //LIVESTAT
public:
  int                         liveObjects;
  int                         deadObjects;
  size_t                      liveBytes;
  size_t                      deadBytes;
private:
#endif //LIVESTAT
  void                       *data[1];
                                //the actual data, the data proceeds out of the object
};


#endif
