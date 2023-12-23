#include <Wire.h>
#include  <SPI.h>
#include  <RTClib.h>
#include  <SdFat.h>
#include  <RTCZero.h>
#include  <Adafruit_PCF8575.h>
#include  <Adafruit_NeoPixel.h>
/*  Sample program to demonstrate MethaneV4 board under C
 *   
 */


#define serialNo          1
#define MAX_CONSOLE_WAIT  60000L   // pause to allow console connection
#define BRIDGE_VOLTAGE    1000    // mV for bridge exciter (0-3300)
#define R_REF             20000     // reference resistor for system 2 (rev 3) addr 28FF4FD950160383

#define ERROR_HALT        error_halt(__LINE__)

// QT Py pin assignments
#define pinUnused   A0
#define pinBatMon   A1
#define pinCS       A2
#define pinAlarm    A3


// Pin Assignments for port Expander
#define pinEnable5V 0   // enable 5 volt boost supply
#define pinAlertSW  1   // alert push button
#define pinFan1     2   // enable for fan 1
#define pinLed1B    3   //
#define pinLed1G    4
#define pinLed1R    5
#define pinBatCharge  6
#define pinReedSW   7
#define pinPwrGNSS  8
#define pinHeater   9
#define pinFan2     10
#define pinLed2R    11
#define pinLed2G    12
#define pinLed2B    13
#define pinLed3B    14
#define pinLed3G    15
//      pinLed3R      is connected to 5 volt heater regulator enable

Adafruit_NeoPixel pixels(1, PIN_NEOPIXEL);
Adafruit_PCF8575  pcf;
RTCZero           rtc;

/* If the battery is not detected, then the battery
 *  monitor line will have a ~1 second square wave
 *  present while the battery control chip tries to
 *  sense it.  If the battery is connected, the minimum
 *  value will be the battery voltage
 */
//extern void neopixel_blink(uint8_t,uint8_t,uint8_t,uint8_t);
void setup(void) {
  uint32_t  curtime = millis();
  Wire.begin();
  
  // setup neopixel
  pixels.begin();     // init neopixel on device

  Serial.begin(9600); // Wait for console connection
  // while ((!Serial) && ((millis() - curtime) < MAX_CONSOLE_WAIT)) {
  //   neopixel_blink(1,32,0,0);
  // }
  // delay(500);
  
  pcolor(0,16,0);    // indicate startup
  Serial.println("Methane Feather Recorder Demo");
  
  Serial.println("Setting default pin conditions");
  delay(100);
  pinMode(pinCS,OUTPUT);
  pinMode(pinAlarm,INPUT_PULLUP);
  pinMode(pinBatMon,INPUT);  // pullup here to detect disconnected battery

  pcf8575_setup();            // set port expander to low power conditions
  
  #ifdef USE_TINYUSB
  usbDrive();
  #endif
  
  checkBattery();             // check condition of battery (is it connected)
  tmp117_setup();
  rtcc_setup();
  adc_setup();                // setup NAU7802 then switch to low power mode
  gps_setup();
  bme_setup();
  sd_setup();
// FIXME: add datafile name select to start new file
  // FIXME: add header print
 
}
void loop(void) {
  pcolor(0,64,0);
  delay(500);
  pcolor(0,0,0);
  delay(500);
  checkBattery();
}
  
