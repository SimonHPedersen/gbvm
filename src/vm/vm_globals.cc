//$Id: vm_globals.cc,v 1.27 2001/05/28 12:56:26 ilsoe Exp $

#include "vm.hpp"

//these global variables are completed only after running fixObjectDescriptors()

ObjectDescriptor_t *predefBoolPODesc   
  = new ObjectDescriptor_t(sizeof(PredefinedPartObject_t<bool>), 0);
ObjectDescriptor_t *predefCharPODesc   
  = new ObjectDescriptor_t(sizeof(PredefinedPartObject_t<char>), 0);
ObjectDescriptor_t *predefIntPODesc    
  = new ObjectDescriptor_t(sizeof(PredefinedPartObject_t<int>), 0);
ObjectDescriptor_t *predefRealPODesc   
  = new ObjectDescriptor_t(sizeof(PredefinedPartObject_t<real>), 0);
ObjectDescriptor_t *predefStringPODesc 
  = new ObjectDescriptor_t(sizeof(PredefinedStringPartObject_t), 0);

ObjectDescriptor_t *mixinDesc  
  = new ObjectDescriptor_t(sizeof(Mixin_t), 0);
ObjectDescriptor_t *objectDesc 
  = new ObjectDescriptor_t(sizeof(Object_t), 0);
ObjectDescriptor_t *threadDesc 
  = new ObjectDescriptor_t(sizeof(Thread_t), 0);
ObjectDescriptor_t *objRefDesc 
  = new ObjectDescriptor_t(sizeof(ObjectReference_t), 0);
ObjectDescriptor_t *ptnRefDesc 
  = new ObjectDescriptor_t(sizeof(PatternReference_t), 0);
ObjectDescriptor_t *schedDesc  
  = new ObjectDescriptor_t(sizeof(Scheduler_t), 0);
ObjectDescriptor_t *oneCharStringDesc 
  = new ObjectDescriptor_t(2*sizeof(char), 0); //includes zero-termination
ObjectDescriptor_t *emptyPtnDesc 
  = new ObjectDescriptor_t(sizeof(EmptyPattern_t), 0); 

static ObjectDescriptor_t* strDescs[MAXSTRLENGTH/sizeof(void*)];
static ObjectDescriptor_t* ptnDescs[MAXPTNLENGTH];

MainPart *sharedStringPredefMP = new MainPart();
MainPart *sharedBooleanPredefMP = new MainPart();
MainPart *sharedCharPredefMP = new MainPart();
MainPart *sharedIntPredefMP = new MainPart();
MainPart *sharedRealPredefMP = new MainPart();

ObjectDescriptor_t* getStrObjectDescriptor(unsigned int length){
#ifdef DEBUG
  if(length > MAXSTRLENGTH) {
    cerr << "getStrObjectDescriptor(): string too long ("
	 << length << "), increase MAXSTRLENGTH\n";
    exit(1);
  }
#endif //DEBUG
  return (length % sizeof(void*))
    ? strDescs[(length & ~(sizeof(void*) - 1)) / sizeof(void*)]
    : strDescs[(length / sizeof(void*)) - 1];
}

//length = number of mixins
ObjectDescriptor_t* getPtnObjectDescriptor(unsigned int length){
#ifdef DEBUG
  if(length >= MAXPTNLENGTH) {
    cerr << "getPtnObjectDescriptor(): pattern too long, (" 
	 << length << "), increase MAXPTNLENGTH\n";
    exit(1);
  }
#endif //DEBUG
  // lazy evaluation
  if(!ptnDescs[length]){
    ptnDescs[length] 
      = new ObjectDescriptor_t(sizeof(Pattern_t) + 
			       ((sizeof(void*)) * (length - 1)), 0); 
  }
  return ptnDescs[length];
}

