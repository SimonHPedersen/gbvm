//$Id: ByteCodeLoader.cc,v 1.82 2001/06/13 18:01:00 ilsoe Exp $

#include "vm.hpp"

static bool empty(char* line);
// ----------------------------------------------------------------------
// Output, debug

#define OUT (cout << "Snapshot: " << line << endl, 0)
#define TELL(it) (cout << "Tell: " << it << endl, 0)

// NB: everything about indentation is an ugly but easy hack,
// introduced to make output more readable

static int indentation = 0;
static const int indent_step = 3;

ostream& ind(ostream& dest) {
  for (int i=0; i<indentation; i++) dest.put(' ');
  return dest;
}

void indent() { indentation += indent_step; }
void outdent() { if (indentation) indentation -= indent_step; }

// ----------------------------------------------------------------------
// Methods

void MainPart::setId(char* id){
  this->id = strdup(id);
}

void MainPart::setObjectDescriptor(ObjectDescriptor_t *objDesc_) {
  objDesc = objDesc_;
}

ObjectDescriptor_t *MainPart::getObjectDescriptor(void) {
  return objDesc;
}

void MainPart::setNumberOfAttributes(unsigned int nofAttr){
  nofAttributes = nofAttr;
}

unsigned int MainPart::getNumberOfAttributes(void) {
  return nofAttributes;
}

Lst<Instruction>* MainPart::getInstructionListPtr(void) {
  return &doPart;
}

Lst<Instruction>* MainPart::getMPAttrsInstrPtr(void) {
  //has the instruction list been created? (there should at leat be an
  //Instr_ReturnMPAttrInit instruction...)
  if(!(attrInitInstrs.first())) {
    Attribute *attr = attrs.first();
    while(attr) {
      attrInitInstrs.add(new Instr_JumpSubNPPO_t
			 (attr->getInstructionListPtr()->first()));
      attr = attr->next();
    }
    attrInitInstrs.add(new Instr_ReturnMPAttrInit_t());
  }
  return &attrInitInstrs;
}

Lst<Instruction>* MainPart::getAttrInstrPtr(unsigned int index) {
  //find attribute at index
  return getAttributePtr(index)->getInstructionListPtr();
}

AttributeVirtual* MainPart::getVirtAttr(char *parentAttrName,
					int parentAttrIndex,
					char *parentMainPartID) {

  DB_INDENT;
  DB_OUT(vm_iexe_spec, "MainPart::getVirtAttr(): searching for virtual"
	 << " attribute: (\"" << parentAttrName << "/" << parentAttrIndex
	 << "\" in \"" << parentMainPartID << "\") in mainpart: " << id
	 << endl);
  //check if this is the mainpart with the introducing binding
  if(!strcmp(parentMainPartID, id)) {
    Attribute *attr = getAttributePtr(parentAttrIndex);
    AttributeVirtual *attrv = dynamic_cast<AttributeVirtual*>(attr);
    if(attrv) {
      DB_OUTDENT;
      DB_OUT(vm_iexe_spec, "MainPart::getVirtAttr(): found introducing virt binding\n");
      return attrv;
    } else {
      cerr << "MainPart::getVirtAttrGVInstr(): error, expected virtual"
	" attribute at index " << parentAttrIndex << " in mainpart "
	   << this << endl;
      exit(1);
    }
  }

  //A further binding. We have to find a virtual attribute with the parameters
  //given to this function...

  Attribute *attr = 0;
  AttributeVirtual *attrv = 0;
  unsigned int num_attrs = getNumberOfAttributes();
  for(unsigned int i = 0; i < num_attrs; i++) {
    attr = getAttributePtr(i);
    attrv = dynamic_cast<AttributeVirtual*>(attr);
    if(attrv) {
      if(attrv->checkMatch(parentAttrName, parentAttrIndex, parentMainPartID)) {
	DB_OUT(vm_iexe_spec, "MainPart::getVirtAttr(): virtual attr matched");
	DB_OUTDENT;
	return attrv;
      }
    }       
  }
  //could not find requested virtual attribute, which is probably ok...
  DB_OUTDENT;
  return 0;
}

Attribute *MainPart::getAttributePtr(unsigned int index) {
  if(index >= nofAttributes){
    cerr << "MainPart::getAttributePtr() this mainpart: " << this
	 << "does not have that many attributes:" << index << endl;
    exit(1);
  }
  return attributeArray[index];
}

void MainPart::setAttributeArray(Attribute** array){
  attributeArray = array;
}


Attribute** MainPart::getAttributeArray(void){
  return attributeArray;
}

Lst<Attribute> MainPart::getAttributeList(void){
  return attrs;
}


void Attribute::printOn(ostream& dest) {
  dest << ind << '"' << name << '/' << index << "\": (" << endl;
  Instruction* instr=instrs.first();
  indent();
  while (instr) {
    dest << ind << instr << endl;
    instr = instr->next();
  }
  outdent();
  dest << ind << ')' << endl;
}

void AttributeVirtual::printOn(ostream& dest) {
  dest << ind << '"' << name << '/' << index << "\": (" << endl;
  indent();
  dest << ind << "virtual attribute: " << parentAttrName << "/" <<
    parentAttrIndex << " in " << parentMainPartID << endl;
  dest << ind << "gather virtual instructions (" << endl;
  Instruction* instr=gvInstrs.first();
  indent();
  while (instr) {
    dest << ind << instr << endl;
    instr = instr->next();
  }
  outdent();
  dest << ind << ") normal instructions (" << endl;
  indent();
  instr = instrs.first();
  while (instr) {
    dest << ind << instr << endl;
    instr = instr->next();
  }
  outdent();
  dest << ind << ')' << endl;  
  outdent();
  dest << ind << ')' << endl;
}

int AttributeVirtual::checkMatch(char *pAttrName, 
				 int pAttrIndex, 
				 char *pMainPartID) {
  //check if this is called on an introducing binding
#ifdef DEBUG
  if(!parentAttrName) {
    cerr << "AttributeVirtual::checkMatch(): invoked on introducing binding.\n";
    exit(1);
  }
#endif
  return (        pAttrIndex  == parentAttrIndex  &&
	  !strcmp(pAttrName,     parentAttrName)  &&
	  !strcmp(pMainPartID,   parentMainPartID)) ? 1 : 0;
}

void MainPart::printOn(ostream& dest) {
  dest << ind << "MainPart(\"" << id << '"' << endl;
  Attribute* attr=attrs.first();
  indent();
  while (attr) {
    dest << ind << attr << endl;
    attr = attr->next();
  }
  outdent();
  dest << ind << "|" << endl;
  Instruction* instr = doPart.first();
  indent();
  while (instr) {
    dest << ind << instr << endl;
    instr = instr->next();
  }
  outdent();
  dest << ind << ")" << endl;
}

