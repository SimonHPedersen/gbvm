//$Id: young_generation.cc,v 1.7 2000/10/18 13:01:47 ilsoe Exp $
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>

#include "garbage_collector.hpp"


/* YOUNG GENERATION */


YoungGeneration_t::YoungGeneration_t(int size) {
  Generation_t::init(size);
  cout << "yg(int)\n";
  currentStartHeapPointer = heapPointer;
  currentEndHeapPointer = (void*)((int)heapPointer + heapSize/2);
  /*
  //Generation(size);
  VMObject_t* objs[10];
  char nameString[10];
  for(int i = 0; i < 10; i++) {
    sprintf(nameString, "object %i", i);
    objs[i] = new(&freePointer) VMObject_t(0, i, nameString);
  }
  VMObject_t* refs[4] = {new(&freePointer) VMObject_t(0,0,"leaf1"),new(&freePointer) VMObject_t(0,1,"leaf2"),0,0};
  VMObject_t* vmo3 = new(&rootSpacePointer) VMObject_t(refs,2,"root");
  vmo3->setReference(2, objs[1]);
  objs[0]->setReference(0, objs[4]);
  objs[1]->setReference(0, vmo3);
  objs[1]->setReference(1, objs[2]);
  objs[2]->setReference(0, objs[3]);
  objs[3]->setReference(0, objs[1]);
  objs[3]->setReference(1, objs[3]);
  objs[4]->setReference(0, objs[5]);
  objs[5]->setReference(0, objs[0]);

  rootObject = vmo3;
  cout << "Memory subsystem: dumping heap" << endl;
  for(int i = 0; i<size ; i++)
    printf("%c",*(char*)((int)heapPointer + i));
  cout << endl;
  */
}

YoungGeneration_t::~YoungGeneration_t() {
  cout << "~yg\n";
  //Generation(size);
}

void YoungGeneration_t::changeRememberReference(VMObject_t *obj,
						VMObject_t *oldRef,
						VMObject_t *newRef) {
  rememberedSetPtr->changeRememberReference(obj, oldRef, newRef);
}

void YoungGeneration_t::updateRememberSetReferences(VMObject_t *obj,
						    VMObject_t *newLocation) {
  rememberedSetPtr->updateRememberSetReferences(obj, newLocation);
}



