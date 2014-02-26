//$Id: Pattern.cc,v 1.18 2001/05/21 11:42:51 hempe Exp $

#include "vm.hpp"

void* Pattern_t::operator new(size_t size, void* location){
  return location;
}

Pattern_t::Pattern_t(ObjectDescriptor_t *objDesc){
  DB_INDENT;
  //one of the mixins is within the Pattern so the number of mixins is 
  //the complete length from object descriptor minus the size used for 
  //the Pattern_t object + 1
  length = objDesc->getObjLengthRefs() - sizeof(Pattern_t)/sizeof(void*) + 1;
  //clear mixin array (FIRST)
  for(unsigned int i = 0; i < length; i++){
    mixins[i] = 0;
  }
  //mark mixin refs in objdesc (for gc)
  unsigned int patternLengthInRefs = objDesc->getObjLengthRefs();
  unsigned int mixinsStart = ((unsigned int)&mixins - (unsigned int)this)/sizeof(void*);
  DB_OUT(vm_ptn, "Pattern_t::Pattern_t() mixinStart: " << mixinsStart << endl);
  for(unsigned int mixinRef = mixinsStart; mixinRef < patternLengthInRefs; mixinRef++) {
    objDesc->setReferenceAt(mixinRef);
  }
  DB_OUTDENT;
}

void Pattern_t::addMixin(unsigned int index, Mixin_t* mixinPtr){
  //doing: mixins[index] = mixinPtr;
  DB_INDENT;
  if(mixinPtr){
    setVMReference(this, (void**)&mixins[index], mixinPtr); 
  }
  DB_OUT(vm_ptn, "Pattern_t::addMixin(): added mixin: " << mixins[index] << " mixinPtr: " << mixinPtr << endl);
  DB_OUTDENT;
}
	
Object_t* Pattern_t::instantiate(Thread_t *myThread){
  DB_INDENT;
  void *obj = allocateVM(objectDesc);
  Object_t *theObject = new(obj) Object_t(this);

  for(unsigned int i = 0; i < length; i++){
    if(mixins[i]){
      DB_OUT(vm_ptn, "Pattern_t::instantiate(): before " << i 
	   << "th instantiation mixins[i] " << mixins[i] << endl);
      PartObject_t* po = mixins[i]->instantiate(theObject);
      DB_OUT(vm_ptn, "Pattern_t::instantiate() mixin instantiated \n");
      theObject->add(po, obj);
      DB_OUT(vm_ptn, "Pattern_t::instantiate() part object added to object \n");
    }
  }
  //initialize attributes in partobjects of object DEPRECATED !!!!!!!!!
  //theObject->initPartObjects(myThread);
  DB_OUTDENT;
  return theObject;
}

 // --- EmptyPattern ---
  
void* EmptyPattern_t::operator new(size_t size, void* location){
  return location;
}
  
void EmptyPattern_t::addMixin(unsigned int index, Mixin_t* mixinPtr){
  cerr << "EmptyPattern_t::addMixin(): IDIOT" << endl;
  exit(1);
}
 	
Object_t* EmptyPattern_t::instantiate(Thread_t *myThread){
  DB_INDENT;
  Object_t *theObject = new(allocateVM(objectDesc)) Object_t(this);
  DB_OUTDENT;
  return theObject;
}
