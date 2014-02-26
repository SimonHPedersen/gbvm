//$Id: RememberedSet.hpp,v 1.10 2001/04/25 17:26:57 ilsoe Exp $

#ifndef _REMEMBEREDSET_HPP_
#define _REMEMBEREDSET_HPP_

#ifndef STL_REMSET

typedef HashMap_t<void**> RememberedSet_t;

typedef Iterator_t<void**> remSetIt_t;

#else //STL_REMSET reference stl implementation

class RememberedSet_t;

/*
ostream& operator << (ostream& stream, RememberedSet_t* rs, void** tmpVMP, size_t length);
ostream& operator << (ostream& stream, RememberedSet_t& rs, void** tmpVMP, size_t length);
*/

#include <hash_set>

//magic indirection - get value of remembered reference
//#define IT_2_RREF_VAL(iterator) ((void*)(*((unsigned int*)*iterator)))

template <>
class hash<void**>{
public:
  size_t operator()(void** const v1) const{
    hash<int> h;
    return h((int)v1);
  }
};

class EqVoid{
public:
  bool operator()(void** const v1, void** const v2) const
  {
    return (v1 == v2);
  }
};

typedef hash_set<void**, hash<void**>, EqVoid> HashSet_t;

typedef hash_set<void**, hash<void**>, EqVoid>::const_iterator remSetIt_t;

/*
template <class ForwardIterator, class ForwardIterator>
void sequence_delete(ForwardIterator first, ForwardIterator last) {
  while (first != last)
    delete *first++;
}
*/

class RememberedSet_t{
public:
                              RememberedSet_t(){}
                              ~RememberedSet_t();
  void                      **lookup(void** v);
  void                        insert(void** v);
  void                        remove(void** v);
  remSetIt_t                  getIterator(void);
  remSetIt_t                  getEndIterator(void);
  void                        clear(void);
  bool                        empty(void);
  void                        dumpRemSet(void* tmpVMO, size_t length, ostream& stream);
private:
  HashSet_t                   remSet;
};


/*
void lookup(const hash_set<void*, hash<void*>, eq>& Set,
            const void* v)
{
  hash_set<const char*, hash<const char*>, eqstr>::const_iterator
    = Set.find(word);
  cout << word << ": "
       << (it != Set.end() ? "present" : "not present")
       << endl;
}

int main(){
  //hash_set Set = new hash_set();
  hash_set<void*, hash<void*>, EqVoid> Set;
  int p = 7;
  void* pp1 = &p;
  Set.insert(pp1);
  Set.insert(pp1);
  //hash_set<hash_set<void*, hash<void*>, eq>::const_iterator it = Set.find(pp1);
  cout << pp1 << ", " << *Set.find(pp1) << ", " << *((int*)*Set.find(pp1)) << endl;
}
*/

#endif //STL_REMSET

#endif //_REMEMBEREDSET_HPP_
