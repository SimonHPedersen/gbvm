//$Id: VMObject.cc,v 1.14 2001/05/21 11:42:46 hempe Exp $

//#include "VMObject.hpp"
#include "gc_new.hpp"

// ----- initialization -----

#ifdef OBJ_HAS_ID
unsigned int VMObject_t::nextID = 1;
#endif

// ----- operators -----

ostream& operator << (ostream& stream, VMObject_t* vmo) {
  if(vmo) {
    vmo->printOn(stream);
  } else {
    stream << "*** tried to print a null-vmo ***\n";
  }
  return stream;
}

ostream& operator << (ostream& stream, VMObject_t& vmo) {
  vmo.printOn(stream);
  return stream;
}

void* VMObject_t::operator new(size_t size, void *freePointer,
			       ObjectDescriptor_t *objDescriptor) {
  DB_INDENT;
  if(!objDescriptor) {
    DB_OUT(gc_vmo, "VMObject_t::operator new: *** object descriptor is null! ***\n");
  }
  ((VMObject_t*)freePointer)->setObjectDescriptor(objDescriptor);
  DB_OUT(gc_vmo, "Memory allocation subsystem: allocated VMObject_t at size: 0x" <<
	 hex << ((size_t)size - sizeof(void*) + objDescriptor->getObjLengthBytes())
	 << " begin: 0x" << (int)freePointer << dec << endl);
  DB_OUTDENT;
  return freePointer;
}

// ----- member functions -----

VMObject_t::VMObject_t(void):
#ifdef OBJ_HAS_ID
 id(nextID++),
#endif //OBJ_HAS_ID
 forward(0) {
#ifdef DEBUG
  if(!getObjectDescriptor()) {
    cerr << "VMObject_t(): object has been created without (valid)"
      "object descriptor\n";
    exit(1);
  }
  DB_INDENT;
  DB_OUT(gc_vmo_id, "VMObject_t::VMObject_t(void) with id: " << getID() << endl);
  DB_OUTDENT;
#endif //DEBUG
  //clear references
  memset(data, 0, getObjectDescriptor()->getObjLengthBytes());
}

#ifdef LIVESTAT

void VMObject_t::mark(void) {
  //hack - uses forward bit for mark-bit
  if(isForward()) {
    return;
  }
  setForwardBit(true);
  ObjectDescriptor_t *_objDesc = getObjectDescriptor();
  unsigned int *refMask = _objDesc->getReferenceMask();
  unsigned int refsInObject = _objDesc->getObjLengthRefs();
  unsigned int intsInMask = (refsInObject/(sizeof(unsigned int)*8))+1;
  int bits2Check = sizeof(unsigned int)*8;
  void *objRef;

  for(unsigned int i = 0; i < intsInMask; ++i){
    int mask = refMask[i];
    if(mask) {
      if(i == (intsInMask -1)) {
	bits2Check = refsInObject - (intsInMask - 1) * sizeof(unsigned int)*8;
      }
      for(int j = 0; j < bits2Check; ++j){
	if((1<<j) & mask){
	  //we've got a reference, let's find it
	  objRef = data[i * sizeof(int) * 8 + j];
	  if(objRef) {
	    //ok, now let's mark it
	    VMO_FROM_VOIDP(objRef)->mark();
	  } //we allow null-references to be ignored
	}
      }
    }
  }    
}

#endif //LIVESTAT

void VMObject_t::printOn(ostream& stream) {
  DB_INDENT;
  DB_INS_INDT(stream);
  stream << "- VMObject " << getID() << " at " << (void*)this << " with car at " 
	 << (void*)Car_t::getCar(((void*)this));
  if(forward){
    stream << " WARNING: forwardPtr is set:" << forward;
  }

#ifdef DEBUG
  DB_INDENT;
  //print references...
  ObjectDescriptor_t *_objDesc = getObjectDescriptor();
  unsigned int *refMask = _objDesc->getReferenceMask();
  unsigned int refsInObject = _objDesc->getObjLengthRefs();
  unsigned int intsInMask = (refsInObject/(sizeof(unsigned int)*8))+1;
  int bits2Check = sizeof(unsigned int)*8;
  void *objRef;

  for(unsigned int i = 0; i < intsInMask; ++i){
    int mask = refMask[i];
    if(mask) {
      if(i == (intsInMask -1)) {
	bits2Check = refsInObject - (intsInMask - 1) * sizeof(unsigned int)*8;
      }
      for(int j = 0; j < bits2Check; ++j){
	if((1<<j) & mask){
	  //we've got a reference, let's find it
	  objRef = data[i * sizeof(int) * 8 + j];
	  if(objRef) {
	    //ok, now let's print it
	    DB_OUT(gc_vmo_dump_refs, endl);
	    DB_OUT(gc_vmo_dump_refs, "-> " << VMO_FROM_VOIDP(objRef)->getID() << ", "
		   << (void*)VMO_FROM_VOIDP(objRef));
	  } //we allow null-references to be ignored
	}
      }
    }
  }  
  DB_OUTDENT;
#endif
  DB_OUTDENT;
}
