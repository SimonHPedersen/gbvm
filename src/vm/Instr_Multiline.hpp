// $Id: Instr_Multiline.hpp,v 1.8 2001/02/28 12:30:44 ilsoe Exp $

#ifndef INSTR_MULTILINE_HPP
#define INSTR_MULTILINE_HPP

// instruction classes
class Instr_NamedFor;
class Instr_SimpleFor;
class Instr_SimpleIf;
class Instr_LocatedSimpleIf;
class Selection;                                  // non-instruction class
class Alternative;                                // non-instruction class
class Instr_GeneralIf;
class Instr_While;
class WhenAlternative;                            // non-instruction class
class Instr_When;

#include "Instructions.hpp"


class Instr_NamedFor: public Instruction {
public:
                              Instr_NamedFor(char *_indexname)
                                { id = NAMEDFOR_ID;
                                  indexname=strdup(_indexname); }
  virtual void                printOn(ostream& dest);
  void                        addEvalInstr(Instruction* instr)
                                { eval_instrs.add(instr); }
  Instruction                *getFirstEvalInstr(void)
                                { return eval_instrs.first();}
  void                        addBodyInstr(Instruction* instr)
                                { body_instrs.add(instr); }
  Instruction                *getFirstBodyInstr(void)
                                { return body_instrs.first();}
private:
  char*                       indexname;
  Lst<Instruction>            eval_instrs;
  Lst<Instruction>            body_instrs;
};

#define NAMEDFOR_MAC {                                                   \
  PredefinedMixin_t<int> *mix = new (allocateVM(mixinDesc))              \
    PredefinedMixin_t<int>(currentPOStack->peek());                      \
  PredefinedPartObject_t<int> *po = new(allocateVM(predefIntPODesc))     \
    PredefinedPartObject_t<int>(mix, NULL);                              \
  po->value = 1;                                                         \
  currentPOStack->push(po);                                              \
  tmpStack->addNewFrame();                                               \
  executionStack->push(pc->next());                                      \
  pc = ((Instr_NamedFor*)pc)->getFirstEvalInstr();                       \
};


class Instr_SimpleFor: public Instruction {
public:
                              Instr_SimpleFor()
                                {id = SIMPLEFOR_ID;}
  virtual void                printOn(ostream& dest);
  void                        addEvalInstr(Instruction* instr)
                                { eval_instrs.add(instr); }
  Instruction                *getFirstEvalInstr(void)
                                { return eval_instrs.first();}
  void                        addBodyInstr(Instruction* instr)
                                { body_instrs.add(instr); }
  Instruction                *getFirstBodyInstr(void)
                                { return body_instrs.first();}

private:
  Lst<Instruction>            eval_instrs;
  Lst<Instruction>            body_instrs;
};

#define SIMPLEFOR_MAC {                                                  \
  executionStack->push(pc->next());                                      \
  pc = ((Instr_SimpleFor*)pc)->getFirstEvalInstr();                      \
};


// ----- simple if -----

class Instr_SimpleIf: public Instruction {
public:
                              Instr_SimpleIf()
                                {id = SIMPLEIF_ID;}
  virtual void                printOn(ostream& dest);
  void                        addEvalInstr(Instruction* instr)
                                { eval_instrs.add(instr); }
  void                        addThenInstr(Instruction* instr)
                                { then_instrs.add(instr); }
  void                        addElseInstr(Instruction* instr)
                                { else_instrs.add(instr); }
  Instruction                *getFirstEvalInstr(void)
                                { return eval_instrs.first();}
  Instruction                *getFirstThenInstr(void)
                                { return then_instrs.first();}
  Instruction                *getFirstElseInstr(void)
                                { return else_instrs.first();}
private:
  Lst<Instruction>            eval_instrs;
  Lst<Instruction>            then_instrs;
  Lst<Instruction>            else_instrs;
};

#define SIMPLEIF_MAC {                                                   \
  executionStack->push(pc->next());                                      \
  pc = ((Instr_SimpleIf*)pc)->getFirstEvalInstr();                       \
};

