//$Id: Instr_Misc.hpp,v 1.38 2001/06/05 10:04:47 hempe Exp $

#ifndef INSTR_MISC_HPP
#define INSTR_MISC_HPP

class Instr_Label_t;
class Instr_AddMP_t;
class Instr_Call_t;
class Instr_ChkNone_t;
class Instr_ChkPtnNone_t;
class Instr_ChkQualObj_t;
class Instr_Discard_t;
class Instr_GatherVirt_t;
class Instr_Inner_t;
class Instr_Merge_t;
class Instr_NegInt_t;
class Instr_NegReal_t;
class Instr_NewPtnObj_t;
class Instr_NewPtnTmp_t;
class Instr_Not_t;
class Instr_Resetframe_t;
class Instr_StdioIn_t;
class Instr_StdioOut_t;


#include "Instructions.hpp"
#include "RunTimeSteps.hpp"


class Instr_Label_t: public Instruction {
public:
  virtual void printOn(ostream& dest);
  void addInstr(Instruction* instr) { instrs.add(instr); }
private:
  Lst<Instruction> instrs;
};

// ---- Add main part ----

class Instr_AddMP_t: public SingleInstructionWRTP {
public:
  Instr_AddMP_t(char* pc);
  virtual void printOn(ostream &stream);
  char* mainPartId;
};

#define ADDMP_MAC { DB_INDENT;                                            \
  PartObject_t* po = evaluateRuntimePath_PO(((SingleInstructionWRTP*)pc), \
                                            currentPOStack->peek(),       \
                                            this);                        \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  Pattern_t* p1 = patternStack->pop();                                    \
  unsigned int p1Length = p1->getLength();                                \
  Mixin_t** p1Mixins = p1->getMixins();                                   \
  MainPart* mp;                                                           \
  if(!(mp = myScheduler->getMainPart(((Instr_AddMP_t*)pc)->mainPartId))){ \
    cerr << "ADDMP_MAC: Runtime Error could find mainpart with id: "      \
         << ((Instr_AddMP_t*)pc)->mainPartId << endl;                     \
    exit(1);                                                              \
  }                                                                       \
  Mixin_t* newMixin = new(allocateVM(mixinDesc)) Mixin_t(mp, po);         \
  ObjectDescriptor_t *objDesc = getPtnObjectDescriptor(p1Length + 1);     \
                                            /* +1: space for new mixin */ \
  DB_OUT(vm_thread, "ADDMP_MAC: got objectdescriptor for pattern with "   \
         << p1Length + 1 << " mixins, objectdescriptor:");                \
  DB_OUT(vm_thread, objDesc);                                             \
  Pattern_t *resPtn = new(allocateVM(objDesc)) Pattern_t(objDesc);        \
  Mixin_t** resPtnMixins = resPtn->getMixins();                           \
  unsigned int i = 0;                                                     \
  for(; i < p1Length ; i++){                                              \
    setVMReference(resPtn, (void**)&resPtnMixins[i], p1Mixins[i]);        \
  }                                                                       \
  setVMReference(resPtn, (void**)&resPtnMixins[i], newMixin);             \
  patternStack->push(resPtn);                                             \
  pc = pc->next();                                                        \
  DB_OUTDENT;                                                             \
};

// ---- Call ---------

class Instr_Call_t: public SingleInstructionWRTP {
public:
  Instr_Call_t(char *pc); 
  virtual void printOn(ostream &stream);
};


#define CALL_MAC {                                                        \
  DB_INDENT;                                                              \
  Object_t *obj = evaluateRuntimePath_Obj((SingleInstructionWRTP*)pc,     \
					currentPOStack->peek(),           \
					this);                            \
  DB_OUT(vm_thread, "CALL_MAC: found obj " << (void*)obj << endl);        \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  Instruction* inst = pc;                                                 \
  if(!(pc = obj->getFirstInstructionPushPOANF(this))){                    \
    DB_OUT(vm_thread, "CALL_MAC: no instruction to execute: " <<          \
	   pc << endl);                                                   \
    /*restore CPO unneeded since nothing is pushed when null is returned*/\
    pc = inst->next();                                                    \
  } else {                                                                \
    DB_OUT(vm_thread, "CALL_MAC: instruction to execute " << pc << endl); \
    executionStack->push(inst->next());                                   \
  }                                                                       \
  DB_OUTDENT;                                                             \
};

// ------ chk none --------------

class Instr_ChkNone_t: public SingleInstruction {
public:
  Instr_ChkNone_t();
  virtual void printOn(ostream &stream);
};

#define CHKNONE_MAC                                                       \
  if(!objectStack->peek()){                                               \
    cerr << "CHKNONE_MAC: Runtime Error: object reference on objectStack" \
         << " is NONE\n";                                                 \
    exit(1);                                                              \
  }                                                                       \
  pc = pc->next();                                                             

// ------ chk ptn none ----------

class Instr_ChkPtnNone_t: public SingleInstruction {
public:
  Instr_ChkPtnNone_t();
  virtual void printOn(ostream &stream);
};

