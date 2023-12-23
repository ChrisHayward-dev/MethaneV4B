// sd card routines
#define SD_FAT_TYPE  3
#if SD_FAT_TYPE == 0
SdFat SD;
File dir;
File file;
#elif SD_FAT_TYPE == 1
SdFat32 SD;
File32 dir;
File32 file;
#elif SD_FAT_TYPE == 2
SdExFat SD;
ExFile dir;
ExFile file;
#elif SD_FAT_TYPE == 3
SdFs SD;
FsFile dir;
FsFile file;
#else  // SD_FAT_TYPE
#error invalid SD_FAT_TYPE
#endif  // SD_FAT_TYPE

void dateTime(uint16_t* date, uint16_t* time) {
  DateTime now = rtcc.now();
  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(now.year(), now.month(), now.day());
  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(now.hour(), now.minute(), now.second());
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
  df->println(DateTime(rtc.getYear(), rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()).timestamp());
  gps_printLoc(df);
  df->flush();
}
