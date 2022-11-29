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
uint16_t fullScaleDAC = 65535;
uint16_t n = 16;
float maxV = 15000.;
float desiredVoltage = 1000.;


inline uint32_t DesiredVoltageInScale(float voltage)
{
  return uint32_t(float(fullScaleDAC) * voltage / maxV);
}

uint32_t desiredVoltageInScale =  DesiredVoltageInScale(2400.);
uint32_t desiredVoltageInScale2 = uint32_t(float(fullScaleDAC) * 2000 / maxV);

// 16 bit, 128 points normalized Look Up Table
uint16_t LUTSize = sizeof(SineWaveformNormalizedLUT) / sizeof(float);

// SPI Settings for DAC and ADCs
SPISettings DACsettings(clockFreq, MSBFIRST, SPI_MODE1); 
uint32_t messagetoDAC;

float desiredFrequency = 0.2;

uint32_t epsilonUs = 1000000. / (float(LUTSize) * desiredFrequency);


// Functions

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

int measureVoltage(int i){
  return analogRead(i);
}

//Waveforms

void sinewave(){
    for (int i = 0; i < LUTSize; ++i)
  {
    messagetoDAC = DAC8568_Write_Input_Reg_And_Update_All(CH_G, SineWaveformNormalizedLUT[(i) % LUTSize] * desiredVoltageInScale);
    writetoDAC(messagetoDAC);
    messagetoDAC = DAC8568_Write_Input_Reg_And_Update_All(CH_E, SineWaveformNormalizedLUT[(i + 1 * n) % LUTSize] * desiredVoltageInScale);
    writetoDAC(messagetoDAC);
    messagetoDAC = DAC8568_Write_Input_Reg_And_Update_All(CH_A, SineWaveformNormalizedLUT[(i + 2 * n)  % LUTSize] * desiredVoltageInScale2);
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
}

int count = 0;

void loop() {
    //Serial.println(epsilonUs);
    // put your main code here, to run repeatedly:
    messagetoDAC = DAC8568_Write_Input_Reg_And_Update_Single(CH_C, SquareWaveEvenDutyCycleNormalizedLUT[1]* desiredVoltageInScale);
    writetoDAC(messagetoDAC);
    delay(5000);
    Serial.print("2000 V - Measured Voltage: ");
    Serial.println(measureVoltage(A7)*3.3/4096, 4);
    delay(1000);

    messagetoDAC = DAC8568_Write_Input_Reg_And_Update_Single(CH_C, SquareWaveEvenDutyCycleNormalizedLUT[0]* desiredVoltageInScale);
    writetoDAC(messagetoDAC);
    delay(5000);
    Serial.print("0 V - Measured Voltage: ");
    Serial.println(measureVoltage(A7)*3.3/4096, 4);
    delay(1000);
    
}