//$Id: Generation.hpp,v 1.1 2001/03/12 13:38:38 ilsoe Exp $

#ifndef _GENERATION_HPP_
#define _GENERATION_HPP_

class Generation_t;
class MatureGeneration_t;

#include "Train.hpp"
#include "Car.hpp"



class Generation_t {
public:
                              Generation_t(){ };
                              Generation_t(int size);
  virtual void                init(int size);
  virtual                     ~Generation_t();

  virtual VMObject_t         *addNewObject(ObjectDescriptor_t *objDes) = 0;
  virtual VMObject_t         *addRootObject(ObjectDescriptor_t *objDes) = 0;
  virtual void                garbageCollect() = 0; //pure virtual
  //virtual                   int ownsObject(VMObject_t* vmo); 
                                //does this generation own the object?
  //virtual VMObject_t*       promoteObject(VMObject_t* vmo);
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
/*
class YoungGeneration_t:public Generation_t {
public:
                              YoungGeneration_t(int size);
                              //YoungGeneration_t(YoungGeneration_t*);
                              ~YoungGeneration_t();
  void                        changeRememberReference(VMObject_t *vmo,
			                              VMObject_t *oldRef,
			                              VMObject_t *newRef);
  void                        updateRememberSetReferences(VMObject_t *obj, 
				                          VMObject_t *newLocation);
  virtual void                garbageCollect();
private:
  //int                       fromSpace;
  void                       *scanPointer;              
                                //next object to scan during garbage collection
  void                       *currentStartHeapPointer;  
                                //start of the current semi space
  void                       *currentEndHeapPointer;    
                               //end of the current semi space (address excluded)
  RememberedSet_t            *rememberedSetPtr;
};
*/
class MatureGeneration_t:public Generation_t {
public:
                              MatureGeneration_t(int heapSize);
  //MatureGeneration_t(MatureGeneration_t*);
                              ~MatureGeneration_t();
  virtual VMObject_t         *addNewObject(ObjectDescriptor_t *objDes);
  virtual VMObject_t         *addRootObject(ObjectDescriptor_t *objDes);
  virtual void                garbageCollect(void);
  Train_t                    *addTrain(void);
  void                        renumberTrains(void);
  Car_t                      *getCarAddress(void);
  void                        freeCar(Car_t* carPtr);
#ifdef DEBUG
  virtual void                heapDump(void);
  virtual VMObject_t        **gatherObjRefs(VMObject_t **vmoRefArray);
  virtual int                 checkObjRefs(VMObject_t **vmoRefArray, unsigned int length);
#endif
private:
  Train_t                    *trainList;
  Train_t                    *lastTrain;
  Train_t                    *rootTrain;
  unsigned int                nextTrainNumber; 
                                //to enumerate trains
};

#endif
