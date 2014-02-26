// $Id: vm.hpp,v 1.2 2001/03/27 08:34:17 ilsoe Exp $

#ifndef _VM_HPP_
#define _VM_HPP_

#include "gc_globals.hpp"
#include "ByteCodeLoader.hpp"
#include "Lists.hpp"
#include "vm_globals.hpp"
#include "VMObject.hpp"
#include "Thread.hpp"
#include "globals.hpp"
#include "RunTimeSteps.hpp"
#include "GBetaEntities.hpp"
#include "Scheduler.hpp"
#include "Stacks.hpp"
#include "garbage_collector.hpp"

#include "InstructionCodes.hpp"
#include "Instructions.hpp"
#include "Instr_Add.hpp"
#include "Instr_Install.hpp"
#include "Instr_Misc.hpp"
#include "Instr_Mulop.hpp"
#include "Instr_Multiline.hpp"
#include "Instr_Peek.hpp"
#include "Instr_Pop.hpp"
#include "Instr_Push.hpp"
#include "Instr_Relop.hpp"
#include "Instr_Sem.hpp"
#include "Instr_Special.hpp"


#endif //_VM_HPP_