// ----------------------------------------------------------------------
// Parsing functions

// Generally, the members are used like this:
//
//   src: this stream delivers the rest of the program being parsed
//   line: this is a one-line lookahead on 'src'
//
// This means that it is an invariant for 'src' and 'line' that
// "line+rsc" is the part of the source code which has not yet been
// parsed into entities.
//
// Every parsing function should look at 'line' and decide what
// to do.  Whenever the current 'line' has been parsed, 'nextline()'
// should be used to get the next line.  The reason why we need 'line'
// at all is that some parsing functions need to look at the next line
// before they know that their piece of syntax has been seen and
// something new has started - i.e. it's just an ordinary parser
// look-ahead.

Lst<MainPart>* InstructionParser::parse() {
  //check if file exists, and is readable...
  if(src.bad()) {
    cerr << "InstructionParser::parse(): unable to load input file,"
      " bailing out\n";
    exit(1);
  }
  Lst<MainPart>* pmps = new Lst<MainPart>;
  MainPart* pmp;
  while (src) {
    if (!(pmp=parseMainPart())) break;
    pmps->add(pmp);
  }
  src.close();
  return pmps;
}

void InstructionParser::nextline() {
  src.getline(line,linelen);
  //skip line if "#....
  if(line[0] == '#') {
    nextline();
  }
#ifdef DEBUG
  linenumber++;
#endif
}

Instruction* InstructionParser::parseLabel() {
  Instr_Label_t* l_instr = new Instr_Label_t;
  Instruction* sub_instr;

  nextline(); // used "label(" line, fetch next line
  // read the body of the labelledImp instruction
  while (src) {
    if (!(sub_instr=parseInstruction())) break;
    l_instr->addInstr(sub_instr);
  }
  nextline(); // skip ')'
  return l_instr;
}

Instruction* InstructionParser::parseSimpleFor() {
  Instr_SimpleFor* nf_instr = new Instr_SimpleFor;
  Instruction* sub_instr;

  nextline(); // used "simpleFor(" line, fetch next line
  // read evaluation
  while (src) {
    if (!(sub_instr=parseInstruction())) break;
    nf_instr->addEvalInstr(sub_instr);
  }
  nextline(); // skip '|'
  // read body
  while (src) {
    if (!(sub_instr=parseInstruction())) break;
    nf_instr->addBodyInstr(sub_instr);
  }
  nextline(); // skip ')'

  //we need to be able to refer to these instructions before they are inserted
  Instruction *cleanupStartInstr = new Instr_Discard_t(INTEGER_ID);

  //To make sure empty instruction lists in the body don't crash us,
  //we insert the first of the extra body instructions now, so the
  //list will not be empty
  nf_instr->addBodyInstr(new Instr_PushiIntLit_t(1));

  //the evaluation gives an integer N on top of the integer stack, 
  //add index i too
  nf_instr->addEvalInstr(new Instr_PushiIntLit_t(0));
  nf_instr->addEvalInstr(new Instr_CopyTop2_t(INTEGER_ID));
  nf_instr->addEvalInstr(new Instr_RelOpEQ_t(INTEGER_ID));
  nf_instr->addEvalInstr(new Instr_JumpTrueNPPO_t(cleanupStartInstr));
  nf_instr->addEvalInstr(new Instr_JumpNPPO_t(nf_instr->getFirstBodyInstr()));

  //cleanup: pop N and i off the integer stack and return
  nf_instr->addEvalInstr(cleanupStartInstr);
  nf_instr->addEvalInstr(new Instr_Discard_t(INTEGER_ID));
  nf_instr->addEvalInstr(new Instr_ReturnNPPO_t());

  //add increment and check at end of body
  nf_instr->addBodyInstr(new Instr_AddOpPlus_t(INTEGER_ID));
  nf_instr->addBodyInstr(new Instr_CopyTop2_t(INTEGER_ID));
  nf_instr->addBodyInstr(new Instr_RelOpEQ_t(INTEGER_ID));
  nf_instr->addBodyInstr(new Instr_JumpTrueNPPO_t(cleanupStartInstr));
  nf_instr->addBodyInstr(new Instr_JumpNPPO_t(nf_instr->getFirstBodyInstr()));

  return nf_instr;
}

Instruction* InstructionParser::parseNamedFor() {
  // first cut off the index name
  char *pc = line, *pc2;
  while (*pc != 'n') pc++;
  pc += 10; pc2 = pc; // pc and pc2 now at beginning of inxname
  while (*pc2 && *pc2 != '"') pc2++;
  *pc2 = 0;

  Instr_NamedFor* nf_instr = new Instr_NamedFor(pc);
  Instruction* sub_instr;

  nextline(); // used "namedFor(" line, fetch next line
  // read evaluation
  while (src) {
    if (!(sub_instr = parseInstruction())) break;
    nf_instr->addEvalInstr(sub_instr);
  }
  nextline(); // skip '|'

  // read body
  while (src) {
    if (!(sub_instr = parseInstruction())) break;
    nf_instr->addBodyInstr(sub_instr);
  }
  nextline(); // skip ')'

  //add control instructions

  //we need to be able to refer to this instruction before it is inserted
  Instruction *cleanupStartInstr = new Instr_Discard_t(INTEGER_ID);
  
  //To make sure empty instruction lists in the body don't crash us,
  //we insert the first of the extra body instructions now, so the
  //list will not be empty
  nf_instr->addBodyInstr(new Instr_CopyTop_t(INTEGER_ID));   //copy N


  nf_instr->addEvalInstr(new Instr_CopyTop_t(INTEGER_ID)); //copy N
  nf_instr->addEvalInstr(new Instr_PushInt_t("{}"));       //get index
  nf_instr->addEvalInstr(new Instr_RelOpLT_t(INTEGER_ID));
  nf_instr->addEvalInstr(new Instr_JumpTrueNPPO_t(cleanupStartInstr));
  nf_instr->addEvalInstr(new Instr_JumpNPPO_t(nf_instr->getFirstBodyInstr()));

  //cleanup: pop N and i off the integer stack and return
  nf_instr->addEvalInstr(cleanupStartInstr);
  nf_instr->addEvalInstr(new Instr_Return_t());

  //add increment and check at end of body
  nf_instr->addBodyInstr(new Instr_PushInt_t("{}"));        //get index
  nf_instr->addBodyInstr(new Instr_PushiIntLit_t(1));
  nf_instr->addBodyInstr(new Instr_AddOpPlus_t(INTEGER_ID)); //updated index

  nf_instr->addBodyInstr(new Instr_CopyTop_t(INTEGER_ID));   //copy index
  nf_instr->addBodyInstr(new Instr_PopInt_t("{}"));

  nf_instr->addBodyInstr(new Instr_RelOpLT_t(INTEGER_ID));
  nf_instr->addBodyInstr(new Instr_JumpTrueNPPO_t(cleanupStartInstr));
  nf_instr->addBodyInstr(new Instr_JumpNPPO_t(nf_instr->getFirstBodyInstr()));

  return nf_instr;
}

