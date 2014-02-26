//$Id: Instr_Peek.cc,v 1.3 2001/03/22 18:54:42 ilsoe Exp $

//#include "Thread.hpp"
//#include "Instr_Peek.hpp"

#include "vm.hpp"

// ----- Peek Objref -----

Instr_PeekObjRef_t::Instr_PeekObjRef_t(char *pc) {
  DB_INDENT;
  id = PEEKOBJREF_ID;
  parseRuntimePath(pc, this);
  DB_OUTDENT;
}

void Instr_PeekObjRef_t::printOn(ostream &stream){
  stream << "Instr_PeekObjRef_t: rtp = ";
  printRuntimePath(rtp.first(), stream);
}

// ----- Peek Tmpref -----

Instr_PeekTmpRef_t::Instr_PeekTmpRef_t(char *pc) {
  DB_INDENT;
  id = PEEKTMPREF_ID;
  parseRuntimePath(pc, this);
  DB_OUTDENT;
}

void Instr_PeekTmpRef_t::printOn(ostream &stream){
  stream << "Instr_PeekTmpRef_t: rtp = ";
  printRuntimePath(rtp.first(), stream);
}
