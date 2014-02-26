//$Id: gc_globals.hpp,v 1.22 2001/06/11 15:21:45 ilsoe Exp $
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
((garbageCollector.addNewObject(objectDesc))->getDataPtr())

//NT means Non-Traced, it has nothing to do with MicroSoft NT (TM)
#define allocateRootVM_NT(objectDesc) \
  ((new(malloc(sizeof(VMObject_t) + (size_t)objectDesc->getObjLengthBytes() - sizeof(void*)), objectDesc) \
   VMObject_t())->getDataPtr())

//T means that the objects will be traced during gc
#define allocateRootVM_T(objectDesc) \
((garbageCollector.addStackObject(objectDesc))->getDataPtr())

#ifndef SETVMREFERENCE_IS_MACRO
extern void updateRemRef(void **refAdr, void *target);
extern void setVMReference(void *source, void **refAdr, void *target);
#define setVMReferenceGC(source, refAdr, target) (setVMReference(source, refAdr, target));
#define updateRemRefGC(refAdr, target) (updateRemRef(refAdr, target));
#else //SETVMREFERENCE_IS_MACRO
# ifdef NO_GC
#  define updateRemRef(refAdr, target) void
#  define updateRemRefGC(refAdr, target) void
#  define setVMReference(source, refAdr, target) (*refAdr = target);
#  define setVMReferenceGC(source, refAdr, target) (*refAdr = target);
# else  //NO_GC  that is GC

#ifdef undefined
void updateRemRef(void **refAdr, void *target);
void setVMReference(void *source, void **refAdr, void *target);
#endif //undefined

#  define updateRemRefGC(refAdr, target) {                                      \
  DB_INDENT;                                                                    \
  CarTrain_t crTrSrc = trainGeneration.getCarTrain(refAdr);                     \
  CarTrain_t crTrTrg = trainGeneration.getCarTrain(target);                     \
                                                                                \
  DB_OUT(gc_glob_setvmr, "updateRemRef(): ref : " << crTrSrc.getTrain()         \
	 << ", " << crTrSrc.getCar() << " newval: " <<                          \
	 crTrTrg.getTrain() << ", " << crTrTrg.getCar() << endl);               \
                                                                                \
  if(crTrSrc > crTrTrg) {                                                       \
    if(crTrSrc.getTrain() == crTrTrg.getTrain()) {                              \
      /* internal reference */                                                  \
      DB_OUT(gc_glob_setvmr, "updateRemRef(): setting int remref\n");           \
      (Car_t::getCar(target))->addIntRememberReference(refAdr);                 \
    } else {                                                                    \
      /* external reference */                                                  \
      if(introSpace.member(target)) {                                           \
	DB_OUT(gc_glob_setvmr, "updateRemRef(): address to set reference"       \
	       " in was not inside intro space - setting remref\n");            \
	introSpace.addRememberReference(refAdr);                                \
      } else {                                                                  \
	/* someone might have tried to set a reference to 0 */                  \
        /* in an object which has been promoted to the train gen */             \
	/*if(Car_t *trgCar = Car_t::getCar(target)) {*/                         \
	  DB_OUT(gc_glob_setvmr, "updateRemRef(): setting ext remref\n");       \
	  Car_t *trgCar = Car_t::getCar(target);                                \
	  trgCar->addExtRememberReference(refAdr);                              \
	/*}*/                                                                   \
      }                                                                         \
    }                                                                           \
  }                                                                             \
  DB_OUTDENT;                                                                   \
}

#  define setVMReferenceGC(source, refAdr, target) {                            \
  /* NOTE: CURRENTLY source IS UNUSED */                                        \
                                                                                \
  DB_INDENT;                                                                    \
  DB_OUT(gc_glob_setvmr, "setVMReference(): refAdr: " <<                        \
	 (void*)(int)refAdr << " target: " << target << endl);                  \
                                                                                \
  /* set reference */                                                           \
  *refAdr = target;                                                             \
  updateRemRefGC(refAdr, target);                                               \
  DB_OUTDENT;                                                                   \
}

#ifdef WB2
#  define setVMReference(source, refAdr, target) {                              \
  /* NOTE: CURRENTLY source IS UNUSED */                                        \
                                                                                \
  DB_INDENT;                                                                    \
  DB_OUT(gc_glob_setvmr, "setVMReference(): refAdr: " <<                        \
	 (void*)(int)refAdr << " target: " << target << endl);                  \
                                                                                \
  /* set reference */                                                           \
  *refAdr = target;                                                             \
  *refAdr = target;                                                             \
  updateRemRefDouble(refAdr, target);                                           \
  DB_OUTDENT;                                                                   \
}
#else //WB2
#  define setVMReference(source, refAdr, target)                                \
            (setVMReferenceGC(source, refAdr, target));
#endif //WB2


