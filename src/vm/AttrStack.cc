//$Id: AttrStack.cc,v 1.3 2000/12/05 18:28:57 hempe Exp $

#include "AttrStack.hpp"

//size in bytes in input (remembered as size in number of elements)
//which is the number passed to the object descriptor too
AttrStack_t::AttrStack_t(){
  ObjectDescriptor_t *stackObjDesc = getStackObjDesc(STACK_SIZE, UINT_MAX);
  VMObject_t* stackSpace   = garbageCollector->addNewObject(stackObjDesc);
  partObjectStack = new(stackSpace->getDataPtr()) RefStack_t<PartObject_t*>(STACK_SIZE, 
									    stackObjDesc);
  stackObjDesc = getStackObjDesc(STACK_SIZE, 0);
  stackSpace   = garbageCollector->addNewObject(stackObjDesc);
  indexStack = new(stackSpace->getDataPtr()) NonRefStack_t<unsigned int>(STACK_SIZE, 
									    stackObjDesc);
}

AttrStack_t::AttrStack_t(ObjectDescriptor_t* objDesc){
  objDesc->setReferenceAt(((unsigned int)&partObjectStack-(unsigned int)this)/sizeof(void*));
  objDesc->setReferenceAt(((unsigned int)&indexStack-(unsigned int)this)/sizeof(void*));
}

void AttrStack_t::push(PartObject_t* po, unsigned int index) {
  partObjectStack->push(po);
  indexStack->push(index);
}

void AttrStack_t::pop(PartObject_t **po, unsigned int *index) {
  *po = partObjectStack->pop();
  *index = indexStack->pop(); 
}

void AttrStack_t::print(unsigned int index){
	indentPrint(index);
	cout << "Attribute stack - part object stack: \n";
	partObjectStack->print(index + 2, 0);
	
	indentPrint(index);
	cout << "Attribute stack - index stack: \n";
	indexStack->print(index + 2);	
}
