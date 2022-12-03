#include <Arduino.h>
#include <dac8568.h>
#include <LUT.h>
#include <SPI.h>

//********************** NANO33 BLE Pin ***************//
const int DAC_CS = 2; //D2
const int LDAC = 4; //D4
const int CLR = 3; //D3
const int SCKL = 13; //D13

const int ADC1_CS = A2; //A2
const int ADC2_CS = A1; //A1
const int ADC_MDAT = A0; //A0

uint32_t clockFreq = 2000000; 
uint16_t fullScaleDAC = 65535; //the scale of the DAC is 16 bit
uint16_t n = 16; //
float maxV = 15000.; //set maximum voltage
float desiredVoltage = 1000.; //set desired voltage


inline uint32_t DesiredVoltageInScale(float voltage)
{
  return uint32_t(float(fullScaleDAC) * voltage / maxV);
}

uint32_t voltageTo16bit(float voltage){
  return fullScaleDAC * voltage / maxV;
}

uint32_t desiredVoltageInScale =  DesiredVoltageInScale(2400.);
uint32_t desiredVoltageInScale2 = uint32_t(float(fullScaleDAC) * 2000 / maxV);

// 16 bit, 128 points normalized Look Up Table
uint16_t LUTSize = sizeof(SineWaveformNormalizedLUT) / sizeof(float);

// SPI Settings for DAC 
SPISettings DACsettings(clockFreq, MSBFIRST, SPI_MODE1); 
uint32_t messagetoDAC;

// SPI Settings for ADCs
SPISettings ADCsettings(10000000, MSBFIRST, SPI_MODE0); //10 mhz. ADC latches SDI on rising edge
SPISettings ADCsettingsRead(10000000, MSBFIRST, SPI_MODE1); //10 mhz. ADC latches SDO on falling edge. See page 66 datasheet

float desiredFrequency = 0.2;

uint32_t epsilonUs = 1000000. / (float(LUTSize) * desiredFrequency);


// Functions
void writetoDAC(uint32_t message);
float measureVoltage(int i){
  return analogRead(i);
}


int waitTime = 5000;
void measureResistance(int voltage){
  messagetoDAC = DAC8568_Write_Input_Reg_And_Update_Single(CH_C, voltageTo16bit(voltage));
  writetoDAC(messagetoDAC);
  delay(waitTime);
  Serial.print(voltage);
  Serial.print(" V - Measured Voltage on passive side: ");
  Serial.println(measureVoltage(A7)*3.3/4096, 5);
  delay(800);
}


//DAC functions
void writetoDAC(uint32_t message){
  digitalWrite(DAC_CS, LOW);
  SPI.beginTransaction(DACsettings);
  SPI.transfer(message >> 24);
  SPI.transfer(message >> 16);
  SPI.transfer(message >> 8);
  SPI.transfer(message);
  SPI.endTransaction();
  digitalWrite(DAC_CS, HIGH);
}

//DAC outputs
/*void squarewaveSingle(int amplitude, int frequency, int dutycycle, CHANNEL_SELECT channel){
  messagetoDAC = DAC8568_Write_Input_Reg_And_Update_Single(channel, SquareWaveEvenDutyCycleNormalizedLUT[]);
  writetoDAC(messagetoDAC);
}*/

void sinewavewithPhaseShift(){
    for (int i = 0; i < LUTSize; ++i)
  {
    messagetoDAC = DAC8568_Write_Input_Reg_And_Update_All(CH_G, SineWaveformNormalizedLUT[(i) % LUTSize] * desiredVoltageInScale);
    writetoDAC(messagetoDAC);
    messagetoDAC = DAC8568_Write_Input_Reg_And_Update_All(CH_E, SineWaveformNormalizedLUT[(i + 1 * n) % LUTSize] * desiredVoltageInScale);
    writetoDAC(messagetoDAC);
    messagetoDAC = DAC8568_Write_Input_Reg_And_Update_All(CH_A, SineWaveformNormalizedLUT[(i + 2 * n)  % LUTSize] * desiredVoltageInScale);
    writetoDAC(messagetoDAC);
    messagetoDAC = DAC8568_Write_Input_Reg_And_Update_All(CH_C, SineWaveformNormalizedLUT[(i + 3 * n)  % LUTSize] * desiredVoltageInScale);
    writetoDAC(messagetoDAC);
    messagetoDAC = DAC8568_Write_Input_Reg_And_Update_All(CH_H, SineWaveformNormalizedLUT[(i + 4 * n)  % LUTSize] * desiredVoltageInScale);
    writetoDAC(messagetoDAC);
    messagetoDAC = DAC8568_Write_Input_Reg_And_Update_All(CH_F, SineWaveformNormalizedLUT[(i + 5 * n)  % LUTSize] * desiredVoltageInScale);
    writetoDAC(messagetoDAC);
    messagetoDAC = DAC8568_Write_Input_Reg_And_Update_All(CH_D, SineWaveformNormalizedLUT[(i + 6 * n)  % LUTSize] * desiredVoltageInScale);
    writetoDAC(messagetoDAC);
    messagetoDAC = DAC8568_Write_Input_Reg_And_Update_All(CH_B, SineWaveformNormalizedLUT[(i + 7 * n)  % LUTSize] * desiredVoltageInScale);
    writetoDAC(messagetoDAC);
    
    delayMicroseconds(epsilonUs);
  }    
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

//ADC readings

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(2000);
  SPI.begin();

  pinMode(DAC_CS, OUTPUT);
  digitalWrite(DAC_CS, HIGH);
  delay(100);

  pinMode(LDAC, OUTPUT);
  digitalWrite(LDAC, LOW);
  delay(100);

  messagetoDAC = DAC8568_Setup(CONTROL_SETUP_FLEX_MODE, DATA_POWER_UP_INT_REF_ALWAYS_FLEX, FEATURE_POWER_UP_INT_REF_ALWAYS_FLEX);
  writetoDAC(messagetoDAC);
  delay(100);


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

  
}

int currentvoltage = 0;
int increment = 100;
bool setvoltageFlag = 0;
int setvoltage = 0;
int maxvoltage = 0;

void loop() {
  //Serial.println(epsilonUs);
  // put your main code here, to run repeatedly:
  delay(3000);
  while(setvoltageFlag == 0){
    Serial.println("Please set the voltage increment");
    while(Serial.available() == 0);
    increment = Serial.parseInt();
    Serial.println(increment);
    Serial.parseInt();
    
    Serial.println("Please set the maximum voltage");
    while(Serial.available() == 0){;}
    maxvoltage = Serial.parseInt();
    Serial.println(maxvoltage);
    Serial.parseInt();

    Serial.println("Please set the time delay(in milliseconds) between voltage change and measure");
    while(Serial.available() == 0){;}
    waitTime = Serial.parseInt();
    Serial.println(waitTime);
    Serial.parseInt();

    setvoltageFlag = 1;
  }

  for (currentvoltage = 0; currentvoltage <= maxvoltage; currentvoltage += increment){
    measureResistance(currentvoltage);
    if(Serial.read() == 'p'){
      Serial.println("Press 'r' to resume");
      while(Serial.read() != 'r');
    }
  }

  for (currentvoltage = maxvoltage; currentvoltage >= 0; currentvoltage -= increment){
    measureResistance(currentvoltage);
    if(Serial.read() == 'p'){
      Serial.println("Press 'r' to resume");
      while(Serial.read() != 'r');
    }
  }

}