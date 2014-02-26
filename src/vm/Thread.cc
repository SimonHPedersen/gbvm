//$Id: Thread.cc,v 1.99 2001/05/28 12:56:25 ilsoe Exp $

#include "vm.hpp"

#ifdef DEBUG
int instrsExecuted = 0;

void atExitFkt(void) {
  cout << "Last instruction executed was: " << instrsExecuted << endl;
}
#endif //DEBUG

void *Thread_t::operator new(size_t size, void *location) {
  return location;
}

void *Thread_t::operator new(size_t size) {
  return malloc(sizeof(Thread_t));
}

Thread_t::Thread_t(Pattern_t *pattern, Scheduler_t* sch) {
  DB_INDENT;
  myScheduler = sch;

#ifdef DEBUG
  instrsExecuted = 0;
  atexit(atExitFkt);
#endif

  DB_OUT(vm_thread,"Thread_t::Thread_t(): making int stack\n");
  //create integer stack
  ObjectDescriptor_t *stackObjDesc = getStackObjDesc(STACK_SIZE, 0);
#ifdef NEW_STYLE_GC
  //VMObject_t *stackSpace = garbageCollector.addRootObject(stackObjDesc);
  intStack = new(allocateRootVM_NT(stackObjDesc))
    NonRefStack_t<int> (STACK_SIZE, stackObjDesc);
#else //NEW_STYLE_GC
//  VMObject_t *stackSpace = garbageCollector->addNewObject(stackObjDesc);
  setVMReference(this, (void**)&intStack,
		 new(allocateVM(stackObjDesc)) NonRefStack_t<int>(STACK_SIZE, 
								  stackObjDesc));
#endif //NEW_STYLE_GC

  DB_OUT(vm_thread,"Thread_t::Thread_t(): making bool stack\n");
  //bool stack
  stackObjDesc = getStackObjDesc(STACK_SIZE, 0);
#ifdef NEW_STYLE_GC
  boolStack    = new(allocateRootVM_NT(stackObjDesc))
    NonRefStack_t<bool>(STACK_SIZE, stackObjDesc);
#else //NEW_STYLE_GC
  setVMReference(this, (void**)&boolStack,
		 new(allocateVM(stackObjDesc)) NonRefStack_t<bool>(STACK_SIZE, 
								   stackObjDesc));
#endif //NEW_STYLE_GC


  DB_OUT(vm_thread,"Thread_t::Thread_t(): making char stack\n");
  //char stack
  stackObjDesc = getStackObjDesc(STACK_SIZE, 0);
#ifdef NEW_STYLE_GC
  charStack    =  new(allocateRootVM_NT(stackObjDesc))
    NonRefStack_t<unsigned char>(STACK_SIZE, stackObjDesc);
#else //NEW_STYLE_GC
  setVMReference(this, 
		 (void**)&charStack,
		 new(allocateVM(stackObjDesc)) 
		     NonRefStack_t<unsigned char>(STACK_SIZE, 
						  stackObjDesc));
#endif //NEW_STYLE_GC


  DB_OUT(vm_thread,"Thread_t::Thread_t(): making real stack\n");
  //real stack
  stackObjDesc = getStackObjDesc(STACK_SIZE, 0);
#ifdef NEW_STYLE_GC
  realStack    = new(allocateRootVM_NT(stackObjDesc))
    NonRefStack_t<real>(STACK_SIZE, stackObjDesc);
#else //NEW_STYLE_GC
  setVMReference(this, (void**)&realStack,
		 new(allocateVM(stackObjDesc)) NonRefStack_t<real>(STACK_SIZE, 
								   stackObjDesc));
#endif //NEW_STYLE_GC


  DB_OUT(vm_thread,"Thread_t::Thread_t(): making string stack\n");
  //string stack ??????????? FJERNE STRING OBJEKTER FRA ROOT SPACE ?????????????
  stackObjDesc = getStackObjDesc(STACK_SIZE, UINT_MAX);
#ifdef NEW_STYLE_GC
  stringStack  = new(allocateRootVM_T(stackObjDesc))
    RefStack_t<char*>(STACK_SIZE, stackObjDesc);
#else //NEW_STYLE_GC
  setVMReference(this, (void**)&stringStack,
		 new(allocateVM(stackObjDesc)) RefStack_t<char*>(STACK_SIZE, 
								 stackObjDesc));
#endif //NEW_STYLE_GC

 
  DB_OUT(vm_thread,"Thread_t::Thread_t(): making exe stack\n");
  //execution stack
  stackObjDesc = getStackObjDesc(STACK_SIZE, 0);
#ifdef NEW_STYLE_GC
  executionStack = new(allocateRootVM_NT(stackObjDesc))
    NonRefStack_t<Instruction*>(STACK_SIZE, stackObjDesc);
#else //NEW_STYLE_GC
  setVMReference(this, (void**)&executionStack,
		 new(allocateVM(stackObjDesc)) 
		     NonRefStack_t<Instruction*>(STACK_SIZE, stackObjDesc));
#endif //NEW_STYLE_GC


  DB_OUT(vm_thread,"Thread_t::Thread_t(): making obj stack\n");
  //object stack //DEBUG SIZE
  stackObjDesc = getStackObjDesc(STACK_SIZE, UINT_MAX);
#ifdef NEW_STYLE_GC
  objectStack = new(allocateRootVM_T(stackObjDesc))
    RefStack_t<Object_t*>(STACK_SIZE, stackObjDesc);
#else //NEW_STYLE_GC
  setVMReference(this, (void**)&objectStack,
		 new(allocateVM(stackObjDesc)) 
		     RefStack_t<Object_t*>(STACK_SIZE, 
					   stackObjDesc));
#endif //NEW_STYLE_GC

  DB_OUT(vm_thread,"Thread_t::Thread_t(): making ptn stack\n");
  //pattern stack
  stackObjDesc = getStackObjDesc(STACK_SIZE, UINT_MAX);
#ifdef NEW_STYLE_GC
  patternStack = new(allocateRootVM_T(stackObjDesc))
    RefStack_t<Pattern_t*>(STACK_SIZE, stackObjDesc);
#else //NEW_STYLE_GC
  setVMReference(this, (void**)&patternStack,
		 new(allocateVM(stackObjDesc)) 
		     RefStack_t<Pattern_t*>(STACK_SIZE, 
					    stackObjDesc));
#endif //NEW_STYLE_GC

  DB_OUT(vm_thread,"Thread_t::Thread_t(): making tmp stack\n");
  //tmp stack
  stackObjDesc = getStackObjDesc(STACK_SIZE, UINT_MAX);
#ifdef NEW_STYLE_GC
  tmpStack     = new(allocateRootVM_T(stackObjDesc)) 
    RefStack_t<AttributeInstance_t*>(STACK_SIZE, stackObjDesc);
#else
  setVMReference(this, (void**)&tmpStack,
		 new(allocateVM(stackObjDesc)) 
		 RefStack_t<AttributeInstance_t*>(STACK_SIZE, 
						  stackObjDesc));
#endif
  DB_OUT(vm_thread,"Thread_t::Thread_t(): making CPO stack\n");
  //current part object stack
  stackObjDesc = getStackObjDesc(STACK_SIZE, UINT_MAX);
#ifdef NEW_STYLE_GC
  currentPOStack = new(allocateRootVM_T(stackObjDesc))
    RefStack_t<PartObject_t*>(STACK_SIZE, stackObjDesc);
#else
  setVMReference(this, (void**)&currentPOStack,
		 new(allocateVM(stackObjDesc)) 
		     RefStack_t<PartObject_t*>(STACK_SIZE, stackObjDesc));
#endif

  //doing: this->mainObject = pattern->instantiate(this);
  if(pattern){
    //intantiating pattern
    Object_t *object = pattern->instantiate(this);
    setVMReference(this, (void**)&(this->mainObject), object);
  }
  
  //initializing thread execute state
  signal = THR_SIGNAL_NONE;
  state  = THR_STATE_RUNNING;
  pc     = mainObject->getFirstInstructionPushPOANF(this);
  executionStack->push(pc);
  currentPOStack->push(mainObject->first());
  tmpStack->addNewFrame();
  pc     = mainObject->first()->getMPAttrsInstrPtr()->first();
  DB_OUTDENT;
}


