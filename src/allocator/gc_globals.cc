// $Id: gc_globals.cc,v 1.1 2001/03/16 08:26:18 ilsoe Exp $
// gc_new/gc_globals.cc

#include "allocator.hpp"

//NOTE: THIS FUNCTION DOES NOT GO THROUGH WRITE BARRIER YET!!!

//source is the object to set the reference in,
//refAdr is the address to set the reference in
//target is the object the refAdr should contain a pointer to
// none of these are VMObjects!!! but objects as seen in the virtual machine!!!

#ifndef SETVMREFERENCE_IS_MACRO
void setVMReference(void *source, void **refAdr, void *target) {
  DB_INDENT;
  *refAdr = target;

  /* old version
  VMObject_t *src = VMO_FROM_VOIDP(source);
  DB_OUT(gc_glob_setvmr, "setVMReference(): source at " << source << " vmobject at: "
	 << (void*)src << " refAdr: " << refAdr << endl);
  //check if we have to remove old remember reference pointing to src from *refAdr's remset
  if(*refAdr) {
    VMObject_t *oldRef = VMO_FROM_VOIDP(*refAdr);
    oldRef->clearRememberReference(src);
  }
  //add reference to this object
  src->setReference(refAdr, target);
  */
  DB_OUTDENT;
}
#endif
