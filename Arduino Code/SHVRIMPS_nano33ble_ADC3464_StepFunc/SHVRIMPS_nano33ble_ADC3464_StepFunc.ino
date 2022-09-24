#include <dac8568.h>
#include <MCP3464.h>
#include <SPI.h>

//********************** ESP 32 Pin *******************//
//const int DAC_CS = ; //4
//const int LDAC = //17
//const int CLR //16

//const int ADC1_CS = ; //33
//const int ADC2_CS = ; //32
//const int ADC_MDAT = ; //35



//********************** NANO33 BLE Pin ***************//
const int DAC_CS = 2; //D2
const int LDAC = 4; //D4
const int CLR = 3; //D3
const int SCKL = 13; //D13

const int ADC1_CS = A2; //A2
const int ADC2_CS = A2 ; //A1
const int ADC_MDAT = A0; //A0

// SPI Settings for DAC and ADCs

SPISettings DACsettings(20000000, MSBFIRST, SPI_MODE1); //20 mhz

SPISettings ADCsettings(10000000, MSBFIRST, SPI_MODE0); //10 mhz. ADC latches SDI on rising edge
SPISettings ADCsettingsRead(10000000, MSBFIRST, SPI_MODE1); //10 mhz. ADC latches SDO on falling edge. See page 66 datasheet

// Variables for DAC and ADC

int readADC;
MCP3464 ADC1;
int32_t adcbuffer;
int messagetoADC;

uint32_t messagetoDAC;

// Functions

void writetoDAC(uint32_t message){
  //Serial.println("tick");
  digitalWrite(DAC_CS, LOW);
  SPI.beginTransaction(DACsettings);
  SPI.transfer(message >> 24);
  SPI.transfer(message >> 16);
  SPI.transfer(message >> 8);
  SPI.transfer(message);
  //SPI.transfer(&message,4);
  SPI.endTransaction();
  digitalWrite(DAC_CS, HIGH);
}

//ADC functions

void writetoADC2(uint16_t message){
  digitalWrite(ADC2_CS, LOW);
  SPI.beginTransaction(ADCsettings);
  //SPI.transfer(message >> 24);
  //SPI.transfer(message >> 16);
  //SPI.transfer(message >> 8);
  SPI.transfer16(message);
  //SPI.transfer(&message,4);
  SPI.endTransaction();
  digitalWrite(ADC2_CS, HIGH);
}

int steptracker = 0;

int32_t readfromADC2_32bit(uint8_t message){ //ADC returns signed int value
  uint8_t adcstatus = 0;
  digitalWrite(ADC2_CS, LOW);
  SPI.beginTransaction(ADCsettings);
  adcstatus = SPI.transfer(message);
  //SPI.transfer(&message,4);
  SPI.endTransaction();
  
  if (bitRead(adcstatus,2) == 1){
    Serial.print("repeated");
  }


  SPI.beginTransaction(ADCsettingsRead); //page 
  adcbuffer = 0;
  
  adcbuffer = SPI.transfer(0x00);
//  adcbuffer = (adcbuffer << 8) | SPI.transfer(0x00);
//  adcbuffer = (adcbuffer << 8) | SPI.transfer(0x00);
//  adcbuffer = (adcbuffer << 8) | SPI.transfer(0x00);
  
  adcbuffer = (int32_t)(adcbuffer << 8) | (int32_t)SPI.transfer(0x00);
  adcbuffer = (int32_t)(adcbuffer << 8) | (int32_t)SPI.transfer(0x00);
  adcbuffer = (int32_t)(adcbuffer << 8) | (int32_t)SPI.transfer(0x00);

  SPI.endTransaction();
  digitalWrite(ADC2_CS, HIGH);

  adcbuffer = adcbuffer << 4;
  adcbuffer = adcbuffer >> 4;//sign extension

  //Serial.println((adcbuffer & 0xffff));
  
  //if (steptracker == 100000){
    //delay(100000000);
  //}
  
//  if ((adcbuffer) < 3000 | steptracker !=0){
    //steptracker++;

//  }
  return((adcbuffer));
}

void readfromADC2_8bit(uint8_t message){
  digitalWrite(ADC2_CS, LOW);
  SPI.beginTransaction(ADCsettings);
  SPI.transfer(message);
  //SPI.transfer(&message,4);
  SPI.endTransaction();

  SPI.beginTransaction(ADCsettingsRead); //page 
  adcbuffer = 0;
  
  adcbuffer = SPI.transfer(0x00);
  Serial.println(adcbuffer,BIN);

//  adcbuffer = (uint32_t)(adcbuffer << 8) | (uint32_t)SPI.transfer(0x00);

//  adcbuffer = adcbuffer >> 4;

  SPI.endTransaction();

  Serial.println(adcbuffer,HEX);
  digitalWrite(ADC2_CS, HIGH);
}

//typedef enum {
//  CH0,
//  CH1,
//  CH2,
//  CH3,
//  CH4,
//  CH5,
//  CH6,
//  CH7,
//  AGND,
//  AVDD
//} ADC_CHANNEL_SELECT;
//
//void setMUXSingleEnd(ADC_CHANNEL_SELECT channel){
//  digitalWrite(ADC2_CS, LOW);
//
//  
//
//  messagetoADC = 0b0101101000001000;//set MUX to CH1(bit4-8:0000) and AGND(bit 0-3:1000). See page 77 datasheet
//  messagetoADC = messagetoADC | (channel << 4);
//  
//  SPI.beginTransaction(ADCsettings);
//  
//  writetoADC2(messagetoADC);
//  //SPI.transfer(&message,4);
//  SPI.endTransaction();
//  digitalWrite(ADC2_CS, HIGH);
//}

void setup() {
  Serial.begin(9600);
  
  //setting up SPI
  delay(2000);
  SPI.begin();
  
  //setting up ADC
  ADC1.init();

  //Serial.println("Reading before setting");

  //messagetoADC = 0b01010101;
  //readfromADC2_8bit(messagetoADC);

  messagetoADC = 0b0101001011110000; //set config3. 32 bit DATA_FORMAT
  writetoADC2(messagetoADC);
  
  messagetoADC = 0b0100011011110011; //set config0. Internal Clock (bit 4-5: 11)
  writetoADC2(messagetoADC);
  
  messagetoADC = 0b0100101000001100; //select config1 + write to config1
  writetoADC2(messagetoADC);
  
  messagetoADC = 0b0100111010001011; //set config2
  writetoADC2(messagetoADC);

  messagetoADC = 0b0101011000000111; //set IRQ to inactive logic high(bit 2: 1)
  writetoADC2(messagetoADC);

  messagetoADC = 0b0101101000001000; //set MUX to CH0(bit4-8:0000) and AGND(bit 0-3:1000). See page 77 datasheet
  writetoADC2(messagetoADC);

  //Serial.println("Reading after setting");

  //messagetoADC = 0b01010101;
  //readfromADC2_8bit(messagetoADC);
  delay(2000);
}

uint16_t epsilon = 1;
int32_t adcRead;

void loop() {
      messagetoADC = 0b01000001; //read from ADC_DATA
      adcRead = readfromADC2_32bit(messagetoADC);
      Serial.print(adcRead);
      Serial.println();
      
      //delayMicroseconds(epsilon);
      
}