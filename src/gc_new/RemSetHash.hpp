//$Id: RemSetHash.hpp,v 1.6 2001/05/23 08:43:44 ilsoe Exp $

#ifndef REM_SET_HASH_HPP
#define REM_SET_HASH_HPP

#include <iostream.h>
#include <stdlib.h>

template <class T_t> class Iterator_t;
template <class T_t> class HashMap_t;

/*
  NOTES ABOUT ITERATORS:
  1. Never instantiate an iterator directly - go through the HashMap_t
  2. Never insert in a hashmap where you are using an iterator and use
     the iterator afterwards - the map may be resized and the pointer
     in the iterator will become invalid.
*/

//prime list taken from stl_hashtable.h
const int num_primes = 28;
extern const unsigned long prime_list[];
const float remset_fill_thr = 0.75;     //used to calculate the bucketLimit
                                        //1 means fill up completely,
                                        //0 means no elements allowed

template <class T_t>
class Iterator_t {
public:
  Iterator_t(HashMap_t<T_t> *hmap): hMapRef(hmap) {
    //cout << "Iterator constructor run\n";
  }

  ~Iterator_t() {
    //cout << "destructor for Iterator run\n";
  }

  bool operator == (const Iterator_t<T_t> &comp) {
    //cout << "operator == run\n";
    return (comp.elementPtr == elementPtr);
  }

  bool operator != (const Iterator_t<T_t> &comp) {
    //cout << "operator != run\n";
    return (comp.elementPtr != elementPtr);
  }

  T_t operator * () {
    //cout << "indirection operator run\n";
    return *elementPtr;
  }

  //postfix (t++)
  T_t operator ++ (int) {
    //cout << "postfix operator run\n";
    T_t *oldElementPtr = elementPtr;
    //use prefix operator...
    ++(*this);
    return *oldElementPtr;
  }

  //prefix (++t)
  T_t operator++() {
    //cout << "prefix operator run\n";
    T_t *endElementPtr = &(hMapRef->hashMap[hMapRef->mapSize]);
    for(T_t *elmIndexPtr = elementPtr + 1; elmIndexPtr < endElementPtr;
	++elmIndexPtr) {
      if(*elmIndexPtr) {
	elementPtr = elmIndexPtr;
	return *elementPtr;
      }
    }
    //no more elements? set to end iterator then
    elementPtr = endElementPtr;
    return *elementPtr;
  }

private:
  HashMap_t<T_t> *hMapRef;
  T_t *elementPtr;

  friend class HashMap_t<T_t>;
};


template <class T_t>
class HashMap_t {
public:
  HashMap_t() : nextSizeIndex(1), mapSize(prime_list[0]), usedBuckets(0),
		bucketLimit((size_t)(prime_list[0]*remset_fill_thr))
  {
    hashMap = new T_t[mapSize + 1];
    //clear space
    for(T_t *index = hashMap, *endHashMap = &hashMap[mapSize];
	index <= endHashMap; ++index) {
      *index = 0;
    }
    endIterator = new Iterator_t<T_t>(this);
    endIterator->elementPtr = &hashMap[mapSize]; //element _after_ array
    indexIterator = new Iterator_t<T_t>(this);
    indexIterator->elementPtr = &hashMap[mapSize];
    //NOTE: Keep this consistent with clear()
  }

  ~HashMap_t() {
    delete indexIterator;
    delete endIterator;
    delete []hashMap;
    //NOTE: Keep this consistent with clear()
  }

  void insert(T_t elem) {
    //    _empty = false;
    if(usedBuckets < bucketLimit) {
      //not too filled up

      size_t index = hashFkt(elem);
      /*
      T_t elmAtIndex = hashMap[index];
      
      if(!elmAtIndex) {
	//empty bucket, insert and return
	hashMap[index] = elem;
	++usedBuckets;
	return;
      }
      
      if(elmAtIndex == elem) {
	//element already inserted, just return
	return;
      }
      */

      //had a collision? search for free bucket from index+1 to end of map
      for(T_t *i = &hashMap[index], *stopElement = &hashMap[mapSize];
	  i < stopElement; ++i) {
	if(!*i) {
	  *i = elem;
	  ++usedBuckets;
	  return;
	}
	if(*i == elem) {
	  //no need to reinsert element
	  return;
	}
      }
      //continue search from start of map to index
      for(T_t *i = hashMap, *stopElement = &hashMap[index];
	  i < stopElement; ++i) {
	if(!*i) {
	  *i = elem;
	  ++usedBuckets;
	  return;
	}
	if(*i == elem) {
	  //no need to reinsert element
	  return;
	}
      }
      //hash map is full!!! make it bigger and then insert our element
      grow();
      insert(elem);
      return; //phew!
    } else {
      //hash map is getting full - make it bigger
      grow();
      insert(elem);
    }
  }

