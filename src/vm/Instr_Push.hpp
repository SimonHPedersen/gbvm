//$Id: Instr_Push.hpp,v 1.22 2001/05/21 11:42:50 hempe Exp $

#ifndef INSTR_PUSH_HPP
#define INSTR_PUSH_HPP

// ******************** INCOMPLETE ********************

// --- PUSH-ptn_"object"

class Instr_PushBool_t;
class Instr_PushChar_t;
class Instr_PushInt_t;
class Instr_PushObj_t;
class Instr_PushPtn_t;
class Instr_PushObjRef_t;
class Instr_PushPtnObj_t;
class Instr_PushQual_t;
class Instr_PushReal_t;
class Instr_PushStrLen_t;
class Instr_PushStr_t;
class Instr_PushTmpObj_t;
class Instr_PushTmpObjRef_t;
class Instr_PushiBoolLit_t;
class Instr_PushiCharLit_t;
class Instr_PushiIntLit_t;
class Instr_PushiRealLit_t;
class Instr_PushiStrLit_t;

#include "Instructions.hpp"

// ----- PUSH boolean where --------

class Instr_PushBool_t: public SingleInstructionWRTP{
public:
                              Instr_PushBool_t(char* pc);
  virtual void                printOn(ostream &stream);
};

#define PUSHBOOL_MAC {                                                    \
  PartObject_t *po = evaluateRuntimePath_PO(                              \
                           dynamic_cast<SingleInstructionWRTP*>(pc),      \
			   currentPOStack->peek(), this);                 \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  PredefinedPartObject_t<bool> *myBool =                                  \
                         dynamic_cast<PredefinedPartObject_t<bool>*>(po); \
  boolStack->push(myBool->value);                                         \
  pc=pc->next();                                                          \
  };


// ----- PUSH char where --------

class Instr_PushChar_t: public SingleInstructionWRTP{
public:
                              Instr_PushChar_t(char* pc);
  virtual void                printOn(ostream &stream);
};

#define PUSHCHAR_MAC {                                                    \
  PartObject_t *po = evaluateRuntimePath_PO(                              \
                            dynamic_cast<Instr_PushChar_t*>(pc),          \
			    currentPOStack->peek(), this);                \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  PredefinedPartObject_t<char> *myChar =                                  \
                         dynamic_cast<PredefinedPartObject_t<char>*>(po); \
  charStack->push(myChar->value);                                         \
  pc=pc->next();                                                          \
};

// ----- PUSH index where -----

class Instr_PushIndex_t: public SingleInstructionWRTP {
public:
                              Instr_PushIndex_t(char *pc);
  virtual void                printOn(ostream &stream);
};

//similar to push integer where
#define PUSHINDEX_MAC {                                                   \
  PartObject_t *po = evaluateRuntimePath_PO(                              \
                             dynamic_cast<Instr_PushIndex_t*>(pc),        \
			     currentPOStack->peek(), this);               \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  intStack->push(dynamic_cast<PredefinedPartObject_t<int>*>(po)->value);  \
  pc = pc->next();                                                        \
};


// ----- PUSH integer where --------

class Instr_PushInt_t: public SingleInstructionWRTP{
public:
                              Instr_PushInt_t(char* pc);
  virtual void                printOn(ostream &stream);
};

//similar to push index where
#define PUSHINT_MAC {                                                     \
  PartObject_t *po = evaluateRuntimePath_PO(                              \
                            dynamic_cast<SingleInstructionWRTP*>(pc),     \
			    currentPOStack->peek(), this);                \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  intStack->push(dynamic_cast<PredefinedPartObject_t<int>*>(po)->value);  \
  pc = pc->next();                                                        \
};


// ----- PUSH obj where / PUSH objref where --------

class Instr_PushObj_t: public SingleInstructionWRTP{
public:
                              Instr_PushObj_t(char* pc);
  virtual void                printOn(ostream &stream);
};

#define PUSHOBJ_MAC {                                                     \
  DB_INDENT;                                                              \
  Object_t *obj = evaluateRuntimePath_Obj(                                \
                          dynamic_cast<SingleInstructionWRTP*>(pc),       \
	                  currentPOStack->peek(), this);                  \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  DB_OUT(vm_thread,"PUSHOBJ_MAC: got object: " << (void*)obj << endl);    \
  objectStack->push(obj);                                                 \
  pc=pc->next();                                                          \
  DB_OUTDENT;                                                             \
};