#  define updateRemRefDouble(refAdr, target) {                                  \
  DB_INDENT;                                                                    \
  CarTrain_t crTrSrc = trainGeneration.getCarTrain(refAdr);                     \
  crTrSrc = trainGeneration.getCarTrain(refAdr);                                \
  CarTrain_t crTrTrg = trainGeneration.getCarTrain(target);                     \
  crTrTrg = trainGeneration.getCarTrain(target);                                \
                                                                                \
  DB_OUT(gc_glob_setvmr, "updateRemRef(): ref : " << crTrSrc.getTrain()         \
	 << ", " << crTrSrc.getCar() << " newval: " <<                          \
	 crTrTrg.getTrain() << ", " << crTrTrg.getCar() << endl);               \
                                                                                \
  if(crTrSrc > crTrTrg) {                                                       \
    if(crTrSrc > crTrTrg) {                                                     \
      if(crTrSrc.getTrain() == crTrTrg.getTrain()) {                            \
        if(crTrSrc.getTrain() == crTrTrg.getTrain()) {                          \
          /* internal reference */                                              \
          DB_OUT(gc_glob_setvmr, "updateRemRef(): setting int remref\n");       \
          (Car_t::getCar(target))->addIntRememberReference(refAdr);             \
          (Car_t::getCar(target))->addIntRememberReference(refAdr);             \
        }                                                                       \
      } else {                                                                  \
          /* external reference */                                              \
          if(introSpace.member(target)) {                                       \
            if(introSpace.member(target)) {                                     \
	      DB_OUT(gc_glob_setvmr, "updateRemRef(): address to set reference" \
	             " in was not inside intro space - setting remref\n");      \
              introSpace.addRememberReference(refAdr);                          \
              introSpace.addRememberReference(refAdr);                          \
            }                                                                   \
	  }  else {                                                             \
	  /* someone might have tried to set a reference to 0 */                \
          /* in an object which has been promoted to the train gen */           \
	  /*if(Car_t *trgCar = Car_t::getCar(target)) {*/                       \
	  DB_OUT(gc_glob_setvmr, "updateRemRef(): setting ext remref\n");       \
	  Car_t *trgCar = Car_t::getCar(target);                                \
	  trgCar = Car_t::getCar(target);                                       \
	  trgCar->addExtRememberReference(refAdr);                              \
	  trgCar->addExtRememberReference(refAdr);                              \
	  /*}*/                                                                 \
        }                                                                       \
      }                                                                         \
    }                                                                           \
  }                                                                             \
  DB_OUTDENT;                                                                   \
}


# endif //NO_GC
#endif

extern void dumpBlock(void* tmpVMO, size_t length, ostream& stream);

#include "gc_new.hpp"







#ifdef UNDEFINED
#ifdef SETVMREFERENCE_IS_MACRO
# ifndef NO_GC

inline void updateRemRef(void **refAdr, void *target) {
  DB_INDENT;
  CarTrain_t crTrSrc = trainGeneration.getCarTrain(refAdr);
  CarTrain_t crTrTrg = trainGeneration.getCarTrain(target);

  DB_OUT(gc_glob_setvmr, "updateRemRef(): ref : " << crTrSrc.getTrain()
	 << ", " << crTrSrc.getCar() << " newval: " <<
	 crTrTrg.getTrain() << ", " << crTrTrg.getCar() << endl);

  if(crTrSrc > crTrTrg) {
    if(crTrSrc.getTrain() == crTrTrg.getTrain()) {
      /* internal reference */
      DB_OUT(gc_glob_setvmr, "updateRemRef(): setting int remref\n");
      (Car_t::getCar(target))->addIntRememberReference(refAdr);
    } else {
      /* external reference */
      if(introSpace.member(target)) {
	DB_OUT(gc_glob_setvmr, "updateRemRef(): address to set reference"
	       " in was not inside intro space - setting remref\n");
	introSpace.addRememberReference(refAdr);
      } else {
	DB_OUT(gc_glob_setvmr, "updateRemRef(): setting ext remref\n");
	(Car_t::getCar(target))->addExtRememberReference(refAdr);
      }
    }
  }
  DB_OUTDENT;
}

void setVMReference(void *source, void **refAdr, void *target) {
  /* NOTE: CURRENTLY source IS UNUSED */

  DB_INDENT;
  DB_OUT(gc_glob_setvmr, "setVMReference(): refAdr: " <<
	 (void*)(int)refAdr << " target: " << target << endl);

  /* set reference */
#ifndef WB2
  *refAdr = target;
  updateRemRef(refAdr, target);
#else //WB2
  *refAdr = target;
  *refAdr = target;
  updateRemRefDouble(refAdr, target);
#endif //WB2  
  DB_OUTDENT;
}

# endif //NO_GC
#endif //SETVMREFERENCE_IS_MACRO
#endif //UNDEFINED

#endif


