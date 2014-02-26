//$Id: RememberedSet.cc,v 1.13 2001/04/25 17:26:57 ilsoe Exp $

#include "gc_new.hpp"

#ifdef STL_REMSET

/*
//----- operators ----

ostream& operator << (ostream& stream, RememberedSet_t* rs, void* tmpVMO, size_t length){
  if(rs){
    rs->printOn(tmpVMO, length, stream);
  } else {
      } else {
    stream << "*** tried to print a null-car ***\n";
  }
  return stream;
}

ostream& operator << (ostream& stream, RememberedSet_t& rs, void* tmpVMP, size_t length){
  rs.printOn(tmpVMO, length, stream);
  return stream;
}
*/

//------ member functions ----

RememberedSet_t::~RememberedSet_t(){
  //cout << "before delete(remSet) \n";
  //remSet.~hash_set();

  remSetIt_t start = remSet.begin();
  remSetIt_t end = remSet.end();
  
  /*
  while(start != end){
    delete *start++;
  }
  */
  
  destroy(start,end);
  //delete(remSet);

  //cout << "after delete(remSet) \n";
}

void** RememberedSet_t::lookup(void** v1){
  remSetIt_t it = remSet.find(v1);
  if(it == remSet.end()){
    return NULL;
  } else {
    return *it;
  }
}

void RememberedSet_t::insert(void** v1){
  DB_INDENT;
  DB_OUT(gc_remset, "RememberedSet::insert(): inserting " << (void*)v1
	 << " in remembered set at: " << flush << (void*)this << "\n" << flush);
  remSet.insert(v1);
  DB_OUT(gc_remset, "RememberedSet::insert(): inserting " << (void*)v1
	 <<" - done\n");
  DB_OUTDENT;
}

void RememberedSet_t::remove(void** v1){
  remSet.erase(v1);
}

remSetIt_t RememberedSet_t::getIterator(){
  return remSet.begin();
}

remSetIt_t RememberedSet_t::getEndIterator(){
  return remSet.end();
}

void RememberedSet_t::clear(){
  remSet.clear();
}

bool RememberedSet_t::empty(){
  return remSet.empty();
}

void RememberedSet_t::dumpRemSet(void* tmpVMO, size_t length, ostream& stream){
  DB_INDENT;
  size_t tracedLength = 0;
  while(tracedLength < length) {
    bool haveWrittenHead = false;
    for(remSetIt_t i = getIterator(); i != getEndIterator(); i++){
      //cout << "has remset item: " << *i << " pointing to: " << **i << endl;
      if(**i == VOIDP_FROM_VMO(tmpVMO)){
	if(!haveWrittenHead) {
	  DB_INS_INDT(stream);
	  stream << "Remembered set for vmobject" << ((VMObject_t*)tmpVMO)->getID()  
	  << " at " << tmpVMO << " with car at " 
	  << (void*)Car_t::getCar(((void*)tmpVMO)) << endl;
	  haveWrittenHead = true;
	}
	DB_INS_INDT(stream); stream << " - " << *i << endl;
      }
    }
    tracedLength += ((VMObject_t*)tmpVMO)->getFullVMOLength();  
    tmpVMO = NEXT_VMO(((VMObject_t*)tmpVMO));
  }
  DB_OUTDENT;
}

#endif //STL_REMSET
