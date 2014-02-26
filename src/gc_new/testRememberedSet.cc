//$Id: testRememberedSet.cc,v 1.7 2001/04/10 12:01:24 ilsoe Exp $

#include "RememberedSet.hpp"
#include <unistd.h>

//COMMENT: remembered sets generate unreclaimable garbage?!

int main(){

  /*  float tst = 31400.123456;

  cout << (float)tst << endl;
  */
  /*
  unsigned int size = 1000000;

  void* blocks[100];

  for(unsigned int i = 0; i < 100 ; i++){
    blocks[i] = malloc(size);
  }

  int* mem = (int*)malloc(sizeof(int) * size);

  for(unsigned int i = 0; i<size ; i++){
    mem[i] = 1;
  }

  cout << "allocated" << endl;

  sleep(10);
 
  free(mem);

  cout << "freed" << endl;
  
  for(unsigned int i = 0; i < 100 ; i++){
    free(blocks[i]);
  }
  
  sleep(10);
  */

  RememberedSet_t* rs = new RememberedSet_t();
  int i,j,k;
  void* ip = &i;
  void* jp = &j;
  void* kp = &k;

  for(unsigned int i = 0; i < 5000000; i++){
    rs->insert((void**)i);
  }
  /*
  for(unsigned int i = 0; i < 5000000; i++){
    rs->remove((void*)i);
  }
  */

  rs->insert(&ip);
  rs->insert(&jp);
  rs->insert(&kp);

  //  remSetIt_t it = rs->getIterator();

  cout << "ip: " << ip << ", " << rs->lookup(&ip) << endl;
  cout << "jp: " << jp << ", " << rs->lookup(&jp) << endl;
  cout << "kp: " << kp << ", " << rs->lookup(&kp) << endl;

  /*
  for( ; it != rs->getEndIterator(); it++){
    cout << *it << endl;
  }
  */

  //rs->clear();

  cout << "before delete \n";

  delete(rs);

  sleep(20);

}

