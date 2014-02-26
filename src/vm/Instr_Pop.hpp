//$Id: Instr_Pop.hpp,v 1.12 2001/05/21 11:42:49 hempe Exp $

#ifndef INSTR_POP_HPP
#define INSTR_POP_HPP

#include <iostream.h>
#include "Instructions.hpp"
#include "ByteCodeLoader.hpp"

class Instr_PopBool_t;
//class Instr_PopBoolVal_t; DEPRECATED
class Instr_PopChar_t;
class Instr_PopCharPushInt_t;
class Instr_PopCharPushReal_t;
class Instr_PopCharPushStr_t;
//class Instr_PopCharVal_t; DEPRECATED
class Instr_PopIntPushCharAt_t;
class Instr_PopInt_t;
class Instr_PopIntPushChar_t;
class Instr_PopIntPushReal_t;
//class Instr_PopIntVal_t; DEPRECATED
//class PopInxObjRef_t; REPETITION
//class PopInxPtnRef_t; REPETITION
class Instr_PopObjPushPtn_t;
class Instr_PopObjPushTmp_t;
class Instr_PopObjRef_t;
class Instr_PopPtnRef_t;
class Instr_PopRealPushInt_t;

// ----- pop bool -----

class Instr_PopBool_t: public SingleInstructionWRTP {
public:
  Instr_PopBool_t(char *pc);
  void printOn(ostream &stream);
};

#define POPBOOL_MAC {                                                     \
  PartObject_t *po = evaluateRuntimePath_PO                               \
    (dynamic_cast<SingleInstructionWRTP*>(pc),                            \
     currentPOStack->peek(), this);                                       \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  PredefinedPartObject_t<bool> *myBool                                    \
     = dynamic_cast<PredefinedPartObject_t<bool>*>(po);                   \
  myBool->value = boolStack->pop();                                       \
  pc = pc->next();                                                        \
};

// ----- pop char -----

class Instr_PopChar_t: public SingleInstructionWRTP {
public:
  Instr_PopChar_t(char *pc);
  void printOn(ostream &stream);
};

#define POPCHAR_MAC {                                                     \
  PartObject_t *po = evaluateRuntimePath_PO                               \
    (dynamic_cast<SingleInstructionWRTP*>(pc),                            \
     currentPOStack->peek(), this);                                       \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  PredefinedPartObject_t<char> *myChar =                                  \
     dynamic_cast<PredefinedPartObject_t<char>*>(po);                     \
  myChar->value = charStack->pop();                                       \
  pc = pc->next();                                                        \
};

// ----- pop char push int -----

class Instr_PopCharPushInt_t: public SingleInstruction {
public:
  Instr_PopCharPushInt_t();
  void printOn(ostream &stream);
};

#define POPCHARPUSHINT_MAC {                                              \
  intStack->push((int)charStack->pop());                                  \
  pc = pc->next();                                                        \
};

// ----- pop char push real -----

class Instr_PopCharPushReal_t: public SingleInstruction {
public:
  Instr_PopCharPushReal_t();
  void printOn(ostream &stream);
};

#define POPCHARPUSHREAL_MAC {                                             \
  realStack->push((real)charStack->pop());                                \
  pc = pc->next();                                                        \
};

// ----- pop char push string -----

class Instr_PopCharPushStr_t: public SingleInstruction {
public:
  Instr_PopCharPushStr_t();
  void printOn(ostream &stream);
};

#define POPCHARPUSHSTR_MAC {                                              \
  char* newStr = (char*)allocateVM(oneCharStringDesc);                    \
  newStr[0] = charStack->pop();                                           \
  newStr[1] = 0;                                                          \
  stringStack->push(newStr);                                              \
  pc = pc->next();                                                        \
};

// ----- pop int -----

class Instr_PopInt_t: public SingleInstructionWRTP {
public:
  Instr_PopInt_t(char *pc);
  void printOn(ostream &stream);
};

