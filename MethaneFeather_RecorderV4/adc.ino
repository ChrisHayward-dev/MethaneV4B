// MethaneFeatherV4 - cth 12/19/2023
//  -- routines involving the NAU7802 24-bit bridge ADC
// we are using the Sparkfun library which is really intended for a
//  strain gauge based scale because it is more complete than the
//  alternative Adafruit library (especially the ability to control
//  the power)

#include <Wire.h>
#include <SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h>

NAU7802 adc;  //Create instance of the NAU7802 class
bool adcHasPower = false;

void adc_heaterOn() {
  Serial.println("Heater power on");
  delay(1);
  pcf.digitalWrite(pinEnable5V,true);
  pcf.digitalWrite(pinHeater, true);
  pcf.digitalWrite(pinLed2B,false);
}
void adc_heaterOff() {
  Serial.println("Heater power off");
  delay(1);
  pcf.digitalWrite(pinHeater, false);
  pcf.digitalWrite(pinEnable5V,false);
  pcf.digitalWrite(pinLed2B,true);
}
void adc_setup() {
  Serial.println("Setup NAU7802 ADC");
  delay(100);

  Serial.println("Setup NAU7802 ADC");
  Wire.begin();
  if (!adc.begin()) {
    Serial.println("NAU7802 is not detected -- ");
    ERROR_HALT;
  }
  Serial.println("Waiting 600ms for reset");
  delay(600);
  Serial.println("Setting Gain");
  delay(100);
  adc.setGain(NAU7802_GAIN_1);

  Serial.println("Setting sample rate");
  delay(100);
  adc.setSampleRate(NAU7802_SPS_10);

  Serial.print("Setting LDO");
  delay(100);
  adc.setLDO(NAU7802_LDO_2V4);

  Serial.println("Setting Channel");
  delay(100);
  adc.setChannel(NAU7802_CHANNEL_1);

  Serial.println("Calibrating");
  delay(100);
  adc.calibrateAFE();
  Serial.println("Power down");
  delay(100);
  adc_powerDown();

  adc_heaterOff();
  for (int k = 0; k < 50; k++) {
    Serial.print(adc_getReading());
    Serial.println(" counts");
  }
  adc_heaterOn();
  for (int k = 0; k < 3600*10L; k++) {
    Serial.print(adc_getReading());
    Serial.println(" counts");
  }
  adc_heaterOff();
  adc_powerDown();
}
void adc_powerDown() {
  adc.powerDown();
  adcHasPower = false;
}
void adc_powerUp() {
  if (!adcHasPower) {  // NAU7802 takes 600ms to boot and power up
    adc.powerUp();
    Serial.println("NAU7802 powered up");
    delay(600);
  }
  adcHasPower = true;
}
int32_t adc_getReading() {
  int32_t adcReading = 0;
  adc_powerUp();
  while (adcReading == 0) {
    while (!adc.available()) {
      //Serial.println("Waiting for data");
      delay(2);
    }
    adcReading = adc.getReading();
  }
  return (adcReading);
}
