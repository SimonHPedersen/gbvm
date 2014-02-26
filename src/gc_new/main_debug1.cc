#include <iostream.h>

//#include "MemoryBlock.hpp"
//#include "Generation.hpp"
#include "gc_new.hpp"

int main(int argc, char **argv) {

  DB_ENABLE(gc_heapdump);
  DB_ENABLE(gc_dumpblock);
  DB_ENABLE(gc_memblk);
  DB_ENABLE(gc_car);
  DB_ENABLE(gc_vmo);
  DB_ENABLE(gc_tg);

  //create a dummy object descriptor
  ObjectDescriptor_t *od = new ObjectDescriptor_t(BYTES_IN_CAR/6, 0);

  Lst<MemoryBlock_t> mbl;
  mbl.add(new MemoryBlock_t(8));
  mbl.add(new MemoryBlock_t(16));
  void *carSpace = mbl.first()->allocCarSpace();
  if(!carSpace) {
    exit(1);
  }
  cout << "new car space allocated at: " << (void*)carSpace << endl;
  Car_t *car = new(carSpace)Car_t();
  car->allocateVMO(od);
  mbl.first()->next()->freeCar(car); //this should fail
  mbl.first()->freeCar(car);

  carSpace = mbl.first()->next()->allocCarSpace();
  car = new(carSpace)Car_t();
  mbl.first()->next()->freeCar(car); //last memblock should be suggested freed
  
  //fill up first mem block, free one car try to allocate a car again
  carSpace = mbl.first()->allocCarSpace();
  car = new(carSpace)Car_t();
  
  for(int i = 0; i < 8; i++) { //the last allocation (7) should fail
    if((carSpace = mbl.first()->allocCarSpace())) {
      cout << i << " success allocating space for car, address:" << carSpace << "\n";
    } else {
      cout << i << " failure\n";
    }
  }
  cout << "(NOTE: 7 should fail)\n";

  //should work
  mbl.first()->freeCar(car);

  //should also work
  if((carSpace = mbl.first()->allocCarSpace())) {
    cout << " success allocating space for car, address:" << carSpace << "\n";
  } else {
    cout << " failure\n";
  }

  cout << "\nnow testing Train generation\n\n";

  /*
  TrainGeneration_t tg(2);

  cout << "main debug: trying to allocate ordinary object\n";

  tg.addNewObject(od);

  cout << "main debug: allocated ordinary object\n";

  tg.addRootObject(od);

  cout << "main debug: allocated root object\n";


  VMObject_t *vmo;
  CarTrain_t ct;

  for(int i = 0; i < 1000; i++) {
    vmo = tg.addNewObject(od);
    Car_t *tmpCar = Car_t::getCar(vmo);
    cout << "copying object\n";
    tmpCar->moveObj(VOIDP_FROM_VMO(vmo));
    cout << "printing car: " << tmpCar << endl;
    ct = tg.getCarTrain(vmo);
    cout <<"     VMO: train = " << ct->getTrain() << " car = " << ct->getCar()
	 << endl;
  }
  */
  cout << "test finished\n";
  return 0;
}
