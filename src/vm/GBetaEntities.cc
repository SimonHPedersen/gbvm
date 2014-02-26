// $Id: GBetaEntities.cc,v 1.18 2001/05/10 12:36:52 hempe Exp $

#include "vm.hpp"

PartObject_t *getPredef(Scheduler_t* myScheduler){
  DB_INDENT;
  //make part object

  DB_OUT(vm_gbe, "getPredef(): *making surrounding po* \n");

  //Making the surrounding partObject predef. Predef is a UserPartObject
  //because it has primitive types and it should be possible to extend
  //the number of these in the future
  ObjectDescriptor_t* poDesc =
    new ObjectDescriptor_t(sizeof(UserPartObject_t)+
			   (PREDEF_NOF_ATTR-1)*sizeof(void*), 0);

  DB_OUT(vm_gbe, "getPredef(): *made surrounding poDesc* "
	 << " poDesc: " << poDesc << endl);

  void* poData = allocateVM(poDesc);
  //this initializes the object descriptor...
  free(new(malloc(sizeof(UserPartObject_t))) UserPartObject_t(poDesc));

  MainPart* predefMainPart = new MainPart();
  predefMainPart->setNumberOfAttributes(PREDEF_NOF_ATTR);
  Mixin_t* preMixin = new(allocateVM(mixinDesc)) Mixin_t(predefMainPart, 0);

  //create the object
  PartObject_t* predef = new(poData) UserPartObject_t(preMixin, 0);

  DB_OUT(vm_gbe, "getPredef(): *made predef po* \n");

  //install empty object pattern at 0
  ((UserPartObject_t*)predef)->setAttribute(0, myScheduler->
					    getEmptyObjectPattern());

  //make boolean mixin
  Mixin_t* boolPreMixin =
    new(allocateVM(mixinDesc)) PredefinedMixin_t<bool>(predef);

  DB_OUT(vm_gbe, "getPredef(): *made boolean mixin po* \n");

  //Making the boolean Pattern having this mixin
  ObjectDescriptor_t *patDesc = new ObjectDescriptor_t(sizeof(Pattern_t), 0);
  Pattern_t *pattern = new(allocateVM(patDesc)) Pattern_t(patDesc);
  pattern->addMixin(0, boolPreMixin);

  //install boolean pattern at 1
  ((UserPartObject_t*)predef)->setAttribute(1, pattern);

  DB_OUT(vm_gbe, "getPredef(): *installed boolean pattern* \n");

  //make char mixin
  PredefinedMixin_t<char>* charPreMixin =
    new(allocateVM(mixinDesc)) PredefinedMixin_t<char>(predef);

  //Making the char Pattern having this mixin
  pattern = new(allocateVM(patDesc)) Pattern_t(patDesc);
  pattern->addMixin(0, charPreMixin);

  //install char pattern at 2
  ((UserPartObject_t*)predef)->setAttribute(2, pattern);

  DB_OUT(vm_gbe, "getPredef(): *installed char pattern* \n");

  //make int mixin
  PredefinedMixin_t<int>* intPreMixin =
    new(allocateVM(mixinDesc)) PredefinedMixin_t<int>(predef);

  //Making the int Pattern having this mixin
  pattern = new(allocateVM(patDesc)) Pattern_t(patDesc);
  pattern->addMixin(0, intPreMixin);

  //install int pattern at 3
  ((UserPartObject_t*)predef)->setAttribute(3, pattern);

  DB_OUT(vm_gbe, "getPredef(): *installed int pattern* \n");

  //make real mixin
  PredefinedMixin_t<real>* realPreMixin =
    new(allocateVM(mixinDesc)) PredefinedMixin_t<real>(predef);

  //Making the char Pattern having this mixin
  pattern = new(allocateVM(patDesc)) Pattern_t(patDesc);
  pattern->addMixin(0, realPreMixin);

  //install real pattern at 4
  ((UserPartObject_t*)predef)->setAttribute(4, pattern);

  DB_OUT(vm_gbe, "getPredef(): *installed real pattern* \n");  

  //make string mixin 
  PredefinedStringMixin_t* stringPreMixin =
    new(allocateVM(mixinDesc)) PredefinedStringMixin_t(predef);

  //Making the char Pattern having this mixin
  pattern = new(allocateVM(patDesc)) Pattern_t(patDesc);
  pattern->addMixin(0, stringPreMixin);

  //install string pattern at 5
  ((UserPartObject_t*)predef)->setAttribute(5, pattern);

  DB_OUT(vm_gbe, "getPredef(): *installed string pattern* \n");

  DB_OUTDENT;
  return predef;
}
