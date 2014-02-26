//$Id: Mixin.cc,v 1.17 2001/05/25 09:31:20 hempe Exp $

#include "vm.hpp"

void* Mixin_t::operator new(size_t size){
  DB_INDENT
    DB_OUT(vm_mixin, "Mixin_t:new() called - * this should only happen " << 
	   "during initialization*\n");
  DB_OUTDENT;
  return malloc(size); 
}

void* Mixin_t::operator new(size_t size, void* location){
  return location;
}

Mixin_t::Mixin_t():
  mainPart(*((MainPart*)0))
{
}

Mixin_t::Mixin_t(MainPart* mainPart, PartObject_t* origin):
  mainPart(*mainPart)
{
  DB_INDENT;

  //setting origin for the second time
  // - it must be initialized before constructor
  DB_OUT(vm_mixin, "Mixin_t::Mixin_t origin address: " << origin << endl);
  if(origin){
    setVMReference(this, (void**)&(this->origin), origin);
  }
  DB_OUT(vm_mixin, "Mixin_t::Mixin_t origin address: " << origin << endl);
  DB_OUTDENT;
}

unsigned int Mixin_t::getOriginOffset(void) {
  DB_INDENT;
  DB_OUT(vm_mixin, "Mixin_t::getOriginOffset(): &origin: " << &origin 
       << " this: " << this << endl);
  DB_OUTDENT;
  return ((unsigned int)&origin - (unsigned int)this)/sizeof(void*);
}

PartObject_t *Mixin_t::getOrigin(void){
  return origin;
}

MainPart *Mixin_t::getMainPart(void){
  return &mainPart;
}

PartObject_t *Mixin_t::instantiate(Object_t* object) {
  //we do lazy initialization
  DB_INDENT;
  DB_OUT(vm_mixin, "Mixin_t::instantiate(): mainPart: " 
	 << (void*)&mainPart << endl); 

  ObjectDescriptor_t *objDes = getMainPart()->getObjectDescriptor();
  DB_OUT(vm_mixin, "Mixin_t::instantiate(): got object descriptor from" 
	 << " main part\n");
  DB_OUTDENT;
  return new(allocateVM(objDes)) UserPartObject_t(this, object);
}

Lst<Instruction>* Mixin_t::getInstructionListPtr(void) {
  return mainPart.getInstructionListPtr();
}

Lst<Instruction>* Mixin_t::getMPAttrsInstrPtr(void) {
  return mainPart.getMPAttrsInstrPtr();
}

Lst<Instruction>* Mixin_t::getAttrInstrPtr(unsigned int index) {
  return mainPart.getAttrInstrPtr(index);
}

void *PredefinedStringMixin_t::operator new(size_t size, void *location){
  return location;
}
 
PredefinedStringMixin_t::PredefinedStringMixin_t(PartObject_t* origin){
  if(origin){
    setVMReference(this, (void**)&(this->origin), origin);
  }
}
   
PartObject_t *PredefinedStringMixin_t::instantiate(Object_t *object){
  PredefinedStringPartObject_t *prePO = 
    new (allocateVM(predefStringPODesc)) PredefinedStringPartObject_t(this, 
								      object);
  return prePO;
}
 
MainPart* PredefinedStringMixin_t::getMainPart(void){
  return sharedStringPredefMP;
}
   
Lst<Instruction> *PredefinedStringMixin_t::getMPAttrsInstrPtr(void){
  return getMainPart()->getMPAttrsInstrPtr();
}
 

