// $Id: StackSpace.hpp,v 1.12 2001/06/08 10:26:24 ilsoe Exp $

#ifndef _STACK_SPACE_HPP_
#define _STACK_SPACE_HPP_

class StackSpace_t;

ostream& operator << (ostream& stream, StackSpace_t* ss);
ostream& operator << (ostream& stream, StackSpace_t& ss);

#include "gc_new.hpp"

class StackSpace_t {
public:
                              StackSpace_t(size_t bytes);
                              ~StackSpace_t();
  bool                        member(void *address) {
                                return (address >= stackSpacePtr && address < freePtr);
                              }
  VMObject_t                 *allocStackVMO(ObjectDescriptor_t *objDesc);
  int                         moveObjects(void);
                                //moves all objects referenced from the stacks
                                //return value indicates whether the first
                                //train is ext referenced from the stacks
                                //(except for the first car)
  void                        printOn(ostream& stream);
#ifdef LIVESTAT
  void                        doMarking(void);
                                //marks all the live objects' fwdFlag/Ptr
  void                        doLiveStat(size_t *live, size_t *dead, size_t *unused);
#endif //LIVESTAT
#ifdef DEBUG
  void                        gatherObjRefs(VMOHash_t *vmoRefs);
  int                         checkObjRefs (VMOHash_t *vmoRefs);
#endif //DEBUG
private:
  size_t                      stackSpaceSize;
                                //the size of the stack space in bytes
  void                       *freePtr;
                                //pointer to the first free byte
  void                      **stackSpacePtr;
                                //pointer to the allocated space
#ifdef undefined //LIVESTAT
public:
  int                         liveObjects;
  int                         deadObjects;
  size_t                      liveBytes;
  size_t                      deadBytes;
#endif //LIVESTAT
};

#endif

