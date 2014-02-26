//$Id: PatternReference.cc,v 1.8 2001/05/10 13:25:33 hempe Exp $

#include "vm.hpp"

void* PatternReference_t::operator new(size_t size, void *location){
  return location;
}

void *PatternReference_t::operator new(size_t size) {
  return malloc(sizeof(PatternReference_t));
}

PatternReference_t::PatternReference_t(Pattern_t* qualification, 
				       Pattern_t* value)
{
  //doing this->value = value;
  if(value){
    setVMReference(this, (void**)&(this->value), value);
  }
  //doing this->qualification = qualification;
  if(qualification){
    setVMReference(this, (void**)&(this->qualification), qualification);
  }
}

unsigned int PatternReference_t::getQualificationOffset(void) {
  return ((unsigned int)&qualification - (unsigned int)this)/sizeof(void*);
}

unsigned int PatternReference_t::getValueOffset(void) {
  return ((unsigned int)&value - (unsigned int)this)/sizeof(void*);
}

Pattern_t* PatternReference_t::getQualification(void){
  return qualification;
}

Pattern_t* PatternReference_t::getValue(void){
  return value;
}

void PatternReference_t::setValue(Pattern_t* ptn){
#ifdef NEW_STYLE_GC
  if(ptn){
    setVMReference(this, (void**)&value, ptn);
  } else {
    value = 0;
  }
#else //NEW_STYLE_GC
  setVMReference(this, (void**)&value, ptn);
#endif //NEW_STYLE_GC
}