Instruction* InstructionParser::parseSimpleIf() {
  Instr_SimpleIf* if_instr = new Instr_SimpleIf;
  Instruction* sub_instr;

  nextline(); // used "simpleIf(" line, fetch next line
  // read evaluation
  while (src) {
    if (!(sub_instr = parseInstruction())) break;
    if_instr->addEvalInstr(sub_instr);
  }

  nextline(); // skip '|then'
  // read then part
  while (src) {
    if (!(sub_instr=parseInstruction())) break;
    if_instr->addThenInstr(sub_instr);
  }
  // add return (no pop po)
  if_instr->addThenInstr(new Instr_ReturnNPPO_t());

  nextline(); // skip '|else'
  // read else part
  while (src) {
    if (!(sub_instr=parseInstruction())) break;
    if_instr->addElseInstr(sub_instr);
  }
  // add return (no pop po)
  if_instr->addElseInstr(new Instr_ReturnNPPO_t());

  // add control structure to eval block
  if_instr->addEvalInstr(new Instr_JumpTrueNPPO_t
			       (if_instr->getFirstThenInstr()));
  if_instr->addEvalInstr(new Instr_JumpNPPO_t
			       (if_instr->getFirstElseInstr()));
  nextline(); // skip ')'
  return if_instr;
}

Instruction* InstructionParser::parseLocatedSimpleIf() {
  Instr_LocatedSimpleIf* lif_instr = new Instr_LocatedSimpleIf;
  Instruction* sub_instr;
  char* pc;

  pc=line;
  while (*pc && *pc!=':') pc++;
  pc++; // skip ' '; pc now at beginning of run-time path
  lif_instr->where_txt=strdup(pc);

  nextline(); // used "locatedSimpleIf(" line, fetch next line
  // read evaluation
  while (src) {
    if (!(sub_instr=parseInstruction())) break;
    lif_instr->addEvalInstr(sub_instr);
  }
  nextline(); // skip '|then'
  // read then part
  while (src) {
    if (!(sub_instr=parseInstruction())) break;
    lif_instr->addThenInstr(sub_instr);
  }
  nextline(); // skip '|else'
  // read else part
  while (src) {
    if (!(sub_instr=parseInstruction())) break;
    lif_instr->addElseInstr(sub_instr);
  }
  nextline(); // skip ')'
  return lif_instr;
}

Selection* InstructionParser::parseSelection(unsigned int type) {
  Selection* sel = new Selection;
  Instruction* sub_instr;
  
  sel->addInstr(new Instr_CopyTop_t(type));

  while (src) {
    if (!(sub_instr=parseInstruction())) break;
    sel->addInstr(sub_instr);
  }
  return sel;
}

Instruction* InstructionParser::parseGeneralIf(char* pc) {
  Instr_GeneralIf* gif_instr = new Instr_GeneralIf;
  Instruction* sub_instr;
  bool done;
  Alternative* alt=0;

  gif_instr->parseType(&pc[GENERALIF_TLEN]);

  char ch;

  nextline(); // used "generalIf(" line, fetch next line
  // read evaluation
  while (src) {
    if (!(sub_instr=parseInstruction())) break;
    gif_instr->addEvalInstr(sub_instr);
  }
  // each alternative is on the form "<selection>+ <thenpart>",
  // <selection> is "|case <instr>*", and <thenpart> is "|then <instr>*"
  done = false;
  do {
    //char ch,*pc=line;
    pc = line;
    // skip white-space in 'line'
    while (*pc && isspace(*pc)) pc++;
    if (!*pc) {
      if (!src) stop("Alternative expected (\"|case\")");
      nextline(); // line was blank, try next line
      continue;
    }
    // pc is now at first non-blank
    if (*pc != '|') stop("Separator in generalIf expected (\"|...\")");
    ch = *(++pc);
    if(!src) stop("Separator in generalIf expected (\"|...\")");
    nextline(); // skip "|case", "|then", or "|else"
    switch (ch) {
    case 'c': // found a selection: "|case"
      DB_OUT(vm_parse_multi, "genIf: parsing case...\n");
      if (!alt) alt = new Alternative;
      alt->addSel(parseSelection(gif_instr->type));
      break;
    case 't': // found a then part: "|then"
      DB_OUT(vm_parse_multi, "genIf: parsing then...\n");
      while (src) {
	if (!(sub_instr=parseInstruction())) break;
	alt->addInstr(sub_instr);
      }
      alt->addInstr(new Instr_ReturnNPPO_t());
      gif_instr->addAlt(alt);
      alt = 0;
      break;
    case 'e': // found an else part
      DB_OUT(vm_parse_multi, "genIf: parsing else...\n");
      done = true;
      break;
    }
  } while (!done);
  // read else part
  while (src) {
    if (!(sub_instr=parseInstruction())) break;
    gif_instr->addElseInstr(sub_instr);
  }
  gif_instr->addElseInstr(new Instr_ReturnNPPO_t());
  nextline(); // skip ')'

  DB_OUT(vm_parse_multi, "genIf: linking parts together...\n");

  //link instruction lists together...
  alt = gif_instr->getFirstAlternative();
  Selection *sel = 0;
  Selection *sel2 = 0;
  Instruction* instr = 0;

  if(alt) {
    sel   = alt->getFirstSelection();
    instr = alt->getFirstInstruction();
    gif_instr->addEvalInstr(new Instr_JumpSubNPPO_t
			    (sel->getFirstInstruction()));
  } else {
    gif_instr->addEvalInstr(new Instr_JumpSubNPPO_t
			    (gif_instr->getFirstElseInstr()));
  }
  gif_instr->addEvalInstr(new Instr_Discard_t(gif_instr->type));
  gif_instr->addEvalInstr(new Instr_ReturnNPPO_t());

  while(sel) {
    sel->addInstr(new Instr_RelOpEQ_t(gif_instr->type));
    sel->addInstr(new Instr_JumpTrueNPPO_t(instr));
    //find out where we have to go if condition is not met...
    if((sel2 = sel->next())) {
      sel->addInstr(new Instr_JumpNPPO_t(sel2->getFirstInstruction()));
    } else {
      if((alt = alt->next())) {
	//goto next alternative
	if((sel2 = alt->getFirstSelection())) {
	  instr = alt->getFirstInstruction();
	  sel->addInstr(new Instr_JumpNPPO_t(sel2->getFirstInstruction()));
	}
      } else {
	//no more alternatives, link to else part
	sel->addInstr(new Instr_JumpNPPO_t(gif_instr->getFirstElseInstr()));
      }
    }
    sel = sel2;
  }

  return gif_instr;
}

