//==================================================
//== The following routines are used to setup the
//== SD card as a USB drive.  This happens when a
//== deivice is powered from the USB rather than batteries
//== The device shutsdown when the ATTN button is pressed
//== and released
// modified from adafruit msc_sdfat.ino
//  see original for license/headers
//=================================================
#ifdef USE_TINYUSB
#include  <SPI.h>
#include  <SdFat.h>
#include  <Adafruit_TinyUSB.h>

const int chipSelect = pinCS;

SdFs *sd;
bool  changed;
bool  unmount = false;
uint32_t  lastChange = 0;

bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_conditions, bool start, bool load_eject) {
  if (load_eject && !start) {
    unmount = true;
  }
  return (true);
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and
// return number of copied bytes (must be multiple of block size)
int32_t msc_read_cb (uint32_t lba, void* buffer, uint32_t bufsize)
{
  pcolor(16,0,0);
  lastChange = millis();
  return sd->card()->readSectors(lba, (uint8_t*) buffer, bufsize / 512) ? bufsize : -1;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and
// return number of written bytes (must be multiple of block size)
int32_t msc_write_cb (uint32_t lba, uint8_t* buffer, uint32_t bufsize)
{
  pcolor(12,0,0);
  lastChange = millis();
  return sd->card()->writeSectors(lba, buffer, bufsize / 512) ? bufsize : -1;
}

// Callback invoked when WRITE10 command is completed (status received and accepted by host).
// used to flush any pending cache.
void msc_flush_cb (void)
{
  sd->card()->syncDevice();
  lastChange = millis();
  // clear file system's cache to force refresh
  //sd.vol()->cacheClear();

  changed = true;
  pcolor(0,12,0);
}

Adafruit_USBD_MSC usb_msc;
bool usbDrive() {
  SdFile  root;
  SdFile  file;
  sd = &SD;
  uint32_t  startTime = millis();

  pcolor(0,4,0);
 
  // Set disk vendor id, product id and revision with string up to 8, 16, 4 characters respectively
  usb_msc.setID("Methane", "SD Card", "1.0");

  // Set read write callback
  usb_msc.setReadWriteCallback(msc_read_cb, msc_write_cb, msc_flush_cb);

  // Still initialize MSC but tell usb stack that MSC is not ready to read/write
  // If we don't initialize, board will be enumerated as CDC only
  usb_msc.setUnitReady(false);
  usb_msc.begin();
  
  Serial.println("TinyUSB Mass Storage SD Card access");

  Serial.print("\nInitializing SD card ... ");
  Serial.print("CS = "); Serial.println(chipSelect);

  if ( !sd->begin(chipSelect, SD_SCK_MHZ(12)) )
  {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    ERROR_HALT;
  }

  // Size in blocks (512 bytes)
  uint32_t block_count = sd->card()->sectorCount();

  Serial.print("Volume size (MB):  ");
  Serial.println((block_count / 2) / 1024);

  // Set disk size, SD block size is always 512
  usb_msc.setCapacity(block_count, 512);

  // MSC is ready for read/write
  usb_msc.setUnitReady(true);

  changed = true; // to print contents initially

  // -- main loop
  lastChange = millis();
  while (!unmount) {
    if ( changed )
    {
      root.open("/");
      Serial.println("SD contents:");

      // Open next file in root.
      // Warning, openNext starts at the current directory position
      // so a rewind of the directory may be required.
      while ( file.openNext(&root, O_RDONLY) )
      {
        file.printFileSize(&Serial);
        Serial.write(' ');
        file.printName(&Serial);
        if ( file.isDir() )
        {
          // Indicate a directory.
          Serial.write('/');
        }
        Serial.println();
        file.close();
      }
      root.close();
      Serial.println();
      changed = false;
      delay(100); // refresh every 0.1 second
    }

    tud_task();
    if((millis() - startTime)>1000) {
      if(!tud_connected()) {
        unmount = true;
      }
    }
    if((millis() - lastChange)>500) {
      pcolor(0,12,0);
    }
    delay(10);
  }
  usb_msc.setUnitReady(false);
  tud_msc_set_sense(0,SCSI_SENSE_NOT_READY,0x3A,0x00);
  Serial.println("Ending USB drive");
  lastChange = millis();
  while((millis() - lastChange)<2000) {
    tud_task();
    delay(5);
  }
  sd->end();
  SPI.end();
  return(true);
}
#endif
