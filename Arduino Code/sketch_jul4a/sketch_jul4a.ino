#include "Arduino.h"
#ifndef HVMod
#define HVMod


void setup() {
  // put your setup code here, to run once:

  SPI.begin();
}

class HVMod{

private:
  int Vmon;
  int Imon;
  int Vprog;
  
public:
  HVMod(int Vmon, int Imon, int Vprog)
  void setVoltage(int voltage);
  void getVoltage();
  void getCurrent();  
};

HVmod::setVoltage(int voltage){
  digitalWrite(chipSelectPin, LOW); //pull down the CS pin
  
  SPI.transfer(address); // address for device, for example 0x00
  SPI.transfer(value); // value to write

  digitalWrite(chipSelectPin, HIGH); // pull up the CS pin
}

HVMod::getVoltage(){
  
}


#endif
