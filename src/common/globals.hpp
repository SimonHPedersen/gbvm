//$Id: globals.hpp,v 1.40 2001/04/24 14:00:45 ilsoe Exp $
#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <limits.h>
#include <stdlib.h>
#include <iostream.h>

//-------------- debug output macros -------------

#ifdef DEBUG

extern char output_mask[UCHAR_MAX+1];
extern int db_indentation; // = 0;

#  define DB_OUT(cat,arg) if (output_mask[cat]) {                         \
    for(int index_variable_unique = 0;                                    \
	index_variable_unique < db_indentation;                           \
	index_variable_unique++)                                          \
    cout << "  ";                                                         \
    cout << arg << flush;}
#  define DB_OUT0(cat,arg) if (output_mask[cat]) {                        \
    cout << arg << flush;}
#  define DB_INS_INDT(stream)                                             \
    for(int index_variable_unique2 = 0;                                    \
	index_variable_unique2 < db_indentation;                           \
	index_variable_unique2++)                                          \
	  (stream) << "  ";
#  define DB_INDENT db_indentation++;
#  define DB_OUTDENT db_indentation--;
#  define DB_ENABLE(category) output_mask[category] = 1;
#  define DB_DISABLE(category) output_mask[category] = 0;
#  define DB_ENABLED(category) (output_mask[category])
#  define DB_ENABLE_ALL {                                                 \
  cout << "enabling all debug output\n";                                  \
  for(int index_variable_unique3 = first_index_var_unique;                 \
      index_variable_unique3 < last_index_var_unique;                      \
      index_variable_unique3++)                                            \
    { output_mask[index_variable_unique3] = 1; }                           \
};                                                                         
#  define DB_ENABLE_GC_ALL {                                                 \
  cout << "enabling all debug output\n";                                  \
  for(int index_variable_unique4 = first_index_var_unique;                 \
      index_variable_unique4 < first_vm_index_var_unique;                      \
      index_variable_unique4++)                                            \
    { output_mask[index_variable_unique4] = 1; }                           \
};                                                                         
#  define DB_ENABLE_VM_ALL {                                                 \
  cout << "enabling all debug output\n";                                  \
  for(int index_variable_unique5 = first_vm_index_var_unique;                 \
      index_variable_unique5 < st_index_var_unique;                      \
      index_variable_unique5++)                                            \
    { output_mask[index_variable_unique5] = 1; }                           \
};                                                                         \
				

#else //debug output is void

#  define DB_OUT(cat, arg)
#  define DB_OUT0(cat, arg)
#  define DB_INS_INDT(stream)
#  define DB_INDENT
#  define DB_OUTDENT
#  define DB_ENABLE(category)
#  define DB_DISABLE(category)
#  define DB_ENABLED(category) (0)
#  define DB_ENABLE_ALL //sorry!
#endif

enum debug_out {
  //KEEP FIRST_index_var_unique FIRST!!!
  first_index_var_unique,

  gc_gc,                        //garbage collector class
  gc_notify,                    //notification of garbage collects
  gc_mg,                        //mature generation
  gc_mg_heapdump,               //dump heap?
  gc_remset_dump,               //dump remmebered sets in heapdumps?
  gc_remset_intro_dump,         //dump introductory space remmebered sets in heapdumps?
  gc_remset_ext_dump,           //dump external remmebered sets in heapdumps?
  gc_remset_int_dump,           //dump internal remmebered sets in heapdumps?
  gc_car,                       //car
  gc_car_collect,               //car collected objects....
  gc_car_delete,
  gc_vmo,                       //virtual machine object (VMObject)
  gc_vmo_dump_refs,             //dump references in heapdump
  gc_vmo_mro_verb,              //moveReferencedObjects verbose
  gc_trn,                       //train
  gc_objd,                      //object descriptor
  gc_objd_const,                //object descriptor constructor
  gc_glob_setvmr,               //set vm reference
  gc_glob_setvmr_verb,          //set vm reference verbose
  gc_integrity,                 //gc integrity checks

  gc_memblk,                    //gc memory block
  gc_tg,                        //train generation (the new gc)
  gc_heapdump,                  //heapdump in the new gc
  gc_dumpblock,                 //trace block dumps
  gc_introSp,                   //introductory space
  gc_stackSp,                   //stack space
  gc_car_gcsc,                  //car scan
  gc_remset,                    //remembered sets

  gc_vmo_id,                    //vmo_id 

  first_vm_index_var_unique,  

  vm_dummy,                     //work around :)

  vm_bcl,                       //byte-code-loader
  vm_bcl_inst,                  //instruction
  vm_bcl_rtp,                   //run-time-path in byte-code-loader
  vm_rtp,                       //run-time-path
  vm_sch,                       //scheduler
  vm_thread,                    //thread
  vm_thread_merge,              //merging debug information
  vm_thread_instinfo,           //shows which instruction is executed
  vm_gbe,                       //GBetaEntities.cc,.hpp
  vm_mixin,                     //mixin
  vm_object,                    //object
  vm_po,                        //part object
  vm_ptn,                       //pattern
  vm_stack,                     //stacks (ref-stack)
  vm_stack_warn,                //stack warnings
  vm_nrstack,                   //non-ref stacks
  vm_rstack,                    //ref stacks
  vm_inst_relop,                //RELOP instructions
  vm_parse,                     //parsing of bytecode
  vm_parse_misc,
  vm_parse_multi,
  vm_iexe,                      //instruction execution
  vm_iexe_spec,                 //instruction execution - special instructions
  vm_status,                    //status of the thread 

  cmn_lists,                    //common lists
  //  memory_allocation,
  //  remembered_set_update,
  //error

  last_index_var_unique
  //DON'T ADD ANYTHING AFTER last_index_var_unique
};


#endif




