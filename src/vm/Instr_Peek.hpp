//$Id: Instr_Peek.hpp,v 1.5 2001/05/21 11:42:49 hempe Exp $

#ifndef INSTR_PEEK_HPP
#define INSTR_PEEK_HPP

#include <iostream.h>
#include "Instructions.hpp"

// ----- peek objref -----

class Instr_PeekObjRef_t: public SingleInstructionWRTP {
public:
  Instr_PeekObjRef_t(char *pc);
  void printOn(ostream &stream);
};

#define PEEKOBJREF_MAC {                                                  \
  ObjectReference_t *objRef = evaluateRuntimePath_ObjRef                  \
    (dynamic_cast<SingleInstructionWRTP*>(pc),                            \
     currentPOStack->peek(), this);                                       \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  Object_t* obj = objectStack->peek();                                    \
  if(obj){                                                                \
    Pattern_t* p1 = obj->getPattern();                                    \
    Pattern_t* p2 = objRef->getQualification();                           \
    PTNLTEQ(p1,p2);                                                       \
    if(result){                                                           \
      objRef->setValue(obj);                                              \
      pc=pc->next();                                                      \
    } else {                                                              \
      cerr << "PEEKOBJREF_MAC: Runtime Error: Pattern of peeked object is"\
           << " not less than qualfication of object reference" << endl;  \
      exit(1);                                                            \
    }                                                                     \
  } else {                                                                \
    objRef->setValue(obj);                                                \
    pc=pc->next();                                                        \
  }                                                                       \
};

// ----- peek tmpref -----

class Instr_PeekTmpRef_t: public SingleInstructionWRTP {
public:
  Instr_PeekTmpRef_t(char *pc);
  void printOn(ostream &stream);
};

#define PEEKTMPREF_MAC {                                                  \
  ObjectReference_t *objRef = evaluateRuntimePath_ObjRef                  \
    (dynamic_cast<SingleInstructionWRTP*>(pc),                            \
     currentPOStack->peek(), this);                                       \
  if(dynamicInitInst){                                                    \
    DYNINIT_MAC;                                                          \
  }                                                                       \
  Object_t* obj = (Object_t*)tmpStack->peek();                            \
  if(obj){                                                                \
    Pattern_t* p1 = obj->getPattern();                                    \
    Pattern_t* p2 = objRef->getQualification();                           \
    PTNLTEQ(p1,p2);                                                       \
    if(result){                                                           \
      objRef->setValue(obj);                                              \
      pc=pc->next();                                                      \
    } else {                                                              \
      cerr << "PEEKTMPREF_MAC: Runtime Error: Pattern of peeked object is"\
           << " not less than qualfication of object reference" << endl;  \
      exit(1);                                                            \
    }                                                                     \
  } else {                                                                \
    objRef->setValue(obj);                                                \
    pc=pc->next();                                                        \
  }                                                                       \
};

#endif
