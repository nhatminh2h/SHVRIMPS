#include <SPI.h>

const int cs_DAC = 20; //D2
const int cs_ADC1 = 6; //A2
const int cs_ADC2 = 5; //A1


void setup(){

  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));
}

