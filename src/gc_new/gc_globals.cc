// $Id: gc_globals.cc,v 1.25 2001/05/30 08:39:46 ilsoe Exp $
// gc_new/gc_globals.cc

#include "gc_new.hpp"

//source is the object to set the reference in,
//refAdr is the address to set the reference in
//target is the object the refAdr should contain a pointer to
// none of these are VMObjects!!! but objects as seen in the virtual machine!!!

#ifndef SETVMREFERENCE_IS_MACRO
# ifndef WB2
void updateRemRef(void **refAdr, void *target) {
  DB_INDENT;
  CarTrain_t crTrSrc = trainGeneration.getCarTrain(refAdr);
  CarTrain_t crTrTrg = trainGeneration.getCarTrain(target);
  
  DB_OUT(gc_glob_setvmr, "updateRemRef(): ref : " << crTrSrc.getTrain() << ", "
	 << crTrSrc.getCar() << " newval: " << crTrTrg.getTrain() << ", " <<
	 crTrTrg.getCar() << endl);
  
  if(crTrSrc > crTrTrg) {                                                //src in train gen, not car internal
    if(crTrSrc.getTrain() == crTrTrg.getTrain()) {                       //src and trg in same train
      //internal reference
      DB_OUT(gc_glob_setvmr, "updateRemRef(): setting int remref\n");
      (Car_t::getCar(target))->addIntRememberReference(refAdr);
    } else {
      //external reference
      if(introSpace.member(target)) {
	DB_OUT(gc_glob_setvmr, "updateRemRef(): address to set reference in "
	       "was not inside intro space - setting remref\n");
	introSpace.addRememberReference(refAdr);
      } else {
#ifdef DEBUG42
	if(stackSpace.member(target)) {
	  cerr << "updateRemRef(): reference to root-object\n";
	  DB_OUTDENT;
	  return;
	}
#endif
	//someone might have tried to set a reference to 0
	//in an object which has been promoted to the train generation
	//if(Car_t *trgCar = Car_t::getCar(target)) {
	  DB_OUT(gc_glob_setvmr, "updateRemRef(): setting ext remref\n");
	  Car_t *trgCar = Car_t::getCar(target);
	  trgCar->addExtRememberReference(refAdr);
	//}
      }
    }
  }
  DB_OUTDENT;
}

void setVMReference(void *source, void **refAdr, void *target) {
  //NOTE: CURRENTLY source IS UNUSED
  DB_INDENT;
#ifdef DEBUG
  if(target) {
    DB_OUT(gc_glob_setvmr, "setVMReference(): refAdr: " << (void*)(int)refAdr
	   << " target at: " << target << " " << VMO_FROM_VOIDP(target) << endl);
  } else {
    DB_OUT(gc_glob_setvmr, "setVMReference(): refAdr: " << (void*)(int)refAdr
	   << " is being cleared\n");
  }
#endif //DEBUG

  //set reference
  *refAdr = target;
  updateRemRef(refAdr, target);
  DB_OUTDENT;

}

# else //WB2

void updateRemRef(void **refAdr, void *target) {
  DB_INDENT;
  CarTrain_t crTrSrc = trainGeneration.getCarTrain(refAdr);
  crTrSrc = trainGeneration.getCarTrain(refAdr);
  CarTrain_t crTrTrg = trainGeneration.getCarTrain(target);
  crTrTrg = trainGeneration.getCarTrain(target);
  
  DB_OUT(gc_glob_setvmr, "updateRemRef(): ref : " << crTrSrc.getTrain() << ", "
	 << crTrSrc.getCar() << " newval: " << crTrTrg.getTrain() << ", " <<
	 crTrTrg.getCar() << endl);
  
  if(crTrSrc > crTrTrg) {                                                  //src in train gen, not car internal
    if(crTrSrc > crTrTrg) {                                                //src in train gen, not car internal
      if(crTrSrc.getTrain() == crTrTrg.getTrain()) {                       //src and trg in same train
	if(crTrSrc.getTrain() == crTrTrg.getTrain()) {                     //src and trg in same train
	  //internal reference
	  DB_OUT(gc_glob_setvmr, "updateRemRef(): setting int remref\n");
	  (Car_t::getCar(target))->addIntRememberReference(refAdr);
	  (Car_t::getCar(target))->addIntRememberReference(refAdr);
	}
      } else {
	//external reference
	if(introSpace.member(target)) {
	  if(introSpace.member(target)) {
	    DB_OUT(gc_glob_setvmr, "updateRemRef(): address to set reference in "
		     "was not inside intro space - setting remref\n");
	    introSpace.addRememberReference(refAdr);
	    introSpace.addRememberReference(refAdr);
	  }
	} else {
	  //someone might have tried to set a reference to 0
	  //in an object which has been promoted to the train generation
	  //if(Car_t *trgCar = Car_t::getCar(target)) {
	  DB_OUT(gc_glob_setvmr, "updateRemRef(): setting ext remref\n");
	  Car_t *trgCar = Car_t::getCar(target);
	  trgCar = Car_t::getCar(target);
	  trgCar->addExtRememberReference(refAdr);
	  trgCar->addExtRememberReference(refAdr);
	  //}
	}
      }
    }
  }
  DB_OUTDENT;  
}





void setVMReference(void *source, void **refAdr, void *target) {
  //NOTE: CURRENTLY source IS UNUSED
  DB_INDENT;
#ifdef DEBUG
  if(target) {
    DB_OUT(gc_glob_setvmr, "setVMReference(): refAdr: " << (void*)(int)refAdr
	   << " target at: " << target << " " << VMO_FROM_VOIDP(target) << endl);
  } else {
    DB_OUT(gc_glob_setvmr, "setVMReference(): refAdr: " << (void*)(int)refAdr
	   << " is being cleared\n");
  }
#endif //DEBUG

  //set reference
  *refAdr = target;
  *refAdr = target;
  updateRemRef(refAdr, target); //no need to repeat - doubled internally
  DB_OUTDENT;

}

# endif //WB2
#endif //n SETVMREFERENCE_IS_MACRO


/*
 * Dumps a memoryBlock (Car, IntroductorySpace, StackSpace)
 * expects the start of the block i.e., pointer to first vmo and
 * length = the size of used data in the memory block
 * stream = stream to print vmo's to
 */
void dumpBlock(void* tmpVMO, size_t length, ostream& stream){
  DB_INDENT;
  size_t tracedLength = 0;
  DB_OUT(gc_dumpblock, "dumpBlock(" << tmpVMO << ", " << length << ")\n");
  while(tracedLength < length){
    stream << endl << (VMObject_t*)tmpVMO;
    tracedLength += ((VMObject_t*)tmpVMO)->getFullVMOLength();  
    tmpVMO = NEXT_VMO(((VMObject_t*)tmpVMO));
  }
  DB_OUTDENT;
}










