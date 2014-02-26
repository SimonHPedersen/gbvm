//$Id: Thread.hpp,v 1.25 2001/05/10 12:37:00 hempe Exp $

#ifndef THREAD_HPP
#define THREAD_HPP

class Thread_t;

#define STACK_SIZE 1024
#define MERGE_SCRATCH_SIZE 256

#ifdef DEBUG
#define DONT_PRINT_UNTIL 66020 //0
#endif

#ifdef DEBUG
  extern int instrsExecuted;
                                //a debug counter to check 
                                //how many instr we have ex'ed
void atExitFkt(void);           //a debug function to tell the 
                                //last instruction that was ex'ed
#endif //DEBUG

#include <strings.h>
#include "VMObject.hpp"
#include "Stacks.hpp"
#include "GBetaEntities.hpp"
#include "ByteCodeLoader.hpp"
#include "Instructions.hpp"
#include "garbage_collector.hpp"

enum thread_signals {
  THR_SIGNAL_NONE = 1000, //none, just go!
  THR_SIGNAL_GC_STOP,
  THR_SIGNAL_RESUME,
};

enum thread_states {
  THR_STATE_RUNNING = 2000,
  THR_STATE_BLOCKED,
  THR_STATE_GC_STOPPED,
  THR_STATE_FINISHED
};

class Thread_t{
public:
  void                       *operator new(size_t size, void *location);
  void                       *operator new(size_t size);
                                //use in combination with 
                              Thread_t(Pattern_t *pattern, Scheduler_t* sch);
                              Thread_t(ObjectDescriptor_t *threadDesc);
                                //this constructor should 
                                //NOT be used except when initializing the
                                //object descriptor
  int                         run(void);
  int                         getState();
                                //returns the state of the thread
  void                        sendSignal(int signal);
                                //suggest a new state for the thread.
                                //run should return with this value 
                                //at its next chance
  void			      print();
public:
  NonRefStack_t<int>         *intStack;
  NonRefStack_t<bool>        *boolStack;
  NonRefStack_t<unsigned char> *charStack;
  NonRefStack_t<real>        *realStack;
  NonRefStack_t<Instruction*> *executionStack;
  RefStack_t<char*>          *stringStack;
  RefStack_t<Object_t*>      *objectStack;
  RefStack_t<Pattern_t*>     *patternStack;
  RefStack_t<AttributeInstance_t*> *tmpStack;
  RefStack_t<PartObject_t*>  *currentPOStack;
  Instruction                *dynamicInitInst;               
  PartObject_t               *dynamicInitPO;
private:
  Pattern_t*                  merge(Pattern_t* p1, Pattern_t* p2);
  char                        member(Mixin_t** list1, unsigned int it1,
				     Mixin_t** list2, unsigned int it2);
  int                         signal;
                                //used for storing _one_ pending signal
  int                         state;
                                //used for storing the state of the thread
                                //see enum thread_states for valid states
  Object_t                   *mainObject;
  Scheduler_t                *myScheduler;
  Instruction                *pc;
  Mixin_t* mergeScratch[MERGE_SCRATCH_SIZE];
};

#endif


