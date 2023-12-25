// TMP117 routines for Methanev4

#include <Wire.h>
#include  <Adafruit_TMP117.h>
#include  <Adafruit_Sensor.h>

Adafruit_TMP117 tmp117;
void tmp117_setup() {
  Serial.println("Setup TMP117 thermometer");
  delay(100);
  if (!tmp117.begin()) {
    Serial.println("Failed to find TMP117 chip");
    ERROR_HALT;
  }
  Serial.println("TMP117 Found!");

  tmp117.setAveragedSampleCount(TMP117_AVERAGE_1X);
  tmp117.setReadDelay(TMP117_DELAY_125_MS);
  tmp117.setMeasurementMode(TMP117_MODE_CONTINUOUS);

  tmp117_printStatus();
 for (int k = 0; k < 10; k++) {
    Serial.print(tmp117_getTemp());
    Serial.println("degC");
    delay(250);
  }
  tmp117.setMeasurementMode(TMP117_MODE_ONE_SHOT);
  tmp117_getTemp();
}
void tmp117_printStatus() {
  Serial.print("Temperature averaged from ");
  switch (tmp117.getAveragedSampleCount()) {
    case TMP117_AVERAGE_1X: Serial.print(" 1"); break;
    case TMP117_AVERAGE_8X: Serial.print(" 8"); break;
    case TMP117_AVERAGE_32X: Serial.print(" 32"); break;
    case TMP117_AVERAGE_64X: Serial.print(" 64"); break;
  }
  Serial.println(" samples");
 
  Serial.print("Interval between reads is at least ");
  switch (tmp117.getReadDelay()) {
    case TMP117_DELAY_0_MS: Serial.print(0); break;
    case TMP117_DELAY_125_MS: Serial.print(125); break;
    case TMP117_DELAY_250_MS: Serial.print(250); break;
    case TMP117_DELAY_500_MS: Serial.print(500); break;
    case TMP117_DELAY_1000_MS: Serial.print(1000); break;
    case TMP117_DELAY_4000_MS: Serial.print(4000); break;
    case TMP117_DELAY_8000_MS: Serial.print(8000); break;
    case TMP117_DELAY_16000_MS: Serial.print(16000); break;
  }
  Serial.println(" milliseconds");

  Serial.print("Measurement mode: ");
  switch (tmp117.getMeasurementMode()) {
    case TMP117_MODE_SHUTDOWN: Serial.println("Shut down"); break;
    // This will almost never be read because of the switch to SHUTDOWN after the measurement is
    // taken.
    case TMP117_MODE_ONE_SHOT: Serial.println("One shot"); break;
    case TMP117_MODE_CONTINUOUS: Serial.println("Continuous"); break;
  }
}

float tmp117_getTemp() {
  sensors_event_t temp;
  tmp117.getEvent(&temp);
  return (temp.temperature);
}
