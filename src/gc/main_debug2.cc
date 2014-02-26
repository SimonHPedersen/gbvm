//$Id: main_debug2.cc,v 1.5 2000/12/06 14:55:07 ilsoe Exp $
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>

#include "garbage_collector.hpp"

class Test_t {
public:
  void *operator new(size_t size, void *freePointer) {
    return freePointer;
  }
  Test_t() {

  }
  void *a;
  void *b;
  void *c;
};

int main(int argc, char** argv) {

  DB_ENABLE(gc_gc);
  DB_ENABLE(gc_mg);
  DB_ENABLE(gc_mg_heapdump);
  DB_ENABLE(gc_remset_dump);
  DB_ENABLE(gc_car);
  DB_ENABLE(gc_car_delete);
  DB_ENABLE(gc_vmo);
  DB_ENABLE(gc_vmo_dump_refs);
  //DB_ENABLE(gc_vmo_mro_verb);
  DB_ENABLE(gc_trn);
  DB_ENABLE(gc_objd);
  DB_ENABLE(gc_glob_setvmr);

  ObjectDescriptor_t *objDes = new ObjectDescriptor_t(300,0);
  
  //objDes->print();

  objDes->setReferenceAt(0);
  objDes->setReferenceAt(1);
  objDes->setReferenceAt(2);
  
  //objDes->print();

  //test 2 BYTES_IN_CAR should be 1024 with 2 cars in train, objDes should be 300
  
  VMObject_t *o0 = garbageCollector->addRootObject(objDes);
  Test_t     *t0 = new (o0->getDataPtr())Test_t();

  VMObject_t *o1 = garbageCollector->addNewObject(objDes);
  Test_t     *t1 = new (o1->getDataPtr())Test_t();

  VMObject_t *o2 = garbageCollector->addNewObject(objDes);
  Test_t     *t2 = new (o2->getDataPtr())Test_t();
  
  VMObject_t *o3 = garbageCollector->addNewObject(objDes);
  Test_t     *t3 = new (o3->getDataPtr())Test_t();

  VMObject_t *o4 = garbageCollector->addNewObject(objDes);
  Test_t     *t4 = new (o4->getDataPtr())Test_t();

  VMObject_t *o5 = garbageCollector->addNewObject(objDes);
  Test_t     *t5 = new (o5->getDataPtr())Test_t();

  VMObject_t *o6 = garbageCollector->addNewObject(objDes);
  Test_t     *t6 = new (o6->getDataPtr())Test_t();

  VMObject_t *o7 = garbageCollector->addNewObject(objDes);
  Test_t     *t7 = new (o7->getDataPtr())Test_t();

  VMObject_t *o8 = garbageCollector->addNewObject(objDes);
  Test_t     *t8 = new (o8->getDataPtr())Test_t();

  VMObject_t *o9 = garbageCollector->addNewObject(objDes);
  Test_t     *t9 = new (o9->getDataPtr())Test_t();

  VMObject_t *o10 = garbageCollector->addNewObject(objDes);
  Test_t     *t10 = new (o10->getDataPtr())Test_t();

  VMObject_t *o11 = garbageCollector->addNewObject(objDes);
  Test_t     *t11 = new (o11->getDataPtr())Test_t();

  VMObject_t *o12 = garbageCollector->addNewObject(objDes);
  Test_t     *t12 = new (o12->getDataPtr())Test_t();
  
  cout << "main debug: starting to set references\n";
  setVMReference(t0, &(t0->a), t11);
  setVMReference(t1, &(t1->a), t3);
  setVMReference(t3, &(t3->a), t1);
  setVMReference(t4, &(t4->a), t2);
  setVMReference(t5, &(t5->a), t7);
  setVMReference(t6, &(t6->a), t5);
  setVMReference(t7, &(t7->a), t6);
  setVMReference(t7, &(t7->b), t10);
  setVMReference(t8, &(t8->a), t4);
  setVMReference(t9, &(t9->a), t5);
  setVMReference(t10, &(t10->a), t9);
  setVMReference(t11, &(t11->a), t8);
  setVMReference(t12, &(t12->a), t12);
  cout << "main debug: references set\n";

  while(1) {
    garbageCollector->garbageCollect();
  }
}
