//$Id: Instructions.cc,v 1.11 2001/05/21 11:42:51 hempe Exp $

#include "vm.hpp"

Instr_Return_t *sharedReturnInst = new Instr_Return_t();

Instruction* Instruction::execute(Thread_t *myThread) {
  cout << "Instruction::execute(): General function doing nothing\n";
  exit(1);
  return NULL;
}

void WithType::parseType(char* pc){
  DB_INDENT;
  DB_OUT(vm_parse, "WithType::parseType(): pc " << pc << " " << endl);
  if(!strncmp(pc, "integer", 7)){
    type = INTEGER_ID;
  }
  else if(!strncmp(pc, "real", 4)){
    type = REAL_ID;
  }
  else if(!strncmp(pc, "boolean", 7)){
    type = BOOLEAN_ID;
  }
  else if(!strncmp(pc, "string", 6)){
    type = STRING_ID;
  }
  else if(!strncmp(pc, "char", 4)){
    type = CHAR_ID;
  }
  else if(!strncmp(pc, "objref", 6)){
    type = OBJECT_ID;
  }
  else if(!strncmp(pc, "object", 6)){
    if(!strncmp(&pc[7], "reference", 7)){
      type = OBJECTREFERENCE_ID;
    } else {
      type = OBJECT_ID;
    }
  }
  else if(!strncmp(pc, "ptn", 3)){
    type = PATTERN_ID;
  }
  else if(!strncmp(pc, "pattern", 7)){
    if(!strncmp(&pc[8], "reference", 9)){
      type = PATTERNREFERENCE_ID;
    } else {
      type = PATTERN_ID;
    }
  } 
  else {
    cerr << "WithType::parseType(): ERROR: did not recognize type: " << pc 
	 << " in instruction " << this << endl;
    exit(1);
  }
  DB_OUTDENT;
}

// -----------

void Instr_EmptySingle_t::printOn(ostream &stream){
  stream << "Instr_EmptySingle_t";
}

// ----------------------------------------------------------------------
// Printing of run-time entities

ostream& operator << (ostream& stream, Instruction* inst) {
  if(inst) {
    inst->printOn(stream);
  } else {
    stream << "*** tried to print a null Instruction ***\n";
  }
  return stream;
}


ostream& operator << (ostream& stream, Instruction& inst) {
  inst.printOn(stream);
  return stream;
}
