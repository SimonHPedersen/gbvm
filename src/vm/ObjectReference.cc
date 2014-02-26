//$Id: ObjectReference.cc,v 1.6 2001/05/10 12:54:59 hempe Exp $

#include "vm.hpp"

void* ObjectReference_t::operator new(size_t size, void *location){
  return location;
}

void *ObjectReference_t::operator new(size_t size) {
  return malloc(sizeof(ObjectReference_t));
}

ObjectReference_t::ObjectReference_t(Pattern_t* qualification, Object_t* value){
  //doing this->value = value;
  if(value){
    setVMReference(this, (void**)&(this->value), value);
  }
  //doing this->qualification = qualification;
  if(qualification){
    setVMReference(this, (void**)&(this->qualification), qualification);
  }
}


unsigned int ObjectReference_t::getQualificationOffset(void) {
  return ((unsigned int)&qualification - (unsigned int)this)/sizeof(void*);
}

unsigned int ObjectReference_t::getValueOffset(void) {
  return ((unsigned int)&value - (unsigned int)this)/sizeof(void*);
}

Pattern_t* ObjectReference_t::getQualification(void){
  return qualification;
}

Object_t* ObjectReference_t::getValue(void){
  return value;
}

void ObjectReference_t::setValue(Object_t* obj){
#ifdef NEW_STYLE_GC
  if(obj){
    setVMReference(this, (void**)&value, obj);
  } else {
    value = 0;
  }
#else //NEW_STYLE_GC
  setVMReference(this, (void**)&value, obj);
#endif
}
