#include <M5Stack.h>
#include <M5ez.h>
#include <ezTime.h>
#include <Wire.h>

#include "MFRC522_I2C.h"
#include "images.h"

#define MAIN_DECLARED

// 0x28 is i2c address on SDA. Check your address with i2cscanner if not match.
MFRC522 mfrc522(0x28);   // Create MFRC522 instance.

void setup() {
#include <themes/default.h>
#include <themes/dark.h>

  // RFID init
  Serial.begin(115200);           // Initialize serial communications with the PC
  Wire.begin();                   // Initialize I2C
  mfrc522.PCD_Init();             // Init MFRC522

  M5.Speaker.setVolume(1);
  M5.Speaker.update();
  
  ezt::setDebug(INFO);
  ez.begin();
}

void loop() {
  ezMenu mainmenu("Spawnrider M5H@ck");
  mainmenu.txtSmall();
  mainmenu.addItem("RFID Tools", menu_rfid);
  mainmenu.addItem("WIFI Tools", menu_not_implemented);
  mainmenu.addItem("Built-in wifi & other settings", ez.settings.menu);
  mainmenu.addItem("M5H@ck settings", menu_m5hack);
  mainmenu.upOnFirst("last|up");
  mainmenu.downOnLast("first|down");
  mainmenu.run();
}

void menu_rfid() {
  ezMenu rfidmenu("RFID Tools");
  rfidmenu.txtSmall();
  rfidmenu.addItem("Get UID", rfid_tools);
  rfidmenu.addItem("Reader infos", show_reader_infos);
  rfidmenu.addItem("Back");
  rfidmenu.upOnFirst("last|up");
  rfidmenu.downOnLast("first|down");
  rfidmenu.run();
}

void rfid_tools() {
  ez.screen.clear();
  
  char cardUid[15];
  int i = 0;
  do {
    if(i > 10) return;
    M5.Lcd.println("Put the NFC Card on the reader...");
    // Look for new cards, and select one if present
    delay(500);
    i++;
  } while (! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial());

  // Now a card is selected. The UID and SAK is in mfrc522.uid.
  //M5.Speaker.beep();
  
  // Dump UID
  Serial.print(F("Card UID:"));
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    sprintf(&cardUid[i * 2], "%02X", mfrc522.uid.uidByte[i]);
    //M5.Lcd.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    //M5.Lcd.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  ez.msgBox("Card UID", cardUid);
}

void show_reader_infos() {
  // Get the MFRC522 software version
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91) {
    Serial.print(F(" = v1.0"));
    ez.msgBox("MFRC522 Software Version", "Version v1.0");
  } else if (v == 0x92){
    Serial.print(F(" = v2.0"));
    ez.msgBox("MFRC522 Software Version", "Version v2.0");
  } else {
    Serial.print(F(" (unknown)"));
    ez.msgBox("MFRC522 Software Version", "Unknown version");
  }
  Serial.println("");
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
    ez.msgBox("MFRC522 Software Version", "Communication failure, is the MFRC522 properly connected?");
  }
}

void menu_not_implemented() {
  ez.msgBox("Not implemented", "To be implemented");
}

void menu_m5hack() {
  ezMenu systemenu("Spawnrider M5H@ck");
  systemenu.txtSmall();
  systemenu.addItem("System Information", sysInfo);
  systemenu.addItem("WiFi Settings", ez.wifi.menu);
  systemenu.addItem("About M5H@ck", aboutM5Hack);
  systemenu.addItem("Updates via https", mainmenu_ota);
  systemenu.addItem("Power Off", powerOff);
  systemenu.addItem("Back");
  systemenu.upOnFirst("last|up");
  systemenu.downOnLast("first|down");
  systemenu.run();
}

void mainmenu_ota() {
  if (ez.msgBox("Get OTA_https", "This will replace the current program with a new version from GitHub.", "Cancel#OK#") == "OK") {
    ezProgressBar progress_bar("OTA update in progress", "Downloading ...", "Abort");
#include "raw_githubusercontent_com.h" // the root certificate is now in const char * root_cert
    if (ez.wifi.update("https://raw.githubusercontent.com/spawnrider/m5hack/master/binaries/m5hack_esp32.bin", root_cert, &progress_bar)) {
      ez.msgBox("Over The Air updater", "OTA download successful. Reboot to new firmware", "Reboot");
      ESP.restart();
    } else {
      ez.msgBox("OTA error", ez.wifi.updateError(), "OK");
    }
  }
}

void powerOff() {
  m5.powerOFF();
}

void aboutM5Hack() {
  ez.msgBox("About M5H@ck", "M5H@ck was written by | Yohann Ciurlik | | https://spawnrider.net");
}