#define CHKPTNNONE_MAC                                                    \
  if(!patternStack->peek()){                                              \
    cerr << "CHKPTNNONE_MAC: Runtime Error: pattern reference on "        \
         << "patternStack is NONE\n";                                     \
    exit(1);                                                              \
  }                                                                       \
  pc = pc->next();                                                             


// ------ chk ptn none ----------

class Instr_ChkQualObj_t: public SingleInstructionWRTP {
public:
  Instr_ChkQualObj_t(char* pc);
  virtual void printOn(ostream &stream);
};

#define CHKQUALOBJ_MAC {                                                  \
  ObjectReference_t* objRef = evaluateRuntimePath_ObjRef(                 \
                               ((SingleInstructionWRTP*)pc),              \
                               currentPOStack->peek(),                    \
                               this);                                     \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  Pattern_t* qual2 = objRef->getQualification();                          \
  Pattern_t* qual1 = objectStack->peek()->getPattern();                   \
  PTNLTEQ(qual1, qual2);                                                  \
  if(!result){                                                            \
    cerr << "CHKQUALOBJ_MAC: Runtime Error: object reference on "         \
         << "objectStack is not qualified\n";                             \
    exit(1);                                                              \
  }                                                                       \
  pc = pc->next();                                                        \
};
									  

// ------ Discard ----------

class Instr_Discard_t: public SingleInstructionWithType {
public:
  Instr_Discard_t(char* pc);
  Instr_Discard_t(unsigned int _type) {type = _type; id=DISCARD_ID;}
  virtual void printOn(ostream &stream);
};

#define DISCARD_MAC {                                                     \
  switch(((Instr_AddOpPlus_t*)pc)->type){                                 \
   case CHAR_ID:                                                          \
    charStack->discard();                                                 \
    break;                                                                \
   case BOOLEAN_ID:                                                       \
    boolStack->discard();                                                 \
    break;                                                                \
   case INTEGER_ID:                                                       \
    intStack->discard();                                                  \
    break;                                                                \
   case REAL_ID:                                                          \
    realStack->discard();                                                 \
    break;                                                                \
   case STRING_ID:                                                        \
    stringStack->discard();                                               \
    break;                                                                \
   case PATTERN_ID:                                                       \
    patternStack->discard();                                              \
    break;                                                                \
   case OBJECT_ID:                                                        \
    objectStack->discard();                                               \
    break;                                                                \
   default:                                                               \
    cerr << "DISCARD: Unknown type for instruction!\n";                   \
    exit(1);                                                              \
    break;                                                                \
  }                                                                       \
  pc = pc->next();                                                        \
};


// ----- gather virtual --------

class Instr_GatherVirt_t: public SingleInstruction {
public:
                              Instr_GatherVirt_t(char* pc);
  virtual void                printOn(ostream &stream);

  //attributes
  char                       *parentAttrName;
                                //introducing binding
  int                         parentAttrIndex;
  char                       *parentMainPartID;
};

#define GATHERVIRT_MAC {                                                  \
  AttributeVirtual *attrv = currentPOStack->peek()->                      \
    getVirtAttr(((Instr_GatherVirt_t*)pc)->parentAttrName,                \
		((Instr_GatherVirt_t*)pc)->parentAttrIndex,               \
		((Instr_GatherVirt_t*)pc)->parentMainPartID);             \
                                                                          \
  if(!attrv) {                                                            \
    cerr << "GATHERVIRT_MAC: error finding virtual attribute in cpo\n";   \
    exit(1);                                                              \
  }                                                                       \
  currentPOStack->push(currentPOStack->peek());                           \
  tmpStack->addNewFrame();                                                \
  executionStack->push(pc->next());                                       \
  pc = attrv->getGVInstructionListPtr()->first();                         \
};

// ------ inner ----------------

class Instr_Inner_t: public SingleInstruction {
public:
  Instr_Inner_t(char* pc);
  virtual void printOn(ostream &stream);
  unsigned int index;
};

#define INNER_MAC {                                                       \
  DB_INDENT;                                                              \
  PartObject_t* po = currentPOStack->peek();                              \
  for(unsigned int i = 0; i < ((Instr_Inner_t*)pc)->index; i++){          \
    po = po->getOrigin();                                                 \
  }                                                                       \
  if(po->next()){                                                         \
    Instruction* inst = pc;                                               \
    if(!(pc = po->next()->getFirstInstructionPushPOANF(this))){           \
      /*restore CPO unneeded since nothing is pushed if null is returned*/\
      pc = inst->next();                                                  \
    } else {                                                              \
      executionStack->push(inst->next());                                 \
    }                                                                     \
  } else {                                                                \
    pc = pc->next();                                                      \
  }                                                                       \
  DB_OUTDENT;                                                             \
};

// ------ merge ----------------

class Instr_Merge_t: public SingleInstruction {
public:
  Instr_Merge_t();
  virtual void printOn(ostream &stream);
};

#define MERGE_MAC {                                                       \
  Pattern_t *p1 = patternStack->pop();                                    \
  Pattern_t *p2 = patternStack->pop();                                    \
  Pattern_t *p2mergep1 = merge(p2, p1);                                   \
  if(!p2mergep1){                                                         \
    cerr << "Runtime Error: could not merge patterns\n";                  \
    exit(1);                                                              \
  }                                                                       \
  patternStack->push(p2mergep1);                                          \
  pc = pc->next();                                                        \
};

