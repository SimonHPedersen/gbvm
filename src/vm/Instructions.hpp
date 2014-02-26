//$Id: Instructions.hpp,v 1.22 2001/05/10 12:36:57 hempe Exp $

#ifndef INSTRUCTIONS_HPP
#define INSTRUCTIONS_HPP

#define MAX_INPUTSTRING_SIZE 4096

class Instruction;

//et hack class SingleInstruction er afskaffet
typedef class Instruction SingleInstruction;

class SingleInstructionWRTP;
class WithType;
class SingleInstructionWithType;
class EmptySingleInstruction_t;

#include "Lists.hpp"
#include "Thread.hpp"
class Instruction : public LstElm<Instruction> {
public:
  virtual void                printOn(ostream& dest) = 0;
  //  unsigned int                getInstId() { return id; }
  virtual Instruction*        execute(Thread_t *myThread);
  //protected:
  unsigned int id;
};

#include "InstructionCodes.hpp"
#include "RunTimeSteps.hpp"

class SingleInstructionWRTP : public SingleInstruction {
public:
                              SingleInstructionWRTP() {}
  void                        addStep(RuntimeStep* rts) { rtp.add(rts); }
  Lst<RuntimeStep>*           getRuntimePath() { return &rtp; }
protected:
  Lst<RuntimeStep> rtp;
};

class WithType {
public:
  void                        parseType(char* pc);
  int                         type;
};

class SingleInstructionWithType : public SingleInstruction, public WithType {
public:
                              SingleInstructionWithType() {}
};

class Instr_EmptySingle_t: public SingleInstruction {
public:
  virtual void                printOn(ostream &stream);
  virtual Instruction*        execute(Thread_t *myThread){return NULL;}
};

//Checks if p1 <= p2 and saves the result in the variable 'result'
//NB! this macro EXPORTS its result via the var 'result' 
#define PTNLTEQ(p1,p2) Mixin_t** p1MixinList = p1->getMixins(); \
    unsigned int p1Length = p1->getLength();				   \
    Mixin_t** p2MixinList = p2->getMixins();          			   \
    unsigned int p2Length = p2->getLength();				   \
    unsigned int result = 1;						   \
    unsigned int subresult = 0;						   \
    for(unsigned int i = 0 ; i < p2Length && result ; i++){                \
     for(unsigned int j = 0 ; j < p1Length && !subresult ; j++){           \
      if(p2MixinList[i]->origin == p1MixinList[j]->origin &&               \
	 p2MixinList[i]->getMainPart() == p1MixinList[j]->getMainPart()){  \
        subresult = 1;                                                     \
      }                                                                    \
     }                                                                     \
     if(!subresult){                                                       \
      result = 0;                                                          \
     }                                                                     \
     subresult = 0;                                                        \
    }                                                                      

//Checks if p1 >= p2 and saves the result in the variable 'result'
//NB! this macro EXPORTS its result via the var 'result' 
#define PTNGTEQ(p1,p2) Mixin_t** p1MixinList = p1->getMixins(); \
    unsigned int p1Length = p1->getLength();				   \
    Mixin_t** p2MixinList = p2->getMixins();          			   \
    unsigned int p2Length = p2->getLength();				   \
    unsigned int result = 1;						   \
    unsigned int subresult = 0;						   \
    for(unsigned int i = 0 ; i < p1Length && result ; i++){                \
     for(unsigned int j = 0 ; j < p2Length && !subresult ; j++){           \
      if(p1MixinList[i]->origin == p2MixinList[j]->origin &&               \
	 p1MixinList[i]->getMainPart() == p2MixinList[j]->getMainPart()){  \
        subresult = 1;                                                     \
      }                                                                    \
     }                                                                     \
     if(!subresult){                                                       \
      result = 0;                                                          \
     }                                                                     \
     subresult = 0;                                                        \
    }                                                                      

//Checks if p1 == p2 and saves the result in the variable 'result'
//NB! this macro EXPORTS its result via the var 'result' 
#define PTNEQ(p1,p2) Mixin_t** p1MixinList = p1->getMixins(); \
    unsigned int p1Length = p1->getLength();				   \
    Mixin_t** p2MixinList = p2->getMixins();          			   \
    unsigned int p2Length = p2->getLength();				   \
    unsigned int result = 1;						   \
    if(p1Length != p2Length){                                              \
      result = 0;                                                          \
    }                                                                      \
    for(unsigned int i = 0 ; i < p2Length && result ; i++){                \
      if(p2MixinList[i]->origin != p1MixinList[i]->origin ||               \
	 p2MixinList[i]->getMainPart() != p1MixinList[i]->getMainPart()){  \
        result = 0;                                                        \
      }                                                                    \
    }                                                                      


#define DYNINIT_MAC                                                       \
    DB_INDENT;                                                            \
    DB_OUT(vm_thread_instinfo, "DYNINIT_MAC: dynamicInitInst: "           \
	   << dynamicInitInst << " dynamicInitPO: "                       \
	   << (void*)dynamicInitPO);                                      \
    executionStack->push(pc);                                             \
    executionStack->push(sharedReturnInst);                               \
    pc = dynamicInitInst;                                                 \
    dynamicInitInst = 0;                                                  \
    currentPOStack->push(dynamicInitPO);                                  \
    dynamicInitPO = 0;                                                    \
    tmpStack->addNewFrame();                                              \
    DB_OUTDENT;                                                           \
    break;                                                                \


// ----------------------------------------------------------------------
// Inserters for entity printing

ostream& operator<<(ostream& dest, Instruction* instr);
ostream& operator<<(ostream& dest, Instruction& instr);

#include "InstructionCodes.hpp"
#include "Instr_Add.hpp"
#include "Instr_Mulop.hpp"
#include "Instr_Install.hpp"
#include "Instr_Peek.hpp"
#include "Instr_Pop.hpp"
#include "Instr_Push.hpp"
#include "Instr_Relop.hpp"
#include "Instr_Special.hpp"
#include "Instr_Multiline.hpp"
#include "Instr_Misc.hpp"

extern Instr_Return_t* sharedReturnInst;

#endif
