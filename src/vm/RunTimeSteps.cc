//$Id: RunTimeSteps.cc,v 1.16 2001/05/21 11:42:52 hempe Exp $

//#include "Thread.hpp"
//#include "RunTimeSteps.hpp"

#include "vm.hpp"

// ----------------------------------------------------------------------
// Printing of run-time entities

ostream& operator << (ostream& stream, RuntimeStep* inst) {
  if(inst) {
    inst->printOn(stream);
  } else {
    stream << "*** tried to print a null RuntimeStep ***\n";
  }
  return stream;
}


ostream& operator << (ostream& stream, RuntimeStep& inst) {
  inst.printOn(stream);
  return stream;
}


//----------------------------------------------------------------------
// Runtime paths

UpStep::UpStep(char** pc){
  DB_INDENT;
  (*pc) = &(*pc)[1]; // ^skippes 'skippes ikke!
  unsigned int comma = 0;
  for( ; (*pc)[comma]!=',' && (*pc)[comma]!='}' ; comma++);
  char *str = (char*)malloc(comma+1);
  memcpy(str, *pc, comma);
  str[comma] = 0;
  mainPartId = str;
  *pc = *pc + comma;
  DB_OUT(vm_rtp,  "UpStep::UpStep parsed with mainPartId: " 
	 << strdup(mainPartId) << endl);
  DB_OUTDENT;
}

void UpStep::printOn(ostream &stream){
  stream << "^" << mainPartId;
}

char* UpStep::getMainPartId(){
  return mainPartId;
}

PartObject_t* UpStep::getPartObject(PartObject_t* cpo, Thread_t* thr){
  //go upwards until mainpartId found or end
  DB_INDENT;
  DB_OUT(vm_rtp,  "UpStep::getPartObjec: Found mainPartId " << 
	 strdup(cpo->getMainPartId())
	 << "wanted mainPartId: " << strdup(mainPartId) << endl);
  while(strcmp(cpo->getMainPartId(),mainPartId)){
    if(cpo->prev()!=NULL){
      DB_OUT(vm_rtp,  ".");
      cpo = cpo->prev();
    } else {
      cerr << "*_*_*_* UpStep::getPartObject: Did not find partobject *_*_*_*\n";
      exit(1);
    }
  }
  DB_OUT(vm_rtp,  "UpStep::getPartObject(): found po " 
	 << "(dots = number of upsteps)\n");
  DB_OUTDENT;
  return cpo;
}

Pattern_t *UpStep::getPattern(PartObject_t* cpo, Thread_t* thr){
  cout << "UpStep::getPattern : *_*_*getPattern called on wrong runtime step" 
       << "*_*_*";
  exit(1);
  return NULL;
}

Object_t *UpStep::getObject(PartObject_t* cpo, Thread_t* thr){
  return getPartObject(cpo, thr)->getObject();
}

ObjectReference_t *UpStep::getObjectReference(PartObject_t* cpo, Thread_t* thr){
  cout << "UpStep::getObjectReference : *_*_*getObjectReference " 
       << "called on wrong runtime step" 
       << "*_*_*";
  exit(1);
  return NULL;  
}

PatternReference_t *UpStep::getPatternReference(PartObject_t* cpo, 
						Thread_t* thr){
  cout << "UpStep::getPatternReference : *_*_*getPatternReference " 
       << "called on wrong runtime step" 
       << "*_*_*";
  exit(1);
  return NULL;  
}

DownStep::DownStep(char** pc){
  DB_INDENT;
  (*pc) = &(*pc)[2]; // v` skippes
  unsigned int comma = 0;
  for( ; (*pc)[comma]!=',' && (*pc)[comma]!='}'  ; comma++);
  char *str = (char*)malloc(comma+1);
  memcpy(str, *pc, comma);
  str[comma] = 0;
  mainPartId = str;
  *pc = *pc + comma;
  DB_OUT(vm_rtp,  "DownStep::DownStep parsed with mainPartId: " 
	 << strdup(mainPartId) 
	 << endl);
  DB_OUTDENT;
}

void DownStep::printOn(ostream &stream){
  stream << "v" << mainPartId;
}

char* DownStep::getMainPartId(){
  return mainPartId;
}