class Instr_LocatedSimpleIf: public Instruction {
public:
  Instr_LocatedSimpleIf() {id = LOCSIMPIF_ID;}
  virtual void printOn(ostream& dest);
  void addEvalInstr(Instruction* instr) { eval_instrs.add(instr); }
  void addThenInstr(Instruction* instr) { then_instrs.add(instr); }
  void addElseInstr(Instruction* instr) { else_instrs.add(instr); }
  char* where_txt; // temporary! steps should be parsed into 'where'
private:
  Lst<RuntimeStep> where;
  Lst<Instruction> eval_instrs;
  Lst<Instruction> then_instrs;
  Lst<Instruction> else_instrs;
};

class Selection: public LstElm<Selection> {
public:
  virtual void                printOn(ostream& dest);
  void                        addInstr(Instruction* instr)
                                { instrs.add(instr); }
  Instruction                *getFirstInstruction(void)
                                {return instrs.first();}
private:
  Lst<Instruction>            instrs;
};

class Alternative: public LstElm<Alternative> {
public:
  virtual void                printOn(ostream& dest);
  void                        addSel(Selection* sel)
                                { sels.add(sel); }
  void                        addInstr(Instruction* instr)
                                { instrs.add(instr); }
  Selection                  *getFirstSelection(void)
                                {return sels.first();}
  Instruction                *getFirstInstruction(void)
                                {return instrs.first();}
private:
  Lst<Selection>              sels;
  Lst<Instruction>            instrs;
};

class Instr_GeneralIf: public Instruction, public WithType {
public:
                              Instr_GeneralIf()
                                {id = GENERALIF_ID;}
  virtual void                printOn(ostream& dest);
  void                        addEvalInstr(Instruction* instr)
                                { eval_instrs.add(instr); }
  void                        addAlt(Alternative* alt)
                                { alts.add(alt); }
  void                        addElseInstr(Instruction* instr)
                                { else_instrs.add(instr); }
  Instruction                *getFirstEvalInstr(void)
                                {return eval_instrs.first();}
  Alternative                *getFirstAlternative(void)
                                {return alts.first();}
  Instruction                *getFirstElseInstr(void)
                                {return else_instrs.first();}
private:
  Lst<Instruction>            eval_instrs;
  Lst<Alternative>            alts;
  Lst<Instruction>            else_instrs;
};

#define GENERALIF_MAC {                                                  \
  executionStack->push(pc->next());                                      \
  pc = ((Instr_GeneralIf*)pc)->getFirstEvalInstr();                      \
};


// ----- While -----

class Instr_While: public Instruction {
public:
                              Instr_While()
                                { id = WHILE_ID; }
  virtual void                printOn(ostream& dest);
  void                        addEvalInstr(Instruction* instr)
                                { eval_instrs.add(instr); }
  void                        addBodyInstr(Instruction* instr)
                                { body_instrs.add(instr); }
  Instruction                *getFirstEvalInstr(void)
                                {return eval_instrs.first();}
  Instruction                *getFirstBodyInstr(void)
                                {return body_instrs.first();}
private:
  Lst<Instruction>            eval_instrs;
  Lst<Instruction>            body_instrs;
};

#define WHILE_MAC {                                                       \
  executionStack->push(pc->next());                                       \
  pc = ((Instr_While*)pc)->getFirstEvalInstr();                           \
};

class WhenAlternative: public LstElm<WhenAlternative> {
public:
  WhenAlternative(bool _exact): exact(_exact) {}
  virtual void printOn(ostream& dest);
  void addEvalInstr(Instruction* instr) { eval_instrs.add(instr); }
  void addBodyInstr(Instruction* instr) { body_instrs.add(instr); }
private:
  bool exact;
  Lst<Instruction> eval_instrs;
  Lst<Instruction> body_instrs;
};

class Instr_When: public Instruction {
public:
  Instr_When() {id = WHEN_ID;}
  virtual void printOn(ostream& dest);
  void addEvalInstr(Instruction* instr) { eval_instrs.add(instr); }
  void addWAlt(WhenAlternative* walt) { walts.add(walt); }
  void addElseInstr(Instruction* instr) { else_instrs.add(instr); }
private:
  Lst<Instruction> eval_instrs;
  Lst<WhenAlternative> walts;
  Lst<Instruction> else_instrs;
};


#endif
