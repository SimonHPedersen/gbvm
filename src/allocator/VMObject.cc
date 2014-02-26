//$Id: VMObject.cc,v 1.1 2001/03/16 08:26:16 ilsoe Exp $

//#include "VMObject.hpp"
#include "allocator.hpp"

// ----- initialization -----

unsigned int VMObject_t::nextID = 0;

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
  ((VMObject_t*)freePointer)->objDesc = objDescriptor;
  DB_OUT(gc_vmo, "Memory allocation subsystem: allocated VMObject_t at size: 0x" <<
	 hex << ((size_t)size - sizeof(void*) + objDescriptor->getObjLengthBytes())
	 << " begin: 0x" << (int)freePointer << dec << endl);
  DB_OUTDENT;
  return freePointer;
}

// ----- member functions -----

VMObject_t::VMObject_t(void): id(nextID++) {
}

void VMObject_t::printOn(ostream& stream) {
  DB_INDENT;
  DB_INS_INDT(stream);
  stream << "- VMObject " << id << "printOn not really implemented yet\n";
  DB_OUTDENT;
}
