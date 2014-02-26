// $Id: Instr_Add.cc,v 1.7 2001/03/22 18:54:41 ilsoe Exp $

//#include "Thread.hpp"
//#include "Instr_Add.hpp"

#include "vm.hpp"

// ----- Plus -----

Instr_AddOpPlus_t::Instr_AddOpPlus_t(char* pc){
  id = ADDOPPLUS_ID;
  parseType(pc);
};

Instr_AddOpPlus_t::Instr_AddOpPlus_t(int _type){
  id = ADDOPPLUS_ID;
  type = _type;
};

void Instr_AddOpPlus_t::printOn(ostream &stream){
  stream << "Instr_AddOpPlus_t: type = " << type;
}

// ----- Minus -----

Instr_AddOpMinus_t::Instr_AddOpMinus_t(char* pc){
  id = ADDOPMINUS_ID;
  parseType(pc);
};


void Instr_AddOpMinus_t::printOn(ostream &stream){
  stream << "Instr_AddOpMinus_t: type = " << type;
}

// ----- Or -----

Instr_AddOpOr_t::Instr_AddOpOr_t(char* pc){
  id = ADDOPOR_ID;
  parseType(pc);
};


void Instr_AddOpOr_t::printOn(ostream &stream){
  stream << "Instr_AddOpOr_t: type = " << type;
}

// ----- Xor -----

Instr_AddOpXor_t::Instr_AddOpXor_t(char* pc){
  id = ADDOPXOR_ID;
  parseType(pc);
};


void Instr_AddOpXor_t::printOn(ostream &stream){
  stream << "Instr_AddOpXor_t: type = " << type;
}
