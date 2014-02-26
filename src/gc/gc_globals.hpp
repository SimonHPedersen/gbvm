//$Id: gc_globals.hpp,v 1.10 2001/03/27 08:34:16 ilsoe Exp $
#ifndef GC_GLOBALS_HPP
#define GC_GLOBALS_HPP

#include <iostream.h>
#include <stdlib.h>

//size of array for storing object references during consistency check
// - only used during debugging
#define CHECK_INTEGRITY_MAX_OBJS 10000

#define VMO_FROM_VOIDP(voidPtr) ((VMObject_t*)((unsigned int)voidPtr - sizeof(VMObject_t) + sizeof(void*)))
#define VOIDP_FROM_VMO(vmo) ((void*)((unsigned int)vmo + sizeof(VMObject_t) - sizeof(void*)))

//macro to find the next object in car using object descriptors, will have to be checked
//against freeDataObjectPtr in car
#define NEXT_VMO(vmo) ((VMObject_t*)((unsigned int)vmo                                   \
				   + (sizeof(VMObject_t)                                 \
				      +	vmo->getObjectDescriptor()->getObjLengthBytes()) \
				   - sizeof(void*)))


#ifdef DEBUG
extern unsigned int collected;
extern unsigned int allocated;
#endif

//use this to allocate space for a new object in the vm,
//- use this as input to placement new
#define allocateVM(objectDesc) \
((garbageCollector->addNewObject(objectDesc))->getDataPtr())

#define allocateRootVM(objectDesc) \
((garbageCollector->addRootObject(objectDesc))->getDataPtr())


//use this to set a reference in the virtual machine (reference in source to target)
#ifndef SETVMREFERENCE_IS_MACRO
extern void setVMReference(void *source, void **refAdr, void *target);
#else
# ifdef NO_GC
#  define setVMReference(source, refAdr, target) (*refAdr = target);
# else
#  define setVMReference(source, refAdr, target) {                                         \
  DB_INDENT;                                                                               \
  VMObject_t *src = VMO_FROM_VOIDP(source);                                                \
  DB_OUT(gc_glob_setvmr, "setVMReference(): source at " << source << " vmobject at: "      \
	 << (void*)src << " refAdr: " << refAdr << endl);                                  \
  /*check if we have to remove old remember reference pointing to src from *refAdr's remset*/\
  if(*refAdr) {                                                                            \
    DB_OUT(gc_glob_setvmr_verb, "setVMReference(): 1" << endl);                            \
    VMObject_t *oldRef = VMO_FROM_VOIDP(*refAdr);                                          \
    DB_OUT(gc_glob_setvmr_verb, "setVMReference(): 2 oldRef: " << (void*)oldRef << endl);  \
    oldRef->clearRememberReference(src);                                                   \
    DB_OUT(gc_glob_setvmr_verb, "setVMReference(): 3" << endl);                            \
  }                                                                                        \
  /*add reference to this object*/                                                         \
  DB_OUT(gc_glob_setvmr_verb, "setVMReference(): before setReference\n");                  \
  src->setReference(refAdr, target);                                                       \
  DB_OUTDENT;                                                                              \
}
#endif
#endif

#include "VMObject.hpp"

#endif