PartObject_t* DownStep::getPartObject(PartObject_t* cpo, Thread_t* thr){
  //go upwards until end mainpartId found or end 
  DB_INDENT;
  while(strcmp(cpo->getMainPartId(),mainPartId)){
    if(cpo->next()!=NULL){
      cpo = cpo->next();
    } else {
      cout << "*_*_*_* DownStep::getPartObject: Did not find " 
	   << " partobejct *_*_*_*\n";
      exit(1);
    }
  }
  DB_OUT(vm_rtp,  "DownStep::getPartObject(): found po\n");
  DB_OUTDENT;
  return cpo;
}

Pattern_t *DownStep::getPattern(PartObject_t* cpo, Thread_t* thr){
  cout << "DownStep::getPattern : *_*_*getPattern called on wrong runtime step" 
       << "*_*_*";
  return NULL;
}

Object_t *DownStep::getObject(PartObject_t* cpo, Thread_t* thr){
  return getPartObject(cpo, thr)->getObject();
}

ObjectReference_t *DownStep::getObjectReference(PartObject_t* cpo, 
						Thread_t* thr){
  cout << "DownStep::getObjectReference : *_*_*getObjectReference called on "
       << "wrong runtime step" 
       << "*_*_*";
  exit(1);
  return NULL;  
}

PatternReference_t *DownStep::getPatternReference(PartObject_t* cpo, 
						  Thread_t* thr){
  cout << "DownStep::getPatternReference : *_*_*getPatternReference called on "
       << "wrong runtime step" 
       << "*_*_*";
  exit(1);
  return NULL;  
}

OutStep::OutStep(char** pc){ 
  DB_INDENT;
  (*pc)=&(*pc)[2]; //skip <-
  //get number of digits
  if(sscanf(*pc,"%i",&steps) != 1) {
    cout <<  "OutStep::OutStep(): parse error - could not parse int\n";
    exit(1);
  }
  //maintain pc
  for( ; **pc!=',' && **pc!='}' ; (*pc)++);
  //no skipping , skipped in while loop (parseRuntimeStep) 
  DB_OUT(vm_rtp,  "OutStep::OutStep parsed with steps: " << steps << endl);
  DB_OUTDENT;
}

unsigned int OutStep::getSteps(){
  return steps;
}

void OutStep::printOn(ostream &stream){
  stream << "<-" << steps;
}

PartObject_t* OutStep::getPartObject(PartObject_t* cpo, Thread_t* thr){
  DB_INDENT;
  for(unsigned int i = 0; i < steps; i++){
    cpo = cpo->getOrigin();
  }
  DB_OUT(vm_rtp,  "OutStep::getPartObject(): found po\n");
  DB_OUTDENT;
  return cpo;
}

Pattern_t *OutStep::getPattern(PartObject_t* cpo, Thread_t* thr){
  cout << "OutStep::getPattern : *_*_*getPattern called on wrong runtime step" 
       << "*_*_*";
  return NULL;
}

Object_t *OutStep::getObject(PartObject_t* cpo, Thread_t* thr){
  return getPartObject(cpo, thr)->getObject();
}

ObjectReference_t *OutStep::getObjectReference(PartObject_t* cpo, Thread_t* thr){
  cout << "OutStep::getObjectReference : *_*_*getObjectReference called on "
       << "wrong runtime step" 
       << "*_*_*";
  exit(1);
  return NULL;  
}

PatternReference_t *OutStep::getPatternReference(PartObject_t* cpo, 
						 Thread_t* thr){
  cout << "OutStep::getPatternReference : *_*_*getPatternReference called on "
       << "wrong runtime step" 
       << "*_*_*";
  exit(1);
  return NULL;  
}

LookupStep::LookupStep(char** pc){
  //parse name
  DB_INDENT;
  (*pc) = &(*pc)[1]; //skip "
  unsigned int slash = 0;
  for( ; (*pc)[slash]!='/' ; slash++);
  char *str = (char*)malloc(slash+1);
  memcpy(str, *pc, slash);
  str[slash] = 0;
  name = str;
  *pc = *pc + slash + 1;
  //parse index
  if(sscanf(*pc,"%i\"",&index) != 1) {
    cout << "LookupStep::LookupStep(): parse error - could not parse int\n";
    exit(1);
  }
  //maintain pc
  for( ; **pc!='"' ; (*pc)++);
  (*pc) = &(*pc)[1]; //skip "
  DB_OUT(vm_rtp, "LookupStep::LookupStep parsed with name: " << strdup(name) 
       << " and index: " << index << endl);
  DB_OUTDENT;
}

