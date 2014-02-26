//$Id: Stacks.hpp,v 1.32 2001/05/25 09:31:22 hempe Exp $

#ifndef STACKS_HPP
#define STACKS_HPP

template <class T_t>
class NonRefStack_t;

template <class T_t>
class RefStack_t;

#include <typeinfo>
#include "ObjectDescriptor.hpp"
#include "GBetaEntities.hpp"
#include "Thread.hpp"

#define STANDARD_STACK_SIZE 512 //bytes

struct attr_t{
  PartObject_t* po;
  unsigned int index;
};

ObjectDescriptor_t *getStackObjDesc(unsigned int size, unsigned int maskValue);

//-************ NonRefStack **************-

template <class T_t>
class NonRefStack_t{
public:

  void *operator new(size_t size, void *location) {
    return location;
  }

  //size in bytes in input (remembered as size in number of elements)
  //which is the number passed to the object descriptor too
  NonRefStack_t(unsigned int size, ObjectDescriptor_t *objDesc) {
    //unmark c++ NonRefStack_t object
    unsigned int baseBegin = getBaseOffset();
    for(unsigned int i = 0; i < baseBegin; i++){
      objDesc->clearReferenceAt(i);
    }
    this->size = (size-baseBegin*sizeof(void*))/sizeof(T_t);
    
    //clear stack
    for(unsigned int i = 0; i < this->size; i++){
      base[i] = 0;
    }

    //initialize
    top = 0;
  }

  void push(T_t element) {
    if(top == size){
      resize();
    } else {
      base[top++] = element;
    }
  }

  T_t pop(void) {
    if(top > 0){
      T_t tmpElement = base[--top];
      base[top] = 0;
      return tmpElement;
    } else {
      DB_OUT(vm_stack_warn, "NonRefStack_t::pop(void): WARNING: tried to "
	     "pop from an empty stack\n");
      return 0;
    }
  }

  //returns the top element without removing it
  T_t peek(void) { 
    if(top > 0){
      return base[top-1];
    } else {
      DB_OUT(vm_stack_warn, "NonRefStack_t::peek(void): WARNING: tried to"
	     " peep on an empty stack\n");
      return 0;
    }
  }

  //returns the element backOffset elements below the top element
  //without removing it
  T_t peek(unsigned int backOffset) { 
    if(top > backOffset){
      return base[top - (1 + backOffset)];
    } else {
      DB_OUT(vm_stack_warn, "NonRefStack_t::peek(int): WARNING: tried to"
	     " peep on an empty stack\n");
      return 0;
    }
  }

  //discards the top element
  void discard(void) {
    if(top > 0){
      base[--top] = 0;
    } else {
      cerr << "NonRefStack_t::discard(void): ERROR: tried to discard "
	"from an empty stack\n";
      exit(1);
    }
  }

  void printCERR(){
    if(top==0){
      cerr << "empty \n";
    } else {
      for(unsigned int i = 0; i < top; i++){
	cerr << i << ": " << base[i] << " | ";
      }
      cerr << endl;
    }
    DB_OUTDENT;
  }	

  void print(){
    DB_INDENT;
    if(top==0){
      DB_OUT0(vm_nrstack, "empty \n");
    } else {
      for(unsigned int i = 0; i < top; i++){
	DB_OUT0(vm_nrstack, i << ": " << base[i] << " | ");
      }
      DB_OUT0(vm_nrstack, endl);
    }		
    DB_OUTDENT;
  }
  
  
private:
  //calls Thread to make it point to a 
  //new bigger/smaller version of this stack 
  void resize(void) {
    cerr << "NonRefStack_t" << typeid(T_t).name() <<"::resize(void): ERROR: Not implemented\n";
    cerr << "Stacks was:" << endl;
    printCERR();
    exit(1);
  }

  unsigned int                getBaseOffset() {
    return ((unsigned int)&base - (unsigned int)this)/sizeof(void*); 
  }

  unsigned int                size;
  unsigned int                top;
  T_t                         base[1];
};

//-*************** RefStack_t ****************-

template <class T_t>
class RefStack_t{
public:

  void                       *operator new(size_t size, void *location) {
    return location;
  }

  //size in bytes in input (remembered as size in number of elements)
  //which is the number passed to the object descriptor too
  RefStack_t(unsigned int size, ObjectDescriptor_t *objDesc){
    //unmark c++ RefStack_t object
    unsigned int baseBegin = getBaseOffset();
    for(unsigned int i = 0; i < baseBegin; i++){
      objDesc->clearReferenceAt(i);
    }
    this->size = (size-baseBegin*sizeof(void*))/sizeof(T_t);
    
    //clear stack
    for(unsigned int i = 0; i < this->size; i++){
      base[i] = 0;
    }
    
    objectDescriptor = objDesc;

    //initialize
    top = 0;
    frame = 0;
  }

  void push(T_t element) {
    DB_INDENT;
    if(top == size){
      resize();
    } else {
      DB_OUT(vm_rstack, "RefStack::push() before pushing top: " << top 
             << " &base[top]: " << &base[top] << " element: " << element << endl);
#ifdef NEW_STYLE_GC
      base[top++] = element;
#else
      setVMReference(this, (void**)&base[top++], element);
#endif
    }
    DB_OUTDENT;
  }