Instruction* InstructionParser::parseWhile() {
  Instr_While* wh_instr = new Instr_While;
  Instruction* sub_instr;

  nextline(); // used "while(" line, fetch next line
  // read evaluation
  while (src) {
    if (!(sub_instr=parseInstruction())) break;
    wh_instr->addEvalInstr(sub_instr);
  }
  nextline(); // skip '|'
  // read body
  while (src) {
    if (!(sub_instr=parseInstruction())) break;
    wh_instr->addBodyInstr(sub_instr);
  }
  nextline(); // skip ')'

  //add control structure to while evaluation
  wh_instr->addEvalInstr(new Instr_JumpTrueNPPO_t(wh_instr->
						  getFirstBodyInstr()));
  wh_instr->addEvalInstr(new Instr_ReturnNPPO_t());

  //make body return to a new evaluation
  wh_instr->addBodyInstr(new Instr_JumpNPPO_t(wh_instr->getFirstEvalInstr()));

  return wh_instr;
}

WhenAlternative* InstructionParser::parseWhenAlternative(bool exact) {
  WhenAlternative* walt = new WhenAlternative(exact);
  Instruction* sub_instr;

  // read the code to get hold of the pattern to compare with
  while (src) {
    if (!(sub_instr=parseInstruction())) break;
    walt->addEvalInstr(sub_instr);
  }
  nextline(); // skip "|then"
  // read the body
  while (src) {
    if (!(sub_instr=parseInstruction())) break;
    walt->addBodyInstr(sub_instr);
  }
  return walt;
}

Instruction* InstructionParser::parseWhen() {
  Instr_When* wn_instr = new Instr_When;
  Instruction* sub_instr;
  bool done;

  nextline(); // used "when(" line, fetch next line
  // read evaluation
  while (src) {
    if (!(sub_instr=parseInstruction())) break;
    wn_instr->addEvalInstr(sub_instr);
  }
  // each whenalternative is on the form "<evalpart> <thenpart>",
  // <evalpart> is "|case <instr>*", or "|=case <instr>*"
  // and <thenpart> is "|then <instr>*"
  done = false;
  do {
    char ch,*pc=line;
    // skip white-space in 'line'
    while (*pc && isspace(*pc)) pc++;
    if (!*pc) {

      if (!src) stop("WhenAlternative expected (\"|case\" or \"|=case\")");
      nextline(); // line was blank, try next line
      continue;
    }
    // pc is now at first non-blank
    if (*pc!='|') stop("Separator in when expected (\"|...\")");
    ch=*(++pc);
    if (!src) stop("Separator in when expected (\"|...\")");
    nextline(); // skip "|case", "|=case", "|then", or "|else"
    switch (ch) {
    case 'c': // found a normal whenAlternative: "|case"
      wn_instr->addWAlt(parseWhenAlternative(false));
      break;
    case '=': // found an exact whenAlternative: "|=case"
      wn_instr->addWAlt(parseWhenAlternative(true));
      break;
    case 't': // found a then part: "|then"
      stop("Extra \"|then\" encountered in when instruction");
      break; // well, break looks nice anyway
    case 'e': // found an else part
      done=true;
      break;
    }
  } while (!done);
  // read else part
  while (src) {
    if (!(sub_instr=parseInstruction())) break;
    wn_instr->addElseInstr(sub_instr);
  }
  nextline(); // skip ')'

  return wn_instr;
}

