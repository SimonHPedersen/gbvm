//$Id: Instr_Special.cc,v 1.10 2001/05/10 12:36:56 hempe Exp $

#include "vm.hpp"

// --- Return ---

Instr_Return_t::Instr_Return_t() {
  id = RETURN_ID;
}

void Instr_Return_t::printOn(ostream &stream) {
  stream << "Instr_Return_t";
}


// --- Return no pop po ---

Instr_ReturnNPPO_t::Instr_ReturnNPPO_t() {
  id = RETURNNPPO_ID;
}

void Instr_ReturnNPPO_t::printOn(ostream &stream) {
  stream << "Instr_ReturnNPPO_t";
}

// --- ReturnMPAttrInit ---

Instr_ReturnMPAttrInit_t::Instr_ReturnMPAttrInit_t() {
  id = RETURNMPATTRINIT_ID;
}

void Instr_ReturnMPAttrInit_t::printOn(ostream &stream) {
  stream << "Instr_ReturnMPAttrInit_t";
}

// --- Cont gather virt ---

void Instr_ContGatherVirt_t::printOn(ostream &stream) {
  stream << "Instr_ContGatherVirt_t: " << parentAttrName << "/" <<
    parentAttrIndex << " in \"" << parentMainPartID << "\"";
}

// --- Jump no push po ---

Instr_JumpNPPO_t::Instr_JumpNPPO_t(Instruction *instr) {
  instrPtr = instr;
  id = JUMPNPPO_ID;
}

void Instr_JumpNPPO_t::printOn(ostream &stream) {
  stream << "Instr_JumpNPPO_t: pointing to: " << instrPtr;
}

// --- Jump on true, no push po ---

Instr_JumpTrueNPPO_t::Instr_JumpTrueNPPO_t(Instruction *instr) {
  instrPtr = instr;
  id = JUMPTRUENPPO_ID;
}

void Instr_JumpTrueNPPO_t::printOn(ostream &stream) {
  stream << "Instr_JumpTrueNPPO_t: pointing to: " << instrPtr;
}

// --- Jump to subroutine ---

Instr_JumpSubNPPO_t::Instr_JumpSubNPPO_t(Instruction *instr) {
  instrPtr = instr;
  id = JUMPSUBNPPO_ID;
}

void Instr_JumpSubNPPO_t::printOn(ostream &stream) {
  stream << "Instr_JumpSubNPPO_t: pointing to: " << instrPtr;
}

// --- Copy Top ---

void Instr_CopyTop_t::printOn(ostream &stream) {
  stream << "Instr_CopyTop_t: with type: " << type;
}


// --- Copy Top 2 ---

void Instr_CopyTop2_t::printOn(ostream &stream) {
  stream << "Instr_CopyTop2_t: with type: " << type;
}

// --- Unsupported instruction ---

void Instr_Unsupported_t::printOn(ostream &stream) {
  stream << "Instr_Unsupported_t: " << name;
}
