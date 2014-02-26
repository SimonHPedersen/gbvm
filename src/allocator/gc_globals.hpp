//$Id: gc_globals.hpp,v 1.1 2001/03/27 09:19:43 ilsoe Exp $
#ifndef GC_GLOBALS_HPP
#define GC_GLOBALS_HPP

#include <iostream.h>
#include <stdlib.h>

//FURTHER INCLUDES AFTER MACRO DEFINITIONS

//size of array for storing object references during consistency check
// - only used during debugging
#define CHECK_INTEGRITY_MAX_OBJS 10000

#define VMO_FROM_VOIDP(voidPtr) ((VMObject_t*)((unsigned int)(voidPtr) - sizeof(VMObject_t) + sizeof(void*)))
#define VOIDP_FROM_VMO(vmo) ((void*)((unsigned int)(vmo) + sizeof(VMObject_t) - sizeof(void*)))

//macro to find the next object in car using object descriptors, will have to be checked
//against freeDataObjectPtr in car
#define NEXT_VMO(vmo) ((VMObject_t*)((unsigned int)(vmo)                                 \
				   + (sizeof(VMObject_t)                                 \
				      +	(vmo)->getObjectDescriptor()->getObjLengthBytes()) \
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

#ifndef SETVMREFERENCE_IS_MACRO
extern void setVMReference(void *source, void **refAdr, void *target);
#else
# ifdef NO_GC
#  define setVMReference(source, refAdr, target) (*refAdr = target);
# else
#  define setVMReference(source, refAdr, target) {                        \
   (*refAdr) = (target);                                                  \
};

# endif 
#endif

//extern void dumpBlock(void* tmpVMO, size_t length, ostream& stream);

#include "VMObject.hpp"

#endif