Instruction* InstructionParser::parseSingleInstruction(char* pc) {
  DB_INDENT;
#ifdef DEBUG
  DB_OUT(vm_parse, "parseSingleInstruction() line " << linenumber <<
	 ": " << pc << endl);
#endif
  if(!strncmp(pc, ADDMP_TOK, ADDMP_TLEN)){
    DB_OUTDENT;
    return new Instr_AddMP_t(pc = pc + ADDMP_TLEN + 1); //skipping ` 
  }
  if(!strncmp(pc, ADDOPPLUS_TOK, ADDOPPLUS_TLEN)){ 
    DB_OUTDENT;
    return new Instr_AddOpPlus_t(pc = pc + ADDOPPLUS_TLEN); 
  }
  else if(!strncmp(pc, ADDOPMINUS_TOK, ADDOPMINUS_TLEN)) {
    DB_OUTDENT;
    return new Instr_AddOpMinus_t(pc = pc + ADDOPMINUS_TLEN);
  }
  else if(!strncmp(pc, ADDOPOR_TOK, ADDOPOR_TLEN)) {
    DB_OUTDENT;
    return new Instr_AddOpOr_t(pc = pc + ADDOPOR_TLEN);
  }
  else if(!strncmp(pc, ADDOPXOR_TOK, ADDOPXOR_TLEN)) {
    DB_OUTDENT;
    return new Instr_AddOpXor_t(pc = pc + ADDOPXOR_TLEN);
  }
  else if(!strncmp(pc, CALL_TOK, CALL_TLEN)){ 
    DB_OUTDENT;
    return new Instr_Call_t(pc = pc + CALL_TLEN); 
  } 
  else if(!strncmp(pc, CHKNONE_TOK, CHKNONE_TLEN)){ 
    DB_OUTDENT;
    return new Instr_ChkNone_t(); 
  }
  else if(!strncmp(pc, CHKPTNNONE_TOK, CHKPTNNONE_TLEN)){ 
    DB_OUTDENT;
    return new Instr_ChkPtnNone_t(); 
  }
  else if(!strncmp(pc, CHKQUALOBJ_TOK, CHKQUALOBJ_TLEN)){ 
    DB_OUTDENT;
    return new Instr_ChkQualObj_t(pc = pc + CHKQUALOBJ_TLEN); 
  }
  else if(!strncmp(pc, DISCARD_TOK, DISCARD_TLEN)){ 
    DB_OUTDENT;
    return new Instr_Discard_t(pc = pc + DISCARD_TLEN); 
  }
  else if(!strncmp(pc, ENDGVIRT_TOK, ENDGVIRT_TLEN)){ 
    DB_OUTDENT;
    return 0; //this is not an instruction!
  }
  else if(!strncmp(pc, GATHERVIRT_TOK, GATHERVIRT_TLEN)){
    DB_OUTDENT;
    return new Instr_GatherVirt_t(pc = pc + GATHERVIRT_TLEN);
  }
  else if(!strncmp(pc, MERGE_TOK, MERGE_TLEN)){ 
    DB_OUTDENT;
    return new Instr_Merge_t(); 
  }
  else if(!strncmp(pc, NEGINT_TOK, NEGINT_TLEN)){ 
    DB_OUTDENT;
    return new Instr_NegInt_t(); 
  }
  else if(!strncmp(pc, NEGREAL_TOK, NEGREAL_TLEN)){ 
    DB_OUTDENT;
    return new Instr_NegReal_t(); 
  }
  else if(!strncmp(pc, INNER_TOK, INNER_TLEN)){ 
    DB_OUTDENT;
    return new Instr_Inner_t(pc = pc + INNER_TLEN + 1); 
  }  
  else if(!strncmp(pc, INSTALLPTN_TOK, INSTALLPTN_TLEN)){ 
    DB_OUTDENT;
    return new Instr_InstallPtn_t(pc = pc + INSTALLPTN_TLEN + 1); 
  }
  else if(!strncmp(pc, INSTALLOBJ_TOK, INSTALLOBJ_TLEN)){ 
    DB_OUTDENT;
    return new Instr_InstallObj_t(pc = pc + INSTALLOBJ_TLEN + 1); 
  }
  else if(!strncmp(pc, INSTALLQUALPTN_TOK, INSTALLQUALPTN_TLEN)){ 
    DB_OUTDENT;
    return new Instr_InstallQualPtn_t(pc = pc + INSTALLQUALPTN_TLEN + 1); 
  }
  else if(!strncmp(pc, INSTALLQUALOBJ_TOK, INSTALLQUALOBJ_TLEN)){ 
    DB_OUTDENT;
    return new Instr_InstallQualObj_t(pc = pc + INSTALLQUALOBJ_TLEN + 1); 
  }
  else if(!strncmp(pc, MULOPMUL_TOK, MULOPMUL_TLEN)) {
    DB_OUTDENT;
    return new Instr_MulOpMul_t(pc = pc + MULOPMUL_TLEN);
  }
  else if(!strncmp(pc, MULOPDIV_TOK, MULOPDIV_TLEN)) {
    DB_OUTDENT;
    return new Instr_MulOpDiv_t(pc = pc + MULOPDIV_TLEN);
  }
  else if(!strncmp(pc, MULOPAND_TOK, MULOPAND_TLEN)) {
    DB_OUTDENT;
    return new Instr_MulOpAnd_t(pc = pc + MULOPAND_TLEN);
  }
  else if(!strncmp(pc, MULOPDIVINT_TOK, MULOPDIVINT_TLEN)) {
    DB_OUTDENT;
    return new Instr_MulOpDivInt_t(pc = pc + MULOPDIVINT_TLEN);
  }
  else if(!strncmp(pc, MULOPMOD_TOK, MULOPMOD_TLEN)) {
    DB_OUTDENT;
    return new Instr_MulOpMod_t(pc = pc + MULOPMOD_TLEN);
  }
  else if(!strncmp(pc, NEWPTNOBJ_TOK, NEWPTNOBJ_TLEN)){ 
    DB_OUTDENT;
    return new Instr_NewPtnObj_t(); 
  }
  else if(!strncmp(pc, NEWPTNTMP_TOK, NEWPTNTMP_TLEN)){ 
    DB_OUTDENT;
    return new Instr_NewPtnTmp_t(pc = pc + NEWPTNTMP_TLEN + 1); 
  }
  else if(!strncmp(pc, NOT_TOK, NOT_TLEN)){
    DB_OUTDENT;
    return new Instr_Not_t();
  }
  else if(!strncmp(pc, PEEKOBJREF_TOK, PEEKOBJREF_TLEN)) {
    DB_OUTDENT;
    return new Instr_PeekObjRef_t(pc = pc + PEEKOBJREF_TLEN);
  }
  else if(!strncmp(pc, PEEKTMPREF_TOK, PEEKTMPREF_TLEN)) {
    DB_OUTDENT;
    return new Instr_PeekTmpRef_t(pc = pc + PEEKTMPREF_TLEN);
  }
  else if(!strncmp(pc, POPBOOL_TOK, POPBOOL_TLEN)) {
    DB_OUTDENT;
    return new Instr_PopBool_t(pc = pc + POPBOOL_TLEN);
  }
  else if(!strncmp(pc, POPCHARPUSHINT_TOK, POPCHARPUSHINT_TLEN)) {
    DB_OUTDENT;
    return new Instr_PopCharPushInt_t();
  }
  else if(!strncmp(pc, POPCHARPUSHREAL_TOK, POPCHARPUSHREAL_TLEN)) {
    DB_OUTDENT;
    return new Instr_PopCharPushReal_t();
  }
  else if(!strncmp(pc, POPCHARPUSHSTR_TOK, POPCHARPUSHSTR_TLEN)) {
    DB_OUTDENT;
    return new Instr_PopCharPushStr_t();
  }
  else if(!strncmp(pc, POPCHAR_TOK, POPCHAR_TLEN)) {
    DB_OUTDENT;
    return new Instr_PopChar_t(pc = pc + POPCHAR_TLEN);
  }
  else if(!strncmp(pc, POPINTPUSHCHARAT_TOK, POPINTPUSHCHARAT_TLEN)) {
    DB_OUTDENT;
    return new Instr_PopIntPushCharAt_t(pc = pc + POPINTPUSHCHARAT_TLEN);
  }
  else if(!strncmp(pc, POPINTPUSHCHAR_TOK, POPINTPUSHCHAR_TLEN)) {
    DB_OUTDENT;
    return new Instr_PopIntPushChar_t();
  }
  else if(!strncmp(pc, POPINTPUSHREAL_TOK, POPINTPUSHREAL_TLEN)) {
    DB_OUTDENT;
    return new Instr_PopIntPushReal_t();
  }
  else if(!strncmp(pc, POPINT_TOK, POPINT_TLEN)) {
    DB_OUTDENT;
    return new Instr_PopInt_t(pc = pc + POPINT_TLEN);
  }
  else if(!strncmp(pc, POPOBJPUSHPTN_TOK, POPOBJPUSHPTN_TLEN)) {
    DB_OUTDENT;
    return new Instr_PopObjPushPtn_t();
  }
  else if(!strncmp(pc, POPOBJPUSHTMP_TOK, POPOBJPUSHTMP_TLEN)) {
    DB_OUTDENT;
    return new Instr_PopObjPushTmp_t();
  }
  else if(!strncmp(pc, POPOBJREF_TOK, POPOBJREF_TLEN)) {
    DB_OUTDENT;
    return new Instr_PopObjRef_t(pc = pc + POPOBJREF_TLEN);
  }
  else if(!strncmp(pc, POPPTNREF_TOK, POPPTNREF_TLEN)) {
    DB_OUTDENT;
    return new Instr_PopPtnRef_t(pc = pc + POPPTNREF_TLEN);
  }
  else if(!strncmp(pc, POPREALPUSHINT_TOK, POPREALPUSHINT_TLEN)) {
    DB_OUTDENT;
    return new Instr_PopRealPushInt_t();
  }
  else if(!strncmp(pc, POPREAL_TOK, POPREAL_TLEN)) {
    DB_OUTDENT;
    return new Instr_PopReal_t(pc = pc + POPREAL_TLEN);
  }
  else if(!strncmp(pc, POPSTRPUSHCHAR_TOK, POPSTRPUSHCHAR_TLEN)) {
    DB_OUTDENT;
    return new Instr_PopStrPushChar_t();
  }
  else if(!strncmp(pc, POPSTRPUSHINT_TOK, POPSTRPUSHINT_TLEN)) {
    DB_OUTDENT;
    return new Instr_PopStrPushInt_t();
  }
  else if(!strncmp(pc, POPSTRPUSHREAL_TOK, POPSTRPUSHREAL_TLEN)) {
    DB_OUTDENT;
    return new Instr_PopStrPushReal_t();
  }
  else if(!strncmp(pc, POPSTR_TOK, POPSTR_TLEN)) {
    DB_OUTDENT;
    return new Instr_PopStr_t(pc = pc + POPSTR_TLEN);
  }
  else if(!strncmp(pc, PUSHBOOL_TOK, PUSHBOOL_TLEN)){ 
    DB_OUTDENT;
    return new Instr_PushBool_t(pc = pc + PUSHBOOL_TLEN);
  }
  else if(!strncmp(pc, PUSHCHAR_TOK, PUSHCHAR_TLEN)){ 
    DB_OUTDENT;
    return new Instr_PushChar_t(pc = pc + PUSHCHAR_TLEN);
  }
  else if(!strncmp(pc, PUSHINDEX_TOK, PUSHINDEX_TLEN)){ 
    DB_OUTDENT;
    return new Instr_PushIndex_t(pc = pc + PUSHINDEX_TLEN);
  }
  else if(!strncmp(pc, PUSHINT_TOK, PUSHINT_TLEN)){ 
    DB_OUTDENT;
    return new Instr_PushInt_t(pc = pc + PUSHINT_TLEN);
  }
  else if(!strncmp(pc, PUSHOBJREF_TOK, PUSHOBJREF_TLEN)){ 
    DB_OUTDENT;
    return new Instr_PushObj_t(pc = pc + PUSHOBJREF_TLEN);
  }
  else if(!strncmp(pc, PUSHOBJ_TOK, PUSHOBJ_TLEN)){ 
    DB_OUTDENT;
    return new Instr_PushObj_t(pc = pc + PUSHOBJ_TLEN);
  }
  else if(!strncmp(pc, PUSHPTNREF_TOK, PUSHPTNREF_TLEN)){ 
    DB_OUTDENT;
    return new Instr_PushPtn_t(pc = pc + PUSHPTNREF_TLEN);
  }
  else if(!strncmp(pc, PUSHPTNOBJ_TOK, PUSHPTNOBJ_TLEN)){ 
    DB_OUTDENT;
    return new Instr_PushPtnObj_t();
  }
  else if(!strncmp(pc, PUSHPTN_TOK, PUSHPTN_TLEN)){ 
    DB_OUTDENT;
    return new Instr_PushPtn_t(pc = pc + PUSHPTN_TLEN);
  }
  else if(!strncmp(pc, PUSHQUAL_TOK, PUSHQUAL_TLEN)){ 
    DB_OUTDENT;
    return new Instr_PushQual_t(pc = pc + PUSHQUAL_TLEN);
  }
  else if(!strncmp(pc, PUSHREAL_TOK, PUSHREAL_TLEN)){ 
    DB_OUTDENT;
    return new Instr_PushReal_t(pc = pc + PUSHREAL_TLEN);
  }
  else if(!strncmp(pc, PUSHSTRLEN_TOK, PUSHSTRLEN_TLEN)){ 
    DB_OUTDENT;
    return new Instr_PushStrLen_t(pc = pc + PUSHSTRLEN_TLEN);
  }
  else if(!strncmp(pc, PUSHSTR_TOK, PUSHSTR_TLEN)){ 
    DB_OUTDENT;
    return new Instr_PushStr_t(pc = pc + PUSHSTR_TLEN);
  }
  else if(!strncmp(pc, PUSHTMPOBJREF_TOK, PUSHTMPOBJREF_TLEN)){ 
    DB_OUTDENT;
    return new Instr_PushTmpObjRef_t(pc = pc + PUSHTMPOBJREF_TLEN);
  }
  else if(!strncmp(pc, PUSHTMPOBJ_TOK, PUSHTMPOBJ_TLEN)){ 
    DB_OUTDENT;
    return new Instr_PushTmpObj_t(pc = pc + PUSHTMPOBJ_TLEN);
  }
  else if(!strncmp(pc, PUSHINONEOBJ_TOK, PUSHINONEOBJ_TLEN)) {
    DB_OUTDENT;
    return new Instr_PushiNoneObj_t();
  }
  else if(!strncmp(pc, PUSHINONEPTN_TOK, PUSHINONEPTN_TLEN)) {
    DB_OUTDENT;
    return new Instr_PushiNonePtn_t();
  }
  else if(!strncmp(pc, PUSHIBOOLLIT_TOK, PUSHIBOOLLIT_TLEN)) {
    DB_OUTDENT;
    return new Instr_PushiBoolLit_t(pc = pc + PUSHIBOOLLIT_TLEN + 1);
  }
  else if(!strncmp(pc, PUSHICHARLIT_TOK, PUSHICHARLIT_TLEN)) {
    DB_OUTDENT;
    return new Instr_PushiCharLit_t(pc = pc + PUSHICHARLIT_TLEN + 2);
  }
  else if(!strncmp(pc, PUSHIINTLIT_TOK, PUSHIINTLIT_TLEN)){ 
    DB_OUTDENT;
    return new Instr_PushiIntLit_t(pc = pc + PUSHIINTLIT_TLEN + 1);
  }
  else if(!strncmp(pc, PUSHIREALLIT_TOK, PUSHIREALLIT_TLEN)) {
    DB_OUTDENT;
    return new Instr_PushiRealLit_t(pc = pc + PUSHIREALLIT_TLEN + 1);
  }
  else if(!strncmp(pc, PUSHISTRLIT_TOK, PUSHISTRLIT_TLEN)) {
    DB_OUTDENT;
    return new Instr_PushiStrLit_t(pc = pc + PUSHISTRLIT_TLEN + 2);
  }
  else if(!strncmp(pc, RELOPLT_TOK, RELOPLT_TLEN)){ 
    DB_OUTDENT;
    return new Instr_RelOpLT_t(pc = pc + RELOPLT_TLEN); 
  }
  else if(!strncmp(pc, RELOPLTEQ_TOK, RELOPLTEQ_TLEN)){ 
    DB_OUTDENT;
    return new Instr_RelOpLTEQ_t(pc = pc + RELOPLTEQ_TLEN); 
  }
  else if(!strncmp(pc, RELOPEQ_TOK, RELOPEQ_TLEN)){ 
    DB_OUTDENT;
    return new Instr_RelOpEQ_t(pc = pc + RELOPEQ_TLEN); 
  }
  else if(!strncmp(pc, RELOPNEQ_TOK, RELOPNEQ_TLEN)){ 
    DB_OUTDENT;
    return new Instr_RelOpNEQ_t(pc = pc + RELOPNEQ_TLEN); 
  }
  else if(!strncmp(pc, RELOPGT_TOK, RELOPGT_TLEN)){ 
    DB_OUTDENT;
    return new Instr_RelOpGT_t(pc = pc + RELOPGT_TLEN); 
  }
  else if(!strncmp(pc, RELOPGTEQ_TOK, RELOPGTEQ_TLEN)){ 
    DB_OUTDENT;
    return new Instr_RelOpGTEQ_t(pc = pc + RELOPGTEQ_TLEN); 
  }
  else if(!strncmp(pc, RESETFRAME_TOK, RESETFRAME_TLEN)){
    DB_OUTDENT;
    return new Instr_Resetframe_t();
  }
  else if(!strncmp(pc, STDIOOUT_TOK, STDIOOUT_TLEN)) {
    DB_OUTDENT;
    return new Instr_StdioOut_t();
  }
  else if(!strncmp(pc, STDIOIN_TOK, STDIOIN_TLEN)) {
    DB_OUTDENT;
    return new Instr_StdioIn_t();
  }
  else {
    DB_OUT(vm_bcl,  "InstructionParser::parseSingleInstruction() instruction: " 
	   << pc << " not implemented yet\n");
    DB_OUTDENT;
    return new Instr_Unsupported_t(pc);
  }
}

