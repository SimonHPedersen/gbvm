//$Id: Instr_Add.hpp,v 1.11 2001/05/21 11:42:48 hempe Exp $

#ifndef INSTR_ADD_HPP
#define INSTR_ADD_HPP

//#include <iostream.h>
//#include "Instructions.hpp"

class Instr_AddOpPlus_t;
class Instr_AddOpMinus_t;
class Instr_AddOpOr_t;
class Instr_AddOpXor_t;

#include "vm.hpp"

// ------- PLUS -------

class Instr_AddOpPlus_t: public SingleInstructionWithType{
public:
  Instr_AddOpPlus_t(char* pc);
  Instr_AddOpPlus_t(int pc);
  void printOn(ostream &stream);
private:

};

#define ADDOPPLUS_MAC                                                      \
  switch(((Instr_AddOpPlus_t*)pc)->type){                                  \
   case CHAR_ID:                                                           \
    charStack->push(charStack->pop()+charStack->pop());                    \
    break;                                                                 \
   case INTEGER_ID:                                                        \
    intStack->push(intStack->pop()+intStack->pop());                       \
    break;                                                                 \
   case REAL_ID:                                                           \
    realStack->push(realStack->pop()+realStack->pop());                    \
    break;                                                                 \
/* case STRING_ID: stringStack->push(stringStack->pop()+realStack->pop());break;*/\
   case BOOLEAN_ID:                                                        \
    cerr << "ADDOPPLUS: illegal type (boolean)!\n";                        \
    exit(1);                                                               \
    break;                                                                 \
   default:                                                                \
    cerr << "ADDOPPLUS: Unknown type for instruction!\n";                  \
    exit(1);                                                               \
    break;                                                                 \
  }                                                                        \
  pc = pc->next();

// -------- MINUS -------

class Instr_AddOpMinus_t: public SingleInstructionWithType{
public:
  Instr_AddOpMinus_t(char* pc);
  void printOn(ostream &stream);
};

#define ADDOPMINUS_MAC                                                     \
  switch(((Instr_AddOpMinus_t*)pc)->type){                                 \
   case CHAR_ID:                                                           \
    charStack->push(-charStack->pop() + charStack->pop());  \
    break;                                                                 \
   case INTEGER_ID:                                                        \
    intStack->push(-intStack->pop() + intStack->pop());                    \
    break;                                                                 \
   case REAL_ID:                                                           \
    realStack->push(-realStack->pop() + realStack->pop());                 \
    break;                                                                 \
   case BOOLEAN_ID:                                                        \
    cerr << "ADDOPMINUS: illegal type (boolean)!\n";                       \
    exit(1);                                                               \
    break;                                                                 \
   default:                                                                \
    cerr << "ADDOPMINUS: Unknown type for instruction!\n";                 \
    exit(1);                                                               \
    break;                                                                 \
  }                                                                        \
  pc = pc->next();

// ------ Or ------

class Instr_AddOpOr_t: public SingleInstructionWithType{
public:
  Instr_AddOpOr_t(char* pc);
  void printOn(ostream &stream);
};

#define ADDOPOR_MAC                                                        \
  switch(((Instr_AddOpOr_t*)pc)->type){                                    \
   case BOOLEAN_ID: {                                                      \
    int b1 = boolStack->pop();                                             \
    int b2 = boolStack->pop();                                             \
    boolStack->push(b1 | b2);                                              \
    };                                                                     \
    break;                                                                 \
   default:                                                                \
    cerr << "ADDOPOR: Illegal type, " << ((Instr_AddOpOr_t*)pc)->type      \
         << " for instruction!\n";                                         \
    exit(1);                                                               \
    break;                                                                 \
  }                                                                        \
  pc = pc->next();

// ------ Xor ------

class Instr_AddOpXor_t: public SingleInstructionWithType{
public:
  Instr_AddOpXor_t(char* pc);
  void printOn(ostream &stream);
};

#define ADDOPXOR_MAC                                                       \
  switch(((Instr_AddOpXor_t*)pc)->type){                                   \
   case BOOLEAN_ID:{                                                       \
    int b1 = boolStack->pop();                                             \
    int b2 = boolStack->pop();                                             \
    boolStack->push(b1 ^ b2);                                              \
    };                                                                     \
    break;                                                                 \
   default:                                                                \
    cerr << "ADDOPXOR: Illegal type, " << ((Instr_AddOpXor_t*)pc)->type    \
         << " for instruction!\n";                                         \
    exit(1);                                                               \
    break;                                                                 \
  }                                                                        \
  pc = pc->next();

#endif
