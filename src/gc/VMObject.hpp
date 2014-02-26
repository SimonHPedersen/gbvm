//$Id: VMObject.hpp,v 1.19 2001/03/12 13:39:42 ilsoe Exp $

#ifndef VMOBJECT_HPP
#define VMOBJECT_HPP

class VMObject_t;

#include <stdlib.h>
#include "globals.hpp"
#include "gc_globals.hpp"
#include "ObjectDescriptor.hpp"
#include "garbage_collector.hpp"
#include "Generation.hpp"

class Car_t;

//#include "Car.hpp"

/*
#define NUMBER_OF_REFERENCES 4
#define REMEMBER_SET_SIZE    4
*/

//#define ROOT_OBJECT_FLAG 1

ostream& operator << (ostream& stream, VMObject_t* vmo);
ostream& operator << (ostream& stream, VMObject_t& vmo);

class VMObject_t {
public:
  void* operator              new(size_t size, void* freePointer,
				  ObjectDescriptor_t* objDescriptor);
  void operator               delete(void* vmo);

                              VMObject_t();
  //                              VMObject_t(unsigned int id, char* name);
                              ~VMObject_t();

                              void init(void);

  ObjectDescriptor_t         *getObjectDescriptor(void);
  void                        setNextObjectInCar(VMObject_t *nextObject);
  VMObject_t                 *getNextObjectInCar(void);
  int                         getExtendedSize();

  /*
  void                        markAsRootObject();
                                //this will make the garbage collector consider this object a
                                //root object
  void                        clearAsRootObject();
                                //this will make the garbage collector stop considering this
                                //object a root object
  unsigned int                checkAsRootObject();
                                //check if this object is marked a root object
  */
  int                         getGeneration();
                                //VMObject_t** getReferences(); //use objectdescriptor now
                                //set/overwrite refence refNumber to objRef in references
  int                         setReference(int refNumber, void *objRef);
                                //update existing reference to new reference in references
  int                         setReference(void *refLocation, void *target);
  void                        changeReference(void *oldRef, void *newRef);
                                //add a new reference to remembered set
                                //int     addRememberReference(VMObject_t* newRef); now in car
                                //update existing reference to new reference in remembered set
  void                        changeRememberReference(VMObject_t* oldRef, VMObject_t* newRef);

  void                        clearRememberReference(VMObject_t *ref2Clear);
                                //Removes a (1) remember reference from this object as
                                //as indicated by ref2Clear
  void                        updateReferencesRememberSet(VMObject_t* old_vmo);
                                //void updateRememberSetReferences(VMObject_t* old_vmo);
                                //void updateRememberSet();
  //VMObject_t*                 moveToSpace(void** heapPointer);
                                //Train algorithm method to move object and followers
                                //to another train if externally referenced or to the 
                                //end of this train if internally referenced
  VMObject_t*                 moveObject();
  void                        moveReferencedObjects(Car_t* destCar, int forced);
  void                        setCarPtr(Car_t* car);
  Car_t*                      getCarPtr();
  void                        setForward(VMObject_t* forward_ptr);
  VMObject_t*                 getForward(void);
                                //get a pointer to an object referencing this object from _another_
                                //train or NULL if this does not exist
  VMObject_t*                 getARefObjOutsideTrain();
  VMObject_t*                 getARefObjOutsideCar();

  void                        setID(unsigned int id);
  unsigned int                getID(void);
  void                        printVMO();
  void                       *getDataPtr();
  void                        printOn(ostream&);

private:
  int                         isInternallyReferenced();
                                //object info
  ObjectDescriptor_t         *objectDescriptor;
  VMObject_t                 *nextObjectInCarPtr;
  unsigned int                flags;
                                //various attributes of the object (currently root-object)
  unsigned int                id;
    //garbage collection
  unsigned int                generation;
  VMObject_t                 *forwardPtr;
                                //if null this is not a forwarding object
  Car_t                      *carPtr;
  //  Car_t                      *oldCarPtr; //removed in VMObject::setCarPtr: oldCarPtr = carPtr
                                //used in mature generation only
                                //VMObject_t* rememberedSet[REMEMBER_SET_SIZE];
                                //userfields - early implementation - only references are allowed now
                                //VMObject_t* references[NUMBER_OF_REFERENCES];
#ifdef DEBUG
public:
#endif
  void                       *data[1]; //_MUST_ BE LAST IN OBJECT
                                //this array may grow out of the object!
                                //objectDescriptor knows how much
};

#endif