void parseRuntimePath(char* pc, SingleInstructionWRTP* inst){
  DB_INDENT;
  pc++;  //skip space
  pc++; //skip start brace
  while(*pc!='}'){
    if(*pc=='<'){
      inst->addStep(new OutStep(&pc));
    } else if(*pc=='^'){
      inst->addStep(new UpStep(&pc));
    } else if(*pc=='v'){
      inst->addStep(new DownStep(&pc));
    } else if(*pc=='"'){
      //TO BE REMOVED
      if(!strncmp(pc, "\"length/-1", 10) || !strncmp(pc, "\"at/-1", 6)){
	DB_OUT(vm_bcl_rtp, "parseRuntimePath:" <<
               "length/-1 or at/-1 found skipping rest of rtp\n");
	//skipping last part not needed (will be removed)
	break;
      }
      inst->addStep(new LookupStep(&pc));
    } else if(*pc=='\''){
      inst->addStep(new IndirectLookupStep(&pc));
    } else if(*pc=='t'){
      inst->addStep(new TmpStep(&pc));
    } else {
      break;
    }
    if(*pc==','){
      pc++;
    }
  }
    DB_OUTDENT;
}


void printRuntimePath(RuntimeStep *step, ostream &stream) {
  stream << "{";
  if(step) {
    stream << step;
    step = step->next();
    while(step){
      stream << "," << step;
      step = step->next();
    }
  }
  stream << "}";
}