void LookupStep::printOn(ostream &stream){
  stream << "\"" << name << "/" << index << "\"";
}

char* LookupStep::getName(){
  return name;
}

unsigned int LookupStep::getIndex(){
  return index;
}

PartObject_t* LookupStep::getPartObject(PartObject_t* cpo, Thread_t* thr){
  DB_INDENT;
  //if the attribute is NULL getAttribute could result in an initialization
  DB_OUT(vm_rtp,  "LookupStep::getPartObject(): \n");
  AttributeInstance_t *object = ((UserPartObject_t*)cpo)->getAttribute(index, 
								       thr);
  DB_OUTDENT;
  if(!object){
    return NULL;
  }
  return ((Object_t*)object)->last();
}

Pattern_t *LookupStep::getPattern(PartObject_t* cpo, Thread_t* thr){
  //if the attribute is NULL getAttribute could result in an initialization
  return ((Pattern_t*) ((UserPartObject_t*)cpo)->getAttribute(index, thr));
}

Object_t *LookupStep::getObject(PartObject_t* cpo, Thread_t* thr){
  //if the attribute is NULL getAttribute could result in an initialization
  return ((Object_t*) ((UserPartObject_t*)cpo)->getAttribute(index, thr));
}

PatternReference_t *LookupStep::getPatternReference(PartObject_t* cpo, 
						    Thread_t* thr){
  //if the attribute is NULL getAttribute could result in an initialization
  return ((PatternReference_t*) ((UserPartObject_t*)cpo)->getAttribute(index, 
								       thr));
}

ObjectReference_t *LookupStep::getObjectReference(PartObject_t* cpo, 
						  Thread_t* thr){
  //if the attribute is NULL getAttribute could result in an initialization
  return ((ObjectReference_t*) ((UserPartObject_t*)cpo)->getAttribute(index, 
								      thr));
}

IndirectLookupStep::IndirectLookupStep(char** pc){
  //parse name
  DB_INDENT;
  (*pc) = &(*pc)[1]; //skip '
  unsigned int slash = 0;
  for( ; (*pc)[slash]!='/' ; slash++);
  char *str = (char*)malloc(slash+1);
  memcpy(str, *pc, slash);
  str[slash] = 0;
  name = str;
  *pc = *pc + slash + 1;
  //parse index
  if(sscanf(*pc,"%i'",&index) != 1) {
    cout << "IndirectLookupStep::IndirectLookupStep(): parse error - " 
	 << "could not parse int\n";
    exit(1);
  }
  //maintain pc
  for( ; **pc!='\'' ; (*pc)++);
  (*pc) = &(*pc)[1]; //skip '
  DB_OUT(vm_rtp,  "IndirectLookupStep::IndirectLookupStep parsed with name: " 
	 << strdup(name) 
	 << " and index: " << index << endl);
  DB_OUTDENT;
}


void IndirectLookupStep::printOn(ostream &stream){
  stream << "'" << name << "/" << index << "'";
}

char* IndirectLookupStep::getName(){
  return name;
}

unsigned int IndirectLookupStep::getIndex(){
  return index;
}

PartObject_t* IndirectLookupStep::getPartObject(PartObject_t* cpo, 
						Thread_t* thr){
  DB_INDENT;
  //if the attribute is NULL getAttribute could result in an initialization
  AttributeInstance_t *objectref = ((UserPartObject_t*)cpo)->getAttribute(index, 
									  thr);
  if(!objectref){
    DB_OUTDENT;
    return NULL;
  }
  DB_OUTDENT;
  return ((ObjectReference_t*)objectref)->getValue()->last();
}

Pattern_t *IndirectLookupStep::getPattern(PartObject_t* cpo, Thread_t* thr){
  DB_INDENT;
  //if the attribute is NULL getAttribute could result in an initialization
  AttributeInstance_t *patternref = ((UserPartObject_t*)cpo)->getAttribute(index,
									   thr);
  DB_OUT(vm_rtp, "ILS::getPattern(): got patternReference: " 
	 << (void*)patternref << endl);
  if(!patternref){
    DB_OUTDENT;
    return NULL;
  }
  DB_OUTDENT;
  return ((PatternReference_t*)patternref)->getValue();
}

