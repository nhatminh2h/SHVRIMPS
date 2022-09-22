#include <Arduino.h>
#include <dac8568.h>
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
float desiredVoltage = 2000.;

uint32_t desiredVoltageInScale = uint32_t(float(fullScaleDAC) * desiredVoltage / maxV);
uint32_t desiredVoltageInScale2 = uint32_t(float(fullScaleDAC) * 2000 / maxV);


// 16 bit, 128 points normalized Look Up Table
float normalizedSineLUT[] = {5.00007630e-01, 5.24528878e-01, 5.49004349e-01, 5.73357748e-01, 5.97543297e-01, 6.21484703e-01, 6.45136187e-01, 6.68451972e-01,
                             6.91340505e-01, 7.13771267e-01, 7.35698482e-01, 7.57045853e-01, 7.77782864e-01, 7.97848478e-01, 8.17196918e-01, 8.35782406e-01,
                             8.53559167e-01, 8.70481422e-01, 8.86503395e-01, 9.01609827e-01, 9.15739681e-01, 9.28862440e-01, 9.40962844e-01, 9.51995117e-01,
                             9.61943999e-01, 9.70778973e-01, 9.78469520e-01, 9.85015640e-01, 9.90386816e-01, 9.94583047e-01, 9.97589075e-01, 9.99404898e-01,
                             1.00000000e+00, 9.99404898e-01, 9.97589075e-01, 9.94583047e-01, 9.90386816e-01, 9.85015640e-01, 9.78469520e-01, 9.70778973e-01,
                             9.61943999e-01, 9.51995117e-01, 9.40962844e-01, 9.28862440e-01, 9.15739681e-01, 9.01609827e-01, 8.86503395e-01, 8.70481422e-01,
                             8.53559167e-01, 8.35782406e-01, 8.17196918e-01, 7.97848478e-01, 7.77782864e-01, 7.57045853e-01, 7.35698482e-01, 7.13771267e-01,
                             6.91340505e-01, 6.68451972e-01, 6.45136187e-01, 6.21484703e-01, 5.97543297e-01, 5.73357748e-01, 5.49004349e-01, 5.24528878e-01,
                             5.00007630e-01, 4.75471122e-01, 4.50995651e-01, 4.26642252e-01, 4.02456703e-01, 3.78515297e-01, 3.54863813e-01, 3.31548028e-01,
                             3.08659495e-01, 2.86228733e-01, 2.64301518e-01, 2.42954147e-01, 2.22217136e-01, 2.02151522e-01, 1.82803082e-01, 1.64217594e-01,
                             1.46440833e-01, 1.29518578e-01, 1.13496605e-01, 9.83901732e-02, 8.42603189e-02, 7.11375601e-02, 5.90371557e-02, 4.80048829e-02,
                             3.80560006e-02, 2.92210269e-02, 2.15304799e-02, 1.49843595e-02, 9.61318379e-03, 5.41695277e-03, 2.41092546e-03, 5.95101854e-04,
                             0.00000000e+00, 5.95101854e-04, 2.41092546e-03, 5.41695277e-03, 9.61318379e-03, 1.49843595e-02, 2.15304799e-02, 2.92210269e-02,
                             3.80560006e-02, 4.80048829e-02, 5.90371557e-02, 7.11375601e-02, 8.42603189e-02, 9.83901732e-02, 1.13496605e-01, 1.29518578e-01,
                             1.46440833e-01, 1.64217594e-01, 1.82803082e-01, 2.02151522e-01, 2.22217136e-01, 2.42954147e-01, 2.64301518e-01, 2.86228733e-01,
                             3.08659495e-01, 3.31548028e-01, 3.54863813e-01, 3.78515297e-01, 4.02456703e-01, 4.26642252e-01, 4.50995651e-01, 4.75471122e-01};

uint16_t LUTSize = sizeof(normalizedSineLUT) / sizeof(float);

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

void loop() {
    Serial.println(epsilonUs);
    // put your main code here, to run repeatedly:
    for (int i = 0; i < LUTSize; ++i)
    {
      messagetoDAC = DAC8568_Write_Input_Reg_And_Update_All(CH_A, normalizedSineLUT[i] * desiredVoltageInScale);
      writetoDAC(messagetoDAC);
      messagetoDAC = DAC8568_Write_Input_Reg_And_Update_All(CH_B, normalizedSineLUT[(i + n) % LUTSize] * desiredVoltageInScale);
      writetoDAC(messagetoDAC);
      messagetoDAC = DAC8568_Write_Input_Reg_And_Update_All(CH_C, normalizedSineLUT[(i + 2*n) % LUTSize] * desiredVoltageInScale2);
      writetoDAC(messagetoDAC);
      messagetoDAC = DAC8568_Write_Input_Reg_And_Update_All(CH_D, normalizedSineLUT[(i + 3*n) % LUTSize] * desiredVoltageInScale);
      writetoDAC(messagetoDAC);
      messagetoDAC = DAC8568_Write_Input_Reg_And_Update_All(CH_E, normalizedSineLUT[(i + 4*n) % LUTSize] * desiredVoltageInScale);
      writetoDAC(messagetoDAC);
      delayMicroseconds(epsilonUs);
    }    
}