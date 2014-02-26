//$Id: Instr_Relop.hpp,v 1.13 2001/05/21 11:42:50 hempe Exp $

#ifndef INSTR_RELOP_HPP
#define INSTR_RELOP_HPP

//#include <strings.h> STAA I THREAD
#include "Instructions.hpp"

class Instr_RelOpLT;
class Instr_RelOpLTEQ;
class Instr_RelOpNEQ;
class Instr_RelOpEQ;
class Instr_RelOpGT;
class Instr_RelOpGTEQ;

// ------- RELOP less than -------

class Instr_RelOpLT_t: public SingleInstructionWithType{
public:
  Instr_RelOpLT_t(char* pc);
  Instr_RelOpLT_t(int type);
  void printOn(ostream &stream);
};

#define RELOPLT_MAC                                                        \
  switch(((Instr_RelOpLT_t*)pc)->type){                                    \
   case BOOLEAN_ID:                                                        \
    boolStack->push(boolStack->pop() > boolStack->pop());                  \
    break;                                                                 \
   case CHAR_ID:                                                           \
    boolStack->push(charStack->pop() > charStack->pop());                  \
    break;                                                                 \
   case INTEGER_ID:                                                        \
    boolStack->push(intStack->pop() > intStack->pop());                    \
    break;                                                                 \
   case REAL_ID:                                                           \
    boolStack->push(realStack->pop() > realStack->pop());                  \
    break;                                                                 \
   case STRING_ID:                                                         \
    if(strcmp(stringStack->pop(),stringStack->pop()) > 0){                 \
      boolStack->push(1);                                                  \
    } else {                                                               \
      boolStack->push(0);                                                  \
    }                                                                      \
    break;                                                                 \
   case PATTERNREFERENCE_ID:{						   \
    DB_INDENT;                                                             \
    DB_OUT(vm_inst_relop, "RELOPLT_MAC: comparing patterns");              \
    Pattern_t* p2 = patternStack->pop();      			           \
    Pattern_t* p1 = patternStack->pop();              		           \
    if(p1 == p2){                                                          \
      boolStack->push(0);                                                  \
    } else if(!p1){                                                        \
      boolStack->push(1);                                                  \
    } else if(!p2){                                                        \
      boolStack->push(0);                                                  \
    } else {                                                               \
      PTNLTEQ(p1,p2);                                                      \
      if(result && (p1Length > p2Length)){                                 \
        boolStack->push(1);                                                \
      } else {                                                             \
        boolStack->push(0);                                                \
      }                                                                    \
    }                                                                      \
    };                                                                     \
    DB_OUTDENT;                                                            \
    break;                                                                 \
  default:                                                                 \
    cerr << "RELOPLT: Unknown type for instruction : << "                  \
         << ((Instr_RelOpLT_t*)pc)->type << endl;                          \
    exit(1);                                                               \
    break;                                                                 \
  }                                                                        \
  pc = pc->next();

// ------- RELOP less than equal -------

class Instr_RelOpLTEQ_t: public SingleInstructionWithType{
public:
  Instr_RelOpLTEQ_t(char* pc);
  void printOn(ostream &stream);
};


#define RELOPLTEQ_MAC                                                      \
  switch(((Instr_RelOpLTEQ_t*)pc)->type){                                  \
   case BOOLEAN_ID:                                                        \
    boolStack->push(boolStack->pop() >= boolStack->pop());                 \
    break;                                                                 \
   case CHAR_ID:                                                           \
    boolStack->push(charStack->pop() >= charStack->pop());                 \
    break;                                                                 \
   case INTEGER_ID:                                                        \
    boolStack->push(intStack->pop() >= intStack->pop());                   \
    break;                                                                 \
   case REAL_ID:                                                           \
    boolStack->push(realStack->pop() >= realStack->pop());                 \
    break;                                                                 \
   case STRING_ID:                                                         \
    if(strcmp(stringStack->pop(),stringStack->pop()) >= 0){                \
      boolStack->push(1);                                                  \
    } else {                                                               \
      boolStack->push(0);                                                  \
    }                                                                      \
    break;                                                                 \
   case PATTERNREFERENCE_ID:{						   \
    DB_INDENT;                                                             \
    DB_OUT(vm_inst_relop, "RELOPLTEQ_MAC: comparing patterns");            \
    Pattern_t* p2 = patternStack->pop();      			           \
    Pattern_t* p1 = patternStack->pop();              		           \
    if(p1 == p2){                                                          \
      boolStack->push(1);                                                  \
    } else if(!p1){                                                        \
      boolStack->push(1);                                                  \
    } else if(!p2){                                                        \
      boolStack->push(0);                                                  \
    } else {                                                               \
      PTNLTEQ(p1,p2); /* exports result in 'result' */                     \
      boolStack->push(result);                                             \
    }                                                                      \
    };                                                                     \
    DB_OUTDENT;                                                            \
    break;                                                                 \
  default:                                                                 \
    cerr << "RELOPLTEQ: Unknown type for instruction : << "                \
         << ((Instr_RelOpLTEQ_t*)pc)->type << endl;                        \
    exit(1);                                                               \
    break;                                                                 \
  }                                                                        \
  pc = pc->next();

