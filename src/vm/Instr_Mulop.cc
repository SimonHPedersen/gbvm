// $Id: Instr_Mulop.cc,v 1.4 2001/05/10 12:36:53 hempe Exp $

//#include "Thread.hpp"
//#include "Instr_Mulop.hpp"

#include "vm.hpp"

// --- MUL MUL --

Instr_MulOpMul_t::Instr_MulOpMul_t(char* pc) {
  id = MULOPMUL_ID;
  parseType(pc);
};


void Instr_MulOpMul_t::printOn(ostream &stream) {
  stream << "Instr_MulOpMul_t: type = " << type;
}


// --- MUL DIV --

Instr_MulOpDiv_t::Instr_MulOpDiv_t(char* pc) {
  id = MULOPDIV_ID;
  parseType(pc);
};


void Instr_MulOpDiv_t::printOn(ostream &stream) {
  stream << "Instr_MulOpDiv_t: type = " << type;
}


// --- MUL AND --

Instr_MulOpAnd_t::Instr_MulOpAnd_t(char* pc) {
  id = MULOPAND_ID;
  parseType(pc);
};


void Instr_MulOpAnd_t::printOn(ostream &stream) {
  stream << "Instr_MulOpAnd_t: type = " << type;
}


// --- MUL DIVINT --

Instr_MulOpDivInt_t::Instr_MulOpDivInt_t(char* pc) {
  id = MULOPDIVINT_ID;
  parseType(pc);
};


void Instr_MulOpDivInt_t::printOn(ostream &stream) {
  stream << "Instr_MulOpDivInt_t: type = " << type;
}


// --- MUL MOD --

Instr_MulOpMod_t::Instr_MulOpMod_t(char* pc) {
  id = MULOPMOD_ID;
  parseType(pc);
};


void Instr_MulOpMod_t::printOn(ostream &stream) {
  stream << "Instr_MulOpMod_t: type = " << type;
}
