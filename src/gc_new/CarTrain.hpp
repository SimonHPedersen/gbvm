// $Id: CarTrain.hpp,v 1.2 2001/05/30 08:39:45 ilsoe Exp $

#ifndef _CARTRAIN_HPP_
#define _CARTRAIN_HPP_

class CarTrain_t;

const unsigned int HALF_INT_BITS = sizeof(unsigned int)*4; //4 = bits in char/2. typically 16
const unsigned int LOW_HALF_INT_MASK = (1<<HALF_INT_BITS)-1; //typically 0x0000ffff
const unsigned int HIGH_HALF_INT_MASK = ~LOW_HALF_INT_MASK;  //typically 0xffff0000
const unsigned int HALF_INT_MAX = LOW_HALF_INT_MASK;

class CarTrain_t {
public:
                              CarTrain_t(void): trainAndCar(0) {};
  void                        setTrain(unsigned int _train)
                                {trainAndCar = (trainAndCar & LOW_HALF_INT_MASK) //delete old train
						| (_train<<HALF_INT_BITS);} //or new train into trainAndCar
  unsigned int                getTrain(void)
                                {return trainAndCar >> HALF_INT_BITS;}
  void                        setCar(unsigned int _car)
                                {trainAndCar = (trainAndCar & HIGH_HALF_INT_MASK)
				                | _car;}
  unsigned int                getCar(void) {return trainAndCar & LOW_HALF_INT_MASK;}

  unsigned int                getTrainAndCar(void) {return trainAndCar;}
  void                        setTrainAndCar(unsigned int train, unsigned int car)
                                {
				  trainAndCar = (train <<HALF_INT_BITS) | car;
				}
  
  // --- operators ---
  bool operator <  (CarTrain_t& ct) {
    return (getTrainAndCar() < ct.getTrainAndCar());
  }
  
  bool operator >  (CarTrain_t& ct) {
    return (getTrainAndCar() > ct.getTrainAndCar());
  }
  
  bool operator <= (CarTrain_t& ct) {
    return (getTrainAndCar() <= ct.getTrainAndCar());
  }
  
  bool operator >= (CarTrain_t& ct) {
    return (getTrainAndCar() >= ct.getTrainAndCar());
  }
  
  bool operator == (CarTrain_t& ct) {
    return (getTrainAndCar() == ct.getTrainAndCar());
  }

private:
  unsigned int                trainAndCar; //train has the first half, car the second
};


#endif //_CARTRAIN_HPP_
