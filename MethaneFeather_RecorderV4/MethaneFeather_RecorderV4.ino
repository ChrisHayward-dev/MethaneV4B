#include <Wire.h>
#include  <SPI.h>
#include  <RTClib.h>
#include  <SdFat.h>
#include  <Adafruit_PCF8575.h>
#include  <Adafruit_NeoPixel.h>
#include <ZeroPowerManager.h>     // RTC control and wakeup
#include  <Adafruit_seesaw.h>     // moisture sensor
/*  Sample program to demonstrate MethaneV4 board under C
 *   
 */

#define USE_TINYUSB
#define serialNo          1
#define MAX_CONSOLE_WAIT  60000L   // pause to allow console connection

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
  pixels.clear();
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
  
  pcf8575_ledtest();
  checkBattery();             // check condition of battery (is it connected)
  moisture_setup();
  tmp117_setup();
  rtcc_setup();
  adc_setup();                // setup NAU7802 then switch to low power mode
  gps_setup();
  bme_setup();
  sd_setup();

  //rtcc_setupInterrupt();
  pcf8575_setidle();          
  //zpmSetup();
  //zpmSleepFor(5000L);         // FIXME: Needs to work w/ SD card and I2C stuff
// FIXME: add datafile name select to start new file
  // FIXME: add header print
  adc_heaterOn();
  adc_powerUp();
}
void loop(void) {
  static uint32_t ftime = millis();
  static bool     fan1On = false;
  if((!fan1On) && (millis() - ftime > 30000)) {    // once every 30 seconds start time
    pcf.digitalWrite(pinFan1,true);
    fan1On = true;
  }
  if(fan1On && (millis() - ftime > 35000)) {
    ftime = millis();
    pcf.digitalWrite(pinFan1,false);
    fan1On = false;
  }
  pcolor(0,64,0);
  sd_recordData();
  pcolor(0,0,64);
  delay(100);
  //Serial.println("I am alive!");
  //sd_printHeader(&file);
}
  
