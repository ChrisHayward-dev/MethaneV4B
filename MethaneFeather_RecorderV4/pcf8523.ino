// RTC routines for PCF8523
#include  <RTClib.h>

RTC_PCF8523   rtcc;

void secondISR(){
  pcf.digitalWrite(pinLed2B,!pcf.digitalRead(pinLed2B));
}
void rtcc_setupInterrupt(){
  pinMode(pinAlarm,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinAlarm),secondISR,LOW);
  rtcc.enableSecondTimer();
}

void rtcc_setup() {
  Serial.println("Setup PCF87233 RTC");
  delay(2000);      // takes up to 2 seconds for oscillator to stabilize
  Wire.begin();
  
  Serial.println("Setup PDF8523 Real Time Clock");
  delay(100);
  if(!rtcc.begin()) {
    Serial.println("Can't find realtime clock!");
    ERROR_HALT;
  }
  if(!rtcc.initialized() || rtcc.lostPower()) {
    Serial.println("RTC is not initialized -- init to bogus time");
    rtcc.adjust(DateTime(2000,1,1,0,0,0));
  }
  rtcc.start();
  DateTime cTime = rtcc.now();
  Serial.print("Internal time set to: ");
  Serial.println(cTime.timestamp());
}

void rtcc_getTime(uint8_t *yr,uint8_t *mo,uint8_t *da,uint8_t *hr,uint8_t *mn,uint8_t *sec) {
  DateTime now = rtcc.now();
  *yr = now.year()-2000;
  *mo = now.month();
  *da = now.day();
  *hr = now.hour();
  *mn = now.minute();
  *sec= now.second();
}