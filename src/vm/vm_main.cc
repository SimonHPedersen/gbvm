//$Id: vm_main.cc,v 1.52 2001/06/05 10:04:47 hempe Exp $

//#include "Scheduler.hpp"

#include "vm.hpp"

int main(int argc, char** argv){
  //DB_DISABLE_ALL;
  //DB_ENABLE_ALL;
  //DB_ENABLE_GC_ALL;
  //DB_ENABLE_VM_ALL;
  //DB_ENABLE(gc_gc);                      //garbage collector class
  //DB_ENABLE(gc_notify);                  //notification of garbage collections
  //DB_ENABLE(gc_mg);                      //mature generation
  //DB_ENABLE(gc_car);                     //car
  //DB_ENABLE(gc_car_collect);             //write which objects get collected
  //DB_ENABLE(gc_car_delete);              //
  //DB_ENABLE(gc_vmo);                     //virtual machine object (VMObject)
  //DB_ENABLE(gc_vmo_dump_refs);           //virtual machine object (VMObject)
  //DB_ENABLE(gc_vmo_mro_verb);            //moveReferencedObjects verbose
  //DB_ENABLE(gc_trn);                     //train
  //DB_ENABLE(gc_objd);                    //object descriptor
  //DB_ENABLE(gc_objd_const);              //object descriptor constructor
  //DB_ENABLE(gc_glob_setvmr);             //set vm reference
  //DB_ENABLE(gc_glob_setvmr_verb);        //set vm reference
  //DB_ENABLE(gc_memblk);                  //gc mememory blocks
  //DB_ENABLE(gc_tg);                      //gc train generation

  //DB_DISABLE(gc_integrity);              //disable integrity check output
  /*
  DB_ENABLE(vm_bcl);                       //byte-code-loader
  DB_ENABLE(vm_bcl_inst);                  //instruction
  DB_ENABLE(vm_bcl_rtp);                   //run-time-path
  DB_ENABLE(vm_sch);                       //scheduler
  DB_ENABLE(vm_thread);                    //thread
  DB_ENABLE(vm_gbe);                       //GBetaEntities.cc, .hpp
  DB_ENABLE(vm_mixin);                     //mixin
  DB_ENABLE(vm_object);                    //object
  DB_ENABLE(vm_po);                        //part object
  DB_ENABLE(vm_stack);                     //stacks (ref-stack)
  DB_ENABLE(vm_stack_warn);                //warnings from stacks
  DB_ENABLE(vm_parse_misc);
  DB_ENABLE(vm_parse_multi);
  DB_ENABLE(vm_iexe);                      //instruction execution
  DB_ENABLE(vm_iexe_spec);                 //instruction execution 
                                             - special instr
  DB_ENABLE(vm_thread);                    //thread

  DB_ENABLE(vm_rtp);                       //runtime paths
  DB_ENABLE(vm_thread_merge);              //merge debug information
  DB_ENABLE(vm_ptn);                       //pattern
  */
  //DB_ENABLE(vm_stack_warn);
  //DB_ENABLE(vm_parse);                   //parsing of bytecode
  //DB_ENABLE(vm_rstack);                  //stacks (ref-stack)
  //DB_ENABLE(vm_nrstack);                 //stacks (ref-stack)
  //DB_ENABLE(vm_thread_instinfo);         ////*** instruction execution ***
  //DB_ENABLE(vm_gbe);                     //GBetaEntities.cc, .hpp
  //DB_ENABLE(vm_status);                  //GBetaEntities.cc, .hpp
  //DB_ENABLE(gc_vmo_id);                  //When is a new vmobject created?
  //DB_ENABLE(first_index_var_unique);     ////instruction execution
  //DB_ENABLE(cmn_lists);                  //common lists
  //DB_ENABLE(gc_mg_heapdump);
  //DB_ENABLE(gc_remset_dump);
  //DB_ENABLE(gc_vmo_dump_refs);

  if(argc != 2){
    cout << "USAGE: " << argv[0] << " <filename>" << endl; 
    exit(1);
  }
  fixObjectDescriptors();
#ifdef NEW_STYLE_GC
  scheduler = new(allocateRootVM_T(schedDesc)) Scheduler_t();
  garbageCollector.registerMutator(gcs_prepareForGC);
#else //NEW_STYLE_GC
  scheduler = new(allocateRootVM(schedDesc)) Scheduler_t();
  garbageCollector->registerMutator(gcs_prepareForGC);
#endif //NEW_STYLE_GC
  scheduler->startEngine(argv[1]);
  return 0;
}
