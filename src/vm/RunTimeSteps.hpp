//$Id: RunTimeSteps.hpp,v 1.6 2001/05/10 12:36:59 hempe Exp $

#ifndef RUNTIMESTEPS_HPP
#define RUNTIMESTEPS_HPP

class RuntimeStep;
class UpStep;
class DownStep;
class OutStep;
class LookupStep;
class IndirectLookupStep;
class TmpStep;

#include <iostream.h>
#include "Lists.hpp"
#include "Thread.hpp"
#include "Instructions.hpp"

void parseRuntimePath(char* pc, SingleInstructionWRTP *inst);

PartObject_t *evaluateRuntimePath_PO(SingleInstructionWRTP *inst, 
				     PartObject_t *po,
				     Thread_t *thr);

Object_t *evaluateRuntimePath_Obj(SingleInstructionWRTP *inst, 
				  PartObject_t *po,
				  Thread_t *thr);

Pattern_t *evaluateRuntimePath_Ptn(SingleInstructionWRTP *inst, 
				   PartObject_t *po,
				   Thread_t *thr);

ObjectReference_t *evaluateRuntimePath_ObjRef(SingleInstructionWRTP *inst, 
				  PartObject_t *po,
				  Thread_t *thr);

PatternReference_t *evaluateRuntimePath_PtnRef(SingleInstructionWRTP *inst, 
				   PartObject_t *po,
				   Thread_t *thr);


ostream& operator<<(ostream& dest, RuntimeStep* instr);
ostream& operator<<(ostream& dest, RuntimeStep& instr);


// -------------------- class definitions --------------------

class RuntimeStep : public LstElm<RuntimeStep> {
public:
  virtual void                printOn(ostream &stream)=0;
  virtual PartObject_t       *getPartObject(PartObject_t* cpo, Thread_t* thr)=0;
  virtual Pattern_t          *getPattern(PartObject_t* cpo, Thread_t* thr)=0;
  virtual Object_t           *getObject(PartObject_t* cpo, Thread_t* thr)=0;
  virtual PatternReference_t *getPatternReference(PartObject_t* cpo, 
						  Thread_t* thr)=0;
  virtual ObjectReference_t  *getObjectReference(PartObject_t* cpo, 
						 Thread_t* thr)=0;
};

class UpStep : public RuntimeStep {
public:
                              UpStep(char** pc);
  virtual void                printOn(ostream &stream);
  char*                       getMainPartId();
  virtual PartObject_t       *getPartObject(PartObject_t* cpo, Thread_t* thr);
  virtual Pattern_t          *getPattern(PartObject_t* cpo, Thread_t* thr);
  virtual Object_t           *getObject(PartObject_t* cpo, Thread_t* thr);
  virtual PatternReference_t *getPatternReference(PartObject_t* cpo, 
						  Thread_t* thr);
  virtual ObjectReference_t  *getObjectReference(PartObject_t* cpo, 
						 Thread_t* thr);
private:
  char*                       mainPartId;
}; //mainpart id (string)

class DownStep : public RuntimeStep {
public:
                              DownStep(char** pc);
  virtual void                printOn(ostream &stream);
  char*                       getMainPartId();
  virtual PartObject_t       *getPartObject(PartObject_t* cpo, Thread_t* thr);
  virtual Pattern_t          *getPattern(PartObject_t* cpo, Thread_t* thr);
  virtual Object_t           *getObject(PartObject_t* cpo, Thread_t* thr);
  virtual PatternReference_t *getPatternReference(PartObject_t* cpo, 
						  Thread_t* thr);
  virtual ObjectReference_t  *getObjectReference(PartObject_t* cpo, 
						 Thread_t* thr);
private:
  char*                       mainPartId;
}; //mainpart id (string)

class OutStep : public RuntimeStep {
public:
                              OutStep(char** pc);
  virtual void                printOn(ostream &stream);
  unsigned int                getSteps();
  virtual PartObject_t       *getPartObject(PartObject_t* cpo, Thread_t* thr);
  virtual Pattern_t          *getPattern(PartObject_t* cpo, Thread_t* thr);
  virtual Object_t           *getObject(PartObject_t* cpo, Thread_t* thr);
  virtual PatternReference_t *getPatternReference(PartObject_t* cpo, 
						  Thread_t* thr);
  virtual ObjectReference_t  *getObjectReference(PartObject_t* cpo, 
						 Thread_t* thr);
private:
  unsigned int                steps;
}; //int

class LookupStep : public RuntimeStep {
public:
                              LookupStep(char** pc);
  virtual void                printOn(ostream &stream);
  char                       *getName();
  unsigned int                getIndex();
  virtual PartObject_t       *getPartObject(PartObject_t* cpo, Thread_t* thr);
  virtual Pattern_t          *getPattern(PartObject_t* cpo, Thread_t* thr);
  virtual Object_t           *getObject(PartObject_t* cpo, Thread_t* thr);
  virtual PatternReference_t *getPatternReference(PartObject_t* cpo, 
						  Thread_t* thr);
  virtual ObjectReference_t  *getObjectReference(PartObject_t* cpo, 
						 Thread_t* thr);
private:
  char*                       name;
  unsigned int                index;
}; //index (name for debug)

class IndirectLookupStep : public RuntimeStep {
public:
                              IndirectLookupStep(char** pc);
  virtual void                printOn(ostream &stream);
  char*                       getName();
  unsigned int                getIndex();
  virtual PartObject_t       *getPartObject(PartObject_t* cpo, Thread_t* thr);
  virtual Pattern_t          *getPattern(PartObject_t* cpo, Thread_t* thr);
  virtual Object_t           *getObject(PartObject_t* cpo, Thread_t* thr);
  virtual PatternReference_t *getPatternReference(PartObject_t* cpo, 
						  Thread_t* thr);
  virtual ObjectReference_t  *getObjectReference(PartObject_t* cpo, 
						 Thread_t* thr);
private:
  char*                       name;
  unsigned int                index;
}; //index (name for debug)

class TmpStep : public RuntimeStep {
public:
                              TmpStep(char** pc);
  virtual void                printOn(ostream &stream);
  unsigned int                getIndex();
  virtual PartObject_t       *getPartObject(PartObject_t* cpo, Thread_t* thr);
  virtual Pattern_t          *getPattern(PartObject_t* cpo, Thread_t* thr);
  virtual Object_t           *getObject(PartObject_t* cpo, Thread_t* thr);
  virtual PatternReference_t *getPatternReference(PartObject_t* cpo, 
						  Thread_t* thr);
  virtual ObjectReference_t  *getObjectReference(PartObject_t* cpo, 
						 Thread_t* thr);
private:
  unsigned int                index;
};


#endif