// ------- RELOP equal -------

class Instr_RelOpEQ_t: public SingleInstructionWithType{
public:
                              Instr_RelOpEQ_t(char* pc);
                              Instr_RelOpEQ_t(int type);
                                //used in e.g. generalIf
  void printOn(ostream &stream);
private:

};


#define RELOPEQ_MAC {                                                      \
  switch(((Instr_RelOpEQ_t*)pc)->type){                                    \
   case BOOLEAN_ID:                                                        \
    boolStack->push(boolStack->pop() == boolStack->pop());                 \
    break;                                                                 \
   case CHAR_ID:                                                           \
    boolStack->push(charStack->pop() == charStack->pop());                 \
    break;                                                                 \
   case INTEGER_ID:                                                        \
    boolStack->push(intStack->pop() == intStack->pop());                   \
    break;                                                                 \
   case REAL_ID:                                                           \
    boolStack->push(realStack->pop() == realStack->pop());                 \
    break;                                                                 \
   case STRING_ID:                                                         \
    if(strcmp(stringStack->pop(),stringStack->pop()) == 0){                \
      boolStack->push(1);                                                  \
    } else {                                                               \
      boolStack->push(0);                                                  \
    }                                                                      \
    break;                                                                 \
   case OBJECT_ID: case OBJECTREFERENCE_ID:           			   \
    DB_INDENT;                                                             \
    DB_OUT(vm_inst_relop, "RELOPEQ_MAC: comparing objects");               \
    boolStack->push(objectStack->pop() == objectStack->pop());             \
    DB_OUTDENT;                                                            \
    break;                                                                 \
   case PATTERN_ID: case PATTERNREFERENCE_ID:{				   \
    DB_INDENT;                                                             \
    DB_OUT(vm_inst_relop, "RELOPEQ_MAC: comparing patterns");              \
    Pattern_t* p2 = patternStack->pop();      			           \
    Pattern_t* p1 = patternStack->pop();              		           \
    if(p1 == p2){                                                          \
      boolStack->push(1);                                                  \
    } else if (!p1 || !p2){                                                \
      boolStack->push(0);                                                  \
    } else {                                                               \
      PTNEQ(p1,p2);                                                        \
      boolStack->push(result);                                             \
    }                                                                      \
    DB_OUTDENT;                                                            \
    break;                                                                 \
    };                                                                     \
  default:                                                                 \
    cerr << "RELOPEQ: Unknown type for instruction : << "                  \
         << ((Instr_RelOpEQ_t*)pc)->type << endl;                          \
    exit(1);                                                               \
    break;                                                                 \
  }                                                                        \
  pc = pc->next();                                                         \
  };

// ------- RELOP not equal -------

class Instr_RelOpNEQ_t: public SingleInstructionWithType{
public:
  Instr_RelOpNEQ_t(char* pc);
  void printOn(ostream &stream);
private:

};


#define RELOPNEQ_MAC                                                       \
  switch(((Instr_RelOpNEQ_t*)pc)->type){                                   \
   case BOOLEAN_ID:                                                        \
    boolStack->push(boolStack->pop() != boolStack->pop());                 \
    break;                                                                 \
   case CHAR_ID:                                                           \
    boolStack->push(charStack->pop() != charStack->pop());                 \
    break;                                                                 \
   case INTEGER_ID:                                                        \
    boolStack->push(intStack->pop() != intStack->pop());                   \
    break;                                                                 \
   case REAL_ID:                                                           \
    boolStack->push(realStack->pop() != realStack->pop());                 \
    break;                                                                 \
   case STRING_ID:                                                         \
    if(strcmp(stringStack->pop(),stringStack->pop()) != 0){                \
      boolStack->push(1);                                                  \
    } else {                                                               \
      boolStack->push(0);                                                  \
    }                                                                      \
    break;                                                                 \
   case OBJECT_ID: case OBJECTREFERENCE_ID:				   \
    DB_INDENT;                                                             \
    DB_OUT(vm_inst_relop, "RELOPNEQ_MAC: comparing objects");              \
    boolStack->push(objectStack->pop() != objectStack->pop());             \
    DB_OUTDENT;                                                            \
    break;                                                                 \
   case PATTERN_ID: case PATTERNREFERENCE_ID:{				   \
    DB_INDENT;                                                             \
    DB_OUT(vm_inst_relop, "RELOPNEQ_MAC: comparing patterns");             \
    Pattern_t* p2 = patternStack->pop();      			           \
    Pattern_t* p1 = patternStack->pop();              		           \
    if(p1 == p2){                                                          \
      boolStack->push(0);                                                  \
    } else if(!p1 || !p2){                                                 \
      boolStack->push(1);                                                  \
    } else {                                                               \
      PTNLTEQ(p1,p2); /* exports result in 'result' */                     \
      boolStack->push(!result);                                            \
    }                                                                      \
    };                                                                     \
    DB_OUTDENT;                                                            \
    break;                                                                 \
  default:                                                                 \
    cerr << "RELOPNEQ: Unknown type for instruction : << "                 \
         << ((Instr_RelOpNEQ_t*)pc)->type << endl;                         \
    exit(1);                                                               \
    break;                                                                 \
  }                                                                        \
  pc = pc->next();