void YoungGeneration_t::garbageCollect() {
  VMObject_t* ref;
  cout << "***YG: garbage collecting***\n";
  freePointer = (currentStartHeapPointer == heapPointer) ? currentEndHeapPointer : heapPointer;
  scanPointer = freePointer;

  //handling root object
  ObjectDescriptor_t* objectDescriptor = rootObject->getObjectDescriptor();
  int* refMask = objectDescriptor->getReferenceMask();
  //ifrefs...
  int intsInMask = (((objectDescriptor->getObjLength())/sizeof(void*))/sizeof(int)*8)+1;

  cout << "Garbage collecting root object\n";

  for(int i = 0; i <= intsInMask; i++){
    int mask = refMask[i];
    //no reason to check for references if no bits are set
    if(mask) {
      //check each bit in turn
      for(unsigned int j = 0; j < sizeof(int)*8 ; j++){
	if((1<<j) & mask){
	  //we've got a reference
	  ref = (VMObject_t*)rootObject->data[(i * sizeof(int) * 8 + j)];
	  //reference must be defined ie not null and placed in from space
	  if(ref){
	    if(ref >= currentStartHeapPointer && ref < currentEndHeapPointer){
	      //reference in generation
	      ref = ref->moveToSpace(&freePointer); //move object and update reference to it
	      cout << "Freepointer after move: " << (int)freePointer << endl;
	    } else {
	      //reference to other generation
	      cout << "Garbage Collection SubSystem: reference at "<< (int)(void*)&ref
		   << " to other generation with reference to " << (int)(void*)ref
		   << " or from from space to to space ???" << endl;
	    }
	  } else {
	    //cout << "reference " << i << "null " << endl;
	  }
	}
      }
    }
  }

  printf(" scan = %i freepointer = %i\n",(int)scanPointer,(int)freePointer);

  //handle mature remember-references
  // 1. move objects to to-space
  // 2. update remember-references and copy them to to-spaces remember-set
  RememberedSetNode_t* remSetNode;
  RememberedObjectNode_t* remObjNode;
  VMObject_t *vmo, *movedTo, *remRef;
  RememberedSet_t* newRememberedSet = new RememberedSet_t();

  //iterate this generations rememberedSet
  for(remSetNode = rememberedSetPtr->getRememberRoot(); 
      remSetNode != NULL; 
      remSetNode = remSetNode->remSetNodePtr){ 
    //remember the object which remembered set we are about to iterate
    vmo = &(remSetNode->vmIdObjectPtr);
    remObjNode = remSetNode->remObjNodePtr;

    //copy object if first remember-reference is from other generation
    remRef = &(remObjNode->vmRemObjectPtr);
    if(remRef < currentStartHeapPointer || remRef >= currentEndHeapPointer){
      movedTo = vmo->moveToSpace(&freePointer);

      //update reference in object pointing to the current object (vmo)
      remRef->changeReference(vmo,movedTo);

      //copy remember reference to new rememberset
      newRememberedSet->addRememberReference(movedTo, remRef);
          
      //iterate the objects remembered set to update them and copy them
      for(remObjNode = remObjNode->remObjNodePtr; 
	  remObjNode != NULL; 
	  remObjNode = remObjNode->remObjNodePtr)
	{
	  remRef = &(remObjNode->vmRemObjectPtr);
	
	  //remember reference 
	  if(remRef < currentStartHeapPointer || remRef >= currentEndHeapPointer){
	    //update reference
	    remRef->changeReference(vmo,movedTo);
	    //copy remember reference to new rememberset
	    newRememberedSet->addRememberReference(movedTo,remRef);
	  } else {
	    break;
	  }
	}
    }
  }



  //scan to-space while scan < next
  while(scanPointer < freePointer){
    cout << "Garbage collecting " << (int)scanPointer << endl;
    
    //scan object for references
    VMObject_t* scanObjPtr = (VMObject_t*)scanPointer;
    ObjectDescriptor_t *objectDescriptor = scanObjPtr->getObjectDescriptor();
    int* refMask = objectDescriptor->getReferenceMask();
    unsigned int intsInMask = (((objectDescriptor->getObjLength())/sizeof(void*))/sizeof(int)*8)+1;
    
    for(unsigned int i = 0; i <= intsInMask; i++){
      int mask = refMask[i];
      for(unsigned int j = 0; j < sizeof(int)*8 ; j++){
	if((1<<j) & mask){
	  //we've got a reference
	  ref = (VMObject_t*)scanObjPtr->data[i * sizeof(int) * 8 + j];

	  // + sizeof(VMObject_t) - sizeof(void*) + (i * sizeof(int) * 8 + j) * (void*);

	  //reference must be defined and placed in from space
	  if(ref){
	    if(ref >= currentStartHeapPointer && ref < currentEndHeapPointer){
	      ref = ref->moveToSpace(&freePointer); //move object and update reference to it
	    } else {
	      //reference to other space
	      //their remember-reference has been updated
	    }
	  } else {
	    //cout << "reference " << i << " null " << endl;
	  }
	}
      }
    }

    cout << "Done garbage collecting " << (int)scanPointer << endl;
    scanPointer = (void*)((unsigned int)scanPointer + sizeof(VMObject_t) + ref->getExtendedSize());    
  }

  //REMEMBER to scan other generations - update references and move referenced objects in from space
  
  rememberedSetPtr = newRememberedSet;

  //swap spaces and handle heap pointers 
  if(currentStartHeapPointer == heapPointer){
    currentStartHeapPointer = (void*)((int)heapPointer + heapSize/2);
    currentEndHeapPointer = (void*)((int)heapPointer + heapSize);
  } else {
    currentStartHeapPointer = heapPointer;
    currentEndHeapPointer = (void*)((int)heapPointer + heapSize/2);
  }    
  
}