//This constructor should NOT be used except when initializing the object descriptor
Thread_t::Thread_t(ObjectDescriptor_t *objDesc) {
  DB_INDENT;
#ifndef NEW_STYLE_GC
  objDesc->setReferenceAt(((unsigned int)&intStack      -(unsigned int)this)/sizeof(void*));
  objDesc->setReferenceAt(((unsigned int)&boolStack     -(unsigned int)this)/sizeof(void*));
  objDesc->setReferenceAt(((unsigned int)&charStack     -(unsigned int)this)/sizeof(void*));
  objDesc->setReferenceAt(((unsigned int)&realStack     -(unsigned int)this)/sizeof(void*));
  objDesc->setReferenceAt(((unsigned int)&stringStack   -(unsigned int)this)/sizeof(void*));
  objDesc->setReferenceAt(((unsigned int)&executionStack-(unsigned int)this)/sizeof(void*));
  objDesc->setReferenceAt(((unsigned int)&objectStack   -(unsigned int)this)/sizeof(void*));
  objDesc->setReferenceAt(((unsigned int)&patternStack  -(unsigned int)this)/sizeof(void*));
  objDesc->setReferenceAt(((unsigned int)&tmpStack      -(unsigned int)this)/sizeof(void*));
  objDesc->setReferenceAt(((unsigned int)&currentPOStack-(unsigned int)this)/sizeof(void*));
#endif
  //OBJECT + CPO markeres
  objDesc->setReferenceAt(((unsigned int)&mainObject  -(unsigned int)this)/sizeof(void*));

  DB_OUT(gc_objd, "Thread_T::Thread_t(): made object descriptor: " << objDesc);
  DB_OUTDENT;
}

