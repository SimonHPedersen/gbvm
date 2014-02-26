//$Id: ObjectDescriptor.hpp,v 1.15 2001/04/10 12:01:22 ilsoe Exp $

#ifndef OBJECTDESCRIPTOR_HPP
#define OBJECTDESCRIPTOR_HPP

#include <stdlib.h>
#include <iostream.h>

class ObjectDescriptor_t;

ostream& operator << (ostream& stream, ObjectDescriptor_t* objD);
ostream& operator << (ostream& stream, ObjectDescriptor_t& objD);

#include "globals.hpp"

class ObjectDescriptor_t{
public:
                              ObjectDescriptor_t(unsigned int length, 
						 unsigned int maskValue);
                                //save length and ensure that refMask is cleared
                                //length is in BYTES
                              ~ObjectDescriptor_t();
  
  //unsigned int                getHeaderObjLength();
  unsigned int                getObjLengthBytes();
  //use VMObject_t::getFullVMOLengt() to get the full length
  unsigned int                getObjLengthRefs();
  unsigned int               *getReferenceMask();
  int                         setReferenceAt(unsigned int number);
  int                         clearReferenceAt(unsigned int number);
  int                         hasReferenceAt(unsigned int number);
  //  void                        print();
  void                        printOn(ostream& stream);
private:
  //unsigned int                headerObjLength;
                                // the length of the object inherited from VMObject
                                // (which is the c++ object length)
  unsigned int                objLength;
                                // the length of the object (not vm!) in bytes
                                // objLength = total object size - sizeof(VMObject_t) + sizeof(void*)
                                //           = sizeof data including overlapping data[0]
                                //  unsigned int referenceMask[1000];
  unsigned int               *referenceMask;
};

#endif