// 
//
//  // setup the internal clock
//  rtc.begin();
//  DateTime dt = DateTime(__DATE__, __TIME__);
//  rtc.setDate(dt.day(), dt.month(), dt.year());
//  rtc.setTime(dt.hour(), dt.minute(), dt.second());
//
//  haveBattery = checkBattery();
//  if (!haveBattery) errorHalt("Cannot detect a battery");
//  if (getBattery() < 3.4) errorHalt("Battery Voltage Low");
//
//  if (digitalRead(pinSDdetect) == LOW) errorHalt("Cannot detect SD Card");
//  if (!SD.begin(pinSDselect)) errorHalt("Card failed to init!");
//  dataFile = SD.open("Methane.txt", FILE_WRITE);
//  Serial.println("\nData file opened");
//  dataFile.println(header);
//  dataFile.println(units);
//  dataFile.flush();
//
//  gps_setup();
//  ds_setup();
//  bme_setup();
//  mcp_setup();
//
//  exciteSet(BRIDGE_VOLTAGE);  // Enable the bridge exciter at 1 volt
//  mcp_setup();
//
//  enable5V(true);
//  enableHeater(true);
//  delay(2000);
//  mcp_setup();
//  digitalWrite(pinFan1,HIGH);
//  delay(3000);
//  digitalWrite(pinFan1,LOW);
//  digitalWrite(pinFan2,HIGH);
//  delay(3000);
//  digitalWrite(pinFan2,LOW);
// 
//  digitalWrite(redLed, LOW);
//  digitalWrite(greenLed, HIGH);
//  enable5V(false);
//  enableHeater(false);
//
// 
//
//  DateTime dc = DateTime(rtc.getYear(), rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
//  while (TimeSpan(dc - dt).minutes() < 1) {
//    gps_process();
//    dc = DateTime(rtc.getYear(), rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
//    delay(250);
//    digitalWrite(redLed, !digitalRead(redLed));
//  }
//  gps_setTime();
//  Serial.println("GPS is supposed to be locked now!");
//  digitalWrite(A2, HIGH); //disable GPS
//  digitalWrite(redLed, LOW);
//  digitalWrite(greenLed, HIGH);
//  delay(2000);
//
//  // wait for top of minute
//  while(rtc.getSeconds() !=0) {
//    delay(500);
//  }
//   // setup the rtc alarm
//  rtc.setAlarmSeconds(0);
//  rtc.enableAlarm(rtc.MATCH_SS);
//  rtc.attachInterrupt(topOfMinute);
//}
//
//void loop(void) {
//
//  Stream *out = &dataFile;
//  static uint32_t loopCount = 0;
//  static int32_t lastTime = 30;
//
//  
//  loopCount++;
//  //Serial.print("Loop: ");Serial.println(loopCount);
//  
//  if (lastTime == 30 && rtc.getSeconds() < 5) {
//    lastTime = 0;
//    //Serial.println("Wakeup - 5V power on, Heater on, exciter on");
//    exciteSet(1000);
//    enable5V(true);
//    enableHeater(true);
//    //Serial.println("Fan1 on");
//    digitalWrite(pinFan1, HIGH);
//  } else if(lastTime == 0 && rtc.getSeconds() > 5) {
//    lastTime = 5;
//    //Serial.println("Fan 2 on");
//    digitalWrite(pinFan2, HIGH);
//  } else if(lastTime == 5 && rtc.getSeconds() > 10) {
//    lastTime = 10;
//    //Serial.println("Fan 1 off");
//    digitalWrite(pinFan1, LOW);
//  } else if (lastTime == 10 && rtc.getSeconds() > 15) {
//    lastTime = 15;
//    //Serial.println("Fan 2 off");
//    digitalWrite(pinFan2, LOW);
//  } else if (lastTime == 15 && rtc.getSeconds() > 30) {
//    lastTime = 30;
//    //Serial.println("Set power to standby");
//    digitalWrite(redLed, LOW);
//    digitalWrite(greenLed, LOW);
//    digitalWrite(pinFan1, LOW);
//    digitalWrite(pinFan2, LOW);
//    enableHeater(false);
//    enable5V(false);
//    exciteSet(0);
//  }
//  
//  yield();
//  if (rtc.getSeconds() < 30) {
//    digitalWrite(greenLed, !digitalRead(greenLed));
//    gps_printTime(out);
//    ds_process();   // start the DS18B20 temperature reading to interleave w/ mcp
//    bme_process();  // start the BME reading to interleave as well
//    mcp_process();
//    mcp_printVals(out);
//    ds_printTemps(out);
//    bme_printReadings(out);
//    gps_printLoc(out);
//    soh_printBattery(out);
//    mcp_printGas(out);
//    out->println(-999);
//    out->flush();
//  }
//
//  if (rtc.getSeconds() > 32) {
//    digitalWrite(greenLed,LOW);
//    digitalWrite(redLed,HIGH);
//    delay(100);
//    rtc.standbyMode();
//  }
//
//  // if VBAT < 3.4 volts, then we need to shut down everyting on the VBAT line since
//  //  the protrection circuitry on the Feather boards won't shut down the power discharge
//  //  on the VBAT line itself
//
//  if (getBattery() < 3.50) {
//    rtc.disableAlarm();
//    set_sleepPower();
//    digitalWrite(greenLed, LOW);
//    digitalWrite(redLed, HIGH);
//    while (true) {
//      __WFI();
//    }
//  }
//}
//
//// used just before sleep.  Turn off all the externals
//void set_sleepPower() {
//  digitalWrite(pinGPSenable, LOW);
//  digitalWrite(pinFan1, LOW);
//  digitalWrite(pinFan2, LOW);
//  enableHeater(false);
//  enable5V(false);
//  exciteSet(0);
//}
//void set_wakePower() {
//  //digitalWrite(pinGPSenable, HIGH);
//  digitalWrite(pinFan1, HIGH);
//  digitalWrite(pinFan2, HIGH);
//  enable5V(true);
//  enableHeater(true);
//  exciteSet(BRIDGE_VOLTAGE);
//}