//this macro is used in the run method (twice)
#define CHECK_SIGNALS                                                           \
  if(signal != THR_SIGNAL_NONE) {                                               \
    switch (signal) {                                                           \
    case THR_SIGNAL_GC_STOP:                                                    \
      state = THR_STATE_GC_STOPPED;                                             \
      signal = THR_SIGNAL_NONE;                                                 \
      /*possible cleanup before gc goes here (eg. stack traversal in the future)*/ \
      DB_OUTDENT;                                                               \
      return state;                                                             \
      break;                                                                    \
    case THR_SIGNAL_RESUME:                                                     \
      state = THR_STATE_RUNNING;                                                \
      signal = THR_SIGNAL_NONE;                                                 \
      break;                                                                    \
    default:                                                                    \
      cerr << "Thread_t::run(): unknown signal, " << signal                     \
	   << "internal error, quitting!";                                      \
      exit(1);                                                                  \
      break;                                                                    \
    }                                                                           \
  }

int Thread_t::run(void) {
  DB_INDENT;

  DB_OUT(vm_status, "mainThread ... starting execution ... wish me luck\n");

  CHECK_SIGNALS;

  //DB_OUT(vm_status, "mainThread ... my state is: ... " << state << endl);

  while(state == THR_STATE_RUNNING){

#ifdef DEBUG
    //check object integrity - this may be very slow!
    
# ifdef INTEGRITY_CHECK
#  ifdef NEW_STYLE_GC
    if(instrsExecuted%50 == 0) {
    //if(instrsExecuted > 99600) {
      //DB_ENABLE_GC_ALL;
      //cout << "(instr: " << instrsExecuted << ")" << flush;
      garbageCollector.checkIntegrity();
    }
#  else //NEW_STYLE_GC
    if(instrsExecuted%50 == 0) {
      garbageCollector->checkIntegrity();
    }
#  endif //NEW_STYLE_GC
# endif //INTEGRITY_CHECK

    print();
    ++instrsExecuted;

    /*
    if(instrsExecuted == 66019){ // DONT_PRINT_UNTIL){
      cout << "enabling all debug output\n";                                  
      DB_ENABLE_ALL;
    }
    */

    DB_OUT(vm_thread_instinfo, "Thread_t:: executing instruction " <<
	   instrsExecuted <<": " << pc << " with id: " << pc->id << endl);

#endif //DEBUG

    switch(pc->id) {
    case ADDMP_ID:
      ADDMP_MAC;
      break;
    case ADDOPPLUS_ID:
      ADDOPPLUS_MAC;
      break;
    case ADDOPMINUS_ID:
      ADDOPMINUS_MAC;
      break;
    case ADDOPOR_ID:
      ADDOPOR_MAC;
      break;
    case ADDOPXOR_ID:
      ADDOPXOR_MAC;
      break;
    case CALL_ID:
      CALL_MAC;
      break;
      /*
    case CALLREP_ID:
      CALLREP_MAC;
      break;
      */
    case CHKNONE_ID:
      CHKNONE_MAC;
      break;
    case CHKPTNNONE_ID:
      CHKPTNNONE_MAC;
      break;
    case CHKQUALOBJ_ID:
      CHKQUALOBJ_MAC;
      break;
    case DISCARD_ID:
      DISCARD_MAC;
      break;
      /*
    case ENSURECMP_ID:
      ENSURECMP_MAC;
      break;
    case EXTEND_ID:
      EXTEND_MAC;
      break;
    case FORK_ID:
      FORK_MAC;
      break;
      */
    case GATHERVIRT_ID:
      GATHERVIRT_MAC;
      break;
      /*
    case GETSIZE_ID:
      GETSIZE_MAC;
      break;
      */
    case GENERALIF_ID:
      GENERALIF_MAC;
      break;
    case INNER_ID:
      INNER_MAC;
      break;
    case INSTALLPTN_ID:
      INSTALLPTN_MAC;
      break;
    case INSTALLQUALPTN_ID:
      INSTALLQUALPTN_MAC;
      break;
    case INSTALLOBJ_ID:
      INSTALLOBJ_MAC;
      break;
    case INSTALLQUALOBJ_ID:
      INSTALLQUALOBJ_MAC;
      break;
      /*
    case INSTALLREP_ID:
      INSTALLREP_MAC;
      break;
    case KILL_ID:
      KILL_MAC;
      break;
    case LOCSIMPIF_ID:
      LOCSIMPIF_MAC;
      break;
      */
    case MERGE_ID:
      MERGE_MAC;
      break;
    case MULOPMUL_ID:
      MULOPMUL_MAC;
      break;
    case MULOPDIV_ID:
      MULOPDIV_MAC;
      break;
    case MULOPAND_ID:
      MULOPAND_MAC;
      break;
    case MULOPDIVINT_ID:
      MULOPDIVINT_MAC;
      break;
    case MULOPMOD_ID:
      MULOPMOD_MAC;
      break;
    case NEGINT_ID:
      NEGINT_MAC;
      break;
    case NEGREAL_ID:
      NEGREAL_MAC;
      break;
    case NEWPTNOBJ_ID:
      NEWPTNOBJ_MAC;
      break;
    case NEWPTNTMP_ID:
      NEWPTNTMP_MAC;
      break;
      /*
    case NEWREP_ID:
      NEWREP_MAC;
      break;
      */
    case NOT_ID:
      NOT_MAC;
      break;
    case NAMEDFOR_ID:
      NAMEDFOR_MAC;
      break;
      /*
    case OSSYSIN_ID:
      OSSYSIN_MAC;
      break;
    case OSSYSINOUT_ID:
      OSSYSINOUT_MAC;
      break;
    case PEEKINXOBJREF_ID:
      PEEKINXOBJREF_MAC;
      break;
    case PEEKINXTMPREF_ID:
      PEEKINXTMPREF_MAC;
      break;
      */     
    case PEEKOBJREF_ID:
      PEEKOBJREF_MAC;
      break;
    case PEEKTMPREF_ID:
      PEEKTMPREF_MAC;
      break;
    case POPBOOL_ID:
      POPBOOL_MAC;
      break;
      /*
    case POPBOOLVAL_ID:
      POPBOOLVAL_MAC;
      break;
      */
    case POPCHAR_ID:
      POPCHAR_MAC;
      break;
    case POPCHARPUSHINT_ID:
      POPCHARPUSHINT_MAC;
      break;
    case POPCHARPUSHREAL_ID:
      POPCHARPUSHREAL_MAC;
      break;
    case POPCHARPUSHSTR_ID:
      POPCHARPUSHSTR_MAC;
      break;
      /*
    case POPCHARVAL_ID:
      POPCHARVAL_MAC;
      break;
      */
    case POPINTPUSHCHARAT_ID:
      POPINTPUSHCHARAT_MAC;
      break;
    case POPINT_ID:
      POPINT_MAC;
      break;
    case POPINTPUSHCHAR_ID:
      POPINTPUSHCHAR_MAC;
      break;
    case POPINTPUSHREAL_ID:
      POPINTPUSHREAL_MAC;
      break;
      /*
    case POPINTVAL_ID:
      POPINTVAL_MAC;
      break;
    case POPINXOBJREF_ID:
      POPINXOBJREF_MAC;
      break;
    case POPINXPTNREF_ID:
      POPINXPTNREF_ID;
      break;
      */
    case POPOBJPUSHPTN_ID:
      POPOBJPUSHPTN_MAC;
      break;
    case POPOBJPUSHTMP_ID:
      POPOBJPUSHTMP_MAC;
      break;
    case POPOBJREF_ID:
      POPOBJREF_MAC;
      break;
      /*
    case POPPTNSPECOBJ_ID:
      POPPTNSPECOBJ_MAC;
      break;
      */
    case POPPTNREF_ID:
      POPPTNREF_MAC;
      break;
    case POPREAL_ID:
      POPREAL_MAC;
      break;
    case POPREALPUSHINT_ID:
      POPREALPUSHINT_MAC;
      break;
      /* deprecated
    case POPREALVALUE_ID:
      POPREALVALUE_MAC;
      break;
      */
    case POPSTR_ID:
      POPSTR_MAC;
      break;
      /*
    case POPSTRREP_ID:
      POPSTRREP_MAC;
      break;
      */
      /* deprecated
    case POPSTRVAL_ID:
      POPSTRVAL_MAC;
      break;
      */
    case POPSTRPUSHCHAR_ID:
      POPSTRPUSHCHAR_MAC;
      break;
    case POPSTRPUSHINT_ID:
      POPSTRPUSHINT_MAC;
      break;
    case POPSTRPUSHREAL_ID:
      POPSTRPUSHREAL_MAC;
      break;
    case PUSHBOOL_ID:
      PUSHBOOL_MAC;
      break;
      /*
    case PUSHBOOLVAL_ID:
      PUSHBOOLVAL_MAC;
      break;
      */
    case PUSHCHAR_ID:
      PUSHCHAR_MAC;
      break;
      /*
    case PUSHCHARVAL_ID:
      PUSHCHARVAL_MAC;
      break;
      */
    case PUSHINDEX_ID:
      PUSHINDEX_MAC;
      break;
    case PUSHINT_ID:
      PUSHINT_MAC;
      break;
      /* deprecated ...
    case PUSHINTVAL_ID:
      PUSHINTVAL_MAC;
      break;
    case PUSHINXOBJ_ID:
      PUSHINXOBJ_MAC;
      break;
    case PUSHINXOBJREF_ID:
      PUSHINXOBJREF_MAC;
      break;
    case PUSHINXPTNREF_ID:
      PUSHINXPTNREF_MAC;
      break;
    case PUSHINXQUAL_ID:
      PUSHINXQUAL_MAC;
      break;
    case PUSHINXTMPOBJ_ID:
      PUSHINXTMPOBJ_MAC;
      break;
    case PUSHINXTMPOBJREF_ID:
      PUSHINXTMPOBJREF_MAC;
      break;
      */
    case PUSHOBJ_ID:
      PUSHOBJ_MAC;
      break;
      /* parsed to PUSHOBJ
    case PUSHOBJREF_ID:
      PUSHOBJREF_MAC;
      break;
      */
    case PUSHPTN_ID:
      PUSHPTN_MAC;
      break;
    case PUSHPTNOBJ_ID:
      PUSHPTNOBJ_MAC;
      break;
      /* parsed to PUSHPTN
    case PUSHPTNREF_ID:
      PUSHPTNREF_MAC;
      break;
      */
    case PUSHQUAL_ID:
      PUSHQUAL_MAC;
      break;
    case PUSHREAL_ID:
      PUSHREAL_MAC;
      break;
      /* deprecated
    case PUSHREALVAL_ID:
      PUSHREALVAL_MAC;
      break;
      */
    case PUSHSTRLEN_ID:
      PUSHSTRLEN_MAC;
      break;
      /*
    case PUSHSTRREP_ID:
      PUSHSTRREP_MAC;
      break;
      */
    case PUSHSTR_ID:
      PUSHSTR_MAC;
      break;
      /* deprecated
    case PUSHSTRVAL_ID:
      PUSHSTRVAL_MAC;
      break;
      */
    case PUSHTMPOBJ_ID:
      PUSHTMPOBJ_MAC;
      break;
    case PUSHTMPOBJREF_ID:
      PUSHTMPOBJREF_MAC;
      break;
    case PUSHINONEOBJ_ID:
      PUSHINONEOBJ_MAC;
      break;
    case PUSHINONEPTN_ID:
      PUSHINONEPTN_MAC;
      break;
    case PUSHIBOOLLIT_ID:
      PUSHIBOOLLIT_MAC;
      break;
    case PUSHICHARLIT_ID:
      PUSHICHARLIT_MAC;
      break;
    case PUSHIINTLIT_ID:
      PUSHIINTLIT_MAC;
      break;
    case PUSHIREALLIT_ID:
      PUSHIREALLIT_MAC;
      break;
    case PUSHISTRLIT_ID:
      PUSHISTRLIT_MAC;
      break;
    case RELOPLT_ID:
      RELOPLT_MAC;
      break;
    case RELOPLTEQ_ID:
      RELOPLTEQ_MAC;
      break;
    case RELOPNEQ_ID:
      RELOPNEQ_MAC;
      break;
    case RELOPEQ_ID:
      RELOPEQ_MAC;
      break;
    case RELOPGT_ID:
      RELOPGT_MAC;
      break;
    case RELOPGTEQ_ID:
      RELOPGTEQ_MAC;
      break;
    case RESETFRAME_ID:
      RESETFRAME_MAC;
      break;
      /*
    case RESIZEREP_ID:
      RESIZEREP_MAC;
      break;
    case SEMCOUNT_ID:
      SEMCOUNT_MAC;
      break;
    case SEMP_ID:
      SEMP_MAC;
      break;
    case SEMTRYP_ID:
      SEMTRYP_MAC;
      break;
    case SEMV_ID:
      SEMV_MAC;
      break;
    case SUSPEND_ID:
      SUSPEND_MAC;
      break;
      */
    case SIMPLEFOR_ID:
      SIMPLEFOR_MAC;
      break;
    case SIMPLEIF_ID:
      SIMPLEIF_MAC;
      break;
    case STDIOIN_ID:
      STDIOIN_MAC;
      break;
    case STDIOOUT_ID:
      STDIOOUT_MAC;
      break;
      /*
    case WHEN_ID:
      WHEN_MAC;
      break;
      */
    case WHILE_ID:
       WHILE_MAC;
       break;

      //Special instructions

    case RETURN_ID:
      RETURN_MAC;
      break;
    case RETURNNPPO_ID:
      RETURNNPPO_MAC;
      break;
    case RETURNMPATTRINIT_ID:
      RETURNMPATTRINIT_MAC;
      break;
    case CONTGATHERVIRT_ID:
      CONTGATHERVIRT_MAC;
      break;
    case JUMPNPPO_ID:
      JUMPNPPO_MAC;
      break;
    case JUMPTRUENPPO_ID:
      JUMPTRUENPPO_MAC;
      break;
    case JUMPSUBNPPO_ID:
      JUMPSUBNPPO_MAC;
      break;
    case COPYTOP_ID:
      COPYTOP_MAC;
      break;
    case COPYTOP2_ID:
      COPYTOP2_MAC;
      break;
    case UNSUPPORTED_ID:
      UNSUPPORTED_MAC;
      break;
    default: 
      cerr << "Thread_t::start(void): unsupported instruction id("
	   << pc->id << "): " << pc << endl;
      exit(1);
      break;
    }
    CHECK_SIGNALS; //check for signals after each instruction
  }
  
  //print stacks
#ifdef DEBUG
  print();
#endif  

  DB_OUTDENT;
  return state;
}


