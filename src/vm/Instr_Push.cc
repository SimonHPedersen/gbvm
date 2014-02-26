//$Id: Instr_Push.cc,v 1.20 2001/05/28 12:56:25 ilsoe Exp $

//#include "Thread.hpp"
//#include "Instr_Push.hpp"

#include "vm.hpp"

// ----- Push Bool where -----

Instr_PushBool_t::Instr_PushBool_t(char *pc) {
  DB_INDENT;
  id = PUSHBOOL_ID;
  parseRuntimePath(pc, this);
  DB_OUTDENT;
}

void Instr_PushBool_t::printOn(ostream &stream){
  stream << "Instr_PushBool_t: rtp = ";
  printRuntimePath(rtp.first(), stream);
}

// ----- Push Char where -----

Instr_PushChar_t::Instr_PushChar_t(char *pc) {
  DB_INDENT;
  id = PUSHCHAR_ID;
  parseRuntimePath(pc, this);
  DB_OUTDENT;
}

void Instr_PushChar_t::printOn(ostream &stream){
  stream << "Instr_PushChar_t: rtp = ";
  printRuntimePath(rtp.first(), stream);
}

// ----- Push Index -----

Instr_PushIndex_t::Instr_PushIndex_t(char *pc) {
  DB_INDENT;
  id = PUSHINDEX_ID;
  parseRuntimePath(pc, this);
  DB_OUTDENT;
}

void Instr_PushIndex_t::printOn(ostream &stream){
  stream << "Instr_PushIndex_t: rtp = ";
  printRuntimePath(rtp.first(), stream);
}


// ----- Push Integer where -----

Instr_PushInt_t::Instr_PushInt_t(char *pc) {
  DB_INDENT;
  id = PUSHINT_ID;
  parseRuntimePath(pc, this);
  DB_OUTDENT;
}

void Instr_PushInt_t::printOn(ostream &stream){
  stream << "Instr_PushInt_t: rtp = ";
  printRuntimePath(rtp.first(), stream);
}

// ----- Push Object where / Push ObjectReference where -----

Instr_PushObj_t::Instr_PushObj_t(char *pc) {
  DB_INDENT;
  id = PUSHOBJ_ID;
  parseRuntimePath(pc, this);
  DB_OUTDENT;
}

void Instr_PushObj_t::printOn(ostream &stream){
  stream << "Instr_PushObj_t: rtp = ";
  printRuntimePath(rtp.first(), stream);
}


// ----- Push Pattern where / Push PatternReference where -----

Instr_PushPtn_t::Instr_PushPtn_t(char *pc) {
  DB_INDENT;
  id = PUSHPTN_ID;
  parseRuntimePath(pc, this);
  DB_OUTDENT;
}

void Instr_PushPtn_t::printOn(ostream &stream){
  stream << "Instr_PushPtn_t: rtp = ";
  printRuntimePath(rtp.first(), stream);
}

//--- PushPtn "object" ---

void Instr_PushPtnObj_t::printOn(ostream &stream){
  stream << "Instr_PushPtnObj_t";
}

// ----- Push Qual where -----------------

Instr_PushQual_t::Instr_PushQual_t(char *pc) {
  DB_INDENT;
  id = PUSHQUAL_ID;
  parseRuntimePath(pc, this);
  DB_OUTDENT;
}

void Instr_PushQual_t::printOn(ostream &stream){
  stream << "Instr_PushQual_t: rtp = ";
  printRuntimePath(rtp.first(), stream);
}

// ----- Push Real where -----

Instr_PushReal_t::Instr_PushReal_t(char *pc) {
  DB_INDENT;
  id = PUSHREAL_ID;
  parseRuntimePath(pc, this);
  DB_OUTDENT;
}

void Instr_PushReal_t::printOn(ostream &stream){
  stream << "Instr_PushReal_t: rtp = ";
  printRuntimePath(rtp.first(), stream);
}

// ----- Push str-len where -----

Instr_PushStrLen_t::Instr_PushStrLen_t(char *pc) {
  DB_INDENT;
  id = PUSHSTRLEN_ID;
  parseRuntimePath(pc, this);
  DB_OUTDENT;
}

void Instr_PushStrLen_t::printOn(ostream &stream){
  stream << "Instr_PushStrLen_t: rtp = ";
  printRuntimePath(rtp.first(), stream);
}


// ----- Push String where -----

Instr_PushStr_t::Instr_PushStr_t(char *pc) {
  DB_INDENT;
  id = PUSHSTR_ID;
  parseRuntimePath(pc, this);
  DB_OUTDENT;
}

void Instr_PushStr_t::printOn(ostream &stream){
  stream << "Instr_PushStr_t: rtp = ";
  printRuntimePath(rtp.first(), stream);
}


// ----- Push Tmpobj where -----

Instr_PushTmpObj_t::Instr_PushTmpObj_t(char *pc) {
  DB_INDENT;
  id = PUSHTMPOBJ_ID;
  parseRuntimePath(pc, this);
  DB_OUTDENT;
}

void Instr_PushTmpObj_t::printOn(ostream &stream){
  stream << "Instr_PushTmpObj_t: rtp = ";
  printRuntimePath(rtp.first(), stream);
}

