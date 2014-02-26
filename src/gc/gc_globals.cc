//$Id: gc_globals.cc,v 1.10 2001/02/28 18:54:05 ilsoe Exp $
#include "gc_globals.hpp"

#ifdef DEBUG
unsigned int collected = 0;
unsigned int allocated = 0;
#endif

//source is the object to set the reference in,
//refAdr is the address to set the reference in
//target is the object the refAdr should contain a pointer to
// none of these are VMObjects!!! but objects as seen in the virtual machine!!!
#ifndef SETVMREFERENCE_IS_MACRO
void setVMReference(void *source, void **refAdr, void *target) {
  DB_INDENT;
  VMObject_t *src = VMO_FROM_VOIDP(source);
  DB_OUT(gc_glob_setvmr, "setVMReference(): source at " << source << " vmobject at: "
	 << (void*)src << " refAdr: " << refAdr << endl);
  //check if we have to remove old remember reference pointing to src from *refAdr's remset
  if(*refAdr) {
    DB_OUT(gc_glob_setvmr_verb, "setVMReference(): 1" << endl);
    VMObject_t *oldRef = VMO_FROM_VOIDP(*refAdr);
    DB_OUT(gc_glob_setvmr_verb, "setVMReference(): 2 oldRef: " << (void*)oldRef << endl);
    oldRef->clearRememberReference(src);
    DB_OUT(gc_glob_setvmr_verb, "setVMReference(): 3" << endl);
  }
  //add reference to this object
  DB_OUT(gc_glob_setvmr_verb, "setVMReference(): before setReference\n");
  src->setReference(refAdr, target);
  DB_OUTDENT;
}
#endif
