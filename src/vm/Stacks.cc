//$Id: Stacks.cc,v 1.7 2001/05/10 12:37:00 hempe Exp $

//#include "Thread.hpp"
//#include "Stacks.hpp"

#include "vm.hpp"


ObjectDescriptor_t *getStackObjDesc(unsigned int size, unsigned int maskValue){
  ObjectDescriptor_t *objDesc = new ObjectDescriptor_t(size, maskValue);
  return objDesc;
}

//IMPLEMENTATION OF TEMPLATE CLASSES (STACKS) GOES IN HEADER FILE