Object_t *IndirectLookupStep::getObject(PartObject_t* cpo, Thread_t* thr){
  DB_INDENT;
  //if the attribute is NULL getAttribute could result in an initialization
  AttributeInstance_t *objectref = ((UserPartObject_t*)cpo)->getAttribute(index, 
									  thr);
  if(!objectref){
    DB_OUTDENT;
    return NULL;
  }
  DB_OUT(vm_rtp, "IndirectLookupStep::getObject() found objRef: " << objectref 
	 << " with value: " << ((ObjectReference_t*)objectref)->getValue() 
	 << endl);
  DB_OUTDENT;
  return ((ObjectReference_t*)objectref)->getValue();
}


PatternReference_t *IndirectLookupStep::getPatternReference(PartObject_t* cpo, 
							    Thread_t* thr){
  //if the attribute is NULL getAttribute could result in an initialization
  return ((PatternReference_t*) ((UserPartObject_t*)cpo)->getAttribute(index, 
								       thr));
}

ObjectReference_t *IndirectLookupStep::getObjectReference(PartObject_t* cpo, 
							  Thread_t* thr){
  //if the attribute is NULL getAttribute could result in an initialization
  return ((ObjectReference_t*) ((UserPartObject_t*)cpo)->getAttribute(index, 
								      thr));
}

TmpStep::TmpStep(char** pc){
  DB_INDENT;
  (*pc)=&(*pc)[4]; //skip 'tmp('
  //get number of digits
  if(sscanf(*pc,"%i)",&index) != 1) {
    cout << "TmpStep::TmpStep(): parse error - could not parse int\n";
    exit(1);
  }
  //maintain pc
  for( ; **pc!=')' ; (*pc)++);
  (*pc) = &(*pc)[1]; //skip )
  DB_OUT(vm_rtp,  "TmpStep::TmpStep parsed with index: " << index << endl);
  DB_OUTDENT;
}


void TmpStep::printOn(ostream &stream){
  stream << "tmp(" << index << ")";
}

unsigned int TmpStep::getIndex(){
  return index;
}

PartObject_t* TmpStep::getPartObject(PartObject_t* cpo, Thread_t* thr){
  //Take the most specific 
  DB_INDENT
  DB_OUT(vm_rtp,  "TmpStep::getPartObject(): found po\n");
  DB_OUTDENT;
  return ((Object_t*)(thr->tmpStack->getElementAt(index)))->last();
}

Pattern_t *TmpStep::getPattern(PartObject_t* cpo, Thread_t* thr){
  cout << "TmpStep::getPattern : *_*_*getPattern called on wrong runtime step" 
       << "*_*_*";
  return NULL;
}

Object_t *TmpStep::getObject(PartObject_t* cpo, Thread_t* thr){
  return ((Object_t*)(thr->tmpStack->getElementAt(index)));
}

ObjectReference_t *TmpStep::getObjectReference(PartObject_t* cpo, Thread_t* thr){
  cout << "TmpStep::getObjectReference : *_*_*getObjectReference called " 
       << " on wrong runtime step" 
       << "*_*_*";
  exit(1);
  return NULL;  
}

PatternReference_t *TmpStep::getPatternReference(PartObject_t* cpo, 
						 Thread_t* thr){
  cout << "TmpStep::getPatternReference : *_*_*getPatternReference called " 
       << "on wrong runtime step" 
       << "*_*_*";
  exit(1);
  return NULL;  
}


// ******************** functions ********************

//internal, use same without _steps in its name externally
static PartObject_t* evaluateRuntimePath_PO_steps(SingleInstructionWRTP *inst, 
						  PartObject_t *po, 
						  Thread_t *thr,
						  RuntimeStep **rs){
  DB_INDENT;
  Lst<RuntimeStep> *rtp = inst->getRuntimePath();

  //handle empty lists...
  if(!(rtp->getCount())) {
    *rs = 0;
    DB_OUTDENT;
    return po;
  }  

  DB_OUT(vm_rtp,  "evaluateRuntimePath_PO_steps() got inst's runtimepath count: "
	 << rtp->getCount() << endl);

  unsigned int rtpLength = rtp->getCount();
  *rs = rtp->first();
  
  DB_OUT(vm_rtp,  "evaluateRuntimePath_PO_steps() got first runtime step\n");
  
  PartObject_t *currentPartObject = po;
  
  //follow partobject path: 
  //skal ikke medtage den sidste SKAL KALDE rs->getObjOrPtn()..
  for(unsigned int i = 1;
      i < rtpLength; 
      *rs = (*rs)->next(), i++){
    DB_OUT(vm_rtp,  "evaluateRuntimePath_PO() before getPartObject " 
	   << i << endl);
    currentPartObject = (*rs)->getPartObject(currentPartObject, thr);
    if(!currentPartObject){
      //attribute was not initialized
      DB_OUTDENT;
      return NULL;
    }
    DB_OUT(vm_rtp,  "evaluateRuntimePath_PO() after getPartObject " 
	   << i << endl);
  }
  DB_OUT(vm_rtp,  "evaluateRuntimePath_PO_steps() after for\n");
  DB_OUTDENT;
  return currentPartObject;
}

