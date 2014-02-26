//$Id: Instr_Relop.cc,v 1.6 2001/05/10 12:36:55 hempe Exp $

#include "vm.hpp"

// ----- Relop less than -----

Instr_RelOpLT_t::Instr_RelOpLT_t(char* pc){
  id = RELOPLT_ID;
  parseType(pc);
};

Instr_RelOpLT_t::Instr_RelOpLT_t(int _type){
  id = RELOPLT_ID;
  type = _type;
};

void Instr_RelOpLT_t::printOn(ostream &stream){
  stream << "Instr_RelOpLT_t: type = " << type;
}

// ----- Relop less than equal -----

Instr_RelOpLTEQ_t::Instr_RelOpLTEQ_t(char* pc){
  id = RELOPLTEQ_ID;
  parseType(pc);
};


void Instr_RelOpLTEQ_t::printOn(ostream &stream){
  stream << "Instr_RelOpLTEQ_t: type = " << type;
}


// ----- Relop equal -----

Instr_RelOpEQ_t::Instr_RelOpEQ_t(char* pc){
  id = RELOPEQ_ID;
  parseType(pc);
};


Instr_RelOpEQ_t::Instr_RelOpEQ_t(int _type){
  id = RELOPEQ_ID;
  type = _type;
};


void Instr_RelOpEQ_t::printOn(ostream &stream){
  stream << "Instr_RelOpEQ_t: type = " << type;
}

// ----- Relop not equal -----

Instr_RelOpNEQ_t::Instr_RelOpNEQ_t(char* pc){
  id = RELOPNEQ_ID;
  parseType(pc);
};


void Instr_RelOpNEQ_t::printOn(ostream &stream){
  stream << "Instr_RelOpNEQ_t: type = " << type;
}


// ----- Relop greater than -----

Instr_RelOpGT_t::Instr_RelOpGT_t(char* pc){
  id = RELOPGT_ID;
  parseType(pc);
};

Instr_RelOpGT_t::Instr_RelOpGT_t(int _type){
  id = RELOPGT_ID;
  type = _type;
};


void Instr_RelOpGT_t::printOn(ostream &stream){
  stream << "Instr_RelOpGT_t: type = " << type;
}

// ----- Relop greater than equal -----

Instr_RelOpGTEQ_t::Instr_RelOpGTEQ_t(char* pc){
  id = RELOPGTEQ_ID;
  parseType(pc);
};


void Instr_RelOpGTEQ_t::printOn(ostream &stream){
  stream << "Instr_RelOpGTEQ_t: type = " << type;
}
