//$Id: Instr_Pop.cc,v 1.9 2001/05/10 12:36:54 hempe Exp $

#include "vm.hpp"

// ----- Pop Bool -----

Instr_PopBool_t::Instr_PopBool_t(char *pc) {
  DB_INDENT;
  id = POPBOOL_ID;
  parseRuntimePath(pc, this);
  DB_OUTDENT;
}

void Instr_PopBool_t::printOn(ostream &stream){
  stream << "Instr_PopBool_t: rtp = ";
  printRuntimePath(rtp.first(), stream);
}

// ----- Pop Char -----

Instr_PopChar_t::Instr_PopChar_t(char *pc) {
  DB_INDENT;
  id = POPCHAR_ID;
  parseRuntimePath(pc, this);
  DB_OUTDENT;
}

void Instr_PopChar_t::printOn(ostream &stream){
  stream << "Instr_PopChar_t: rtp = ";
  printRuntimePath(rtp.first(), stream);
}

// ----- Pop Char Push Int -----

Instr_PopCharPushInt_t::Instr_PopCharPushInt_t() {
  DB_INDENT;
  id = POPCHARPUSHINT_ID;
  DB_OUTDENT;
}

void Instr_PopCharPushInt_t::printOn(ostream &stream){
  stream << "Instr_PopCharPushInt_t";
}

// ----- Pop CharPushReal -----

Instr_PopCharPushReal_t::Instr_PopCharPushReal_t() {
  DB_INDENT;
  id = POPCHARPUSHREAL_ID;
  DB_OUTDENT;
}

void Instr_PopCharPushReal_t::printOn(ostream &stream){
  stream << "Instr_PopCharPushReal_t";
}

// ----- Pop Char Push String -----

Instr_PopCharPushStr_t::Instr_PopCharPushStr_t() {
  DB_INDENT;
  id = POPCHARPUSHSTR_ID;
  DB_OUTDENT;
}

void Instr_PopCharPushStr_t::printOn(ostream &stream){
  stream << "Instr_PopCharPushStr_t";
}

// ----- Pop Int -----

Instr_PopInt_t::Instr_PopInt_t(char *pc) {
  DB_INDENT;
  id = POPINT_ID;
  parseRuntimePath(pc, this);
  DB_OUTDENT;
}

void Instr_PopInt_t::printOn(ostream &stream){
  stream << "Instr_PopInt_t: rtp = ";
  printRuntimePath(rtp.first(), stream);
}

// ----- Pop Int Push Char -----

Instr_PopIntPushCharAt_t::Instr_PopIntPushCharAt_t(char* pc) {
  DB_INDENT;
  id = POPINTPUSHCHARAT_ID;
  parseRuntimePath(pc, this);
  DB_OUTDENT;
}

void Instr_PopIntPushCharAt_t::printOn(ostream &stream){
  stream << "Instr_PopIntPushCharAt_t: rtp = ";
  printRuntimePath(rtp.first(), stream);
}

// ----- Pop Int Push Char -----

Instr_PopIntPushChar_t::Instr_PopIntPushChar_t() {
  DB_INDENT;
  id = POPINTPUSHCHAR_ID;
  DB_OUTDENT;
}

void Instr_PopIntPushChar_t::printOn(ostream &stream){
  stream << "Instr_PopIntPushChar_t";
}

// ----- Pop Int Push Real -----

Instr_PopIntPushReal_t::Instr_PopIntPushReal_t() {
  DB_INDENT;
  id = POPINTPUSHREAL_ID;
  DB_OUTDENT;
}

void Instr_PopIntPushReal_t::printOn(ostream &stream){
  stream << "Instr_PopIntPushReal_t";
}

// ----- Pop Obj Push Ptn -----

Instr_PopObjPushPtn_t::Instr_PopObjPushPtn_t() {
  DB_INDENT;
  id = POPOBJPUSHPTN_ID;
  DB_OUTDENT;
}

void Instr_PopObjPushPtn_t::printOn(ostream &stream){
  stream << "Instr_PopObjPushPtn_t";
}

// ----- Pop Obj Push Tmp -----

Instr_PopObjPushTmp_t::Instr_PopObjPushTmp_t() {
  DB_INDENT;
  id = POPOBJPUSHTMP_ID;
  DB_OUTDENT;
}

void Instr_PopObjPushTmp_t::printOn(ostream &stream){
  stream << "Instr_PopObjPushTmp_t";
}

// ----- Pop Objref -----

Instr_PopObjRef_t::Instr_PopObjRef_t(char *pc) {
  DB_INDENT;
  id = POPOBJREF_ID;
  parseRuntimePath(pc, this);
  DB_OUTDENT;
}

void Instr_PopObjRef_t::printOn(ostream &stream){
  stream << "Instr_PopObjref_t: rtp = ";
  printRuntimePath(rtp.first(), stream);
}

// ----- Pop Ptnref -----

Instr_PopPtnRef_t::Instr_PopPtnRef_t(char *pc) {
  DB_INDENT;
  id = POPPTNREF_ID;
  parseRuntimePath(pc, this);
  DB_OUTDENT;
}

void Instr_PopPtnRef_t::printOn(ostream &stream){
  stream << "Instr_PopPtnref_t: rtp = ";
  printRuntimePath(rtp.first(), stream);
}

// ----- Pop Real -----

Instr_PopReal_t::Instr_PopReal_t(char *pc) {
  DB_INDENT;
  id = POPREAL_ID;
  parseRuntimePath(pc, this);
  DB_OUTDENT;
}

void Instr_PopReal_t::printOn(ostream &stream){
  stream << "Instr_PopReal_t: rtp = ";
  printRuntimePath(rtp.first(), stream);
}

// ----- Pop Real Push Int -----

Instr_PopRealPushInt_t::Instr_PopRealPushInt_t() {
  DB_INDENT;
  id = POPREALPUSHINT_ID;
  DB_OUTDENT;
}

void Instr_PopRealPushInt_t::printOn(ostream &stream){
  stream << "Instr_PopRealPushInt_t";
}

// ----- Pop Str -----

Instr_PopStr_t::Instr_PopStr_t(char *pc) {
  DB_INDENT;
  id = POPSTR_ID;
  parseRuntimePath(pc, this);
  DB_OUTDENT;
}

void Instr_PopStr_t::printOn(ostream &stream){
  stream << "Instr_PopStr_t: rtp = ";
  printRuntimePath(rtp.first(), stream);
}


// ----- Pop Str Push Char -----

Instr_PopStrPushChar_t::Instr_PopStrPushChar_t() {
  DB_INDENT;
  id = POPSTRPUSHCHAR_ID;
  DB_OUTDENT;
}

void Instr_PopStrPushChar_t::printOn(ostream &stream){
  stream << "Instr_PopStrPushChar_t";
}

// ----- Pop Str Push Int -----

Instr_PopStrPushInt_t::Instr_PopStrPushInt_t() {
  DB_INDENT;
  id = POPSTRPUSHINT_ID;
  DB_OUTDENT;
}

void Instr_PopStrPushInt_t::printOn(ostream &stream){
  stream << "Instr_PopStrPushInt_t";
}

// ----- Pop Str Push Real -----

Instr_PopStrPushReal_t::Instr_PopStrPushReal_t() {
  DB_INDENT;
  id = POPSTRPUSHREAL_ID;
  DB_OUTDENT;
}

void Instr_PopStrPushReal_t::printOn(ostream &stream){
  stream << "Instr_PopStrPushReal_t";
}