int Thread_t::getState(void) {
  return state;
}

void Thread_t::sendSignal(int signal) {
  this->signal = signal;
}

void Thread_t::print(){
  DB_INDENT;
  DB_OUT(vm_nrstack, "integer stack:   ");
  intStack->print();
  DB_OUT(vm_nrstack, "boolean stack:   ");
  boolStack->print();
  DB_OUT(vm_nrstack, "char stack:      ");
  charStack->print();
  DB_OUT(vm_nrstack, "real stack:      ");
  realStack->print();
  DB_OUT(vm_rstack, "string stack:    ");
  stringStack->printStr();
  DB_OUT(vm_rstack, "object stack:    ");
  objectStack->print(1);
  DB_OUT(vm_rstack, "pattern stack:   ");
  patternStack->print(1);
  DB_OUT(vm_rstack, "tmp stack:       ");
  tmpStack->print(1);
  DB_OUT(vm_rstack, "cpo stack:       ");
  currentPOStack->print(1);
  DB_OUT(vm_rstack, "execution stack: ");
  executionStack->print();
  DB_OUTDENT;
}

//merges p1&p2 and returns the result or NULL 
Pattern_t* Thread_t::merge(Pattern_t* p1, Pattern_t* p2){
  DB_INDENT;
  
  Mixin_t** p1MixinList = p1->getMixins(); 
  unsigned int p1Length = p1->getLength();				   
  Mixin_t** p2MixinList = p2->getMixins();
  unsigned int p2Length = p2->getLength();
  unsigned int p1I = p1Length-1; //iterator for p1
  unsigned int p2I = p2Length-1; //iterator for p2
  unsigned int resI = 0; //result iterator

  //check for EmptyPattern
  if(!p1MixinList[0]){
    DB_OUTDENT;
    return p2;
  }
  if(!p2MixinList[0]){
    DB_OUTDENT;
    return p1;
  }

  DB_OUT(vm_thread_merge, "p1Length: " << p1Length 
	 << " p2Length: " << p2Length << endl); 
  DB_OUT(vm_thread_merge, "p1MixinList[p1I]" << (void*)p1MixinList[p1I] << endl);
  //	 << " p1MixinList[p1I]->origin: " << (void*)p1MixinList[p1I]->origin
  //	 << " p1MixinList[p1I]->getMainPart(): " 
  //     << (void*)p1MixinList[p1I]->getMainPart() << endl);
  DB_OUT(vm_thread_merge, "p2MixinList[p2I]" << (void*)p1MixinList[p2I] << endl);
  // << " p2MixinList[p2I]->origin: " << (void*)p2MixinList[p2I]->origin
  // << " p2MixinList[p2I]->getMainPart(): " 
  // << (void*)p2MixinList[p2I]->getMainPart() << endl);

  //printing patterns
  DB_OUT(vm_thread_merge, "Thread_t::merge: p1" << endl);
  DB_INDENT;
  for(unsigned int i = 0; i < p1Length; i++){
    DB_OUT(vm_thread_merge, "mixins[" << i << "] mainpart id " 
	   << p1MixinList[i]->getMainPart()->getId() << endl);
    DB_OUT(vm_thread_merge, "mixins[" << i << "] mainpart " 
	   << (void*)p1MixinList[i]->getMainPart() << endl);
    DB_OUT(vm_thread_merge, "mixins[" << i << "] origin " 
	   << (void*)(p1MixinList[i]->origin) << endl);
  }
  DB_OUTDENT;
  DB_OUT(vm_thread_merge, "Thread_t::merge: p2" << endl);
  DB_INDENT;
  for(unsigned int i = 0; i < p2Length; i++){
    DB_OUT(vm_thread_merge, "mixins[" << i << "] mainpart id " 
	   << p2MixinList[i]->getMainPart()->getId() << endl);
    DB_OUT(vm_thread_merge, "mixins[" << i << "] mainpart " 
	   << (void*)p2MixinList[i]->getMainPart() << endl);
    DB_OUT(vm_thread_merge, "mixins[" << i << "] origin " 
	   << (void*)(p2MixinList[i]->origin) << endl);
  }
  DB_OUTDENT;
  
  //merge p1 & p2
  DB_OUT(vm_thread_merge, "Thread_t::merge() merging..." << endl);
  bool notP1Finished = 1; 
  bool notP2Finished = 1; 
  while(notP1Finished || notP2Finished){
    DB_OUT(vm_thread_merge, "Thread_t::merge() merging..." << endl);
    if((p1MixinList[p1I]->origin == p2MixinList[p2I]->origin) &&
       (p1MixinList[p1I]->getMainPart() == p2MixinList[p2I]->getMainPart())){
      mergeScratch[resI++] = p1MixinList[p1I];
      if(p1I){
	p1I--;
      } else {
	notP1Finished = 0;
      }
      if(p2I){
	p2I--;
      } else {
	notP2Finished = 0;
      }
    } else if(!(member(p2MixinList, p2I, p1MixinList, p1Length)) && 
	      notP2Finished){
      mergeScratch[resI++] = p2MixinList[p2I];
      if(p2I){
	p2I--;
      } else {
	notP2Finished = 0;
      }
    } else if(!(member(p1MixinList, p1I, p2MixinList, p2Length)) && 
	      notP1Finished){
      mergeScratch[resI++] = p1MixinList[p1I];
      if(p1I){
	p1I--;
      } else {
	notP1Finished = 0;
      }
    } else {
      DB_OUT(vm_thread_merge,"Thread_t::merge(): p1MixinList[" << p1I 
     	     << "]: " << p1MixinList[p1I] << endl);
      DB_OUT(vm_thread_merge,"Thread_t::merge(): p2MixinList[" << p2I 
	     << "]: " << p2MixinList[p2I] << endl);
      DB_OUT(vm_thread_merge,"Thread_t::merge(): " 
	     << "member(p2MixinList, p2I, p1MixinList, p1I) " 
	     << (int)member(p2MixinList, p2I, p1MixinList, p1I) << endl);
      DB_OUT(vm_thread_merge,"Thread_t::merge(): " 
	     << "member(p1MixinList, p1I, p2MixinList, p2I) " 
	     << (int)member(p1MixinList, p1I, p2MixinList, p2I) << endl);
      DB_OUTDENT;
      return NULL;
      break;
    }
  }
  DB_OUT(vm_thread_merge, "Thread_t::merge(): merge done, creating " 
	 << "resulting pattern with " 
	 << resI << " mixins " << endl);

  //create new pattern for result
  ObjectDescriptor_t* ptnDesc = getPtnObjectDescriptor(resI);
  Pattern_t *result = new(allocateVM(ptnDesc)) Pattern_t(ptnDesc);
  DB_OUT(vm_thread_merge, "1 " << endl);
  Mixin_t** resultMixinList = result->getMixins();
  //copy result from scratch to new pattern
  for(unsigned int i = resI, j = 0; i > 0; i--,j++){
    setVMReference(result, (void**)&resultMixinList[j], mergeScratch[i-1]);
  }
  DB_OUT(vm_thread_merge, "Merge completed: " << result << endl);

  DB_OUT(vm_thread_merge, "Thread_t::merge: result" << endl);
  DB_INDENT;
  for(unsigned int i = 0; i < resI; i++){
    DB_OUT(vm_thread_merge, "mixins[" << i << "] mainpart id " 
	   << resultMixinList[i]->getMainPart()->getId() << endl);
    DB_OUT(vm_thread_merge, "mixins[" << i << "] mainpart " 
	   << (void*)resultMixinList[i]->getMainPart() << endl);
    DB_OUT(vm_thread_merge, "mixins[" << i << "] origin " 
	   << (void*)(resultMixinList[i]->origin) << endl);
  }
  DB_OUTDENT;

  DB_OUTDENT;
  return result;
}

//returns true if element list1[it1] is a member of the rest of list2
char Thread_t::member(Mixin_t** list1, unsigned int it1, 
		      Mixin_t** list2, unsigned int list2Length){
  DB_OUT(vm_thread_merge,"Thread_t::merge() .... :" << endl);
  DB_OUT(vm_thread_merge,"List 1 Mixin " << it1 
	 << "'s origin " << list1[it1]->origin << endl);
  DB_OUT(vm_thread_merge,"List 1 Mixin " << it1 
	 << "'s mainpart " << endl 
	 << list1[it1]->getMainPart() << endl);
  for(unsigned int i = 0; i < list2Length; i++){
    if((list1[it1]->origin == list2[i]->origin) && 
      (list1[it1]->getMainPart() == list2[i]->getMainPart())){
      return 1;
    }
  }
  return 0;
}


