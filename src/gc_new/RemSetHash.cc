//$Id: RemSetHash.cc,v 1.5 2001/05/21 11:42:44 hempe Exp $

#ifdef DEBUG_MAIN
# include "RemSetHash.hpp"
#else //DEBUG_MAIN
# include "gc_new.hpp"
#endif //DEBUG_MAIN

/*
  NOTES ABOUT ITERATORS:
  1. Never instantiate an iterator directly - go through the HashMap_t
  2. Never insert in a hashmap where you are using an iterator and use
     the iterator afterwards - the map may be resized and the pointer
     in the iterator will become invalid.
*/

//prime list taken from stl_hashtable.h
//const int num_primes = 28;
const unsigned long prime_list[num_primes] =
{
  53ul,         97ul,         193ul,       389ul,       769ul,
  1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
  49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
  1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
  50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul, 
  1610612741ul, 3221225473ul, 4294967291ul
};

#ifdef DEBUG_MAIN

int main(int argc, char **argv) {
  char c1 = '1';
  char c2 = '2';
  char ca[100];

  HashMap_t<char*> map;
  map.insert(&c1);
  map.insert(&c1);
  map.insert(&c2);
  for(int i = 0; i < 100; i++) {
    ca[i] = 'a';
    map.insert(&ca[i]);
  }
  cout << "element " << *map.lookup(&c1) << endl;

  /* This crashes since iterators pointer is not properly initialized
     instead you should use member functions on HashMap_t to obtain
     properly initialized iterators.

     Iterator_t<char*> it(&map);
     cout << "main 1\n";
     it++;
     cout << "main 1\n";
     ++it;
     cout << "main 1\n";
     *it;
     cout << "main 1\n";
  */

  Iterator_t<char*> it2 = map.getIterator();
  Iterator_t<char*> endit = map.getEndIterator();
  int i = 1;
  while(it2 != endit) {
    cout << "element " << i++ << ": " << **it2 << endl;
    ++it2;
  }
  //delete it2;
  if(map.empty()) {
    cout << "ERROR: map reported empty, when this is clearly not the case!\n";
    exit(1);
  }
  map.clear();
  if(!(map.empty())) {
    cout << "ERROR: map reported non-empty, when it should be empty!\n";
    exit(1);
  }
  it2 = map.getIterator();
  endit = map.getEndIterator();
  if(*it2 != *endit) {
    cout << "ERROR: empty map end iterator error 1!\n";
  }
  if(it2 != endit) {
    cout << "ERROR: empty map end iterator error 2!\n";
  }
  if(!(it2 == endit)) {
    cout << "ERROR: empty map end iterator error 3!\n";
  }

  ++it2;
  if(*it2 != *endit) {
    cout << "ERROR: empty map end iterator error 4!\n";
  }
  if(it2 != endit) {
    cout << "ERROR: empty map end iterator error 5!\n";
  }
  if(!(it2 == endit)) {
    cout << "ERROR: empty map end iterator error 6!\n";
  }

  return 0;
}

#endif //DEBUG_MAIN
