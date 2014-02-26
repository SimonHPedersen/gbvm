//$Id: Instr_Special.hpp,v 1.19 2001/05/21 11:42:51 hempe Exp $

#ifndef INSTR_SPECIAL_HPP
#define INSTR_SPECIAL_HPP

class Instr_Return_t;
class Instr_ReturnNPPO_t;
class Instr_ReturnMPAttrInit_t;
class Instr_ContGatherVirt_t;
class Instr_JumpNPPO_t;
class Instr_JumpTrueNPPO_t;
class Instr_JumpSubNPPO_t;
class Instr_CopyTop_t;
class Instr_CopyTop2_t;
class Instr_Unsupported_t;

#include "Instructions.hpp"


//returns from a list of instructions.
// pc = execStack->pop()
// cpoStack->pop()
class Instr_Return_t: public SingleInstruction {
public:
                              Instr_Return_t();
  void                        printOn(ostream &stream);
};

#define RETURN_MAC {                                                      \
  currentPOStack->pop();                                                  \
  tmpStack->deleteFrame();                                                \
  if(!(pc = executionStack->pop())) {                                     \
    if(!currentPOStack->peek()) {                                         \
      DB_OUT(vm_thread, "Instr_Return: thread is finished\n");            \
      state = THR_STATE_FINISHED;                                         \
    } else {                                                              \
      cerr << "Instr_Return: illegal null-instruction (po stack was"      \
	" not empty)\n";                                                  \
      exit(1);                                                            \
    }                                                                     \
  }                                                                       \
};


//terminates a list of attribute initialization insturctions
// pc = execStack->pop()
// DOES NOT POP CPO!!!
class Instr_ReturnNPPO_t: public SingleInstruction {
public:
                              Instr_ReturnNPPO_t();
  void                        printOn(ostream &stream);
};

#define RETURNNPPO_MAC {                                                  \
  pc = executionStack->pop();                                             \
};

//checks whether the current PO has a next (more specific) part object
//if this is the case the attributes in this po will be initialized.
// 1. pop the current po
// 2. if popped po has next pointer
//      push the next po to the cpo stack
//      set the pc to the first instruction in the initAttrInstrList
//      of the pushed object
// 3. else
//      return from object initialization by setting pc = execStac->pop()
//      (this (3.) is actually the same as an ordinary return
class Instr_ReturnMPAttrInit_t: public SingleInstruction {
public:
                              Instr_ReturnMPAttrInit_t();
  void                        printOn(ostream &stream);
};

#define RETURNMPATTRINIT_MAC {                                           \
  PartObject_t *po = currentPOStack->pop()->next();                      \
  tmpStack->deleteFrame();                                               \
  if(po) {                                                               \
    currentPOStack->push(po);                                            \
    tmpStack->addNewFrame();                                             \
    pc = po->mixin->getMainPart()->getMPAttrsInstrPtr()->first();        \
  } else {                                                               \
    pc = executionStack->pop();                                          \
  }                                                                      \
};


// ----- continue gather virtual -----

class Instr_ContGatherVirt_t: public SingleInstruction {
public:
                              Instr_ContGatherVirt_t(char *_parentAttrName,
						     int _parentAttrIndex,
						     char *_parentMainPartID):
				parentAttrName(_parentAttrName),
				parentAttrIndex(_parentAttrIndex),
				parentMainPartID(_parentMainPartID)
                                { id = CONTGATHERVIRT_ID; }
  void                        printOn(ostream &stream);
  char                       *parentAttrName;
  int                         parentAttrIndex;
  char                       *parentMainPartID;
};

#define CONTGATHERVIRT_MAC {                                              \
  DB_INDENT;                                                              \
  PartObject_t *po = currentPOStack->peek()->next();                      \
  AttributeVirtual *attrv;                                                \
  while(po) {                                                             \
    if((attrv = po->getVirtAttr(                                          \
                    ((Instr_ContGatherVirt_t*)pc)->parentAttrName,        \
		    ((Instr_ContGatherVirt_t*)pc)->parentAttrIndex,       \
		    ((Instr_ContGatherVirt_t*)pc)->parentMainPartID))) {  \
      DB_OUT(vm_iexe_spec, "CONTGATHERVIRT_MAC: found virtual attr\n");   \
      currentPOStack->pop();                                              \
      currentPOStack->push(po);                                           \
      tmpStack->deleteFrame();                                            \
      tmpStack->addNewFrame();                                            \
      pc = attrv->getGVInstructionListPtr()->first();                     \
      break;                                                              \
    }                                                                     \
    po = po->next();                                                      \
  }                                                                       \
  if(!po) {                                                               \
    DB_OUT(vm_iexe_spec, "CONTGATHERVIRT_MAC: no matching virt attr\n");  \
    currentPOStack->pop();                                                \
    tmpStack->deleteFrame();                                              \
    pc = executionStack->pop();                                           \
  }                                                                       \
  DB_OUTDENT;                                                             \
};


//initiates initialization of an attribute in current PO by pushing
//the next instruction to the execution stack and setting pc to
//firstInitInst.
//
//Attributes in a mainpart are initialized using a list of these
//instructions terminated by a Instr_ReturnMPAttrInit_t

