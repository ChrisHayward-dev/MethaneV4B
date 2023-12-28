/* Misc functions here
 * enableHeater - enable heater 2
   enable5V - enable the 5V supply
   exciteSet - set exciter voltage
   doScan - check for ds18b20 temperature sensors
   scanPins - check the status of various control pins
   checkFans - exercise the fans
   checkBattery - report connected battery voltage
   I2Cscanner - report devices on I2C bus
*/

// Optional moisture sensor
Adafruit_seesaw ss;
bool  haveMoisture = false;
bool moisture_setup() {
  Serial.println("Setting up moisture sensor");
  gps_standby();
  if(!ss.begin(0x36)) {
    gps_shutdown();
    Serial.println(" -- no moisture sensor found");
    return(haveMoisture=false);
  }
  Serial.println("Found moisture sensor");
  Serial.println(ss.getVersion(),HEX);
  float tempC = ss.getTemp();
  uint16_t capread = ss.touchRead(0);
  Serial.print("Temperature: ");Serial.println(tempC);
  Serial.print(" Capactivie: ");Serial.println(capread);
  gps_shutdown();
  return(haveMoisture=true);
}

uint16_t moisture_get() {
  if(!haveMoisture) return(0);
   gps_standby();
   if(!ss.begin(0x36)) {
    gps_shutdown();
    Serial.println("Unable to reinit moisture sensor");
    return(0);
   }
  uint16_t capread = ss.touchRead(0);
  gps_shutdown();
  return(capread);
}
// low power
void zpmSetup() {
  zpmCPUClk48M();
  zpmRTCInit();
}
void zpmSleepFor(uint32_t sleep_ms){
  uint32_t now = zpmRTCGetClock();
  pcolor(0,0,0);
  delay(5);
  // Serial.end();
  // USBDevice.detach();
  // zpmPortDisableUSB();
  // zpmPortDisableDigital();
  // zpmPortDisableSPI();
  // USB->DEVICE.CTRLA.bit.SWRST = 1;
  // while (USB->DEVICE.SYNCBUSY.bit.SWRST | USB->DEVICE.CTRLA.bit.SWRST == 1);
  zpmRTCInterruptAt(now+sleep_ms,NULL);
  //zpmCPUClk8M();
  //zpmSleep();
  //zpmCPUClk48M();
  //  USB->DEVICE.CTRLA.bit.SWRST = 1;
  // while (USB->DEVICE.SYNCBUSY.bit.SWRST | USB->DEVICE.CTRLA.bit.SWRST == 1);
  // USBDevice.attach();
  // Serial.begin(9600);     //restore USB serial
  delay(1);
}

//pcf8575
void pcf8575_setup() {
   Serial.println("Setting up PCF8575");
  if(!pcf.begin(0x20, &Wire)) {
    Serial.println("PCF8575 is not answering");
    ERROR_HALT;
  }
 
  pcf8575_setidle();
}
void pcf8575_ledtest() {
  pcf8575_setidle();
  uint8_t ledpins[]={3,4,5,11,12,13,14,15,0};
  for(int k=0;ledpins[k]>0;k++) {
    pcf.digitalWrite(ledpins[k],false);
    delay(500);
    pcf.digitalWrite(ledpins[k],true);
  }
}
void pcf8575_setidle() {
  pcf.digitalWriteWord(0xF8FA);
  pcolor(0,0,16);
}

void checkBattery() {
  uint16_t  batValue;
  uint16_t  minValue = 4096;
  float     batVolts;

   Serial.println("Checking Battery");
  pinMode(pinBatCharge,INPUT);
  if(digitalRead(pinBatCharge) == true) {
    Serial.println("Battery is charging");
  } else {
    Serial.println("Battery is not charging");
  }
  analogReadResolution(12);
  for(int k=0;k<100;k++) {
    batValue = analogRead(pinBatMon);
    minValue = min(minValue,batValue);
    //Serial.println(batValue);
    delay(10);
  }
  if(minValue < 1000) {
    Serial.println("No Battery detected!");
  } else {
    batVolts = 3.3*2.0*batValue/4096;
    Serial.print("Battery: ");
    Serial.println(batVolts);
  }
}

float batteryVolts() {
  uint16_t  batValue;
  float     batVolts;
  analogReadResolution(12);
  batValue = analogRead(pinBatMon);
  batVolts = 3.3*2.0*batValue/4096;
  return(batVolts);
}

void neopixel_blink( uint8_t n,uint8_t r,uint8_t g,uint8_t b) {
  uint8_t r2=0;
  uint8_t g2=0;
  uint8_t b2=0;
  for(int k=0;k<n || n<0 ;k++){
    pcolor(r,g,b);
    delay(250);
    pcolor(r2,g2,b2);
    delay(250);
  }
}
void pcolor(uint8_t r,uint8_t g,uint8_t b) {
  pixels.setPixelColor(0,pixels.Color(r,g,b));
  pixels.show();
}

void error_halt(uint32_t line) {
  Serial.print("Error halt at line");
  Serial.println(line);
  neopixel_blink(-1,15,0,0);
}


