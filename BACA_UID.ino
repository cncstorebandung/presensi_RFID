
#include <SPI.h>
#include <MFRC522.h>

// CNC STORE BANDUNG
// TEST RFID ABSENSI - TAHAP 1

#define SS_PIN   5
#define RST_PIN  4

#define SCK_PIN  18
#define MISO_PIN 19
#define MOSI_PIN 23

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  delay(1000);

  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  rfid.PCD_Init();

  Serial.println("==========================");
  Serial.println("   SISTEM ABSENSI RFID");
  Serial.println("   CNC STORE BANDUNG");
  Serial.println("==========================");
  Serial.println("Tempelkan kartu RFID...");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  String uid = "";

  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      uid += "0";
    }

    uid += String(rfid.uid.uidByte[i], HEX);
  }

  uid.toUpperCase();

  Serial.println("--------------------------");
  Serial.print("UID KARTU : ");
  Serial.println(uid);
  Serial.println("Kartu berhasil terbaca!");
  Serial.println("--------------------------");

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  delay(1000);
}
