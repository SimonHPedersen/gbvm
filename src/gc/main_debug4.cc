//$Id: main_debug4.cc,v 1.2 2000/12/06 14:55:01 ilsoe Exp $
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
  void *d;
  void *e;
  void *f;
  void *g;
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
  objDes->setReferenceAt(3);
  objDes->setReferenceAt(4);
  objDes->setReferenceAt(5);
  objDes->setReferenceAt(6);
  
  //objDes->print();

  // test 1 BYTES_IN_CAR should be 1024, 2 cars in train
    
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
  
  /* creating popular object */
  for(int i = 0; i < 100; i++) {
  }


  setVMReference(t0, &(t0->a), t5);
  setVMReference(t1, &(t1->a), t2);
  setVMReference(t2, &(t2->a), t3);
  setVMReference(t3, &(t3->a), t4);
  setVMReference(t4, &(t4->a), t3);
  setVMReference(t5, &(t5->a), t1);
  setVMReference(t5, &(t5->b), t6);
  setVMReference(t7, &(t7->a), t8);
  setVMReference(t8, &(t8->a), t7);

  /*
  setVMReference(t0, &(t0->b), t1);
  setVMReference(t0, &(t0->c), t1);
  setVMReference(t0, &(t0->d), t1);
  setVMReference(t0, &(t0->e), t1);
  setVMReference(t0, &(t0->f), t1);
  setVMReference(t0, &(t0->g), t1);

  setVMReference(t2, &(t2->b), t1);
  setVMReference(t2, &(t2->c), t1);
  setVMReference(t2, &(t2->d), t1);
  setVMReference(t2, &(t2->e), t1);
  setVMReference(t2, &(t2->f), t1);
  setVMReference(t2, &(t2->g), t1);

  setVMReference(t3, &(t3->b), t1);
  setVMReference(t3, &(t3->c), t1);
  setVMReference(t3, &(t3->d), t1);
  setVMReference(t3, &(t3->e), t1);
  setVMReference(t3, &(t3->f), t1);
  setVMReference(t3, &(t3->g), t1);

  setVMReference(t4, &(t4->b), t1);
  setVMReference(t4, &(t4->c), t1);
  setVMReference(t4, &(t4->d), t1);
  setVMReference(t4, &(t4->e), t1);
  setVMReference(t4, &(t4->f), t1);
  setVMReference(t4, &(t4->g), t1);

  setVMReference(t5, &(t5->c), t1);
  setVMReference(t5, &(t5->d), t1);
  setVMReference(t5, &(t5->e), t1);
  setVMReference(t5, &(t5->f), t1);
  setVMReference(t5, &(t5->g), t1);
  */

  VMObject_t *o_n, *o_k;
  Test_t *t_n, *t_k;

  while(1) {
    garbageCollector->garbageCollect();
    o_n = garbageCollector->addNewObject(objDes);
    t_n = new (o_n->getDataPtr()) Test_t();
    o_k = garbageCollector->addNewObject(objDes);
    t_k = new (o_k->getDataPtr()) Test_t();
    setVMReference(t0->a, &(((Test_t*)(t0->a))->c), t_n);
    setVMReference(t0->a, &(((Test_t*)(t0->a))->b), t_k);
  }
}
