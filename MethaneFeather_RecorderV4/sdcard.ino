// sd card routines

SdFs SD;
FsFile dir;
FsFile file;

void dateTime(uint16_t* date, uint16_t* time) {
  DateTime now = rtcc.now();
  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(now.year(), now.month(), now.day());
  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(now.hour(), now.minute(), now.second());
}

void sd_recordData() {
  int32_t adcGas;
  float   adcR;
  file.print(rtcc.now().timestamp());file.print(",");
  file.print(adcGas = adc_getReading());file.print(",");
  file.print(tmp117_getTemp());file.print(",");
  bme_printReadings(&file);
  file.print(batteryVolts());file.print(",");
  file.print(digitalRead(pinBatCharge));file.print(",");
  file.print(adcR=adc_gasResistivity(adcGas));file.print(",");
  file.print(adc_ppmGas(adcR),4);file.print(",");
  file.print(moisture_get());file.print(",");
  file.print("-999");file.println();
  file.flush();
}
void sd_setup() {
  char fname[] = "Ssta_yymmdd_sssV4.txt";
  uint8_t yr,mo,da,hr,mn,sec;
  static int fileNum = 0;

  Serial.println("Setup uSD card");
  delay(100);
  
  if (!SD.begin(pinCS,SD_SCK_MHZ(12))) {
    Serial.println("Unable to init uSD card!");
    delay(100);
    ERROR_HALT;
  }
  
  rtcc_getTime(&yr,&mo,&da,&hr,&mn,&sec);
  bool isPresent = false;
  do {
    snprintf(fname,sizeof(fname),"S%03d_%02d%02d%02d_%03dV4.txt",serialNo,yr,mo,da,fileNum++);
    Serial.print("Checking for file: ");
    Serial.print(fname);
    isPresent = SD.exists(fname);
    if(isPresent) Serial.println(" -- file exists");
  } while(isPresent);
  Serial.println(" -- does not exist -- yet");
  delay(100);
  file = SD.open(fname, FILE_WRITE);
  if(!file) {
    Serial.println("Unable to open file!!");
    ERROR_HALT;
  }
  Serial.println("File Opened successfully");
  sd_printHeader(&file);
}
void sd_printHeader(Stream *df){
  df->print("MethaneV4 output file started on ");
  df->println(rtcc.now().timestamp());
  gps_printLoc(df);
  df->flush();
}
