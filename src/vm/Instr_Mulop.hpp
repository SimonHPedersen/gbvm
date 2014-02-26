//$Id: Instr_Mulop.hpp,v 1.8 2001/05/25 09:31:20 hempe Exp $

#ifndef INSTR_MULOP_HPP
#define INSTR_MULOP_HPP

#include <iostream.h>
#include "Instructions.hpp"

class Instr_MulOpMul_t;
class Instr_MulOpDiv_t;
class Instr_MulOpAnd_t;
class Instr_MulOpDivInt_t;
class Instr_MulOpMod_t;

// ------- MUL MUL -------

class Instr_MulOpMul_t: public SingleInstructionWithType {
public:
  Instr_MulOpMul_t(char* pc);
  void printOn(ostream &stream);
};

#define MULOPMUL_MAC                                                       \
  switch(((Instr_MulOpMul_t*)pc)->type){                                   \
   case CHAR_ID:                                                           \
    cerr << "MULOPMUL: illegal type (char)!\n";                            \
    exit(1);                                                               \
    break;                                                                 \
   case INTEGER_ID:                                                        \
    intStack->push(intStack->pop()*intStack->pop());                       \
    break;                                                                 \
   case REAL_ID:                                                           \
    realStack->push(realStack->pop()*realStack->pop());                    \
    break;                                                                 \
/*case STRING_ID: stringStack->push(stringStack->pop()*realStack->pop());break;*/\
   case BOOLEAN_ID:                                                        \
    cerr << "MULOPMUL: illegal type (boolean)!\n";                         \
    exit(1);                                                               \
    break;                                                                 \
   default:                                                                \
    cerr << "MULOPMUL: Unknown type for instruction!\n";                   \
    exit(1);                                                               \
    break;                                                                 \
  }                                                                        \
  pc = pc->next();



// ------- MUL DIV (REAL) -------

class Instr_MulOpDiv_t: public SingleInstructionWithType {
public:
  Instr_MulOpDiv_t(char* pc);
  void printOn(ostream &stream);
};

#define MULOPDIV_MAC                                                       \
  switch(((Instr_MulOpDiv_t*)pc)->type) {                                  \
  case REAL_ID: {                                                          \
    double d1 = realStack->pop();                                          \
    double d2 = realStack->pop();                                          \
    if(d1 == 0) {                                                          \
      cerr << "MULOPDIV: division by zero error\n";                        \
      exit(1);                                                             \
    } else {                                                               \
      realStack->push(d2/d1);                                              \
    }                                                                      \
  };                                                                       \
  break;                                                                   \
  default:                                                                 \
    cerr << "MULOPDIV: wrong type, only reals are supported\n";            \
    exit(1);                                                               \
  }                                                                        \
  pc = pc->next();

// ------- MUL AND (boolean) -------

class Instr_MulOpAnd_t: public SingleInstructionWithType {
public:
  Instr_MulOpAnd_t(char* pc);
  void printOn(ostream &stream);
};

#define MULOPAND_MAC                                                       \
  switch(((Instr_MulOpAnd_t*)pc)->type) {                                  \
  case BOOLEAN_ID: {                                                       \
    char b1 = boolStack->pop();                                            \
    char b2 = boolStack->pop();                                            \
    boolStack->push(b1 & b2);                                              \
   };                                                                      \
  break;                                                                   \
  default:                                                                 \
    cerr << "MULOPAND: wrong type, only bools are supported\n";            \
  }                                                                        \
  pc = pc->next();

// ------- MUL DIV INT -------

class Instr_MulOpDivInt_t: public SingleInstructionWithType {
public:
  Instr_MulOpDivInt_t(char* pc);
  void printOn(ostream &stream);
};

#define MULOPDIVINT_MAC                                                    \
  switch(((Instr_MulOpDivInt_t*)pc)->type) {                               \
  case INTEGER_ID: {                                                       \
    int i1 = intStack->pop();                                              \
    int i2 = intStack->pop();                                              \
    if(i1 == 0) {                                                          \
      cerr << "MULOPDIVINT: division by zero error\n";                     \
      exit(1);                                                             \
    } else {                                                               \
      intStack->push((int)(i2/i1));                                        \
    }                                                                      \
  };                                                                       \
  break;                                                                   \
  case REAL_ID: {                                                          \
    real r1 = realStack->pop();                                            \
    real r2 = realStack->pop();                                            \
    if(r1 == 0) {                                                          \
      cerr << "MULOPDIVINT: division by zero error\n";                     \
      exit(1);                                                             \
    } else {                                                               \
      realStack->push((int)(r2/r1));                                        \
    }                                                                      \
  };                                                                       \
  break;                                                                   \
  default:                                                                 \
    cerr << "MULOPDIVINT: wrong type, only integers and reals"             \
         <<  " are supported\n";                                           \
    exit(1);                                                               \
  break;                                                                   \
  }                                                                        \
  pc = pc->next();


// ------- MUL MOD -------

class Instr_MulOpMod_t: public SingleInstructionWithType {
public:
  Instr_MulOpMod_t(char* pc);
  void printOn(ostream &stream);
};

#define MULOPMOD_MAC                                                       \
  switch(((Instr_MulOpMod_t*)pc)->type) {                                  \
  case INTEGER_ID: {                                                       \
    int i1 = intStack->pop();                                              \
    int i2 = intStack->pop();                                              \
    if(i1 == 0) {                                                          \
      cerr << "MULOPDIVINT: division by zero error\n";                     \
      exit(1);                                                             \
    } else {                                                               \
      intStack->push((int)(i2%i1));                                        \
    }                                                                      \
  };                                                                       \
  break;                                                                   \
  default:                                                                 \
    cerr << "MULOPMOD: wrong type, only integers supported\n";             \
    break;                                                                 \
  }                                                                        \
  pc = pc->next();

#endif