  T_t lookup(T_t elem) {
    size_t index = hashFkt(elem);
    if(hashMap[index] == elem) {
      return elem;
    }
    for(T_t *i = &hashMap[index + 1], *stopElement = &hashMap[mapSize];
	i < stopElement; ++i) {
      if(*i == elem) {
	return elem;
      }
      if(!*i) {
	return 0;
      }
    }
    for(T_t *i = hashMap, *stopElement = &hashMap[index];
	i < stopElement; ++i) {
      if(*i == elem) {
	return elem;
      }
      if(!*i) {
	return 0;
      }
    }
    return 0;
  }

  //return iterator pointing to the first element
  Iterator_t<T_t> getIterator() {
    if(!usedBuckets) {
      return *indexIterator;
    }
    for(T_t *elmPtr = hashMap, *endElmPtr = &hashMap[mapSize];
	elmPtr < endElmPtr; ++elmPtr) {
      if(*elmPtr) {
	indexIterator->elementPtr = elmPtr;
	return *indexIterator;
      }
    }
    cerr << "internal error in Iterator<>::getIterator():"
      " marked not empty, but could not find element!\n";
    exit(1);
  }

  //return iterator pointing at the end (actually after)
  Iterator_t<T_t> getEndIterator() {
    return *endIterator;
  }
  
  //is the map empty?
  bool empty() {
    return !usedBuckets;
  }

  //delete all contents!
  void clear() {
    //keep this consistent with constructor/destructor!
    delete []hashMap;
    nextSizeIndex = 1;
    mapSize = prime_list[0];
    usedBuckets = 0;
    bucketLimit = (size_t)(mapSize*remset_fill_thr);
    hashMap = new T_t[mapSize + 1];

    //clear new space
    for(T_t *index = hashMap, *endHashMap = &hashMap[mapSize];
	index <= endHashMap; ++index) {
      *index = 0;
    }

    indexIterator->elementPtr = &hashMap[mapSize]; //element _after_ array
    endIterator->elementPtr   = &hashMap[mapSize]; //element _after_ array
  }

  void dumpRemSet(void* tmpVMO, size_t length, ostream &stream) {
    stream << "dump remset not implemented\n";
  }

private:

  size_t hashFkt(T_t elem) {
    return ((size_t)elem<<3)%mapSize;
  }

  void grow() {
    if(nextSizeIndex > num_primes) {
      cerr << "HashMap_t::grow(): error - hashmap cannot grow any larger\n";
      exit(1);
    }
    T_t *oldHashMap = hashMap;
    size_t oldMapSize = mapSize;
    mapSize = prime_list[nextSizeIndex++];
    hashMap = new T_t[mapSize + 1];
    //clear space
    for(T_t *index = hashMap, *endHashMap = &hashMap[mapSize];
	index <= endHashMap; ++index) {
      *index = 0;
    }

    endIterator->elementPtr = &hashMap[mapSize]; //element _after_ array
    //cout << "resizing hashmap, new size: " << mapSize << endl;
    //do a rehash
    usedBuckets = 0;
    bucketLimit = (size_t)(mapSize*remset_fill_thr);
    for(T_t *i = oldHashMap, *stopElement = &oldHashMap[oldMapSize];
	i < stopElement; ++i) {
      if(*i) {
	insert(*i);
      }
    }
    delete []oldHashMap;
  }

private:
  T_t                        *hashMap; //pointer to the actual hashmap
  int                         nextSizeIndex;
  size_t                      mapSize; //in <T_t>s
  size_t                      usedBuckets;
                                //the currently used number of buckets
  size_t                      bucketLimit;
                                //the max number of buckets we allow in the map
  Iterator_t<T_t>            *indexIterator;
  Iterator_t<T_t>            *endIterator;

  friend class Iterator_t<T_t>;
};

#endif //REM_SET_HASH_HPP
