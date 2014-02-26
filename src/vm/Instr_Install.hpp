//$Id: Instr_Install.hpp,v 1.3 2001/03/02 08:45:05 hempe Exp $

#ifndef INSTR_INSTALL_HPP
#define INSTR_INSTALL_HPP

// ******************** INCOMPLETE ********************

class Instr_InstallPtn_t;
class Instr_InstallObj_t;
class Instr_InstallQuaPtn_t;
class Instr_InstallQuaObj_t;

#include "Instructions.hpp"

class Instr_InstallPtn_t: public SingleInstruction{
public:
  Instr_InstallPtn_t(char* pc);
  virtual void printOn(ostream &stream);
  unsigned int index; //SKAL PARSES MAASKE MED SUPERKLASSE!
};

#define INSTALLPTN_MAC {                                                  \
  Pattern_t* ptn = patternStack->pop();                                   \
  ((UserPartObject_t*)(currentPOStack->peek()))                           \
            ->setAttribute(((Instr_InstallPtn_t*)pc)->index, ptn);        \
  pc = pc->next();                                                        \
};


class Instr_InstallObj_t: public SingleInstruction{
public:
  Instr_InstallObj_t(char* pc);
  virtual void printOn(ostream &stream);
  unsigned int index; 
};

#define INSTALLOBJ_MAC {                                                  \
  Object_t* obj = objectStack->pop();                                     \
  ((UserPartObject_t*)(currentPOStack->peek()))                           \
            ->setAttribute(((Instr_InstallObj_t*)pc)->index, obj);        \
  pc = pc->next();                                                        \
};

class Instr_InstallQualPtn_t: public SingleInstruction{
public:
  Instr_InstallQualPtn_t(char* pc);
  virtual void printOn(ostream &stream);
  unsigned int index; 
};

#define INSTALLQUALPTN_MAC {                                              \
  Pattern_t* ptn = patternStack->pop();                                   \
  PatternReference_t* ptnRef = new(allocateVM(ptnRefDesc))                \
                                  PatternReference_t(ptn, NULL);          \
  ((UserPartObject_t*)(currentPOStack->peek()))                           \
            ->setAttribute(((Instr_InstallQualPtn_t*)pc)->index, ptnRef); \
  pc = pc->next();                                                        \
};

class Instr_InstallQualObj_t: public SingleInstruction{
public:
  Instr_InstallQualObj_t(char* pc);
  virtual void printOn(ostream &stream);
  unsigned int index; 
};

#define INSTALLQUALOBJ_MAC {                                               \
  Pattern_t* ptn = patternStack->pop();                                    \
  ObjectReference_t* objRef = new(allocateVM(objRefDesc))                  \
                                 ObjectReference_t(ptn, NULL);             \
  ((UserPartObject_t*)(currentPOStack->peek()))                            \
            ->setAttribute(((Instr_InstallQualObj_t*)pc)->index, objRef);  \
  pc = pc->next();                                                         \
};


#endif
