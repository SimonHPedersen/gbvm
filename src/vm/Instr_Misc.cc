// $Id: Instr_Misc.cc,v 1.19 2001/05/23 08:43:45 ilsoe Exp $

#include "vm.hpp"

void Instr_Label_t::printOn(ostream& dest)
{
  dest << ind << "label(" << endl;
  Instruction* current=instrs.first();
  indent();
  while (current) { current->printOn(dest); current=current->next(); }
  outdent();
  dest << ind << ')' << endl;
}

//--- Add mainpart ---

Instr_AddMP_t::Instr_AddMP_t(char* pc){
  id = ADDMP_ID;
  //parse mainpart
  unsigned int space = 0;
  for( ; pc[space] != ' ' ; space++);
  char *str = (char*)malloc(space+1);
  memcpy(str, pc, space);
  str[space] = 0;
  mainPartId = str;
  parseRuntimePath(&pc[space + 8], this);
}

void Instr_AddMP_t::printOn(ostream &stream){
  stream << "Instr_AddMP_t with id: " << mainPartId 
	 << " and with run-time path: ";
  printRuntimePath(rtp.first(), stream);
}


//--- Call ---

void Instr_Call_t::printOn(ostream &stream){
  stream << "Instr_Call_t with run-time path: ";
  printRuntimePath(rtp.first(), stream);
}

Instr_Call_t::Instr_Call_t(char* pc){
  DB_INDENT;
  id = CALL_ID;
  DB_OUT(vm_bcl_inst,  "Instr_Call_t::Instr_Call_t pc: " << *pc << endl);
  parseRuntimePath(pc, this);
  DB_OUTDENT;
}

//--- chk none ---

void Instr_ChkNone_t::printOn(ostream &stream){
  stream << "Instr_ChkNone_t";
}

Instr_ChkNone_t::Instr_ChkNone_t(){
  id = CHKNONE_ID;
}

//--- chk ptn none ---

void Instr_ChkPtnNone_t::printOn(ostream &stream){
  stream << "Instr_ChkPtnNone_t";
}

Instr_ChkPtnNone_t::Instr_ChkPtnNone_t(){
  id = CHKPTNNONE_ID;
}

//--- chk ptn none ---

void Instr_ChkQualObj_t::printOn(ostream &stream){
  stream << "Instr_ChkQualObj_t";
}

Instr_ChkQualObj_t::Instr_ChkQualObj_t(char* pc){
  id = CHKQUALOBJ_ID;
  parseRuntimePath(pc, this);
}

//--- discard ---

void Instr_Discard_t::printOn(ostream &stream){
  stream << "Instr_Discard_t: with type: " << type;
}

Instr_Discard_t::Instr_Discard_t(char* pc){
  id = DISCARD_ID;
  parseType(pc);
}

//--- gather virtual ---

Instr_GatherVirt_t::Instr_GatherVirt_t(char *pc) {
  DB_INDENT;
  id = GATHERVIRT_ID;

  char *pc2 = pc = &pc[2];   // pc, pc2 now at 1st char of name

  while (*pc2 && *pc2 != '/') pc2++; // pc2 now after end of name
  *pc2 = 0;
  parentAttrName = strdup(pc);
  
  pc2++; // pc2 now at beginning of index
  parentAttrIndex = atoi(pc2);
  
  //skip to end of number
  while (*pc2 != '"' && *pc2 != 0) pc2++;
  
  //goto start of mainpart id
  pc2 = &pc2[6];
  if(*pc2 != '`') {
    cerr << "mainpart name expected\n";
  }
  //  pc = ++pc2;
  pc = pc2; //don't skip backquote

  //find end of string
  for(pc2 = pc; *pc2 && *pc2 != '"'; pc2++);
  *pc2 = 0;

  parentMainPartID = strdup(pc);

  DB_OUT(vm_parse_misc,  "Instr_GatherVirt_t: nameAndIndex    : " << 
	 parentAttrName << "/" << parentAttrIndex << 
	 "\n                        parentMainPartId: "
	 << parentMainPartID << endl);
  DB_OUTDENT;
}

void Instr_GatherVirt_t::printOn(ostream &stream){
  stream << "Instr_GatherVirt_t";
}

//--- inner ---

void Instr_Inner_t::printOn(ostream &stream){
  stream << "Instr_Inner_t with \"out-index\": " << index;
}

Instr_Inner_t::Instr_Inner_t(char* pc){
  if(sscanf(pc,"%i",&index) != 1) {
    cerr <<  "Instr_NewPtnTmp_t::Instr_PushiNewPtnTmp_t(): parse error - " 
	 <<  "could not parse index " << pc << endl;
    exit(1);
  }  
  id = INNER_ID;
}

//--- merge ---

void Instr_Merge_t::printOn(ostream &stream){
  stream << "Instr_Merge_t";
}

Instr_Merge_t::Instr_Merge_t(){
  id = MERGE_ID;
}

//--- neg int ---

void Instr_NegInt_t::printOn(ostream &stream){
  stream << "Instr_NegInt_t";
}

Instr_NegInt_t::Instr_NegInt_t(){
  id = NEGINT_ID;
}

//--- neg real ---

void Instr_NegReal_t::printOn(ostream &stream){
  stream << "Instr_NegReal_t";
}

Instr_NegReal_t::Instr_NegReal_t(){
  id = NEGREAL_ID;
}

//--- NewPtnObj ---

Instr_NewPtnObj_t::Instr_NewPtnObj_t(){
  id = NEWPTNOBJ_ID;
}

void Instr_NewPtnObj_t::printOn(ostream &stream){
  stream << "Instr_NewPtnObj_t";
}

//--- NewPtnTmp ---

Instr_NewPtnTmp_t::Instr_NewPtnTmp_t(char* pc){
  id = NEWPTNTMP_ID;
  if(sscanf(pc,"%i",&index) != 1) {
    cerr <<  "Instr_NewPtnTmp_t::Instr_PushiNewPtnTmp_t(): parse error - " 
	 <<  "could not parse index " << pc << endl;
    exit(1);
  }

}

void Instr_NewPtnTmp_t::printOn(ostream &stream){
  stream << "Instr_NewPtnTmp_t";
}


//--- not ---

void Instr_Not_t::printOn(ostream &stream){
  stream << "Instr_Not_t";
}

Instr_Not_t::Instr_Not_t(){
  id = NOT_ID;
}

//--- resetframe ---

void Instr_Resetframe_t::printOn(ostream &stream){
  stream << "Instr_Resetframe_t";
}

Instr_Resetframe_t::Instr_Resetframe_t(){
  id = RESETFRAME_ID;
}

//--- stdio out ---

Instr_StdioOut_t::Instr_StdioOut_t() {
  id = STDIOOUT_ID;
}

void Instr_StdioOut_t::printOn(ostream &stream) {
  stream << "Instr_StdioOut_t";
}

//--- stdio in ---

Instr_StdioIn_t::Instr_StdioIn_t() {
  id = STDIOIN_ID;
}

void Instr_StdioIn_t::printOn(ostream &stream) {
  stream << "Instr_StdioIn_t";
}