// ----- PUSH ptn where / PUSH ptnref where --------

class Instr_PushPtn_t: public SingleInstructionWRTP{
public:
                              Instr_PushPtn_t(char* pc);
  virtual void                printOn(ostream &stream);
};

#define PUSHPTN_MAC {                                                     \
  Pattern_t *ptn = evaluateRuntimePath_Ptn(                               \
                           dynamic_cast<SingleInstructionWRTP*>(pc),      \
			   currentPOStack->peek(), this);                 \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  patternStack->push(ptn);                                                \
  pc=pc->next();                                                          \
};


// ----- PUSH PATTERN "OBJECT" -----
 
class Instr_PushPtnObj_t: public SingleInstruction{
public:
                              Instr_PushPtnObj_t() { id = PUSHPTNOBJ_ID; }
  virtual void                printOn(ostream &stream);
};

#define PUSHPTNOBJ_MAC                                                    \
  patternStack->push(myScheduler->getEmptyObjectPattern());               \
  pc=pc->next();

// ----- PUSH qual where ----------

class Instr_PushQual_t: public SingleInstructionWRTP{
public:
                              Instr_PushQual_t(char* pc);
  virtual void                printOn(ostream &stream);
};

#define PUSHQUAL_MAC {                                                    \
  ObjectReference_t *objRef = evaluateRuntimePath_ObjRef                  \
    (dynamic_cast<SingleInstructionWRTP*>(pc),                            \
     currentPOStack->peek(), this);                                       \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  patternStack->push(objRef->getQualification());                         \
  pc=pc->next();                                                          \
};


// ----- PUSH real where --------

class Instr_PushReal_t: public SingleInstructionWRTP{
public:
                              Instr_PushReal_t(char* pc);
  virtual void                printOn(ostream &stream);
};

#define PUSHREAL_MAC {                                                    \
  PartObject_t *po = evaluateRuntimePath_PO(                              \
                            dynamic_cast<SingleInstructionWRTP*>(pc),     \
			    currentPOStack->peek(), this);                \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  PredefinedPartObject_t<real> *myReal =                                  \
                         dynamic_cast<PredefinedPartObject_t<real>*>(po); \
  realStack->push(myReal->value);                                         \
  pc=pc->next();                                                          \
};


// ----- PUSH str-len where --------

class Instr_PushStrLen_t: public SingleInstructionWRTP{
public:
                              Instr_PushStrLen_t(char* pc);
  virtual void                printOn(ostream &stream);
};

#define PUSHSTRLEN_MAC {                                                  \
  PartObject_t *po = evaluateRuntimePath_PO(                              \
                          dynamic_cast<SingleInstructionWRTP*>(pc),       \
			  currentPOStack->peek(), this);                  \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  PredefinedStringPartObject_t *myString =                                \
      dynamic_cast<PredefinedStringPartObject_t*>(po);                    \
  intStack->push(strlen(myString->getStr()));                             \
  pc=pc->next();                                                          \
};


// ----- PUSH string where --------

class Instr_PushStr_t: public SingleInstructionWRTP{
public:
                              Instr_PushStr_t(char* pc);
  virtual void                printOn(ostream &stream);
};

#define PUSHSTR_MAC {                                                     \
  PartObject_t *po = evaluateRuntimePath_PO(                              \
                             dynamic_cast<SingleInstructionWRTP*>(pc),    \
                             currentPOStack->peek(), this);               \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  PredefinedStringPartObject_t *myStr =                                   \
                         dynamic_cast<PredefinedStringPartObject_t*>(po); \
  stringStack->push(myStr->getStr());                                     \
  pc=pc->next();                                                          \
};

// ----- PUSH tmpobj where --------

class Instr_PushTmpObj_t: public SingleInstructionWRTP{
public:
                              Instr_PushTmpObj_t(char* pc);
  virtual void                printOn(ostream &stream);
};

