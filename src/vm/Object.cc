//$Id: Object.cc,v 1.13 2001/05/10 12:36:57 hempe Exp $

#include "vm.hpp"

void *Object_t::operator new(size_t size) { 
  DB_INDENT;
  DB_OUT(vm_object, "Object_t:new() called - " 
	 << "* this should only happen during initialization*\n");
  DB_OUTDENT;
  return malloc(size);
}

void *Object_t::operator new(size_t size, void* location) {
  return location;
}

Object_t::Object_t() {
  DB_INDENT;
  DB_OUT(vm_object, "Object_t::Object_t:"
	 " --- initializing object descriptor constructor ---\n");
  DB_OUTDENT;
}

Object_t::Object_t(Pattern_t *ptn) {
  setVMReference(this, (void**)&pattern, ptn);
}

unsigned int Object_t::getHeadOffset() {
  //returns the offset to the head pointer (in void pointers)
  DB_INDENT;
  DB_OUT(vm_object, "Object_t::getHeadOffset(): " 
       << ((unsigned int)&head - (unsigned int)this)/sizeof(void*) << endl);
  DB_OUTDENT;
  return ((unsigned int)&head - (unsigned int)this)/sizeof(void*);
}

unsigned int Object_t::getTailOffset() {
  //returns the offset to the tail pointer (in void pointers)
  DB_INDENT;
  DB_OUT(vm_object, "Object_t::getTailOffset(): " 
       << ((unsigned int)&tail - (unsigned int)this)/sizeof(void*) << endl);
  DB_OUTDENT;
  return ((unsigned int)&tail - (unsigned int)this)/sizeof(void*);
}

unsigned int Object_t::getPatternOffset() {
  //returns the offset to the tail pointer (in void pointers)
  DB_INDENT;
  DB_OUT(vm_object, "Object_t::getPatternOffset(): " 
       << ((unsigned int)&pattern - (unsigned int)this)/sizeof(void*) << endl);
  DB_OUTDENT;
  return ((unsigned int)&pattern - (unsigned int)this)/sizeof(void*);
}


void Object_t::initPartObjects(Thread_t *myThread) {
  //iterate all partobjects 
  for(PartObject_t* po = first(); po !=NULL; po=po->next()){
    po->initAttributes(myThread);
  }
}

Instruction *Object_t::getFirstInstructionPushPOANF(Thread_t *myThread) {
  DB_INDENT;
  DB_OUT(vm_object, "Object_t::getFirstInstructionPOANF()\n");
  DB_OUTDENT;
  if(first()){
    return first()->getFirstInstructionPushPOANF(myThread);
  } else {
    return NULL;
  }
}
