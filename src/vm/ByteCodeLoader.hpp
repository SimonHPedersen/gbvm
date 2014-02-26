//$Id: ByteCodeLoader.hpp,v 1.33 2001/05/07 08:03:57 hempe Exp $

#ifndef BYTECODELOADER_HPP
#define BYTECODELOADER_HPP

class MainPart;
class Attribute;
class AttributeVirtual;
class InstructionParser;

#include <fstream.h>
//#include <iostream.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "Instructions.hpp"
#include "Lists.hpp"
#include "Thread.hpp"
#include "RunTimeSteps.hpp"

void parseRuntimePath(char* pc, SingleInstructionWRTP *inst);
void printRuntimePath(RuntimeStep *firstStep, ostream &stream);

ostream& ind(ostream& dest);

void indent();
void outdent();

// ----------------------------------------------------------------------
// Entities


class Attribute : public LstElm<Attribute> {
public:
                              Attribute(): name(0), index(0) {
				DB_OUT(vm_parse, "Attribute::Attribute():"
                                       "started\n");
			      }
  virtual void                printOn(ostream& dest);
  void                        addInstr(Instruction* instr) { instrs.add(instr); }
  Lst<Instruction>           *getInstructionListPtr(void) { return &instrs; }
  virtual unsigned int        getIndex() { return index; }
protected:
  char                       *name;
  unsigned int                index;
  Lst<Instruction>            instrs;
  friend class InstructionParser;
};

class AttributeVirtual: public Attribute {
public:
                              AttributeVirtual():
				parentAttrName(0),
				parentAttrIndex(0),
				parentMainPartID(0) { }
  void                        addGVInstr(Instruction *instr)
                                { gvInstrs.add(instr); }
  Lst<Instruction>           *getGVInstructionListPtr(void)
                                {return &gvInstrs;}
  int                         checkMatch(char *pAttrName, 
					 int pAttrIndex, 
					 char *pMainPartID);
                                //checks if this virtual attribute 
                                //matches the parameters
  virtual void                printOn(ostream &dest);
  virtual unsigned int        getIndex(){ return index; }
private:
  Lst<Instruction>            gvInstrs;
                                //gather virtual instructions
  char                       *parentAttrName;
  int                         parentAttrIndex;
  char                       *parentMainPartID;
  friend class InstructionParser;
};

class MainPart : public LstElm<MainPart> {
public:
                              MainPart(): id(0) {}
  void                        printOn(ostream& dest);
  void 			      addAttr(Attribute* attr) { attrs.add(attr); }
  void      		      addInstr(Instruction* instr) { doPart.add(instr); }
  void                        addMPAttrInitInstr(Instruction* instr) { 
                                                    attrInitInstrs.add(instr); 
                              }
  void                        setId(char* id);
  char                       *getId() { return id; }
  void                        setObjectDescriptor(ObjectDescriptor_t *objDesc_);
  ObjectDescriptor_t         *getObjectDescriptor(void);
  void                        setNumberOfAttributes(unsigned int nofAttr);
  unsigned int                getNumberOfAttributes(void);
  Lst<Instruction>*           getInstructionListPtr(void);
                                //get the do-part instrtructions
  Lst<Instruction>*           getMPAttrsInstrPtr(void);
                                //get the list of instrs which initializes
                                //all attrs
  Lst<Instruction>*           getAttrInstrPtr(unsigned int index);
                                //get the list of instructions needed for
                                //initializing the attribute at index
  AttributeVirtual           *getVirtAttr(char *parentAttrName,
					  int parentAttrIndex,
					  char *parentMainPartID);
  void                        setAttributeArray(Attribute** array);
  Attribute**                 getAttributeArray();
  Lst<Attribute>              getAttributeList();

private:
  Attribute                  *getAttributePtr(unsigned int index);
                                //get the attribute at index
  char*                      id;
  ObjectDescriptor_t         *objDesc;
                                // object descriptor to be used with this
                                // mainpart
  Lst<Attribute>              attrs;
                                // attributes
  Attribute**                 attributeArray;
                                // attributes placed in array
  unsigned int                nofAttributes;
  Lst<Instruction>            attrInitInstrs;
                                // list of instructions to initialize attributes
  Lst<Instruction>            doPart;
                                // dopart
  friend class InstructionParser;
};

