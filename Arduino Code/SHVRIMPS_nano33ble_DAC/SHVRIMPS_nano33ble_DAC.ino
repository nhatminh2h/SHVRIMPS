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
uint32_t adcbuffer;
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

uint32_t readfromADC2_32bit(uint8_t message){
  digitalWrite(ADC2_CS, LOW);
  SPI.beginTransaction(ADCsettings);
  SPI.transfer(message);
  //SPI.transfer(&message,4);
  SPI.endTransaction();

  SPI.beginTransaction(ADCsettingsRead); //page 
  adcbuffer = 0;
  
  adcbuffer = SPI.transfer(0x00);
//  adcbuffer = (adcbuffer << 8) | SPI.transfer(0x00);
//  adcbuffer = (adcbuffer << 8) | SPI.transfer(0x00);
//  adcbuffer = (adcbuffer << 8) | SPI.transfer(0x00);
  
  adcbuffer = (uint32_t)(adcbuffer << 8) | (uint32_t)SPI.transfer(0x00);
  adcbuffer = (uint32_t)(adcbuffer << 8) | (uint32_t)SPI.transfer(0x00);
  adcbuffer = (uint32_t)(adcbuffer << 8) | (uint32_t)SPI.transfer(0x00);

//  adcbuffer = adcbuffer >> 4;

  SPI.endTransaction();
  digitalWrite(ADC2_CS, HIGH);

  //Serial.println((adcbuffer & 0xffff));
  return((adcbuffer ));
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
//  messagetoADC = 0b0101101000001000;//set MUX to CH0(bit4-8:0000) and AGND(bit 0-3:1000). See page 77 datasheet
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
  SerialUSB.begin(115200);
  
  //setting up SPI
  delay(2000);
  SPI.begin();
  
  //creating DAC
  pinMode(DAC_CS, OUTPUT);
  digitalWrite(DAC_CS, HIGH);
  delay(100);

  pinMode(LDAC, OUTPUT);
  digitalWrite(LDAC, LOW);
  
  messagetoDAC = DAC8568_Setup(CONTROL_SETUP_FLEX_MODE, DATA_POWER_UP_INT_REF_ALWAYS_FLEX, FEATURE_POWER_UP_INT_REF_ALWAYS_FLEX);
  //messagetoDAC = 0b0000100100001010000000000000000;
  writetoDAC(messagetoDAC);
  delay(10);

  
}

uint32_t arr[] = 
{0x8000,0x8c8b,0x98f8,0xa527,0xb0fb,0xbc56,0xc71c,0xd133, 
0xda82,0xe2f1,0xea6d,0xf0e2,0xf641,0xfa7c,0xfd89,0xff61, 
0xffff,0xff61,0xfd89,0xfa7c,0xf641,0xf0e2,0xea6d,0xe2f1, 
0xda82,0xd133,0xc71c,0xbc56,0xb0fb,0xa527,0x98f8,0x8c8b, 
0x8000,0x7374,0x6707,0x5ad8,0x4f04,0x43a9,0x38e3,0x2ecc,
0x257d,0x1d0e,0x1592,0xf1d,0x9be,0x583,0x276,0x9e, 
0x0,0x9e,0x276,0x583,0x9be,0xf1d,0x1592,0x1d0e, 
0x257d,0x2ecc,0x38e3,0x43a9,0x4f04,0x5ad8,0x6707,0x7374};

uint16_t epsilon = 10;
uint32_t adcRead;

void loop() {
      messagetoDAC = DAC8568_Write_Input_Reg_And_Update_Single(CH_A, arr[i]);
      writetoDAC(messagetoDAC);

      messagetoDAC = DAC8568_Write_Input_Reg_And_Update_Single(CH_B, arr[(i+ 32)%64]);
      writetoDAC(messagetoDAC);
      delay(epsilon);
      
      Serial.print(arr[i]);

      messagetoADC = 0b0101101000111000; //set MUX to CH4(bit4-8:0011) and AGND(bit 0-3:1000). See page 77 datasheet
      writetoADC2(messagetoADC);
      
      Serial.print(" ");
      messagetoADC = 0b01000001; //read from ADC_DATA
      adcRead = readfromADC2_32bit(messagetoADC);
      Serial.print(adcRead);

      delay(epsilon);

      Serial.print(" ");
      Serial.print(arr[(i+ 32)%64]);

      messagetoADC = 0b0101101000101000; //set MUX to CH3(bit4-8:0010) and AGND(bit 0-3:1000). See page 77 datasheet
      writetoADC2(messagetoADC);

      Serial.print(" ");
      messagetoADC = 0b01000001; //read from ADC_DATA
      adcRead = readfromADC2_32bit(messagetoADC);
      Serial.print(adcRead);

      
      //Serial.print(" ");
      //Serial.print(( (int32_t)arr[i])  - (int32_t)(2 * adcRead));
      
      Serial.println();
      
      delay(epsilon);
      
  }
}
