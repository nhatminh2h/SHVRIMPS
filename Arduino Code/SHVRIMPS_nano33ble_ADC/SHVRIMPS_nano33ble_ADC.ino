#include <dac8568.h>
#include <MCP346X.h>
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
const int ADC2_CS = A1; //A1
const int ADC_MDAT = 4; //A0

SPISettings DACsettings(20000000, MSBFIRST, SPI_MODE1); //20 mhz
SPISettings ADCsettings(20000000, MSBFIRST, SPI_MODE0); //20 mhz

int readADC;
MCP346X ADC1(ADC1_CS, ADC_MDAT);

uint32_t messagetoDAC;

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

void setup() {
  //setting up SPI
  delay(2000);
  SPI.begin();
  
  //creating one ADC
  //MCP346X ADC1(ADC1_CS, ADC_MDAT);
  ADC1.begin();
  //writing to config registers
  ADC1.setConfig0(ADC1.CLK_INTERNAL, ADC1.CS_NONE, ADC1.ADC_MODE_CONV);
  ADC1.setConfig1(ADC1.MCLK_1, ADC1.OSR_256);
  ADC1.setConfig2(ADC1.CH_I_1, ADC1.GAIN_1, ADC1.AZ_MUX_OFF);
  ADC1.setConfig3(ADC1.CONV_CONTINUOUS, ADC1.DATA_FORM_16B, ADC1.CRC_FORM_16B, ADC1.CRC_COM_OFF, ADC1.OFFCAL_OFF, ADC1.GAINCAL_OFF);
  ADC1.setIRQ(ADC1.IRQ_PIN_IRQ, ADC1.IRQ_PINMODE_PP, ADC1.FASTCMD_ON, ADC1.EN_STP_ON);
  
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

  Serial.begin(9600);
}

void loop() {
  //messagetoDAC = 0b0000100100001010000000000000000;
  //writetoDAC(messagetoDAC);
  //delay(2000);

//  while(1){
//  messagetoDAC = DAC8568_Write_Input_Reg_And_Update_Single(BROADCAST,20);
//  Serial.println(messagetoDAC, HEX);
//  writetoDAC(messagetoDAC);
//  delay(1000);
//
//  messagetoDAC = DAC8568_Write_Input_Reg_And_Update_Single(BROADCAST,1000);
//  Serial.println(messagetoDAC, HEX);
//  writetoDAC(messagetoDAC);
//  delay(1000);
//
//  messagetoDAC = DAC8568_Write_Input_Reg_And_Update_Single(BROADCAST, 3589);
//  Serial.println(messagetoDAC, HEX);
//  writetoDAC(messagetoDAC);
//  delay(1000);
//
//  }
  messagetoDAC = DAC8568_Write_Input_Reg_And_Update_Single(BROADCAST, 65535);
  

  //messagetoDAC = 0b00000010111111111111111111110000;
  Serial.println(messagetoDAC, HEX);
  writetoDAC(messagetoDAC);
  
  ADC1.setMux(ADC1.MUX_CH0, ADC1.MUX_AGND);
  delay(500);
  readADC = ADC1.readRegister16(ADC1.REG_ADCDATA);
  delay(750);
  Serial.print("ADC: ");
  Serial.println(readADC);
  
  delay(2000);
  

  //messagetoDAC = 0b00000010000000000000000000000000;
  messagetoDAC = DAC8568_Write_Input_Reg_And_Update_All(BROADCAST,0);
  Serial.println(messagetoDAC, HEX);
  writetoDAC(messagetoDAC);
  
  readADC = ADC1.readRegister16(ADC1.REG_ADCDATA);
  Serial.print("ADC: ");
  Serial.println(readADC);
  
  delay(3000);
}