void fixObjectDescriptors(void) {

  // --- create predefined part objects' (primitive's) object descriptors ---

  // - bool -
  PredefinedPartObject_t<bool> *dummyPOBool 
    = new PredefinedPartObject_t<bool>(0, 0);
  predefBoolPODesc->setReferenceAt(dummyPOBool->getMixinOffset());
  predefBoolPODesc->setReferenceAt(dummyPOBool->getObjectOffset());
  predefBoolPODesc->setReferenceAt(dummyPOBool->get_nextOffset());
  predefBoolPODesc->setReferenceAt(dummyPOBool->get_prevOffset());
  free(dummyPOBool);

  // - char -
  PredefinedPartObject_t<char> *dummyPOChar 
    = new PredefinedPartObject_t<char>(0, 0);
  predefCharPODesc->setReferenceAt(dummyPOChar->getMixinOffset());
  predefCharPODesc->setReferenceAt(dummyPOChar->getObjectOffset());
  predefCharPODesc->setReferenceAt(dummyPOChar->get_nextOffset());
  predefCharPODesc->setReferenceAt(dummyPOChar->get_prevOffset());
  free(dummyPOChar);

  // - integer -
  PredefinedPartObject_t<int> *dummyPOInt 
    = new PredefinedPartObject_t<int>(0, 0);
  predefIntPODesc->setReferenceAt(dummyPOInt->getMixinOffset());
  predefIntPODesc->setReferenceAt(dummyPOInt->getObjectOffset());
  predefIntPODesc->setReferenceAt(dummyPOInt->get_nextOffset());
  predefIntPODesc->setReferenceAt(dummyPOInt->get_prevOffset());
  free(dummyPOInt);

  // - real -
  PredefinedPartObject_t<real> *dummyPOReal 
    = new PredefinedPartObject_t<real>(0, 0);
  predefRealPODesc->setReferenceAt(dummyPOReal->getMixinOffset());
  predefRealPODesc->setReferenceAt(dummyPOReal->getObjectOffset());
  predefRealPODesc->setReferenceAt(dummyPOReal->get_nextOffset());
  predefRealPODesc->setReferenceAt(dummyPOReal->get_prevOffset());
  free(dummyPOReal);

  // - string -
  PredefinedStringPartObject_t* dummyPredefStrPO 
    = new PredefinedStringPartObject_t(0,0);
  //mark prev and next pointers (for gc)
  predefStringPODesc->setReferenceAt(dummyPredefStrPO->get_nextOffset());
  predefStringPODesc->setReferenceAt(dummyPredefStrPO->get_prevOffset());
  predefStringPODesc->setReferenceAt(dummyPredefStrPO->getMixinOffset());
  predefStringPODesc->setReferenceAt(dummyPredefStrPO->getObjectOffset());
#ifdef NEW_STYLE_GC
  //no need to mark str as reference since we do not move str_PO's
  //(resident in stack/root space)
#else //NEW_STYLE_GC
  predefStringPODesc->setReferenceAt(dummyPredefStrPO->getStrOffset());
#endif //NEW_STYLE_GC

  free(dummyPredefStrPO);

  // --- Gbeta & vm entities ---

  // - mixin -
  //set reference points in general objectDescriptor (for Mixin_t)
  //we need a dummy-object for this
  Mixin_t *dummyMixin = new Mixin_t(0, 0);
  mixinDesc->setReferenceAt(dummyMixin->getOriginOffset());
  free(dummyMixin);

  // - object -
  //set reference points in general objectDescriptor (for Object_t)
  //we need a dummy-object for this
  Object_t *dummyObj = new Object_t();
  objectDesc->setReferenceAt(dummyObj->getHeadOffset());
  objectDesc->setReferenceAt(dummyObj->getTailOffset());
  objectDesc->setReferenceAt(dummyObj->getPatternOffset());
  free(dummyObj);

  // - thread -
  //set reference points in object descriptor
  //dummy-constructor handles setting the reference points in threadDesc
  Thread_t *dummyThread = new Thread_t(threadDesc);
  free(dummyThread);

  // - object ref -
  ObjectReference_t *dummyObjRef = new ObjectReference_t();
  objRefDesc->setReferenceAt(dummyObjRef->getQualificationOffset());
  objRefDesc->setReferenceAt(dummyObjRef->getValueOffset());
  free(dummyObjRef);

  // - pattern ref -
  PatternReference_t *dummyPtnRef = new PatternReference_t();
  ptnRefDesc->setReferenceAt(dummyPtnRef->getQualificationOffset());
  ptnRefDesc->setReferenceAt(dummyPtnRef->getValueOffset());
  free(dummyPtnRef);

  // - scheduler -
  Scheduler_t *dummySched = new Scheduler_t();
  schedDesc->setReferenceAt(dummySched->getPredefOffset());
  schedDesc->setReferenceAt(dummySched->getMainThreadOffset());
  schedDesc->setReferenceAt(dummySched->getEmptyPatternOffset());
  free(dummySched);

  // - one char string -
  //no fixing needed

  // - empty pattern (descriptor) -
  //no fixing needed

  // - empty pattern -
  //has been moved to scheduler, to make a non-root object

  // - create string object descriptor array -
  for(unsigned int i = 0; i < (MAXSTRLENGTH / sizeof(void*)); i++){
    strDescs[i] = new ObjectDescriptor_t(i * sizeof(void*) + sizeof(void*), 0);
  }

  //add shared mainpart for PredefMixins
  sharedStringPredefMP->setObjectDescriptor(predefStringPODesc); 
  sharedStringPredefMP->addMPAttrInitInstr(new Instr_ReturnMPAttrInit_t());

  sharedBooleanPredefMP->setObjectDescriptor(predefBoolPODesc);
  sharedBooleanPredefMP->addMPAttrInitInstr(new Instr_ReturnMPAttrInit_t());

  sharedCharPredefMP->setObjectDescriptor(predefCharPODesc); 
  sharedCharPredefMP->addMPAttrInitInstr(new Instr_ReturnMPAttrInit_t());

  sharedIntPredefMP->setObjectDescriptor(predefIntPODesc);
  sharedIntPredefMP->addMPAttrInitInstr(new Instr_ReturnMPAttrInit_t());

  sharedRealPredefMP->setObjectDescriptor(predefRealPODesc);
  sharedRealPredefMP->addMPAttrInitInstr(new Instr_ReturnMPAttrInit_t());
}
