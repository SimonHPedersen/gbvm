//$Id: Car.hpp,v 1.31 2001/04/04 13:35:05 hempe Exp $
#ifndef GARBAGE_CAR_HPP
#define GARBAGE_CAR_HPP

#include <typeinfo>
#include <stdio.h>
#include <iostream.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

class Car_t;

#include "globals.hpp"
#include "ObjectDescriptor.hpp"
#include "VMObject.hpp"
#include "Train.hpp"
#include "rememberedset.hpp"
#include "garbage_collector.hpp"
#include "Generation.hpp"

ostream& operator << (ostream& stream, Car_t* train);
ostream& operator << (ostream& stream, Car_t& train);

//#define NUM_OBJS_IN_CAR 4
//DO NOT USE BYTES_IN_CAR FOR THE NEW OPERATOR, USE sizeof(Car_t) INSTEAD!!!
//#define BYTES_IN_CAR 2048
#ifdef DEBUG
# define BYTES_IN_CAR (1<<16)
#else
# define BYTES_IN_CAR 4096
#endif

//#define BYTES_IN_CAR 1024 //testsize!
//#define BYTES_IN_CAR 65536 //very reasonable size
//#define BYTES_IN_CAR 200000 //needed for simon's instruction test
#define NEW_CAR_FREE_THRESHOLD (unsigned int)(BYTES_IN_CAR * 0.2)
//#define NEW_CAR_FREE_THRESHOLD 10 //test
class Car_t {
public:
  void* operator              new(size_t size, void* address);
  void  operator              delete(void* objref_or_what);

                              Car_t(MatureGeneration_t &matureGenPtr, Train_t &trainPtr,
				    unsigned int numberPtr);
                              ~Car_t();

  /* ----- public methods -----*/
  Train_t &                   getTrainRef(void);
  unsigned int                getCarNumber(void);
  void                        setNextCarPtr(Car_t *nextCar);
  Car_t*                      getNextCarPtr();
  VMObject_t                 *addNewObject(ObjectDescriptor_t *objDes);
  VMObject_t*                 addObject(VMObject_t *vmo, RememberedSetNode_t *remSetNodePtr);
                                //adds an object (vmo) pointed to this car (copies it)
  VMObject_t*                 isReferencedOutsideCar(void);
                                //are any of the objects in the car referenced
                                //outside the car itself?
                                //- returns the first such object or 0

  VMObject_t*                 isReferencedOutsideTrain(void);
                                //are any of the objects in the car referenced
                                //from outside the train?
                                //- returns the first such object or 0

  int                         addRememberReference(VMObject_t *obj, VMObject_t *remPtr);
                                //adds remPtr to obj's remembered set. Returns 0 if obj
                                //was not in remembered set, nonzero otherwise

  void                        addRememberReference(unsigned int thisTrainNum, VMObject_t *remRef,
						   RememberedSetNode_t *rsn,
						   RememberedObjectNode_t *node2Insert);

  void                        clearRememberReference(VMObject_t *obj, VMObject_t *remRef);
                                //removes 1 occurrence of remRef from obj's remset

  void                        changeRememberReference(VMObject_t* vmo, VMObject_t* oldPtr,
						      VMObject_t* newPtr);
                                //searches for vmo in remembered set, when found searches
                                //for oldPtr in vmo's remset.
                                //Changes all oldPtr to newPtr.
                                //If vmo was not found it must be added to remset.
                                //If oldRef was not found in vmo's remset it must be added.
                                //called on all references in an object when it is moved

  //  void                        updateRememberSetReferences(VMObject_t *vmo,
  //							  VMObject_t *oldPtr,
  //							  VMObject_t *newPtr);
                                //finds vmo's remembered set. In the remembered set objects
                                //all oldPtr's are changed to newPtr's


  //  void                        updateRememberSet(VMObject_t *vmo, VMObject_t *oldPtr,
  //					VMObject_t *newPtr);
                                //updates vmo's remembered set structure after the object
                                //has been moved.
                                //References to objects in cars before this car are removed.
                                //updateRememberSetReferences() should be called BEFORE this
                                //method


  RememberedSetNode_t*        getCarsRememberSetPtr(void);

  RememberedSetNode_t*        getObjectsRememberSetPtr(VMObject_t *vmo);
                                //find remembered set for object vmo

  void                        printOn(ostream &stream);

#ifdef DEBUG
  VMObject_t                **gatherObjRefs(VMObject_t **vmoRefArray);
  int                         checkObjRefs(VMObject_t **vmoRefArray, unsigned int length);
#endif

  /* ----- public attributes ----- */
  MatureGeneration_t          &matureGenerationRef;

private:
  /* ----- private methods -----*/
  void*                       newRememberedSetItem(size_t objectSize);
                                //allocates a new remember set node/object in this car or the
                                //overflowCar - data in it is uninitialized

  /* ----- private attributes ----- */
  unsigned int                carNumber;
  Train_t                    &trainRef;
  Car_t                      *nextCarInTrainPtr;
  //MatureGeneration_t         &matureGenerationRef; now public!
  Car_t                      *overflowCar;
  VMObject_t                 *vmObjectList;
  //  VMObject_t                 *lastObject;
  RememberedSetNode_t        *rememberedSetPtr;
  void                       *freeDataObjectPtr;
  void                       *freeDataRemPtr;
  char                        data[BYTES_IN_CAR];

};

#endif