// ----------------------------------------------------------------------
// Inserters for entity printing

ostream& operator<<(ostream& dest, Attribute* attr);
ostream& operator<<(ostream& dest, Attribute& attr);
ostream& operator<<(ostream& dest, MainPart* mp);
ostream& operator<<(ostream& dest, MainPart& mp);

// ----------------------------------------------------------------------
// Parsing

class InstructionParser {
public:
  InstructionParser(char* fileName): dummySrc(fileName), src(dummySrc),
#ifdef DEBUG
                                     linenumber(0),
#endif
				     currentMainPart(0)
  {
    line[linelen]=0; // terminate string in case of overflow
  }
  Lst<MainPart>* parse();

  void executemainparts(Lst<MainPart>* mainpartlist);
private:
  // buffer used to hold next line of source
  static const int linelen=2000;
  // multi-line instruction tokens and lengths
  static const char * const MainPart_tok  = "MainPart(\"";
  static const int          MainPart_tlen = 10;
  static const char * const    Label_tok  = "label(";
  //instruction tokens
  static const char * const ADDMP_TOK = "ADD-mainpart";
  static const int          ADDMP_TLEN = 12;
  static const char * const ADDOPPLUS_TOK = "ADDOP(_+_)_";
  static const int          ADDOPPLUS_TLEN = 11;
  static const char * const ADDOPMINUS_TOK = "ADDOP(_-_)_";
  static const int          ADDOPMINUS_TLEN = 11;
  static const char * const ADDOPOR_TOK = "ADDOP(_or_)_";
  static const int          ADDOPOR_TLEN = 12;
  static const char * const ADDOPXOR_TOK = "ADDOP(_xor_)_";
  static const int          ADDOPXOR_TLEN = 13;
  static const char * const CALL_TOK = "CALL";
  static const int          CALL_TLEN = 4;
  static const char * const CALLREP_TOK = "CALL-rep"; //?EE
  static const int          CALLREP_TLEN = 8;
  static const char * const CHKNONE_TOK = "CHK_NONE";
  static const int          CHKNONE_TLEN = 8;
  static const char * const CHKPTNNONE_TOK = "CHK_PTN_NONE";
  static const int          CHKPTNNONE_TLEN = 12;
  static const char * const CHKQUALOBJ_TOK = "CHK_QUA_OBJ";
  static const int          CHKQUALOBJ_TLEN = 11;
  static const char * const DISCARD_TOK = "DISCARD-";
  static const int          DISCARD_TLEN = 8;
  static const char * const ENDGVIRT_TOK = "END_OF_GATHERVIRT_CODE";
  static const int          ENDGVIRT_TLEN = 22;
  static const char * const ENSURECMP_TOK = "ENSURE-component";
  static const int          ENSURECMP_TLEN = 16;
  static const char * const EXTEND_TOK = "EXTEND-rep"; //?EE
  static const int          EXTEND_TLEN = 10;
  static const char * const FORK_TOK = "FORK";
  static const int          FORK_TLEN = 4;
  static const char * const GATHERVIRT_TOK = "GATHER-virt";
  static const int          GATHERVIRT_TLEN = 11;
  static const char * const GETSIZE_TOK = "GETSIZE-rep"; //?EE
  static const int          GETSIZE_TLEN = 11;
  static const char * const GENERALIF_TOK = "generalIf(";
  static const int          GENERALIF_TLEN = 10;
  static const char * const INNER_TOK = "INNER";
  static const int          INNER_TLEN = 5;
  static const char * const INSTALLPTN_TOK = "INSTALL-ptn";
  static const int          INSTALLPTN_TLEN = 11;
  static const char * const INSTALLQUALPTN_TOK = "INSTALL-qua-ptn";
  static const int          INSTALLQUALPTN_TLEN = 15;
  static const char * const INSTALLOBJ_TOK = "INSTALL-obj";
  static const int          INSTALLOBJ_TLEN = 11;
  static const char * const INSTALLQUALOBJ_TOK = "INSTALL-qua-obj";
  static const int          INSTALLQUALOBJ_TLEN = 15;
  static const char * const INSTALLREP_TOK = "INSTALL-rep";
  static const int          INSTALLREP_TLEN = 11;
  static const char * const KILL_TOK = "KILL";
  static const int          KILL_TLEN = 4;
  static const char * const LOCSIMPIF_TOK = "locatedSimpleIf(";
  static const int          LOCSIMPIF_TLEN = 16;
  static const char * const MERGE_TOK = "MERGE-ptn"; 
  static const int          MERGE_TLEN = 9;
  static const char * const MULOPMUL_TOK = "MULOP(_*_)_";
  static const int          MULOPMUL_TLEN = 11;
  static const char * const MULOPDIV_TOK = "MULOP(_/_)_";
  static const int          MULOPDIV_TLEN = 11;
  static const char * const MULOPAND_TOK = "MULOP(_and_)_";
  static const int          MULOPAND_TLEN = 13;
  static const char * const MULOPDIVINT_TOK = "MULOP(_div_)_";
  static const int          MULOPDIVINT_TLEN = 13;
  static const char * const MULOPMOD_TOK = "MULOP(_mod_)_";
  static const int          MULOPMOD_TLEN = 13;
  static const char * const NEGINT_TOK = "NEG(integer)";
  static const int          NEGINT_TLEN = 15;
  static const char * const NEGREAL_TOK = "NEG(real)";
  static const int          NEGREAL_TLEN = 12;
  static const char * const NEWPTNOBJ_TOK = "NEW,_ptn->obj";
  static const int          NEWPTNOBJ_TLEN = 13;
  static const char * const NEWPTNTMP_TOK = "NEW,_ptn->tmp";
  static const int          NEWPTNTMP_TLEN = 13;
  static const char * const NEWREP_TOK = "NEW-rep";
  static const int          NEWREP_TLEN = 7;
  static const char * const NOT_TOK = "NOT";
  static const int          NOT_TLEN = 3;
  static const char * const NAMEDFOR_TOK = "namedFor(\"";
  static const int          NAMEDFOR_TLEN = 10;
  static const char * const Object_tok = "Object(";
  static const int          Object_tlen = 7;
  static const char * const OSSYSIN_TOK = "osSystem/in";
  static const int          OSSYSIN_TLEN = 11;
  static const char * const OSSYSINOUT_TOK = "osSystem/inout";
  static const int          OSSYSINOUT_TLEN = 14;
  static const char * const Pattern_tok = "Pattern(";
  static const int          Pattern_tlen = 8;
  static const char * const PEEKINXOBJREF_TOK = "PEEK-inx-objref";
  static const int          PEEKINXOBJREF_TLEN = 15;
  static const char * const PEEKINXTMPREF_TOK = "PEEK-inx-tmpref";
  static const int          PEEKINXTMPREF_TLEN = 15;
  static const char * const PEEKOBJREF_TOK = "PEEK-objref"; 
  static const int          PEEKOBJREF_TLEN = 11;
  static const char * const PEEKTMPREF_TOK = "PEEK-tmpref";
  static const int          PEEKTMPREF_TLEN = 11;
  static const char * const POPBOOL_TOK = "POP-boolean";
  static const int          POPBOOL_TLEN = 11;
  static const char * const POPBOOLVAL_TOK = "POP-boolean-value";
  static const int          POPBOOLVAL_TLEN = 17;
  static const char * const POPCHAR_TOK = "POP-char";
  static const int          POPCHAR_TLEN = 8;
  static const char * const POPCHARPUSHINT_TOK = "POP-char,_C-->I,_PUSH-integer";
  static const int          POPCHARPUSHINT_TLEN = 29;
  static const char * const POPCHARPUSHREAL_TOK = "POP-char,_C-->R,_PUSH-real";
  static const int          POPCHARPUSHREAL_TLEN = 26;
  static const char * const POPCHARPUSHSTR_TOK = "POP-char,_C-->S,_PUSH-string";
  static const int          POPCHARPUSHSTR_TLEN = 28;
  static const char * const POPCHARVAL_TOK = "POP-char-value";
  static const int          POPCHARVAL_TLEN = 14;
  static const char * const POPINTPUSHCHARAT_TOK = "POP-int,_PUSH-char-at-inx";
  static const int          POPINTPUSHCHARAT_TLEN = 25;
  static const char * const POPINT_TOK = "POP-integer";
  static const int          POPINT_TLEN = 11;
  static const char * const POPINTPUSHCHAR_TOK = "POP-integer,_I-->C,_PUSH-char";
  static const int          POPINTPUSHCHAR_TLEN = 29;
  static const char * const POPINTPUSHREAL_TOK = "POP-integer,_I-->R,_PUSH-real";
  static const int          POPINTPUSHREAL_TLEN = 29;
  static const char * const POPINTVAL_TOK = "POP-integer-value";
  static const int          POPINTVAL_TLEN = 17;
  static const char * const POPINXOBJREF_TOK = "POP-inx-objref";
  static const int          POPINXOBJREF_TLEN = 14;
  static const char * const POPINXPTNREF_TOK = "POP-inx-ptnref";
  static const int          POPINXPTNREF_TLEN = 14;
  static const char * const POPOBJPUSHPTN_TOK = "POP-obj,_O-->P,_PUSH-ptn";
  static const int          POPOBJPUSHPTN_TLEN = 24;
  static const char * const POPOBJPUSHTMP_TOK = "POP-obj,_PUSH-tmp";
  static const int          POPOBJPUSHTMP_TLEN = 17;
  static const char * const POPOBJREF_TOK = "POP-objref";
  static const int          POPOBJREF_TLEN = 10;
  static const char * const POPPTNSPECOBJ_TOK = "POP-ptn,_SPECIALIZE-obj";
  static const int          POPPTNSPECOBJ_TLEN = 23;
  static const char * const POPPTNREF_TOK = "POP-ptnref";
  static const int          POPPTNREF_TLEN = 10;
  static const char * const POPREAL_TOK = "POP-real";
  static const int          POPREAL_TLEN = 8;
  static const char * const POPREALPUSHINT_TOK = "POP-real,_R-->I,_PUSH-integer";
  static const int          POPREALPUSHINT_TLEN = 29;
  static const char * const POPREALVALUE_TOK = "POP-real-value";
  static const int          POPREALVALUE_TLEN = 14;
  static const char * const POPSTR_TOK = "POP-string";
  static const int          POPSTR_TLEN = 10;
  static const char * const POPSTRREP_TOK = "POP-string_-->_[char]";
  static const int          POPSTRREP_TLEN = 21;
  static const char * const POPSTRVAL_TOK = "POP-string-value";
  static const int          POPSTRVAL_TLEN = 16;
  static const char * const POPSTRPUSHCHAR_TOK = "POP-string1,_S1-->C,_PUSH-char";
  static const int          POPSTRPUSHCHAR_TLEN = 30;
  static const char * const POPSTRPUSHINT_TOK = "POP-string1,_S1-->I,_PUSH-integer";
  static const int          POPSTRPUSHINT_TLEN = 33;
  static const char * const POPSTRPUSHREAL_TOK = "POP-string1,_S1-->R,_PUSH-real";
  static const int          POPSTRPUSHREAL_TLEN = 30;
  static const char * const PUSHBOOL_TOK = "PUSH-boolean";
  static const int          PUSHBOOL_TLEN = 12;
  static const char * const PUSHBOOLVAL_TOK = "PUSH-boolean-value";
  static const int          PUSHBOOLVAL_TLEN = 18;
  static const char * const PUSHCHAR_TOK = "PUSH-char";
  static const int          PUSHCHAR_TLEN = 9;
  static const char * const PUSHCHARVAL_TOK = "PUSH-char-value";
  static const int          PUSHCHARVAL_TLEN = 15;
  static const char * const PUSHINDEX_TOK = "PUSH-index";
  static const int          PUSHINDEX_TLEN = 10;
  static const char * const PUSHINT_TOK = "PUSH-integer";
  static const int          PUSHINT_TLEN = 12;
  static const char * const PUSHINTVAL_TOK = "PUSH-integer-value";
  static const int          PUSHINTVAL_TLEN = 18;
  static const char * const PUSHINXOBJ_TOK = "PUSH-inx-obj";
  static const int          PUSHINXOBJ_TLEN = 12;
  static const char * const PUSHINXOBJREF_TOK = "PUSH-inx-objref";
  static const int          PUSHINXOBJREF_TLEN = 15;
  static const char * const PUSHINXPTNREF_TOK = "PUSH-inx-ptnref";
  static const int          PUSHINXPTNREF_TLEN = 15;
  static const char * const PUSHINXQUAL_TOK = "PUSH-inx-qual";
  static const int          PUSHINXQUAL_TLEN = 13;
  static const char * const PUSHINXTMPOBJ_TOK = "PUSH-inx-tmpobj";
  static const int          PUSHINXTMPOBJ_TLEN = 15;
  static const char * const PUSHINXTMPOBJREF_TOK = "PUSH-inx-tmpobjref";
  static const int          PUSHINXTMPOBJREF_TLEN = 18;
  static const char * const PUSHOBJ_TOK = "PUSH-obj";
  static const int          PUSHOBJ_TLEN = 8;
  static const char * const PUSHOBJREF_TOK = "PUSH-objref";
  static const int          PUSHOBJREF_TLEN = 11;
  static const char * const PUSHPTN_TOK = "PUSH-ptn";
  static const int          PUSHPTN_TLEN = 8;
  static const char * const PUSHPTNOBJ_TOK = "PUSH-ptn_\"object\"";
  static const int          PUSHPTNOBJ_TLEN = 17;
  static const char * const PUSHPTNREF_TOK = "PUSH-ptnref";
  static const int          PUSHPTNREF_TLEN = 11;
  static const char * const PUSHQUAL_TOK = "PUSH-qual";
  static const int          PUSHQUAL_TLEN = 9;
  static const char * const PUSHREAL_TOK = "PUSH-real";
  static const int          PUSHREAL_TLEN = 9;
  static const char * const PUSHREALVAL_TOK = "PUSH-real-value";
  static const int          PUSHREALVAL_TLEN = 15;
  static const char * const PUSHSTRLEN_TOK = "PUSH-str-len";
  static const int          PUSHSTRLEN_TLEN = 12;
  static const char * const PUSHSTRREP_TOK = "PUSH-string_<--_[char]";
  static const int          PUSHSTRREP_TLEN = 22;
  static const char * const PUSHSTR_TOK = "PUSH-string";
  static const int          PUSHSTR_TLEN = 11;
  static const char * const PUSHSTRVAL_TOK = "PUSH-string-value";
  static const int          PUSHSTRVAL_TLEN = 17;
  static const char * const PUSHTMPOBJ_TOK = "PUSH-tmpobj";
  static const int          PUSHTMPOBJ_TLEN = 11;
  static const char * const PUSHTMPOBJREF_TOK = "PUSH-tmpobjref";
  static const int          PUSHTMPOBJREF_TLEN = 14;
  static const char * const PUSHINONEOBJ_TOK = "PUSHI_NONE(obj)";
  static const int          PUSHINONEOBJ_TLEN = 15;
  static const char * const PUSHINONEPTN_TOK = "PUSHI_NONE(ptn)";
  static const int          PUSHINONEPTN_TLEN = 15;
  static const char * const PUSHIBOOLLIT_TOK = "PUSHI-boolean";
  static const int          PUSHIBOOLLIT_TLEN = 13;
  static const char * const PUSHICHARLIT_TOK = "PUSHI-char";
  static const int          PUSHICHARLIT_TLEN = 10;
  static const char * const PUSHIINTLIT_TOK = "PUSHI-integer";
  static const int          PUSHIINTLIT_TLEN = 13;
  static const char * const PUSHIREALLIT_TOK = "PUSHI-real";
  static const int          PUSHIREALLIT_TLEN = 10;
  static const char * const PUSHISTRLIT_TOK = "PUSHI-string";
  static const int          PUSHISTRLIT_TLEN = 12;
  static const char * const RELOPLT_TOK = "RELOP(_<_)_";
  static const int          RELOPLT_TLEN = 11;
  static const char * const RELOPLTEQ_TOK = "RELOP(_<=_)_";
  static const int          RELOPLTEQ_TLEN = 12;
  static const char * const RELOPNEQ_TOK = "RELOP(_<>_)_";
  static const int          RELOPNEQ_TLEN = 12;
  static const char * const RELOPEQ_TOK = "RELOP(_=_)_";
  static const int          RELOPEQ_TLEN = 11;
  static const char * const RELOPGT_TOK = "RELOP(_>_)_";
  static const int          RELOPGT_TLEN = 11;
  static const char * const RELOPGTEQ_TOK ="RELOP(_>=_)_";
  static const int          RELOPGTEQ_TLEN = 12;
  static const char * const RESIZEREP_TOK = "RESIZE-rep";
  static const int          RESIZEREP_TLEN = 10;
  static const char * const RESETFRAME_TOK = "RESETFRAME";
  static const int          RESETFRAME_TLEN = 10;
  static const char * const SEMCOUNT_TOK = "SEM-Count"; 
  static const int          SEMCOUNT_TLEN = 9;
  static const char * const SEMP_TOK = "SEM-P";
  static const int          SEMP_TLEN = 5;
  static const char * const SEMTRYP_TOK = "SEM-TryP"; 
  static const int          SEMTRYP_TLEN = 8;
  static const char * const SEMV_TOK = "SEM-V";
  static const int          SEMV_TLEN = 5;
  static const char * const SUSPEND_TOK = "SUSPEND";
  static const int          SUSPEND_TLEN = 7;
  static const char * const SIMPFOR_TOK = "simpleFor(";
  static const int          SIMPFOR_TLEN = 10;
  static const char * const SIMPIF_TOK = "simpleIf(";
  static const int          SIMPIF_TLEN = 9;
  static const char * const STDIOIN_TOK = "stdio/in";
  static const int          STDIOIN_TLEN = 8;
  static const char * const STDIOOUT_TOK = "stdio/out";
  static const int          STDIOOUT_TLEN = 9;
  static const char * const WHEN_TOK = "when(";
  static const int          WHEN_TLEN = 5;
  static const char * const WHILE_TOK = "while(";
  static const int          WHILE_TLEN = 6;

  // _the_ way to get the next line of the source
  void nextline();
  // support for parse
  Instruction* parseLabel();
  Instruction* parseSimpleFor();
  Instruction* parseNamedFor();
  Instruction* parseSimpleIf();
  Instruction* parseLocatedSimpleIf();
  Selection* parseSelection(unsigned int type);
  Instruction* parseGeneralIf(char *_pc);
  WhenAlternative* parseWhenAlternative(bool exact);
  Instruction* parseWhen();
  Instruction* parseWhile();
  Instruction* parseSingleInstruction(char* pc);
  Instruction* parseInstruction();
  Attribute* parseAttribute();
  MainPart* parseMainPart();
  void stop(char *msg);
  // substance
  ifstream dummySrc;
  ifstream& src;
  char line[linelen+1]; // holds the next line to parse; refill from 'src'
#ifdef DEBUG
  int linenumber;
#endif
  char *currentMainPart;
};

#endif