// ------- RELOP greater than -------

class Instr_RelOpGT_t: public SingleInstructionWithType{
public:
  Instr_RelOpGT_t(char* pc);
  Instr_RelOpGT_t(int type);
  void printOn(ostream &stream);
};

#define RELOPGT_MAC                                                        \
  switch(((Instr_RelOpGT_t*)pc)->type){                                    \
   case BOOLEAN_ID:                                                        \
    boolStack->push(boolStack->pop() < boolStack->pop());                  \
    break;                                                                 \
   case CHAR_ID:                                                           \
    boolStack->push(charStack->pop() < charStack->pop());                  \
    break;                                                                 \
   case INTEGER_ID:                                                        \
    boolStack->push(intStack->pop() < intStack->pop());                    \
    break;                                                                 \
   case REAL_ID:                                                           \
    boolStack->push(realStack->pop() < realStack->pop());                  \
    break;                                                                 \
   case STRING_ID:                                                         \
    if(strcmp(stringStack->pop(),stringStack->pop()) < 0){                 \
      boolStack->push(1);                                                  \
    } else {                                                               \
      boolStack->push(0);                                                  \
    }                                                                      \
    break;                                                                 \
   case PATTERNREFERENCE_ID:{						   \
    DB_INDENT;                                                             \
    DB_OUT(vm_inst_relop, "RELOPGT_MAC: comparing patterns");              \
    Pattern_t* p2 = patternStack->pop();      			           \
    Pattern_t* p1 = patternStack->pop();              		           \
    if(p1 == p2){                                                          \
      boolStack->push(0);                                                  \
    } else if(!p1){                                                        \
      boolStack->push(0);                                                  \
    } else if(!p2){                                                        \
      boolStack->push(1);                                                  \
    } else {                                                               \
      PTNGTEQ(p1,p2);                                                      \
      if(result && (p2Length > p1Length)){                                 \
        boolStack->push(1);                                                \
      } else {                                                             \
        boolStack->push(0);                                                \
      }                                                                    \
    }                                                                      \
    };                                                                     \
    DB_OUTDENT;                                                            \
    break;                                                                 \
  default:                                                                 \
    cerr << "RELOPGT: Unknown type for instruction : << "                  \
         << ((Instr_RelOpGT_t*)pc)->type << endl;                          \
    exit(1);                                                               \
    break;                                                                 \
  }                                                                        \
  pc = pc->next();

// ------- RELOP greater than equal -------

class Instr_RelOpGTEQ_t: public SingleInstructionWithType{
public:
  Instr_RelOpGTEQ_t(char* pc);
  void printOn(ostream &stream);
};


#define RELOPGTEQ_MAC                                                      \
  switch(((Instr_RelOpGTEQ_t*)pc)->type){                                  \
   case BOOLEAN_ID:                                                        \
    boolStack->push(boolStack->pop() <= boolStack->pop());                 \
    break;                                                                 \
   case CHAR_ID:                                                           \
    boolStack->push(charStack->pop() <= charStack->pop());                 \
    break;                                                                 \
   case INTEGER_ID:                                                        \
    boolStack->push(intStack->pop() <= intStack->pop());                   \
    break;                                                                 \
   case REAL_ID:                                                           \
    boolStack->push(realStack->pop() <= realStack->pop());                 \
    break;                                                                 \
   case STRING_ID:                                                         \
    if(strcmp(stringStack->pop(),stringStack->pop()) <= 0){                \
      boolStack->push(1);                                                  \
    } else {                                                               \
      boolStack->push(0);                                                  \
    }                                                                      \
    break;                                                                 \
   case PATTERNREFERENCE_ID:{						   \
    DB_INDENT;                                                             \
    DB_OUT(vm_inst_relop, "RELOPGTEQ_MAC: comparing patterns");            \
    Pattern_t* p2 = patternStack->pop();      			           \
    Pattern_t* p1 = patternStack->pop();              		           \
    if(p1 == p2){                                                          \
      boolStack->push(1);                                                  \
    } else if(!p1){                                                        \
      boolStack->push(0);                                                  \
    } else if(!p2){                                                        \
      boolStack->push(1);                                                  \
    } else {                                                               \
      PTNGTEQ(p1,p2);                                                      \
      boolStack->push(result);                                             \
    }                                                                      \
    };                                                                     \
    DB_OUTDENT;                                                            \
    break;                                                                 \
  default:                                                                 \
    cerr << "RELOPGTEQ: Unknown type for instruction : << "                \
         << ((Instr_RelOpGTEQ_t*)pc)->type << endl;                        \
    exit(1);                                                               \
    break;                                                                 \
  }                                                                        \
  pc = pc->next();


#endif