#define PUSHTMPOBJ_MAC {                                                  \
  Object_t *obj = evaluateRuntimePath_Obj(                                \
                           dynamic_cast<SingleInstructionWRTP*>(pc),      \
					  currentPOStack->peek(), this);  \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  tmpStack->push(obj);                                                    \
  pc=pc->next();                                                          \
};


// ----- PUSH tmpobjref where --------

class Instr_PushTmpObjRef_t: public SingleInstructionWRTP{
public:
                              Instr_PushTmpObjRef_t(char* pc);
  virtual void                printOn(ostream &stream);
};

#define PUSHTMPOBJREF_MAC {                                               \
  ObjectReference_t *objRef = evaluateRuntimePath_ObjRef(                 \
                               dynamic_cast<SingleInstructionWRTP*>(pc),  \
		 	         	  currentPOStack->peek(), this);  \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  Object_t* obj = objRef->getValue();                                     \
  if(obj){                                                                \
    tmpStack->push(obj);                                                  \
    pc=pc->next();                                                        \
  } else {                                                                \
    cerr << "PUSHTMPOBJREF_MAC: Runtime Error: Object reference none \n"; \
    exit(1);                                                              \
  }                                                                       \
};


// --- PUSHI NONE(obj) ---

class Instr_PushiNoneObj_t: public SingleInstruction{
public:
                              Instr_PushiNoneObj_t(); 
  virtual void                printOn(ostream &stream);
};

#define PUSHINONEOBJ_MAC                                                  \
  /*push the boolean literal/value to bool stack */                       \
  objectStack->push(0);                                                   \
  pc=pc->next();

// --- PUSHI NONE(ptn) ---

class Instr_PushiNonePtn_t: public SingleInstruction{
public:
                              Instr_PushiNonePtn_t(); 
  virtual void                printOn(ostream &stream);
};

#define PUSHINONEPTN_MAC                                                  \
  /*push the boolean literal/value to bool stack */                       \
  patternStack->push(0);                                                  \
  pc=pc->next();


// --- PUSHI-boolean ---

class Instr_PushiBoolLit_t: public SingleInstruction{
public:
                              Instr_PushiBoolLit_t(char* pc); 
  virtual void                printOn(ostream &stream);
  char                        literal; //value
};

#define PUSHIBOOLLIT_MAC                                                  \
  /*push the boolean literal/value to bool stack */                       \
  boolStack->push(((Instr_PushiBoolLit_t*)pc)->literal);                  \
  pc=pc->next();


// --- PUSHI-char ---

class Instr_PushiCharLit_t: public SingleInstruction{
public:
                              Instr_PushiCharLit_t(char* pc); 
  virtual void                printOn(ostream &stream);
  char                        literal; //value
};

#define PUSHICHARLIT_MAC                                                  \
  /*push the char literal/value to char stack */                          \
  charStack->push(((Instr_PushiCharLit_t*)pc)->literal);                  \
  pc=pc->next();


// --- PUSHI-integer ---

class Instr_PushiIntLit_t: public SingleInstruction{
public:
                              Instr_PushiIntLit_t(char* pc); 
                              Instr_PushiIntLit_t(int lit); 
  virtual void                printOn(ostream &stream);
  int                         literal; //value
};

#define PUSHIINTLIT_MAC                                                   \
  /*push the integer literal/value to int stack */                        \
  intStack->push(((Instr_PushiIntLit_t*)pc)->literal);                    \
  pc=pc->next();


// --- PUSHI-real ---

class Instr_PushiRealLit_t: public SingleInstruction{
public:
                              Instr_PushiRealLit_t(char* pc); 
  virtual void                printOn(ostream &stream);
  real                        literal; //value
};

#define PUSHIREALLIT_MAC                                                  \
  /*push the real literal/value to real stack */                          \
  realStack->push(((Instr_PushiRealLit_t*)pc)->literal);                  \
  pc=pc->next();


// --- PUSHI-string ---

class Instr_PushiStrLit_t: public SingleInstruction{
public:
                              Instr_PushiStrLit_t(char* pc); 
  virtual void                printOn(ostream &stream);
  char*                       literal; //value
};

#define PUSHISTRLIT_MAC                                                   \
  /*push the str literal/value to str stack */                            \
  stringStack->push(((Instr_PushiStrLit_t*)pc)->literal);                 \
  pc=pc->next();

#endif
