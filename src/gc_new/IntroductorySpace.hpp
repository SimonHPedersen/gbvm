// $Id: IntroductorySpace.hpp,v 1.17 2001/06/08 10:26:24 ilsoe Exp $

#ifndef _INTRODUCTORY_SPACE_HPP_
#define _INTRODUCTORY_SPACE_HPP_

class IntroductorySpace_t;

ostream& operator << (ostream& stream, IntroductorySpace_t* is);
ostream& operator << (ostream& stream, IntroductorySpace_t& is);

//const unsigned int INTROSP_EXP = 16;                       //typically 16
#ifndef INTROSP_EXP
#  define INTROSP_EXP 16
#endif //INTROPS_EXP

const unsigned int BYTES_IN_INTROSP = (1 << INTROSP_EXP);  //typically 0x00010000
const unsigned int INTROSP_MASK = ~(BYTES_IN_INTROSP - 1); //typically 0xffff0000

#include "gc_new.hpp"

class IntroductorySpace_t {
public:
                              IntroductorySpace_t();
                              ~IntroductorySpace_t();
  bool                        member(void *address) {
          return (introSpacePtr == (void*)((unsigned int)address & INTROSP_MASK));
                              }

  void                        moveObjects(void);
                                //move externally referenced objects out
  VMObject_t                 *allocVMO(ObjectDescriptor_t *objDesc);
  void                        reset(void);
                                //reset the introductory space so everything
                                //is considered free and no remembered sets exist
  void                        printOn(ostream& stream);
  void                        addRememberReference(void **refAdr) {
                                remSet->insert(refAdr);
                              }
#ifdef   LIVESTAT
  void                        doLiveStat(size_t *live, size_t *dead, size_t *unused);
#endif //LIVESTAT
#ifdef   DEBUG
  bool                        hasRememberReference(void** refAdr);
  void                        gatherObjRefs(VMOHash_t *vmoRefs);
  int                         checkObjRefs (VMOHash_t *vmoRefs);
#endif //DEBUG
private:
  //  size_t                      introSpaceSize;
                                //the size of the introductory space in bytes
  void                       *freePtr;
                                //pointer to the first free byte
  void                      **introSpacePtr;
                                //pointer to the allocated space
  RememberedSet_t            *remSet;
                                //remembered set for the introductory space
#ifdef undefined //LIVESTAT
public:
  int                         liveObjects;
  int                         deadObjects;
  size_t                      liveBytes;
  size_t                      deadBytes;
#endif //LIVESTAT
};

#endif