Instruction* InstructionParser::parseInstruction() {
  char* pc = line;
  
  // skip white-space
  while (*pc && isspace(*pc)) pc++;
  if (!*pc) {
    if (!src) stop("Instruction expected");
    nextline(); // this line was empty, try the next one
    return parseInstruction();
  }

  // line is non-empty, pc is at first non-blank char
  if (*pc == '"' || *pc == '|' || *pc == ')') return 0; // not an instruction
  char* pc2 = pc; // pc and pc2 now at beginning of instr. name
  while (*pc2 && !isspace(*pc2)) pc2++; // pc2 after instr. name
  *pc2 = 0;

  // if composite instruction - read more lines
  if (!strncmp(pc,"label(",6)) return parseLabel();
  else if (!strncmp(pc,SIMPFOR_TOK,SIMPFOR_TLEN)) return parseSimpleFor();
  else if (!strncmp(pc,NAMEDFOR_TOK,NAMEDFOR_TLEN)) return parseNamedFor();
  else if (!strncmp(pc,SIMPIF_TOK,SIMPIF_TLEN)) return parseSimpleIf();
  else if (!strncmp(pc,LOCSIMPIF_TOK,LOCSIMPIF_TLEN)) 
    return parseLocatedSimpleIf();
  else if (!strncmp(pc,GENERALIF_TOK,GENERALIF_TLEN)) return parseGeneralIf(pc);
  else if (!strncmp(pc,WHILE_TOK,WHILE_TLEN)) return parseWhile();
  else if (!strncmp(pc,WHEN_TOK,WHEN_TLEN)) return parseWhen();
  else {
    // a simple, single line instruction
    Instruction* instr = parseSingleInstruction(pc);
    nextline(); // used that line, fetch next line
    return instr;
  }
  return 0; // dummy return, to avoid warning from g++
}

