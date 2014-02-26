//$Id: Scheduler.cc,v 1.25 2001/05/28 12:56:25 ilsoe Exp $

//#include "Scheduler.hpp"

#include "vm.hpp"

Scheduler_t *scheduler;

void gcs_prepareForGC(void) {
  DB_INDENT;
  DB_OUT(vm_sch, "void gcs_prepareForGC(void): called\n");
  if(scheduler) {
    scheduler->prepareForGC();
  }
  DB_OUTDENT;
}

void *Scheduler_t::operator new(size_t size, void *location) {
  return location;
}

void *Scheduler_t::operator new(size_t size) {
  return malloc(sizeof(Thread_t));
}

void Scheduler_t::startEngine(char* fileName){
  DB_INDENT;

  DB_OUT(vm_sch, "Scheduler_t::startEngine(): ***Initializing " 
	 << "object system***\n");  

  setVMReference(this, (void**)&emptyPattern, 
		 new(allocateVM(emptyPtnDesc)) EmptyPattern_t());

  setVMReference(this, (void**)&predef, getPredef(this));

  DB_OUT(vm_sch, "Scheduler_t::startEngine(): *parsing mainparts*\n");  

  InstructionParser parser(fileName);
  mainParts = parser.parse();
  MainPart *firstMainPart = mainParts->first();
  
  DB_OUT(vm_sch, "Scheduler_t::startEngine(): *parsed mainparts*\n");  

  //Making the mixin connected with the first mainPart
  Mixin_t* mixin = new(allocateVM(mixinDesc)) Mixin_t(firstMainPart, predef);

  DB_OUT(vm_sch, "Scheduler_t::startEngine(): made mixin\n");  

  //Making the Pattern having this mixin

  //The first main part pattern always has one mixin
  ObjectDescriptor_t *patDesc = new ObjectDescriptor_t(sizeof(Pattern_t), 0); 
  Pattern_t *pattern = new(allocateVM(patDesc)) Pattern_t(patDesc);
  DB_OUT(vm_sch, "Scheduler_t::startEngine(): made pattern\n");  

  pattern->addMixin(0, mixin);

  DB_OUT(vm_sch, "Scheduler_t::startEngine(): ***pattern made " 
	 << " starting execution***\n");
  
#ifdef NEW_STYLE_GC
  setVMReference(this, 
		 (void**)&mainThread, 
		 new (allocateVM(threadDesc)) Thread_t(pattern, this));

#else //NEW_STYLE_GC
  setVMReference(this, 
		 (void**)&mainThread, 
		 new (allocateRootVM(threadDesc)) Thread_t(pattern, this));
#endif //NEW_STYLE_GC
  mainLoop();
  DB_OUTDENT;
}

unsigned int Scheduler_t::getPredefOffset(){
  return ((unsigned int)&predef - (unsigned int)this)/sizeof(void*);
}

unsigned int Scheduler_t::getMainThreadOffset(){
  return ((unsigned int)&mainThread - (unsigned int)this)/sizeof(void*);
}

unsigned int Scheduler_t::getEmptyPatternOffset(void){
  return ((unsigned int)&emptyPattern - (unsigned int)this)/sizeof(void*);
}

//gc interface function, see header file
void Scheduler_t::prepareForGC(void) {
  if(mainThread) {
    mainThread->sendSignal(THR_SIGNAL_GC_STOP);
  } else {
    DB_OUT(vm_sch, "Scheduler_t::prepareForGC(): Warning: gc request ignored "
	   << "- object system not yet initialized!\n");
  }
}

MainPart* Scheduler_t::getMainPart(char* id){
  DB_INDENT;
  DB_OUT(vm_sch, "Scheduler_t::getMainPart() called with id: " << id << endl);
  MainPart* mp = mainParts->first();
  for( ; mp != NULL ; mp = mp -> next()){
    if(!(strcmp(mp->getId(), id))){
      DB_OUT(vm_sch, "Scheduler_t::getMainPart(): returning mainpart: " 
	     << endl << mp);
      DB_OUTDENT;
      return mp;
    }
  }
  DB_OUTDENT;
  return NULL;
}

void Scheduler_t::mainLoop() {
  //has to be reprogrammed to handle more threads
  int threadState;
  DB_INDENT;
  do {
    threadState = mainThread->run();
    if(threadState == THR_STATE_GC_STOPPED) {
#ifdef NEW_STYLE_GC
      garbageCollector.garbageCollect();
#else  //NEW_STYLE_GC
      garbageCollector->garbageCollect();
#endif //NEW_STYLE_GC
      mainThread->sendSignal(THR_SIGNAL_RESUME);
    }
    //does not handle all signals in the current implementation
  } while (threadState == THR_STATE_GC_STOPPED);

  switch (threadState) {
  case THR_STATE_FINISHED:
    DB_OUT(vm_sch, "Thread terminated normally\n");
    break;
  default:
    DB_OUT(vm_sch, "Thread terminating with state: " << threadState << endl);
    break;
  }
  DB_OUTDENT;
}

Pattern_t* Scheduler_t::getEmptyObjectPattern(){
  return emptyPattern;
}