// ----- Push Tmpobjref where -----

Instr_PushTmpObjRef_t::Instr_PushTmpObjRef_t(char *pc) {
  DB_INDENT;
  id = PUSHTMPOBJREF_ID;
  parseRuntimePath(pc, this);
  DB_OUTDENT;
}

void Instr_PushTmpObjRef_t::printOn(ostream &stream){
  stream << "Instr_PushTmpObjRef_t: rtp = ";
  printRuntimePath(rtp.first(), stream);
}


//--- Pushi NONE(obj) ---

Instr_PushiNoneObj_t::Instr_PushiNoneObj_t() {
  id = PUSHINONEOBJ_ID;
}

void Instr_PushiNoneObj_t::printOn(ostream &stream) {
  stream << "Instr_PushiNoneObj";
}

//--- Pushi NONE(ptn) ---

Instr_PushiNonePtn_t::Instr_PushiNonePtn_t() {
  id = PUSHINONEPTN_ID;
}

void Instr_PushiNonePtn_t::printOn(ostream &stream) {
  stream << "Instr_PushiNonePtn";
}


//--- Pushi boolean ---

Instr_PushiBoolLit_t::Instr_PushiBoolLit_t(char* pc) {
  id = PUSHIBOOLLIT_ID;
  if(strcmp(pc, "true") == 0) {
    literal = 1;
  } else if (strcmp(pc, "false") == 0) {
    literal = 0;
  } else {
    cerr <<  "Instr_PushiBool_t::Instr_PushiBool_t(): parse error - " 
	 <<  "could not parse bool "
	 << pc << endl;
    exit(1);
  }
}

void Instr_PushiBoolLit_t::printOn(ostream &stream) {
  stream << "Instr_PushiBoolLit with literal: " << literal;
}

//--- Pushi character ---

Instr_PushiCharLit_t::Instr_PushiCharLit_t(char* pc) {
  id = PUSHICHARLIT_ID;
  if(sscanf(pc,"%c",&literal) != 1) {
    cerr <<  "Instr_PushiChar_t::Instr_PushiChar_t(): parse error - " 
	 << "could not parse char "
	 << pc << endl;
    exit(1);
  } 
#ifdef DEBUG
  else {
    cout << "Instr_PushiCharLit_t::Instr_PushiCharLit_t(): parsed: " 
	 << literal << endl;
  }
#endif 
}

void Instr_PushiCharLit_t::printOn(ostream &stream) {
  stream << "Instr_PushiCharLit with literal: " << literal;
}

//--- Pushi integer ---

Instr_PushiIntLit_t::Instr_PushiIntLit_t(char* pc) {
  id = PUSHIINTLIT_ID;
  if(sscanf(pc,"%i",&literal) != 1) {
    cerr <<  "Instr_PushiInt_t::Instr_PushiInt_t(): parse error - " 
	 << "could not parse int "
	 << pc << endl;
    exit(1);
  }
}

Instr_PushiIntLit_t::Instr_PushiIntLit_t(int lit) {
  id = PUSHIINTLIT_ID;
  literal = lit;
}

void Instr_PushiIntLit_t::printOn(ostream &stream) {
  stream << "Instr_PushiIntLit with literal: " << literal;
}

//--- Pushi real ---

Instr_PushiRealLit_t::Instr_PushiRealLit_t(char* pc){
  id = PUSHIREALLIT_ID;
  if(sscanf(pc,"%f",&literal) != 1) {
    cerr <<  "Instr_PushiReal_t::Instr_PushiReal_t(): parse error - " 
	 << "could not parse real "
	 << pc << endl;
    exit(1);
  }
}

void Instr_PushiRealLit_t::printOn(ostream &stream) {
  stream << "Instr_PushiRealLit with literal: " << literal;
}

//--- Pushi string ---

Instr_PushiStrLit_t::Instr_PushiStrLit_t(char* pc){

  id = PUSHISTRLIT_ID;
  unsigned int i = 0; //input string
  unsigned int j = 0; //output/"parsed" string

  char tmpStr[MAX_INPUTSTRING_SIZE*sizeof(char)];
  for( ; pc[i] != '"' ; i++, j++){
    //check for newline chars
    if(pc[i] == '\\'){ 
      if(pc[i + 1] == 'n'){ 
	tmpStr[j] = '\n';
	i = i + 1;
      } else if(pc[i + 1] == '0' && pc[i + 2] == '1' && pc[i + 3] == '2'){
	tmpStr[j] = '\n';
	i = i + 3;
      } 
    } else {
      tmpStr[j] = pc[i];
    }
  }
#ifdef NEW_STYLE_GC
  literal = (char*)allocateRootVM_NT(getStrObjectDescriptor(j + 1));
#else //NEW_STYLE_GC
  literal = (char*)allocateRootVM(getStrObjectDescriptor(j + 1));
#endif //NEW_STYLE_GC
  memcpy(literal, tmpStr, j);
  literal[j] = 0;
}

void Instr_PushiStrLit_t::printOn(ostream &stream) {
  stream << "Instr_PushiStrLit with literal: " << literal;
}
