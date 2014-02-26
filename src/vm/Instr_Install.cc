//$Id: Instr_Install.cc,v 1.5 2001/05/10 12:36:53 hempe Exp $

//#include "Thread.hpp"
//#include "Instr_Install.hpp"

#include "vm.hpp"

Instr_InstallPtn_t::Instr_InstallPtn_t(char* pc){
  id = INSTALLPTN_ID;
  if(sscanf(pc,"%i",&index) != 1) {
    cerr <<  "Instr_InstallPtn_t::Instr_InstallPtn_t(): parse error - "
	 <<  "could not parse int "
	 << pc << endl;
    exit(1);
  }
}

void Instr_InstallPtn_t::printOn(ostream &stream){
  stream << "Instr_InstallPtn_t with index" << index;
}

Instr_InstallObj_t::Instr_InstallObj_t(char* pc){
  id = INSTALLOBJ_ID;
  if(sscanf(pc,"%i",&index) != 1) {
    cerr <<  "Instr_InstallObj_t::Instr_InstallObj_t(): parse error - "
	 << "could not parse int "
	 << pc << endl;
    exit(1);
  }
}

void Instr_InstallObj_t::printOn(ostream &stream){
  stream << "Instr_InstallObj_t with index" << index;
}

Instr_InstallQualPtn_t::Instr_InstallQualPtn_t(char* pc){
  id = INSTALLQUALPTN_ID;
  if(sscanf(pc,"%i",&index) != 1) {
    cerr <<  "Instr_InstallQualPtn_t::Instr_InstallQualPtn_t(): parse error - "
	 <<  "could not parse int "
	 << pc << endl;
    exit(1);
  }
}

void Instr_InstallQualPtn_t::printOn(ostream &stream){
  stream << "Instr_InstallQualPtn_t with index" << index;
}

Instr_InstallQualObj_t::Instr_InstallQualObj_t(char* pc){
  id = INSTALLQUALOBJ_ID;
  if(sscanf(pc,"%i",&index) != 1) {
    cerr <<  "Instr_InstallQualObj_t::Instr_InstallQualObj_t(): parse error - "
	 <<  "could not parse int "
	 << pc << endl;
    exit(1);
  }
}

void Instr_InstallQualObj_t::printOn(ostream &stream){
  stream << "Instr_InstallQualObj_t with index" << index;
}






