//$Id: VMObject.hpp,v 1.11 2001/05/02 09:56:13 ilsoe Exp $

#ifndef _VMOBJECT_HPP_
#define _VMOBJECT_HPP_

#include <iostream.h>

class VMObject_t;

ostream& operator << (ostream& stream, VMObject_t* vmo);
ostream& operator << (ostream& stream, VMObject_t& vmo);

#include "ObjectDescriptor.hpp"
//#include "Car.hpp"

class VMObject_t {
public:
  void * operator             new(size_t mySize, void *address,
				  ObjectDescriptor_t *_objDesc);
                              VMObject_t(void);
#ifdef COMPACT_VMO
  void                        setObjectDescriptor(ObjectDescriptor_t *_objDesc) { objDesc = (((unsigned int)_objDesc) >> 1); }
  ObjectDescriptor_t         *getObjectDescriptor(void) { return (ObjectDescriptor_t*)(((unsigned int)objDesc) << 1); }
#else //not COMPACT_VMO
  void                        setObjectDescriptor(ObjectDescriptor_t *_objDesc) { objDesc = _objDesc; }
  ObjectDescriptor_t         *getObjectDescriptor(void) { return objDesc; }
#endif
  unsigned int                getID(void) {
#ifdef OBJ_HAS_ID
    return id;
#else //OBJ_HAS_ID
    return 0;
#endif //OBJ_HAS_ID
 }
#ifdef COMPACT_VMO
  unsigned int                isForward(void) {return forward;}
  void                        setForwardBit(bool fwd) { forward = fwd;}
  void                       *getForwardPtr(void) {return (forward) ? *data : 0;}
  void                        setForwardPtr(void *newForwardPtr)
                                { forward = true;
				  *data = newForwardPtr; }
  
#else //not COMPACT_VMO
  bool                        isForward(void) {return (forward != 0);}
  void                        setForwardBit(bool fwd) { forward = (fwd) ? (void*)1: 0;}
  void                       *getForwardPtr(void) {return forward;}
  void                        setForwardPtr(void *newForwardPtr)
                                { forward = newForwardPtr; }
#endif //COMPACT_VMO
  void                       *getDataPtr(void) { return data; }
  unsigned int                getFullVMOLength()
  {
    return getObjectDescriptor()->getObjLengthBytes() + sizeof(VMObject_t) - sizeof(void*);
  }
#ifdef LIVESTAT
  void                        mark(void);
                                //marks live objects by setting their fwdPtr
#endif //LIVESTAT
  void                        printOn(ostream &stream);
private:
  static unsigned int         nextID;

private: //variables
#ifdef COMPACT_VMO
  unsigned int                objDesc:31;
  bool                        forward:1;
#else //COMPACT_VMO
  ObjectDescriptor_t         *objDesc;
  void*                       forward;
#endif //COMPACT_VMO
#ifdef OBJ_HAS_ID
  unsigned int const          id;
                                //used only for debugging
#endif //OBJ_HAS_ID
  void                       *data[1];
                                //_MUST_ BE LAST IN OBJECT
                                //this array may grow out of the object!
                                //objectDescriptor knows how much
  friend class Car_t;
  friend class IntroductorySpace_t;
  friend class StackSpace_t;
};

#endif