#define POPINT_MAC {                                                      \
  DB_INDENT;                                                              \
  DB_OUT(vm_thread, "POPINT_MAC: 1 \n");                                  \
  PartObject_t *po = evaluateRuntimePath_PO                               \
    (dynamic_cast<SingleInstructionWRTP*>(pc),                            \
     currentPOStack->peek(), this);                                       \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  DB_OUT(vm_thread, "POPINT_MAC: evaluated rtp  - po: " << po             \
         << " type.po: " << typeid(po).name() << " type<int>: "           \
	 << typeid(PredefinedPartObject_t<int>*).name() << endl);         \
  PredefinedPartObject_t<int> *myInt =                                    \
    dynamic_cast<PredefinedPartObject_t<int>*>(po);                       \
  DB_OUT(vm_thread, "POPINT_MAC: casted po to <int> myInt: " <<           \
	 myInt << endl);                                                  \
  myInt->value = intStack->pop();                                         \
  pc = pc->next();                                                        \
  DB_OUTDENT;                                                             \
};

// ----- pop int push char -----

class Instr_PopIntPushChar_t: public SingleInstruction {
public:
  Instr_PopIntPushChar_t();
  void printOn(ostream &stream);
};

#define POPINTPUSHCHAR_MAC {                                              \
  charStack->push((char)intStack->pop());                                 \
  pc = pc->next();                                                        \
};

// ----- pop int push char at -----

class Instr_PopIntPushCharAt_t: public SingleInstructionWRTP {
public:
  Instr_PopIntPushCharAt_t(char* pc);
  void printOn(ostream &stream);
};

#define POPINTPUSHCHARAT_MAC {                                            \
  PartObject_t *po = evaluateRuntimePath_PO                               \
    (dynamic_cast<SingleInstructionWRTP*>(pc),                            \
     currentPOStack->peek(), this);                                       \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  PredefinedStringPartObject_t *myStr =                                   \
    dynamic_cast<PredefinedStringPartObject_t*>(po);                      \
  char * str = myStr->getStr();                                           \
  unsigned int index = intStack->pop();                                   \
  charStack->push(str[index-1]);                                          \
  pc = pc->next();                                                        \
};

// ----- pop int push real -----

class Instr_PopIntPushReal_t: public SingleInstruction {
public:
  Instr_PopIntPushReal_t();
  void printOn(ostream &stream);
};

#define POPINTPUSHREAL_MAC {                                              \
  realStack->push((real)intStack->pop());                                 \
  pc = pc->next();                                                        \
};

// ----- pop obj push ptn -----

class Instr_PopObjPushPtn_t: public SingleInstruction {
public:
  Instr_PopObjPushPtn_t();
  void printOn(ostream &stream);
};

#define POPOBJPUSHPTN_MAC {                                               \
  patternStack->push((objectStack->pop())->getPattern());                 \
  pc = pc->next();                                                        \
};

// ----- pop obj push tmp -----

class Instr_PopObjPushTmp_t: public SingleInstruction {
public:
  Instr_PopObjPushTmp_t();
  void printOn(ostream &stream);
};

#define POPOBJPUSHTMP_MAC {                                               \
  tmpStack->push(objectStack->pop());                                     \
  pc = pc->next();                                                        \
};

// ----- pop objref -----

class Instr_PopObjRef_t: public SingleInstructionWRTP {
public:
  Instr_PopObjRef_t(char *pc);
  void printOn(ostream &stream);
};

#define POPOBJREF_MAC {                                                   \
  ObjectReference_t *objRef = evaluateRuntimePath_ObjRef                  \
    (dynamic_cast<SingleInstructionWRTP*>(pc),                            \
     currentPOStack->peek(), this);                                       \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  Object_t* obj = objectStack->pop();                                     \
  if(obj){                                                                \
    Pattern_t* p1 = obj->getPattern();                                    \
    Pattern_t* p2 = objRef->getQualification();                           \
    PTNLTEQ(p1,p2);                                                       \
    if(result){                                                           \
      objRef->setValue(obj);                                              \
      pc=pc->next();                                                      \
    } else {                                                              \
      cerr << "POPOBJREF_MAC: Runtime Error: Pattern of popped object is" \
           << " not less than qualfication of object reference" << endl;  \
      exit(1);                                                            \
    }                                                                     \
  } else {                                                                \
    objRef->setValue(obj);                                                \
    pc=pc->next();                                                        \
  }                                                                       \
};

