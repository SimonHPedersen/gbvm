// $Id: main_debug3.cc,v 1.6 2001/04/10 12:01:24 ilsoe Exp $

#include "garbage_collector.hpp"

class DummyStack {
public:
  void * operator new(size_t size, void * location) {
    return location;
  }
  void * refs[900];
};

class DummyObj {
public:
  void * operator new(size_t size, void * location) {
    return location;
  }
  void *refs[10];
};

void dumpObj(void** object, int length) {
  for(int i = 0; i < 10; i++) {
    cout << *object << endl;
    object++;
  }
}

int main(int argc, char **argv) {

  DB_ENABLE_ALL;

  DB_DISABLE(gc_dumpblock);

  //create a dummy object descriptor
  ObjectDescriptor_t *refStackDesc = new ObjectDescriptor_t(1000, UINT_MAX);
  cout << refStackDesc;

  ObjectDescriptor_t *objDesc = new ObjectDescriptor_t((unsigned int)BYTES_IN_CAR/6, 0);
  cout << objDesc;

  for(int i = 0; i < 10; i++) {
    objDesc->setReferenceAt(i);
  }

  cout << "allocating stack\n";

  VMObject_t * vmo1       = garbageCollector.addStackObject(refStackDesc);
  DummyStack * ref_stack  = new(vmo1->getDataPtr())DummyStack();

  cout << "allocating dummy objects\n";

  VMObject_t * vmo2;
  DummyObj *dummyO2, *dummyO3 = 0;
  for(int i = 0; i < 20; i++) {
    vmo2 = garbageCollector.addNewObject(objDesc);
    dummyO2 = new(vmo2->getDataPtr())DummyObj();
    ref_stack->refs[i] = dummyO2;
    if(dummyO3) {
      setVMReference(dummyO2, &(dummyO2->refs[0]), dummyO3);
    }
    dummyO3 = dummyO2;
  }
  
#ifdef DEBUG
  garbageCollector.heapDump();
  garbageCollector.checkIntegrity();
#endif //DEBUG
  garbageCollector.garbageCollect();
#ifdef DEBUG
  garbageCollector.heapDump();
  garbageCollector.checkIntegrity();
#endif //DEBUG

  for(int i = 20; i < 40; i++) {
    vmo2 = garbageCollector.addNewObject(objDesc);
    dummyO2 = new(vmo2->getDataPtr())DummyObj();
    ref_stack->refs[i] = dummyO2;
    if(dummyO3) {
      setVMReference(dummyO2, &(dummyO2->refs[0]), dummyO3);
    }
    dummyO3 = dummyO2;
  }

#ifdef DEBUG
  garbageCollector.heapDump();
  garbageCollector.checkIntegrity();
#endif //DEBUG
  garbageCollector.garbageCollect();
#ifdef DEBUG
  garbageCollector.heapDump();
  garbageCollector.checkIntegrity();
#endif //DEBUG


#ifdef UNDEFINED
  for(int j = 0; j < 10; j++) {
    VMObject_t * vmo2;
    DummyObj *dummyO2, *dummyO3 = 0;
    for(int i = j*5; i < (5 + j*5); i++) {
      vmo2 = garbageCollector.addNewObject(objDesc);
      dummyO2 = new(vmo2->getDataPtr())DummyObj();
      ref_stack->refs[i] = dummyO2;
      if(dummyO3) {
	setVMReference(dummyO2, &(dummyO2->refs[0]), dummyO3);
      }
      if(j) {
	setVMReference(ref_stack->refs[i-5], &((DummyObj*)ref_stack->refs[i-5])->refs[1], dummyO2);
      }
      dummyO3 = dummyO2;
    }
    //  vmo2  = garbageCollector.addNewObject(objDesc);
    //  dummyO1 = new(vmo2->getDataPtr())DummyObj();
    
    
    /*
      cout << "dumping ref_stack:\n";
      dumpObj((void**)ref_stack, 10);
      cout << "dumping ref_stack vmo:\n";
      dumpObj((void**)vmo1, 10);
      cout << "dumping dummyO1:\n";
      dumpObj((void**)dummyO1, 10);
      cout << "\n\n";
    */

#ifdef DEBUG
    garbageCollector.heapDump();
#endif //DEBUG

    garbageCollector.garbageCollect();
#ifdef DEBUG
    garbageCollector.heapDump();
    garbageCollector.checkIntegrity();
#endif //DEBUG



  }

  //remove some references from the stack
  for(int i = 0; i < 10; i++) {
    ref_stack->refs[i] = 0;
    ref_stack->refs[49 - i] = 0;
    ref_stack->refs[i*2 + 10] = 0;
  }

  //#ifdef UNDEFINED
  for(int i = 0; i < 200; i++) {
#ifdef DEBUG
    garbageCollector.heapDump();
#endif //DEBUG
    
    garbageCollector.garbageCollect();
#ifdef DEBUG
    garbageCollector.heapDump();
    garbageCollector.checkIntegrity();
#endif //DEBUG
  }
  //#endif //UNDEFINED

#endif //UNDEFINED
  return 0;
}