class Instr_JumpNPPO_t: public SingleInstruction {
public:
                              Instr_JumpNPPO_t(Instruction *instr);
  void                        printOn(ostream &stream);
  Instruction                *getInstrPtr(void) {return instrPtr;}
private:
  Instruction                *instrPtr;
};

#define JUMPNPPO_MAC {                                                   \
  pc = ((Instr_JumpNPPO_t*)pc)->getInstrPtr();                           \
};

// --- jump if pop boolean yields true ---

class Instr_JumpTrueNPPO_t: public SingleInstruction {
public:
                              Instr_JumpTrueNPPO_t(Instruction *instr);
  void                        printOn(ostream &stream);
  Instruction                *getInstrPtr(void) {return instrPtr;}
private:
  Instruction                *instrPtr;
};

#define JUMPTRUENPPO_MAC {                                               \
  if(boolStack->pop()) {                                                 \
    pc = ((Instr_JumpTrueNPPO_t*)pc)->getInstrPtr();                     \
  } else {                                                               \
    /* just continue with next instruction */                            \
    pc = pc->next();                                                     \
  }                                                                      \
};

// --- jump to sub-routine (returns to following instruction afterwards) 

class Instr_JumpSubNPPO_t: public SingleInstruction {
public:
                              Instr_JumpSubNPPO_t(Instruction *instr);
  void                        printOn(ostream &stream);
  Instruction                *getInstrPtr(void) {return instrPtr;}
private:
  Instruction                *instrPtr;
};

#define JUMPSUBNPPO_MAC {                                                \
  executionStack->push(pc->next());                                      \
  pc = ((Instr_JumpSubNPPO_t*)pc)->getInstrPtr();                        \
};


//copies the top stack element according to the type
class Instr_CopyTop_t: public SingleInstruction, public WithType {
public:
                              Instr_CopyTop_t(int _type)
                                { type = _type;
			          id = COPYTOP_ID; }
  void                        printOn(ostream &stream);
};


#define COPYTOP_MAC {                                                     \
  switch (((Instr_CopyTop_t*)pc)->type) {                                 \
  case BOOLEAN_ID:                                                        \
    boolStack->push(boolStack->peek());                                   \
    break;                                                                \
  case CHAR_ID:                                                           \
    charStack->push(charStack->peek());                                   \
    break;                                                                \
  case INTEGER_ID:                                                        \
    intStack->push(intStack->peek());                                     \
    break;                                                                \
  case REAL_ID:                                                           \
    realStack->push(realStack->peek());                                   \
    break;                                                                \
  case STRING_ID:                                                         \
    stringStack->push(stringStack->peek());                               \
    break;                                                                \
  case OBJECT_ID:                                                         \
    objectStack->push(objectStack->peek());                               \
    break;                                                                \
  case PATTERN_ID:                                                        \
    patternStack->push(patternStack->peek());                             \
    break;                                                                \
  default:                                                                \
    cerr << "Instr_CopyTop_t: unsupported type: "                         \
         << ((Instr_CopyTop_t*)pc)->type << endl;                         \
    exit(1);                                                              \
  }                                                                       \
  pc = pc->next();                                                        \
};

//copies the two top stack elements according to the type
class Instr_CopyTop2_t: public SingleInstruction, public WithType {
public:
                              Instr_CopyTop2_t(int _type)
                                { type = _type;
			          id = COPYTOP2_ID; }
  void                        printOn(ostream &stream);
};

#define COPYTOP2_MAC {                                                   \
  switch (((Instr_CopyTop_t*)pc)->type) {                                \
  case BOOLEAN_ID: {                                                     \
    char boolval1 = boolStack->peek(1);                                  \
    char boolval2 = boolStack->peek();                                   \
    boolStack->push(boolval1);                                           \
    boolStack->push(boolval2);                                           \
    break;                                                               \
  }                                                                      \
  case CHAR_ID: {                                                        \
    char charval1 = charStack->peek(1);                                  \
    char charval2 = charStack->peek();                                   \
    charStack->push(charval1);                                           \
    charStack->push(charval2);                                           \
    break;                                                               \
  }                                                                      \
  case INTEGER_ID: {                                                     \
    int intval1 = intStack->peek(1);                                     \
    int intval2 = intStack->peek();                                      \
    intStack->push(intval1);                                             \
    intStack->push(intval2);                                             \
    break;                                                               \
  }                                                                      \
  case REAL_ID: {                                                        \
    real realval1 = realStack->peek(1);                                  \
    real realval2 = realStack->peek();                                   \
    realStack->push(realval1);                                           \
    realStack->push(realval2);                                           \
    break;                                                               \
  }                                                                      \
  default:                                                               \
    cerr << "Instr_CopyTop2_t: unsupported type\n";                      \
    exit(1);                                                             \
  }                                                                      \
  pc = pc->next();                                                       \
};

class Instr_Unsupported_t: public SingleInstruction {
public:
                              Instr_Unsupported_t(char *pc) {
				name = strdup(pc);
				id = UNSUPPORTED_ID;
			      }
  void                        printOn(ostream &stream);
  char *name;
};

#define UNSUPPORTED_MAC {                                                \
  cerr << "Cannot execute instruction: " <<                              \
    ((Instr_Unsupported_t*)pc)->name << endl;                            \
  exit(1);                                                               \
};

#endif
