//$Id: Car.cc,v 1.1 2001/03/16 08:26:13 ilsoe Exp $

//#include "Car.hpp"
#include "allocator.hpp"

//----- operators -----

ostream& operator << (ostream& stream, Car_t* car) {
  if(car) {
    car->printOn(stream);
  } else {
    stream << "*** tried to print a null-car ***\n";
  }
  return stream;
}

ostream& operator << (ostream& stream, Car_t& car) {
  car.printOn(stream);
  return stream;
}

// ----- member functions -----




VMObject_t *Car_t::allocateVMO(ObjectDescriptor_t *objDesc) {
  DB_INDENT;

  //DB_OUT(gc_car, "Car_t::allocateVMO(): available space: " << availSpace()
  //	 << endl);

  size_t objSize = objDesc->getObjLengthBytes() + sizeof(VMObject_t)
    - sizeof(void*);
  if(objSize > availSpace()) {
    // not enough space
    DB_OUT(gc_car, "Car_t::allocateVMO(): out of space\n");
    DB_OUTDENT;
    return 0;
  }

  void *newDataPtr = freePtr;
  freePtr = (void*)((size_t)freePtr + (size_t)objSize);
  VMObject_t *vmo = new(newDataPtr, objDesc)VMObject_t();
  DB_OUTDENT;

  return vmo;
}

void Car_t::printOn(ostream& stream) {
  DB_INDENT;
  DB_INS_INDT(stream);
  stream << "--- Car " /*<< carNumber*/ << " at " << (void*)this;
  // << " with train " << trainRef.getTrainNumber();
  DB_OUTDENT;
}

//void Car_t::setTrainGeneration(TrainGeneration_t *tg) {
//  tg = tg;
//}

// ----- private member functions -----

size_t Car_t::availSpace(void) {
  return ((size_t)this + BYTES_IN_CAR) - (size_t)freePtr;
}

//We have to do this in order not to make the compiler/linker mad
TrainGeneration_t *Car_t::tg = 0;
