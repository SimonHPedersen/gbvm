//$Id: testTrainTable.cc,v 1.1 2001/03/16 08:26:19 ilsoe Exp $

//#include "TrainTable.hpp"
//#include "Car.hpp"
#include "allocator.hpp"

int main(){
  void * carSpace;
 
  carSpace = malloc(BYTES_IN_CAR);
  Car_t* c1 = new(carSpace) Car_t();
  cout << "c1: " << (void*)c1 << endl;
  carSpace = malloc(BYTES_IN_CAR);
  Car_t* c2 = new(carSpace) Car_t();
  cout << "c2: " << (void*)c2 << endl;
  carSpace = malloc(BYTES_IN_CAR);
  Car_t* c3 = new(carSpace) Car_t();
  cout << "c3: " << (void*)c3 << endl;

  TrainTable_t* tt = new TrainTable_t();
  tt->addTrain(c1,c2);
  tt->addTrain(c1,c3);
  tt->addTrain(c2,c2);
  tt->addTrain(c3,c1);
  cout << "getLastCar(0) : " << (void*)tt->getLastCar(0) << endl;
  cout << "getLastCar(1) : " << (void*)tt->getLastCar(1) << endl;
  cout << "getLastCar(2) : " << (void*)tt->getLastCar(2) << endl;
  cout << "getLastCar(3) : " << (void*)tt->getLastCar(3) << endl;
  cout << "getFirstCar(0) : " << (void*)tt->getFirstCar(0) << endl;
  cout << "getFirstCar(1) : " << (void*)tt->getFirstCar(1) << endl;
  cout << "getFirstCar(2) : " << (void*)tt->getFirstCar(2) << endl;
  cout << "getFirstCar(3) : " << (void*)tt->getFirstCar(3) << endl;
  //cout << "getFirstCar(4) : " << (void*)tt->getFirstCar(4) << endl;
  
  tt->setFirstCar(3, NULL);
  tt->setLastCar(3, NULL);
  cout << "getFirstCar(3) : " << (void*)tt->getFirstCar(3) << endl;
  cout << "getLastCar(3) : " << (void*)tt->getLastCar(3) << endl;

  tt->removeTrain();
  tt->addTrain(c1,c1);
  tt->removeTrain();
  tt->addTrain(c2,c2);
  tt->removeTrain();
  tt->addTrain(c3,c3);
  cout << "getLastCar(3) : " << (void*)tt->getLastCar(3) << endl;
  cout << "getLastCar(4) : " << (void*)tt->getLastCar(4) << endl;
  cout << "getLastCar(5) : " << (void*)tt->getLastCar(5) << endl;
  cout << "getLastCar(6) : " << (void*)tt->getLastCar(6) << endl;
  cout << "getFirstCar(3) : " << (void*)tt->getFirstCar(3) << endl;
  cout << "getFirstCar(4) : " << (void*)tt->getFirstCar(4) << endl;
  cout << "getFirstCar(5) : " << (void*)tt->getFirstCar(5) << endl;
  cout << "getFirstCar(6) : " << (void*)tt->getFirstCar(6) << endl;
  //cout << "getFirstCar(7) : " << (void*)tt->getFirstCar(7) << endl;
  tt->setFirstCar(7,NULL);
}
