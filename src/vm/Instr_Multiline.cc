//$Id: Instr_Multiline.cc,v 1.6 2001/03/22 18:54:42 ilsoe Exp $

//#include "Thread.hpp"
//#include "Instr_Multiline.hpp"

#include "vm.hpp"

void WhenAlternative::printOn(ostream& dest) {
  Instruction* curins=eval_instrs.first();
  dest << ind << '|' << (exact? "=":"") << "case" << endl;
  indent();
  while (curins) { curins->printOn(dest); curins=curins->next(); }
  outdent();
  dest << ind << "|then" << endl;
  curins=body_instrs.first();
  indent();
  while (curins) { curins->printOn(dest); curins=curins->next(); }
  outdent();
}

void Instr_NamedFor::printOn(ostream& dest) {
  dest << ind << "namedFor(\"" << indexname << '"' << endl;
  Instruction* current=eval_instrs.first();
  indent();
  while (current) {
    dest << ind;
    current->printOn(dest);
    dest << endl;
    current=current->next();
  }
  outdent();
  dest << ind << '|' << endl;
  indent();
  current=body_instrs.first();
  while (current) {
    dest << ind;
    current->printOn(dest);
    dest << ind;
    current=current->next();
  }
  outdent();
  dest << ind << ')' << endl;
}

void Instr_SimpleFor::printOn(ostream& dest) {
  dest << ind << "simpleFor(" << endl;
  Instruction* current=eval_instrs.first();
  indent();
  while (current) {
    dest << ind;
    current->printOn(dest);
    dest << endl;
    current=current->next();
  }
  outdent();
  dest << ind << '|' << endl;
  indent();
  current=body_instrs.first();
  while (current) {
    dest << ind;
    current->printOn(dest);
    dest << endl;
    current=current->next();
  }
  outdent();
  dest << ind << ')' << endl;
}

void Instr_SimpleIf::printOn(ostream& dest) {
  dest << ind << "simpleIf(" << endl;
  Instruction* current=eval_instrs.first();
  indent();
  while (current) {
    dest << ind;
    current->printOn(dest);
    dest << endl;
    current=current->next();
  }
  outdent();
  dest << ind << "|then" << endl;
  current=then_instrs.first();
  indent();
  while (current) {
    dest << ind;
    current->printOn(dest);
    dest << endl;
    current=current->next();
  }
  outdent();
  dest << ind << "|else" << endl;
  current=else_instrs.first();
  indent();
  while (current) {
    dest << ind;
    current->printOn(dest);
    dest << endl;
    current=current->next();
  }
  outdent();
  dest << ind << ')' << endl;
}

void Instr_LocatedSimpleIf::printOn(ostream& dest) {
  dest << ind << "locatedSimpleIf( " << where_txt << endl;
  Instruction* current=eval_instrs.first();
  indent();
  while (current) { current->printOn(dest); current=current->next(); }
  outdent();
  dest << ind << "|then" << endl;
  current=then_instrs.first();
  indent();
  while (current) { current->printOn(dest); current=current->next(); }
  outdent();
  dest << ind << "|else" << endl;
  current=else_instrs.first();
  indent();
  while (current) { current->printOn(dest); current=current->next(); }
  outdent();
  dest << ind << ')' << endl;
}
void Selection::printOn(ostream& dest) {
  Instruction* current=instrs.first();
  dest << ind << "|case" << endl;
  indent();
  while (current) {
    dest << ind;
    current->printOn(dest);
    dest << endl;
    current=current->next(); }
  outdent();
}

void Alternative::printOn(ostream& dest) {
  Selection* cursel=sels.first();
  while (cursel) {
    dest << ind;
    cursel->printOn(dest);
    dest << endl;
    cursel=cursel->next();
  }
  dest << ind << "|then" << endl;
  Instruction* curins=instrs.first();
  indent();
  while (curins) {
    dest << ind;
    curins->printOn(dest);
    dest << endl;
    curins=curins->next();
  }
  outdent();
}

void Instr_GeneralIf::printOn(ostream& dest) {
  dest << ind << "generalIf(" << endl;
  Instruction* curins=eval_instrs.first();
  indent();
  while (curins) {
    dest << ind;
    curins->printOn(dest);
    dest << endl;
    curins=curins->next();
  }
  outdent();
  Alternative* curalt = alts.first();
  while (curalt) {
    dest << ind;
    curalt->printOn(dest);
    dest << endl;
    curalt=curalt->next();
  }
  dest << ind << "|else" << endl;
  curins=else_instrs.first();
  indent();
  while (curins) {
    dest << ind;
    curins->printOn(dest);
    dest << endl;
    curins=curins->next();
  }
  outdent();
  dest << ind << ')' << endl;
}

void Instr_While::printOn(ostream& dest) {
  dest << ind << "while(" << endl;
  Instruction* current=eval_instrs.first();
  indent();
  while (current) {
    dest << ind;
    current->printOn(dest);
    dest << endl;
    current=current->next();
  }
  outdent();
  dest << ind << '|' << endl;
  indent();
  current=body_instrs.first();
  while (current) {
    dest << ind;
    current->printOn(dest);
    dest << endl;
    current=current->next();
  }
  outdent();
  dest << ind << ')' << endl;
}

void Instr_When::printOn(ostream& dest) {
  dest << ind << "when(" << endl;
  Instruction* curins=eval_instrs.first();
  indent();
  while (curins) { curins->printOn(dest); curins=curins->next(); }
  outdent();
  WhenAlternative* curwalt = walts.first();
  while (curwalt) { curwalt->printOn(dest); curwalt=curwalt->next(); }
  dest << ind << "|else" << endl;
  curins=else_instrs.first();
  indent();
  while (curins) { curins->printOn(dest); curins=curins->next(); }
  outdent();
  dest << ind << ')' << endl;
}