// ------ neg int --------------

class Instr_NegInt_t: public SingleInstruction {
public:
  Instr_NegInt_t();
  virtual void printOn(ostream &stream);
};

#define NEGINT_MAC                                                        \
  intStack->push(-intStack->pop());                                       \
  pc = pc->next();                                                             

// ------ neg real --------------

class Instr_NegReal_t: public SingleInstruction {
public:
  Instr_NegReal_t();
  virtual void printOn(ostream &stream);
};

#define NEGREAL_MAC                                                       \
  realStack->push(-realStack->pop());                                     \
  pc = pc->next();                                                             

// ------ new ptn obj -----------

class Instr_NewPtnObj_t: public SingleInstruction {
public:
  Instr_NewPtnObj_t();
  virtual void printOn(ostream &stream);
};

#define NEWPTNOBJ_MAC {                                                   \
  DB_INDENT;                                                              \
  Pattern_t* poppedPattern = patternStack->pop();                         \
  Object_t* patternInstance = poppedPattern->instantiate(this);           \
  objectStack->push(patternInstance);                                     \
  if(patternInstance->first()){                                           \
    executionStack->push(pc->next());                                     \
    currentPOStack->push(patternInstance->first());                       \
    DB_OUT(vm_thread, "NEWPTNOBJ_MAC: pushed cpo\n");                     \
    tmpStack->addNewFrame();                                              \
    DB_OUT(vm_thread, "NEWPTNOBJ_MAC: added frame null?: "                \
           << patternInstance->first()->mixin->getMPAttrsInstrPtr()       \
	   << endl);                                                      \
    pc = patternInstance->first()->getMPAttrsInstrPtr()->first();         \
    DB_OUT(vm_thread, "NEWPTNOBJ_MAC: set pc to first init inst\n");      \
  } else {                                                                \
    pc = pc->next();                                                      \
  }                                                                       \
  DB_OUTDENT;                                                             \
};

// ------ new ptn tmp -----------

class Instr_NewPtnTmp_t: public SingleInstruction {
public:
  Instr_NewPtnTmp_t(char* pc);
  virtual void printOn(ostream &stream);
  unsigned int index;
};

#define NEWPTNTMP_MAC {                                                   \
  Pattern_t* poppedPattern = patternStack->pop();                         \
  Object_t* patternInstance = poppedPattern->instantiate(this);           \
  tmpStack->push(patternInstance);                                        \
  if(patternInstance->first()){                                           \
    executionStack->push(pc->next());                                     \
    currentPOStack->push(patternInstance->first());                       \
    tmpStack->addNewFrame();                                              \
    pc=patternInstance->first()->getMPAttrsInstrPtr()->first();           \
  } else {                                                                \
    pc = pc->next();                                                      \
  }                                                                       \
};

// ------ not --------------

class Instr_Not_t: public SingleInstruction {
public:
  Instr_Not_t();
  virtual void printOn(ostream &stream);
};
 
#define NOT_MAC                                                           \
  boolStack->push(!boolStack->pop());                                     \
  pc = pc->next();                                                             

// ------ resetframe --------------

class Instr_Resetframe_t: public SingleInstruction {
public:
  Instr_Resetframe_t();
  virtual void printOn(ostream &stream);
};
 
#define RESETFRAME_MAC                                                    \
  DB_INDENT;                                                              \
  DB_OUT(vm_thread,"RESETFRAME_MAC: before tmpStack->resetFrame()\n");    \
  tmpStack->resetFrame();                                                 \
  DB_OUT(vm_thread,"RESETFRAME_MAC: after tmpStack->resetFrame()\n");     \
  DB_OUTDENT;\
  pc = pc->next();                                                             


// ----- stdio input -----

class Instr_StdioIn_t: public SingleInstruction {
public:
                              Instr_StdioIn_t();
  virtual void                printOn(ostream &stream);
};

#define STDIOIN_MAC {                                                     \
  DB_INDENT;                                                              \
  char inStr[MAXSTRLENGTH];                                               \
  cin.getline(inStr, MAXSTRLENGTH);                                       \
  unsigned int end = 0;                                                   \
  for( ; inStr[end] != 0 ; end++);                                        \
  ObjectDescriptor_t *strObjDesc = getStrObjectDescriptor(end);           \
  /* TODO make dynamic strings - i.e. don't allocate in rootNT-space */   \
  char* strippedStr = (char*)allocateRootVM_NT(strObjDesc);               \
  memcpy(strippedStr, inStr, end);                                        \
  stringStack->push(strippedStr);                                         \
  DB_OUTDENT;                                                             \
  pc = pc->next();                                                        \
};

// ----- stdio output -----

class Instr_StdioOut_t: public SingleInstruction {
public:
                              Instr_StdioOut_t();
  virtual void                printOn(ostream &stream);
};

#define STDIOOUT_MAC {                                                    \
  cout << stringStack->pop() << flush;                                    \
  pc = pc->next();                                                        \
};

#endif
