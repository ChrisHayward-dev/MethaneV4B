// GPS routines
#include  <Adafruit_GPS.h>
#include  <RTClib.h>
extern RTC_PCF8523   rtcc;
Adafruit_GPS  GPS(&Wire);
#define   GPSECHO   false

void gps_printTime(Stream *device) {
  DateTime rtctime = rtcc.now();
  device->print(rtctime.unixtime());
  device->print(",");
}

void gps_setTime() {
  rtcc.adjust(DateTime(GPS.year,GPS.month,GPS.day,GPS.hour,GPS.minute,GPS.seconds));
  
}
void gps_printLoc(Stream *device) {
  device->print("Latitude: ");
  device->println(GPS.latitude, 5);
  device->print("Longitude: ");
  device->println(GPS.longitude, 5);
}

void gps_setup() {
  gps_shutdown();
   pcf.digitalWrite(pinLed2G,true);
  Serial.println("\nGPS setup");
  Wire.begin();
  Serial.println("Power up GPS");
  delay(100);
  pcf.digitalWrite(pinPwrGNSS,true);    // power up gps
  Serial.println("GPS Begin");
  delay(100);
  GPS.begin(0x10);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  delay(1000);
  GPS.println(PMTK_Q_RELEASE);
  for(int k=0;k<10;k+=gps_process()) {
    if(k>0) {
      pcolor(8,8,8);
     
    }
    delay(10);
  }
  pcolor(0,2,0);
  pcf.digitalWrite(pinLed2G,true);
  gps_shutdown();
}

void gps_shutdown() {
  GPS.sendCommand(PMTK_STANDBY);
  delay(100);
  pcf.digitalWrite(pinPwrGNSS,false);   // power down GPS
 
}

void gps_standby() {
  pcf.digitalWrite(pinPwrGNSS,true);
  delay(100);                          // allow GPS to boot
  GPS.begin(0x10);
  GPS.sendCommand(PMTK_STANDBY);
}
bool gps_process() {
  while (char c = GPS.read()) {
    if (GPSECHO) Serial.print(c);
    if (GPS.newNMEAreceived()) {
      if (!GPS.parse(GPS.lastNMEA())) {
        continue;
      }
    }
  }
  if (GPS.secondsSinceFix() <= 0.5) {
     pcf.digitalWrite(pinLed2G,false);
    //Serial.println("We have a GPS fix!");
    float s = GPS.seconds + GPS.milliseconds / 1000. + GPS.secondsSinceTime();
    int m = GPS.minute;
    int h = GPS.hour;
    int d = GPS.day;
    // Adjust time and day forward to account for elapsed time.
    // This will break at month boundaries!!! Humans will have to cope with
    // April 31,32 etc.
    while (s > 60) {
      s -= 60;
      m++;
    }
    while (m > 60) {
      m -= 60;
      h++;
    }
    while (h > 24) {
      h -= 24;
      d++;
    }
    // Kluge: Adjust the clock each time we are close to on time.
    // We keep this in case we need to run a DS3231 as well as GPS
    rtcc.adjust(DateTime(GPS.year + 2000, GPS.month, GPS.day, h, m, (int)s));
    Serial.print("GPS time:");
    Serial.print(GPS.month);Serial.print("/");
    Serial.print(d);Serial.print("/");
    Serial.print(GPS.year);Serial.print(" ");
    Serial.print(h);Serial.print(":");
    Serial.print(m);Serial.print(":");
    Serial.print(s);Serial.print(" ");
    Serial.print(GPS.latitude,5);Serial.print(" ");
    Serial.print(GPS.longitude,5);Serial.print(" ");
    Serial.println();
    delay(500);   // force wait so we don't run a second time
    pcf.digitalWrite(pinLed2G,true);
    return(true);
  }
  return(false);
}
