//$Id: VMOHash.cc,v 1.1 2001/04/03 16:11:09 ilsoe Exp $

#include "gc_new.hpp"

//----- operators ----

ostream& operator << (ostream& stream, VMOHash_t* vmoset){
  if(vmoset){
    vmoset->printOn(stream);
  } else {
    stream << "*** tried to print a null-vmohash-set ***\n";
  }
  return stream;
}

ostream& operator << (ostream& stream, VMOHash_t& vmoset){
  vmoset.printOn(stream);
  return stream;
}

VMOHash_t::~VMOHash_t(){
  //cout << "before delete(vmohash) \n";
  //vmohash.~hash_set();

  VMOHashIt_t start = vmohash.begin();
  VMOHashIt_t end = vmohash.end();
  
  /*
  while(start != end){
    delete *start++;
  }
  */
  
  destroy(start,end);
  //delete(vmohash);

}

VMObject_t* VMOHash_t::getFirst(){
  return *(vmohash.begin());
}

VMObject_t* VMOHash_t::lookup(VMObject_t* c1){
  VMOHashIt_t it = vmohash.find(c1);
  if(it == vmohash.end()){
    return NULL;
  } else {
    return *it;
  }
}

void VMOHash_t::insert(VMObject_t* c1){
  vmohash.insert(c1);
}

void VMOHash_t::remove(VMObject_t* c1){
  vmohash.erase(c1);
}

VMOHashIt_t VMOHash_t::getIterator(){
  return vmohash.begin();
}

VMOHashIt_t VMOHash_t::getEndIterator(){
  return vmohash.end();
}

void VMOHash_t::clear(){
  vmohash.clear();
}

bool VMOHash_t::empty(){
  return vmohash.empty();
}

void VMOHash_t::printOn(ostream& stream){
  if(empty()){
    stream << "VMOreferences: empty";
  } else {
    stream << "VMOreferences: " << endl;
    for(VMOHashIt_t i = getIterator(); i != getEndIterator(); i++){
      stream << ", " << (void*)(VMObject_t*)*i;
    }
  }
}
