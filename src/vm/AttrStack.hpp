//$Id: AttrStack.hpp,v 1.5 2000/12/04 12:41:08 hempe Exp $

#ifndef ATTRSTACKS_HPP
#define ATTRSTACKS_HPP

//#define STACK_SIZE 1024

class AttrStack_t;

#include "ObjectDescriptor.hpp"
#include "GBetaEntities.hpp"
#include "Thread.hpp"
#include "Stacks.hpp"

class AttrStack_t{
public:
  void                       *operator new(size_t size, void *location) {
    return location;
  }
  void                       *operator new(size_t size) {
    return malloc(sizeof(AttrStack_t));
  }
                              AttrStack_t();
                              AttrStack_t(ObjectDescriptor_t *objDesc);
  void                        push(PartObject_t* po, unsigned int index);
  void                        pop(PartObject_t** po, unsigned int *index);
  //attr_t*                     peek(void);
  //void                        discard(void);
	void                        print(unsigned int index);
private:
  void                        resize(void) { 
                                cout << "RefStack_t::resize(void): _*_*_To" 
	                        << " be implemented_*_*_\n";}
    //calls Thread to make it point to a new bigger/smaller version of this stack 
  //ObjectDescriptor_t         *objectDescriptor;
  RefStack_t<PartObject_t*>   *partObjectStack;
  NonRefStack_t<unsigned int> *indexStack;
};

#endif


