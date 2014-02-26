//$Id: Lists.hpp,v 1.6 2001/03/13 19:30:59 ilsoe Exp $
// ----------------------------------------------------------------------
// Primitive list support

#ifndef LISTS_H
#define LISTS_H

#include "globals.hpp"

// Singly-linked list

template <class T_t> class Lst;

template <class T_t>
class LstElm {
public:
  LstElm(): _next(0) {}
  T_t* next() { return _next; }
private:
  T_t* _next;
  friend class Lst<T_t>;
};

template <class T_t>
class Lst {
public:
  Lst(): count(0), head(0), tail(0) {}
  void add(T_t* elm) {
    if (tail) {
      tail->_next=elm;
      tail=elm;
    } else {
      head=tail=elm;
    };
    count++;
  };
  T_t* first() { return head; }
  T_t* last()  { return tail; }
  unsigned int getCount(void) {
    return count;
  }
private:
  unsigned int count;
  T_t* head;
  T_t* tail;
};

// Doubly-linked list

template <class T_t> class DLst;

template <class T_t>
class DLstElm {
public:
  DLstElm(): _next(0),_prev(0) {}
  T_t* next() { return _next; }
  T_t* prev() { return _prev; }

  /*
  unsigned int get_nextOffset() {
    //returns the offset to the _next pointer (in void pointers)
    return ((unsigned int)&_next - (unsigned int)this)/sizeof(void*);
  }

  unsigned int get_prevOffset() {
    //returns the offset to the _prev pointer (in void pointers)
    return ((unsigned int)&_prev - (unsigned int)this)/sizeof(void*);
  }
  */

  //private:
  T_t* _next;
  T_t* _prev;
  friend class DLst<T_t>;
};

template <class T_t>
class DLst {
public:
  DLst(): head(0), tail(0) {}
  void add(T_t* elm, void* objectAddress) {
    DB_INDENT;
    if (tail) { 
      // doing tail->_next=elm:
      DB_OUT(cmn_lists, "DLst::add(): doing tail->_next=elm \n");
      setVMReference(tail, (void**)&(tail->_next), elm); 
      DB_OUT(cmn_lists, "DLst::add(): done tail->_next=elm \n");
      // doing elm->_prev=tail; 
      setVMReference(elm, (void**)&(elm->_prev), tail); 
      DB_OUT(cmn_lists, "DLst::add(): done elm->_prev=tail \n");
      // doing tail=elm;
      setVMReference(objectAddress, (void**)&tail, elm); 
      DB_OUT(cmn_lists, "DLst::add(): done elm->_prev=tail \n");
    } else { 
      // doing head=tail=elm
      DB_OUT(cmn_lists, "DLst::add(): doing tail=elm this: " << this << endl);
      setVMReference(objectAddress, (void**)&tail, elm); 
      DB_OUT(cmn_lists, "DLst::add(): done tail=elm \n");
      setVMReference(objectAddress, (void**)&head, elm); 
      DB_OUT(cmn_lists, "DLst::add(): done head=elm \n");
    };
    DB_OUTDENT;
  }
  T_t* first() { return head; }
  T_t* last() { return tail; }
  
  /*
  unsigned int getHeadOffset() {
    //returns the offset to the head pointer (in void pointers)
    DB_OUT(cmn_lists, "DList::getHeadOffset(): " 
	 << ((unsigned int)&head - (unsigned int)this)/sizeof(void*) << endl;
    return ((unsigned int)&head - (unsigned int)this)/sizeof(void*);
  }

  unsigned int getTailOffset() {
    //returns the offset to the tail pointer (in void pointers)
    DB_OUT(cmn_lists, "DList::getTailOffset(): " 
	 << ((unsigned int)&tail - (unsigned int)this)/sizeof(void*) << endl;
    return ((unsigned int)&tail - (unsigned int)this)/sizeof(void*);
  }
  */
  
  //private:
  T_t* head;
  T_t* tail;
};

#endif