// ----- pop ptnref -----

class Instr_PopPtnRef_t: public SingleInstructionWRTP {
public:
  Instr_PopPtnRef_t(char *pc);
  void printOn(ostream &stream);
};

#define POPPTNREF_MAC {                                                   \
  PatternReference_t *ptnRef = evaluateRuntimePath_PtnRef                 \
    (dynamic_cast<SingleInstructionWRTP*>(pc),                            \
     currentPOStack->peek(), this);                                       \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  Pattern_t* ptn = patternStack->pop();                                   \
  if(ptn){                                                                \
    Pattern_t* p2 = ptnRef->getQualification();                           \
    PTNLTEQ(ptn,p2);                                                      \
    if(result){                                                           \
      ptnRef->setValue(ptn);                                              \
      pc=pc->next();                                                      \
    } else {                                                              \
      cerr << "POPPTNREF_MAC: Runtime Error: Popped pattern is not less"  \
           << "than qualfication of pattern reference" << endl;           \
      exit(1);                                                            \
    }                                                                     \
  } else {                                                                \
    ptnRef->setValue(ptn);                                                \
    pc=pc->next();                                                        \
  }                                                                       \
};


// ----- pop real -----

class Instr_PopReal_t: public SingleInstructionWRTP {
public:
  Instr_PopReal_t(char *pc);
  void printOn(ostream &stream);
};

#define POPREAL_MAC {                                                     \
  PartObject_t *po = evaluateRuntimePath_PO                               \
    (dynamic_cast<SingleInstructionWRTP*>(pc),                            \
     currentPOStack->peek(), this);                                       \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  PredefinedPartObject_t<real> *myReal =                                  \
    dynamic_cast<PredefinedPartObject_t<real>*>(po);                      \
  myReal->value = realStack->pop();                                       \
  pc = pc->next();                                                        \
};

// ----- pop real push int -----

class Instr_PopRealPushInt_t: public SingleInstruction {
public:
  Instr_PopRealPushInt_t();
  void printOn(ostream &stream);
};

#define POPREALPUSHINT_MAC {                                              \
  intStack->push((int)realStack->pop());                                  \
  pc = pc->next();                                                        \
};


// ----- pop str -----

class Instr_PopStr_t: public SingleInstructionWRTP {
public:
  Instr_PopStr_t(char *pc);
  void printOn(ostream &stream);
};

#define POPSTR_MAC {                                                      \
  PartObject_t *po = evaluateRuntimePath_PO                               \
    (dynamic_cast<SingleInstructionWRTP*>(pc),                            \
     currentPOStack->peek(), this);                                       \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  PredefinedStringPartObject_t *myStr =                                   \
    dynamic_cast<PredefinedStringPartObject_t*>(po);                      \
  myStr->setStr(stringStack->pop());                                      \
  pc = pc->next();                                                        \
};

// ----- pop str push char -----

class Instr_PopStrPushChar_t: public SingleInstruction {
public:
  Instr_PopStrPushChar_t();
  void printOn(ostream &stream);
};

#define POPSTRPUSHCHAR_MAC {                                              \
  charStack->push((char)*stringStack->pop());                             \
  pc = pc->next();                                                        \
};

// ----- pop str push int -----

class Instr_PopStrPushInt_t: public SingleInstruction {
public:
  Instr_PopStrPushInt_t();
  void printOn(ostream &stream);
};

#define POPSTRPUSHINT_MAC {                                              \
  intStack->push((int)*stringStack->pop());                              \
  pc = pc->next();                                                       \
};


// ----- pop str push real -----

class Instr_PopStrPushReal_t: public SingleInstruction {
public:
  Instr_PopStrPushReal_t();
  void printOn(ostream &stream);
};

#define POPSTRPUSHREAL_MAC {                                             \
  realStack->push((real)*stringStack->pop());                            \
  pc = pc->next();                                                       \
};

#endif