Attribute* InstructionParser::parseAttribute() {
  //Attribute *attr = new Attribute;

  char *attrName;
  int   attrIndex;

  // check for end of attributes
  char* pc = line;
  while (*pc && *pc == ' ') pc++;
  if (!*pc || *pc == '|') {
    if (*pc) nextline(); // used that line, fetch next
    return 0;
  }

  // read name and index
  if (*pc != '"') stop("Attribute name expected");
  pc++; // pc now at 1st char of name
  char* pc2 = pc;
  while (*pc2 && *pc2 != '/') pc2++; // pc2 now after end of name
  *pc2 = 0;
  //attr->name = strdup(pc);
  attrName = strdup(pc);
  pc2++; // pc2 now at beginning of index
  //attr->index = atoi(pc2);
  attrIndex = atoi(pc2);

  //skip to end of number
  while (*pc2 != '"' && *pc2 != 0) pc2++;
  //skip "\": ("
  pc2 = &pc2[4];

  //is this a virtual attribute?
  if(*pc2 == 'v') {
    AttributeVirtual *attrv = new AttributeVirtual();
    attrv->name = attrName;
    attrv->index = attrIndex;
    
    int introducingBinding = 0;
    if(pc2[7] == ' ' && pc2[8] == '"') {
      introducingBinding = 1;

      pc = pc2 = &pc2[9];  // pc, pc2 now at 1st char of name
      while (*pc2 != '/' && *pc2) pc2++; // pc2 now after end of name
      *pc2 = 0;
      attrv->parentAttrName = strdup(pc);

      pc2++; // pc2 now at beginning of index
      attrv->parentAttrIndex = atoi(pc2);

      //skip to end of number
      while (*pc2 != '"' && *pc2) pc2++;

      //goto start of mainpart id
      pc2 = &pc2[6];
      if(*pc2 != '`') stop("mainpart name expected");
      //pc = ++pc2;
      pc = pc2; //don't skip backquote
      //find end of string
      for(pc2 = pc; *pc2 != '"' && *pc2; pc2++);
      *pc2 = 0;
      attrv->parentMainPartID = strdup(pc);
    }

    // read gather virtual instructions
    Instruction* instr;
    nextline();
    while (src) {
      if (!(instr = parseInstruction())) break;
      attrv->addGVInstr(instr);
    }
    //continue after "END_OF_GATHERVIRT_CODE".(parseInstruction(END_OF_GATHER..)
    //returns 0 and the line is advanced, so we do not have to do anything.

    // read initialization instructions
    while (src) {
      if (!(instr = parseInstruction())) break;
      attrv->addInstr(instr);
    }

    nextline(); // skip ')'

    //insert interesting instructions here...

    if(introducingBinding) {
      attrv->addGVInstr(new Instr_ContGatherVirt_t(attrv->parentAttrName,
						   attrv->parentAttrIndex,
						   attrv->parentMainPartID));
    } else {
      attrv->addGVInstr(new Instr_ContGatherVirt_t(attrv->name, 
						   attrv->index, 
						   currentMainPart));
    }

    attrv->addInstr(new Instr_ReturnNPPO_t());

    return attrv;
  } else {
    Attribute *attr = new Attribute;

    attr->name = attrName;
    attr->index = attrIndex;

    // read initialization instructions
    Instruction* instr;
    nextline();
    while (src) {
      if (!(instr = parseInstruction())) break;
      attr->addInstr(instr);
    }
    nextline(); // skip ')'
    //add return
    attr->addInstr(new Instr_ReturnNPPO_t());
    return attr;
  }
}

MainPart* InstructionParser::parseMainPart() {
  MainPart* pmp = new MainPart; // result

  // skip empty lines
  while (src) {
    nextline();
    if (empty(line)) { if (src.eof()) return 0; }
    else break;
  }
  if(!strncmp(line, Pattern_tok, Pattern_tlen)) {
    //we have reached end of the interesting part of the file
    delete pmp;
    return 0;
  }

  if(!strncmp(line, Object_tok, Object_tlen)) {
    //we have reached end of the interesting part of the file
    delete pmp;
    return 0;
  }

  // read beginning of MainPart
  if (strncmp(line, MainPart_tok, MainPart_tlen)) {
    stop("MainPart expected");
  }

  // extract the MainPart ID string
  char* pc=line+MainPart_tlen; // pc now at beg. of ID
  char* pc2=pc;
  while (*pc2 && *pc2!='"') pc2++; // pc2 now at '"' after end of ID
  *pc2=0; // terminate ID string just before '"'
  currentMainPart = pmp->id = strdup(pc);

  nextline(); // skip the "MainPart(" line
  // read attributes; invariant: see parseAttribute
  Attribute* attr;
  while (src) {
    if (!(attr=parseAttribute())) break;
    pmp->addAttr(attr);
  }

  pmp->setNumberOfAttributes(pmp->attrs.getCount());
  //create object descriptor
  unsigned int numAttributes = pmp->getNumberOfAttributes();
  ObjectDescriptor_t *objDes = new ObjectDescriptor_t(sizeof(UserPartObject_t) +
						      (numAttributes-1)*
						      sizeof(void*), 0);

  free(new(malloc(sizeof(UserPartObject_t))) UserPartObject_t(objDes));
  //objDes->print();
  pmp->setObjectDescriptor(objDes); 

  //make Attribute array (sort attributes)
  pmp->setAttributeArray((Attribute**)malloc(sizeof(Attribute*)*numAttributes));

  Attribute** attrArray = pmp->getAttributeArray();

  //sort attributes

  for(unsigned int i = 0; i < numAttributes ; i++){
    for(Attribute* attr = pmp->getAttributeList().first(); 
	attr != NULL ; 
	attr = attr->next()){
      if(attr->getIndex() == i){
	attrArray[i] = attr;
	break;
      }
    }
  }

  // read dopart
  Instruction* instr;
  while (src) {
    if (!(instr=parseInstruction())) {
      DB_OUT(vm_parse, "InstructionParser::parseMainPart():" <<
	     " no more instr in mp\n");
      break;
    }
    pmp->addInstr(instr);
  }
  // add return instruction at the end
  pmp->addInstr(new Instr_Return_t());

  return pmp;
}

void InstructionParser::stop(char* msg) {
  cerr << "ERROR: " << msg;
  cerr << " (looking at \"" << line;
  cerr << "\", line "
#ifdef DEBUG
       << linenumber
#else
       << " (no line number available in non-debug build) "
#endif
       << ')' << endl;
  exit(1);
}

// ----------------------------------------------------------------------
// Aux functions

bool empty(char* line) {
  while (*line) {
    if (!isspace(*line)) return false;
    line++;
  }
  return true;
}

// ----------------------------------------------------------------------
// Printing of run-time entities

ostream& operator<<(ostream& dest, Attribute* attr) {
  attr->printOn(dest);
  return dest;
}

ostream& operator<<(ostream& dest, Attribute& attr) {
  return dest << &attr;
}

ostream& operator<<(ostream& dest, MainPart* mp) {
  mp->printOn(dest);
  return dest;
}

ostream& operator<<(ostream& dest, MainPart& mp) {
  return dest << &mp;
}
