//$Id: ObjectDescriptor.cc,v 1.16 2001/04/10 12:01:22 ilsoe Exp $

#include "ObjectDescriptor.hpp"

//----- operators -----

ostream& operator << (ostream& stream, ObjectDescriptor_t* objD) {
  if(objD) {
    objD->printOn(stream);
  } else {
    stream << "*** tried to print a null-object descriptor ***\n";
  }
  return stream;
}

ostream& operator << (ostream& stream, ObjectDescriptor_t& objD) {
  objD.printOn(stream);
  return stream;
}


ObjectDescriptor_t::ObjectDescriptor_t(unsigned int length, unsigned int maskValue){
  DB_INDENT;
  //make sure our objects keeps the allignment...
  objLength = (length % sizeof(void*))
    ? (length & (~(sizeof(void*)-1))) + sizeof(void*)
    : length;

  unsigned int totalRefs  = /*1 +*/ objLength / sizeof(void*);
  unsigned int intsInMask = 1 + (totalRefs/ (sizeof(unsigned int)*8));
  unsigned int bytesInMask = intsInMask * sizeof(unsigned int);
  

  //allocate memory for referenceMask
  DB_OUT(gc_objd_const, "ObjectDescriptor_t::ObjectDescriptor_t(" << length << "): allocating "
	 << bytesInMask << " bytes (" << intsInMask << " ints) for " << totalRefs
	 << " references\n");
  if(!(referenceMask = (unsigned int*)malloc(bytesInMask))) {
    cerr << "ObjectDescriptor_t::ObjectDescriptor_t: unable to allocate mem\n";
    exit(1);
  }

  //clear referenceMask[]
  if(maskValue){
    for(unsigned int i = 0; i < intsInMask - 1; i++) {
      referenceMask[i] = maskValue;
    }
    //fixing last int bit by bit
    //unsigned int totalRefs = 1 + objLength / sizeof(void*); ???????????????
    //bits left to set in last int in mask
    unsigned int bitsLeft = totalRefs - (intsInMask - 1) * sizeof(unsigned int)*8;
    referenceMask[intsInMask - 1] = UINT_MAX << (sizeof(unsigned int)*8 - bitsLeft) & maskValue;
  } else {
    for(unsigned int i = 0; i < intsInMask; i++) {
      referenceMask[i] = maskValue;
    }
  }

  DB_OUTDENT;
}

ObjectDescriptor_t::~ObjectDescriptor_t() {
  free(referenceMask);
}

// ----- methods -----

/*
unsigned int ObjectDescriptor_t::getHeaderObjectLength() {
  return headerObjLengt;
}

unsigned int ObjectDescriptor_t::getFullObjectLength() {
  return headerObjLength + objLength;
}
*/

unsigned int ObjectDescriptor_t::getObjLengthBytes(){
  return objLength;
}

unsigned int ObjectDescriptor_t::getObjLengthRefs(){
  return objLength/sizeof(void*);
}

unsigned int* ObjectDescriptor_t::getReferenceMask(){
  return referenceMask;
}

int ObjectDescriptor_t::setReferenceAt(unsigned int number) {
#ifdef DEBUG
  unsigned int totalRefs = 1 + objLength / sizeof(void*);

  if(number > totalRefs) {
    return 0;
  }
#endif

  unsigned int index = number/(sizeof(int)*8);
  unsigned int mask  = 1 << (number%(sizeof(int)*8));

  return referenceMask[index] |= mask;
}


int ObjectDescriptor_t::clearReferenceAt(unsigned int number) {
#ifdef DEBUG
  unsigned int totalRefs = 1 + objLength / sizeof(void*);

  if(number > totalRefs) {
    return 0;
  }
#endif

  unsigned int index = number/(sizeof(int)*8);
  unsigned int mask  = ~(1 << (number%(sizeof(int)*8)));

  referenceMask[index] &= mask;
  return 1;
}

int ObjectDescriptor_t::hasReferenceAt(unsigned int number) {
  unsigned int totalRefs = 1 + objLength / sizeof(void*);
  if(number > totalRefs) {
    return 0;
  }
  unsigned int index = number/(sizeof(int)*8);
  unsigned int mask  = 1 << (number%(sizeof(int)*8));

  return referenceMask[index] & mask;
}

#ifdef UNDEFINED
void ObjectDescriptor_t::print(void) {
  printOn(cout);
}
#endif //UNDEFINED

void ObjectDescriptor_t::printOn(ostream& stream) {
#ifdef DEBUG
  DB_INDENT;
  unsigned int totalRefs  = /*1 +*/ objLength / sizeof(void*);
  unsigned int intsInMask = 1 + (totalRefs/ (sizeof(unsigned int)*8));

  if(DB_ENABLED(gc_objd)) {
    stream << "Object Descriptor at " << (void*)this << " with length: "
	   << objLength << ":";
    for(unsigned int i = 0; i < intsInMask; i++) {
      if(!(i%8)) {
	if(i) {
	  stream << ",\n";
	} else {
	  stream << endl;
	}
	DB_INS_INDT(stream);
      } else {
	DB_OUT0(gc_objd, ", ");
      }
      stream << (void*)referenceMask[i];
    }
    stream << endl;
  }
  DB_OUTDENT;
#endif //DEBUG
}
