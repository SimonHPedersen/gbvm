//$Id: rememberedset.hpp,v 1.5 2000/10/11 14:07:28 ilsoe Exp $

#ifndef REMEMBEREDSET_HPP
#define REMEMBEREDSET_HPP

class RememberedSet_t;
class RememberedSetNode_t;
class RememberedObjectNode_t;

// ----- data structures used internally in RememberedSet_t -----

typedef struct dataArrayStruct* dataArrayStruct_t;
typedef struct dataArrayStruct {
  int length;
  void* dataBlockPtr;
} keld;

// ----- The important class in this file -----

class RememberedSet_t {
public:
  RememberedSet_t();
  ~RememberedSet_t();
  RememberedSetNode_t* getRememberRoot();
  void addRememberReference(VMObject_t *obj, VMObject_t *remRef);
  void changeRememberReference(VMObject_t *obj, VMObject_t *oldRemRef,
			       VMObject_t *newRemRef);
  void updateRememberSetReferences(VMObject_t *obj, VMObject_t *newLocation);
private:
  dataArrayStruct_t dataArrayPtr;
  unsigned int dataArraySize;
  RememberedSetNode_t* rememberRoot;
};

// ----- data structure in remembered set -----

class RememberedSetNode_t {
public:
  RememberedSetNode_t*    nextRemSetNodePtr;
  RememberedObjectNode_t* firstRemObjNodePtr;
  RememberedObjectNode_t* lastRemObjNodePtr;
  VMObject_t*             vmIdObjectPtr;
private:
};

class RememberedObjectNode_t {
public:
  RememberedObjectNode_t* nextRemObjNodePtr;
  VMObject_t*             vmRemObjectPtr;
private:
};

#endif
