//$Id: PartObject.cc,v 1.36 2001/05/25 09:31:21 hempe Exp $

#include "vm.hpp"

// Super PartObject_t -----------------------------------------

void* PartObject_t::operator new(size_t size){
  return malloc(size);
}

void* PartObject_t::operator new(size_t size, void* location){
  return location;
}

unsigned int PartObject_t::getMixinOffset(void) {
  return ((unsigned int)&mixin - (unsigned int)this)/sizeof(void*);
}

unsigned int PartObject_t::getObjectOffset(void) {
  return ((unsigned int)&object - (unsigned int)this)/sizeof(void*);
}


unsigned int PartObject_t::get_nextOffset() {
  //returns the offset to the _next pointer (in void pointers)
  return ((unsigned int)&_next - (unsigned int)this)/sizeof(void*);
}

unsigned int PartObject_t::get_prevOffset() {
  //returns the offset to the _prev pointer (in void pointers)
  return ((unsigned int)&_prev - (unsigned int)this)/sizeof(void*);
}


Object_t* PartObject_t::getObject(void){
  return object;
}

PartObject_t* PartObject_t::getOrigin(void){
  return mixin->getOrigin();
}

char* PartObject_t::getMainPartId(void){
  return mixin->getMainPart()->getId();
}

Lst<Instruction>* PartObject_t::getMPAttrsInstrPtr(void) {
  return mixin->getMPAttrsInstrPtr();
}

// UserPartObject_t -----------------------------------------

void* UserPartObject_t::operator new(size_t size, void* location){
  return location;
}

UserPartObject_t::UserPartObject_t(ObjectDescriptor_t *objDes) {
  DB_INDENT;
  DB_OUT(vm_po, "UserPartObject_t::UserPartObject_t(ObjectDescriptor_t *objDes):"
	 << " Only allowed in bytecode loading (not necessarily an error)\n");
  //mark references in object (for gc)
  unsigned int length = objDes->getObjLengthRefs();
  unsigned int refsStart = sizeof(UserPartObject_t)/sizeof(void*) - 1;
  for(unsigned int ref = refsStart; ref < length; ref++) {
    objDes->setReferenceAt(ref);
  }
  //mark prev and next pointers (for gc)
  objDes->setReferenceAt(get_nextOffset());
  objDes->setReferenceAt(get_prevOffset());
  //mark mixin reference if present (for gc)
  objDes->setReferenceAt(getMixinOffset());
  //mark my object
  objDes->setReferenceAt(getObjectOffset());
  DB_OUTDENT;
}


UserPartObject_t::UserPartObject_t(Mixin_t* mixin, Object_t* object) {
  //doing this->mixin=mixin
  DB_INDENT;
  DB_OUT(vm_po, "UserPartObject_t::UserPartObject_t(): " 
	 << "doing this->mixin=mixin\n");
  this->mixin = 0;
  if(mixin){
    setVMReference(this, (void**)&(this->mixin), mixin);
  }
  //doing this->object = object;
  DB_OUT(vm_po, "UserPartObject_t::UserPartObject_t(): " 
	 << "doing this->object=object\n");
  this->object = 0;
  if(object){
    setVMReference(this, (void**)&(this->object), object);
  }

  DB_OUT(vm_po, "UserPartObject_t::UserPartObject_t(): " 
	 << "getting nof attributes\n");  

  //clear all attributes
  if(mixin){
    unsigned int numberOfAttributes = mixin->
      getMainPart()->getNumberOfAttributes();
    
    DB_OUT(vm_po, "UserPartObject_t::UserPartObject_t(): " 
	   << "clearing attributes\n");  

    for(unsigned int i = 0; i < numberOfAttributes; i++){
      attributes[i] = 0;
    }
  }
  DB_OUTDENT;
}

void UserPartObject_t::initAttributes(Thread_t *myThread){
}

void UserPartObject_t::setAttribute(unsigned int index, 
				    AttributeInstance_t* value){
  DB_INDENT;
  DB_OUT(vm_po, "UserPartObject_t::setAttribute() at index: " << index 
       << " with address " << &attributes[index] << endl);
  setVMReference(this, (void**)&attributes[index], value);
  DB_OUTDENT;
}