  T_t pop(void) {
    if(top > 0){
      T_t tmpElement = base[--top];
#ifdef NEW_STYLE_GC
      base[top] = 0;
#else
      setVMReference(this, (void**)&base[top], 0);
#endif
      return tmpElement;
    } else {
      cerr << "RefStack_t::pop(void): ERROR: tried to pop from an "
	"empty stack\n";
      exit(1);
    }
  }

  //returns the top element without removing it
  T_t peek(void) {
    if(top > 0){
      return base[top-1];
    } else {
      DB_OUT(vm_stack_warn, "RefStack_t::peep(void): WARNING: tried to "
	     "peep on an empty stack\n");
      return 0;
    }
  }

  //returns the element backOffset elements below the top element
  //without removing it
  T_t peek(unsigned int backOffset) {
    if(top > backOffset){
      return base[top - (1 + backOffset)];
    } else {
      DB_OUT(vm_stack_warn, "RefStack_t::peep(void): WARNING: tried to "
	     "peep on an empty stack\n");
      return 0;
    }
  }

  //discards the top element
  void discard(void) {
    DB_INDENT;
    if(top > 0){
      //DB_OUT(vm_stack, "RefStack::discard() before descarding top-1: " << top-1 
      //   << " &base[top-1]: " << &base[top-1] << endl);
#ifdef NEW_STYLE_GC
      base[--top] = 0;
#else
      setVMReference(this, (void**)&base[--top], 0);
#endif
      
    } else {
      cerr << "RefStack_t::discard(void): ERROR: tried to discard from "
	"an empty stack\n";
      exit(1);
    }
    DB_OUTDENT;
  }

  void addNewFrame() {
    DB_INDENT;
    //clear objDesc and save old frame pointer
    objectDescriptor->clearReferenceAt(getBaseOffset() + top);
    base[top] = (AttributeInstance_t*)frame;
    //set new frame pointer
    frame = top++;
    DB_OUT(vm_stack, "RefStack_t::addNewFrame: frame added" << endl);
    DB_OUTDENT;
  }

  void deleteFrame(){
    DB_INDENT;
    //DB_OUT(vm_stack, "RefStack_t::deleteFrame(): start frame = "
    //   << frame << " top = " << top << endl);

    //clear frame
    if(top > 0){
      while((top-1)>frame){
	discard();
	//DB_OUT(vm_stack, "RefStack_t::deleteFrame(): discarded frame = "
	//     << frame << " top = " << top << endl);
      }
      //get old frame pointer
      frame = (unsigned int)base[--top];
      
      //DB_OUT(vm_stack, "RefStack_t::deleteFrame(): got frame: " << frame << endl);
      
      //clear old frame pointer entry in stack
      base[top] = 0;
      
      //set reference objDescriptor
      objectDescriptor->setReferenceAt(getBaseOffset() + top);
      
      DB_OUT(vm_stack, "RefStack_t::deleteFrame(): deleted frame \n");
      
    }
    DB_OUTDENT;
  }

  void resetFrame(){
    //clear frame
    while(top > 0 && (top-1)>frame){
      discard();
    }
  }

  T_t getElementAt(unsigned int index){
    return base[frame + index];
  }
  
  void print(unsigned int withID){
    DB_INDENT;
    if(top==0){
      DB_OUT0(vm_rstack, "empty \n");
    } else {
      for(unsigned int i = 0; i < top; i++){
	if(withID){
	  void* ref = base[i];
	  if((unsigned int)ref > STACK_SIZE){
	    DB_OUT0(vm_rstack, i << ": " << ref << " id " 
		    << ((VMObject_t*)(VMO_FROM_VOIDP(ref)))->getID() << " | ");
	  } else {
	    DB_OUT0(vm_rstack, i << ": " << base[i] << " | ");
	  }
	} else { 
	  DB_OUT0(vm_rstack, i << ": " << base[i] << " | ");
	}
      }
      DB_OUT0(vm_rstack, endl);
    }		
    DB_OUTDENT;
  }

  void printStr(){
    DB_INDENT;
    if(top==0){
      DB_OUT0(vm_rstack, "empty \n");
    } else {
      for(unsigned int i = 0; i < top; i++){
	DB_OUT0(vm_rstack, i << ": " << base[i] << " | ");
      }		
      DB_OUT0(vm_rstack, endl);
    }
    DB_OUTDENT;
  }
  
private:

  //calls Thread to make it point to a new bigger/smaller version of this stack 
  void resize(void) {
    cerr << "RefStack_t::resize(void): ERROR: Not implemented\n";
    exit(1);
  }

  //returns offset in number of void* //SKAL DER IKKE STAA & FORAN base ???
  unsigned int getBaseOffset() {
    return ((unsigned int)&base - (unsigned int)this)/sizeof(void*);
  }

  ObjectDescriptor_t         *objectDescriptor;
  unsigned int                size;
  unsigned int                top;
  unsigned int                frame;
  T_t                         base[1];
};


#endif









