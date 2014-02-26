//$Id: main_debug1.cc,v 1.7 2001/02/28 19:29:37 ilsoe Exp $
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
  DB_ENABLE(gc_vmo_mro_verb);
  DB_ENABLE(gc_trn);
  DB_ENABLE(gc_objd);
  DB_ENABLE(gc_glob_setvmr);
  DB_ENABLE(gc_integrity);

  unsigned int id = 0;

  ObjectDescriptor_t *objDes = new ObjectDescriptor_t(300,0);
  
  //objDes->print();

  objDes->setReferenceAt(0);
  objDes->setReferenceAt(1);
  objDes->setReferenceAt(2);
  
  //objDes->print();

  // test 1 BYTES_IN_CAR should be 1024, 2 cars in train
    
  VMObject_t *o0 = garbageCollector->addRootObject(objDes);
  o0->setID(id++);
  Test_t     *t0 = new (o0->getDataPtr())Test_t();
  
  VMObject_t *o1 = garbageCollector->addNewObject(objDes);
  o1->setID(id++);
  Test_t     *t1 = new (o1->getDataPtr())Test_t();
  
  VMObject_t *o2 = garbageCollector->addNewObject(objDes);
  o2->setID(id++);
  Test_t     *t2 = new (o2->getDataPtr())Test_t();
  
  VMObject_t *o3 = garbageCollector->addNewObject(objDes);
  o3->setID(id++);
  Test_t     *t3 = new (o3->getDataPtr())Test_t();
  
  VMObject_t *o4 = garbageCollector->addNewObject(objDes);
  o4->setID(id++);
  Test_t     *t4 = new (o4->getDataPtr())Test_t();
  
  VMObject_t *o5 = garbageCollector->addNewObject(objDes);
  o5->setID(id++);
  Test_t     *t5 = new (o5->getDataPtr())Test_t();
  
  VMObject_t *o6 = garbageCollector->addNewObject(objDes);
  o6->setID(id++); 
  Test_t     *t6 = new (o6->getDataPtr())Test_t();
  
  VMObject_t *o7 = garbageCollector->addNewObject(objDes);
  o7->setID(id++);
  Test_t     *t7 = new (o7->getDataPtr())Test_t();
  
  VMObject_t *o8 = garbageCollector->addNewObject(objDes);
  o8->setID(id++);
  Test_t     *t8 = new (o8->getDataPtr())Test_t();
  
  setVMReference(t0, &(t0->a), t5);
  setVMReference(t1, &(t1->a), t2);
  setVMReference(t2, &(t2->a), t3);
  setVMReference(t3, &(t3->a), t4);
  setVMReference(t4, &(t4->a), t3);
  setVMReference(t5, &(t5->a), t1);
  setVMReference(t5, &(t5->b), t6);
  setVMReference(t7, &(t7->a), t8);
  setVMReference(t8, &(t8->a), t7);

  //testing integrity check

  t7->b = (void*)234;

  while(1) {
#ifdef DEBUG
    garbageCollector->checkIntegrity();
#endif
    garbageCollector->garbageCollect();
  }
}