AttributeInstance_t *UserPartObject_t::getAttribute(unsigned int index, 
						    Thread_t *myThread){
  DB_INDENT;
  DB_OUT(vm_po, "UserPartObject_t::getAttribute() at index: " << index 
	 << " with address " << &attributes[index] 
	 << " myMixin " << (void*)mixin << " myMainPart: " << 
	 (void*)mixin->getMainPart() << endl);
  if(index >= mixin->getMainPart()->getNumberOfAttributes()){
    cerr << "UserPartObject_t::getAttribute(): error this part object: " 
	 << this << " does not have that many attributes: " << index << endl;
    exit(1);
  }
  if(!attributes[index]){
    DB_OUT(vm_po, "UserPartObject_t::getAttribute(): dynamic initialisation " 
	   << "necessary setting thread->dynamicInitInst\n");
    myThread->dynamicInitInst = mixin->getAttrInstrPtr(index)->first();
    DB_OUT(vm_po, "UserPartObject_t::getAttribute(): setting " 
	   << "thread->dynamicInitInst: "
           << myThread->dynamicInitInst);
    myThread->dynamicInitPO = this;
  }
  DB_OUT(vm_po, "UserPartObject_t::getAttribute(): returning: " 
	 << attributes[index] << endl);
  DB_OUTDENT;
  return attributes[index];
}

AttributeInstance_t **UserPartObject_t::getAttributeAddress(unsigned int index){
  return &attributes[index];
}

Instruction *UserPartObject_t::getFirstInstructionPushPOANF(Thread_t *myThread){
  DB_INDENT;
  DB_OUT(vm_po, "UserPartObject_t::execute(): executing partObject\n");

  //get dopart instructions
  Lst<Instruction> *instrs = mixin->getInstructionListPtr();

  //myThread->setCurrentPartObject(this);

  //Side effect 1 !!! - this method also pushes this PO to currentPOStack
  myThread->currentPOStack->push(this);

  //Side effect 2 !!! - this method also adds a new frame to the tmpStack
  myThread->tmpStack->addNewFrame();

  DB_OUTDENT;
  return instrs->first();
}

//-- private methods of UserPartObject

void UserPartObject_t::initAttribute(unsigned int index, Thread_t *myThread){
  cerr << "UserPartObject_t::initAttribute(): not implemented/used(?)\n";
  exit(1);
}

AttributeVirtual *UserPartObject_t::getVirtAttr(char *parentAttrName,
						int parentAttrIndex,
						char *parentMainPartID) {
  MainPart *mp = mixin->getMainPart();
  return (mp->getVirtAttr(parentAttrName, parentAttrIndex, parentMainPartID));
}


// --------- PredefinedStringPartObject_t -----------------------------------

PredefinedStringPartObject_t::PredefinedStringPartObject_t(Mixin_t* mixin, 
							   Object_t* object){
  //doing: this->mixin = mixin
  if(mixin){
    setVMReference(this, (void**)&(this->mixin), mixin);
  }
  //doing: this->object = object;
  if(object){
    setVMReference(this, (void**)&(this->object), object);
  }
}

Instruction* PredefinedStringPartObject_t::getFirstInstructionPushPOANF(Thread_t
									*myThread){
  if(next()){
    return next()->getFirstInstructionPushPOANF(myThread);
  } else {
    return NULL;
  }
}

unsigned int PredefinedStringPartObject_t::getStrOffset(){
  return ((unsigned int)&str - (unsigned int)this)/sizeof(void*);
}

char* PredefinedStringPartObject_t::getStr(){
#ifdef DEBUG
  if(!str){
    cerr << "WARNING: PredefinedStringPartObject_t::getStr(): str = 0x0" << endl;
    cerr << "instructions executed: " << instrsExecuted << endl;
  }
#endif
  return str;
}

void PredefinedStringPartObject_t::setStr(char* _str){
#ifdef NEW_STYLE_GC
  str = _str;
#else //NEW_STYLE_GC
  setVMReference(this, (void**)&str, _str);
#endif //NEW_STYLE_GC
}
