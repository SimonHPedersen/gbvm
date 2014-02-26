//$Id: DirtyCars.cc,v 1.9 2001/03/28 15:01:28 ilsoe Exp $

#include "gc_new.hpp"

//----- operators ----

ostream& operator << (ostream& stream, DirtyCars_t* dc){
  if(dc){
    dc->printOn(stream);
  } else {
    stream << "*** tried to print a null-dirty-carset ***\n";
  }
  return stream;
}

ostream& operator << (ostream& stream, DirtyCars_t& dc){
  dc.printOn(stream);
  return stream;
}

DirtyCars_t::~DirtyCars_t(){
  //cout << "before delete(dirtyCars) \n";
  //dirtyCars.~hash_set();

  dirtyCarsIt_t start = dirtyCars.begin();
  dirtyCarsIt_t end = dirtyCars.end();
  
  /*
  while(start != end){
    delete *start++;
  }
  */
  
  destroy(start,end);
  //delete(dirtyCars);

}

Car_t* DirtyCars_t::getFirst(){
  return *(dirtyCars.begin());
}

Car_t* DirtyCars_t::lookup(Car_t* c1){
  dirtyCarsIt_t it = dirtyCars.find(c1);
  if(it == dirtyCars.end()){
    return NULL;
  } else {
    return *it;
  }
}

void DirtyCars_t::insert(Car_t* c1){
  dirtyCars.insert(c1);
}

void DirtyCars_t::remove(Car_t* c1){
  dirtyCars.erase(c1);
}

dirtyCarsIt_t DirtyCars_t::getIterator(){
  return dirtyCars.begin();
}

dirtyCarsIt_t DirtyCars_t::getEndIterator(){
  return dirtyCars.end();
}

void DirtyCars_t::clear(){
  dirtyCars.clear();
}

bool DirtyCars_t::empty(){
  return dirtyCars.empty();
}

void DirtyCars_t::printOn(ostream& stream){
  if(empty()){
    stream << "Dirty cars: empty";
  } else {
    stream << "Dirty cars: " << endl;
    for(dirtyCarsIt_t i = getIterator(); i != getEndIterator(); i++){
      stream << ", " << (void*)(Car_t*)*i;
    }
  }
}