PartObject_t *evaluateRuntimePath_PO(SingleInstructionWRTP *inst, 
				     PartObject_t *po,
				     Thread_t *thr) {
  DB_INDENT;
  RuntimeStep *rts;

  DB_OUT(vm_rtp, "evaluateRuntimePath_PO(): got po: " << po << endl); 

  /* evaluate the first n-1 steps */
  PartObject_t *currentPartObj = evaluateRuntimePath_PO_steps(inst, 
							      po, 
							      thr, 
							      &rts);

  DB_OUT(vm_rtp,  "evaluateRuntimePath_PO() got currentPartObj: " 
	 << currentPartObj << endl );

  if(!currentPartObj){
    //attribute was not initialized
    DB_OUTDENT;
    return NULL;
  }

  //rtp might have been empty
  if(rts) {
    /* evaluate the final step, n */
    if(rts->next()){
      rts = rts->next();
    }
    currentPartObj = rts->getPartObject(currentPartObj, thr);
  }

  DB_OUT(vm_rtp,  "evaluateRuntimePath_PO() returning po: " 
	 << currentPartObj << endl );

  DB_OUTDENT;
  return currentPartObj;
}


Object_t* evaluateRuntimePath_Obj(SingleInstructionWRTP *inst, 
				  PartObject_t *po, 
				  Thread_t*thr){
  DB_INDENT;
  RuntimeStep *rts;
  PartObject_t *interestingPartObj = evaluateRuntimePath_PO_steps(inst, 
								  po, 
								  thr, 
								  &rts);

  if(!interestingPartObj){
    //attribute was not initialized
    DB_OUTDENT;
    return NULL;
  }

  DB_OUT(vm_rtp,  "evaluateRuntimePath_Obj(): found interesting part object : " 
	 << interestingPartObj << endl);

  //return the object of the last step from interesting partobject
  DB_OUTDENT;
  if(rts){
    return rts->getObject(interestingPartObj, thr);
  } else {
    return interestingPartObj->getObject();
  }
}

Pattern_t* evaluateRuntimePath_Ptn(SingleInstructionWRTP *inst, 
				  PartObject_t *po, 
				  Thread_t*thr){
  RuntimeStep *rts;
  PartObject_t *interestingPartObj = evaluateRuntimePath_PO_steps(inst,
								  po, 
								  thr, 
								  &rts);

  if(!interestingPartObj){
    //attribute was not initialized
    return NULL;
  }

  //return the pattern of the last step from interesting partobject
  return rts->getPattern(interestingPartObj, thr);
}


ObjectReference_t* evaluateRuntimePath_ObjRef(SingleInstructionWRTP *inst, 
				     PartObject_t *po, 
				     Thread_t*thr){
  DB_INDENT;
  RuntimeStep *rts;
  PartObject_t *interestingPartObj = evaluateRuntimePath_PO_steps(inst, 
								  po, 
								  thr, 
								  &rts);

  if(!interestingPartObj){
    DB_OUTDENT;
    //attribute was not initialized
    return NULL;
  }

  DB_OUT(vm_rtp,  "evaluateRuntimePath_Obj(): found interesting part object : " 
	 << interestingPartObj << endl);

  //return the objectreference of the last step from interesting partobject
  DB_OUTDENT;
  return rts->getObjectReference(interestingPartObj, thr);
}

PatternReference_t* evaluateRuntimePath_PtnRef(SingleInstructionWRTP *inst, 
				      PartObject_t *po, 
				      Thread_t*thr){
  RuntimeStep *rts;
  PartObject_t *interestingPartObj = evaluateRuntimePath_PO_steps(inst, 
								  po, 
								  thr, 
								  &rts);

  if(!interestingPartObj){
    //attribute was not initialized
    return NULL;
  }
  
  //return the patternreference of the last step from interesting partobject
  return rts->getPatternReference(interestingPartObj, thr);
}
