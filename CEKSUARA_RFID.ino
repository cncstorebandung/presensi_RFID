
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <DFRobotDFPlayerMini.h>

// ======================================
// CNC STORE BANDUNG
// ABSENSI RFID + SUARA OTOMATIS
// TAHAP 3
// ======================================

// RFID RC522
#define SS_PIN    5
#define RST_PIN   4
#define SCK_PIN   18
#define MISO_PIN  19
#define MOSI_PIN  23

// DFPLAYER MINI
#define DF_RX 16
#define DF_TX 17

MFRC522 rfid(SS_PIN, RST_PIN);

HardwareSerial dfSerial(2);
DFRobotDFPlayerMini dfPlayer;

bool suaraReady = false;

// DATABASE SISWA
struct Siswa {
  String nama;
  String kelas;
  String uid;
  int nomorAudio;
};

Siswa siswa[5] = {
  {"FAHRI UDIN", "12 ELEKTRO 1", "FB8EC101", 1},
  {"SAMSUDIN", "11 TKJ 1", "24B9F603", 2},
  {"JAMALUDIN", "12 RPL 3", "5E2B4B06", 3},
  {"AMIRUDIN", "10 TKJ 1", "D03D085F", 4},
  {"KOMARUDIN", "11 TKR 2", "9EEEC001", 5}
};

// Pencegah scan berulang terlalu cepat
String uidTerakhir = "";
unsigned long waktuTerakhir = 0;
const unsigned long jedaScan = 5000;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // INISIALISASI RFID
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  rfid.PCD_Init();
  delay(100);

  // INISIALISASI DFPLAYER
  dfSerial.begin(
    9600,
    SERIAL_8N1,
    DF_RX,
    DF_TX
  );

  if (dfPlayer.begin(dfSerial, true, true)) {
    suaraReady = true;
    dfPlayer.volume(22);
    Serial.println("DFPlayer berhasil!");
  } else {
    Serial.println("DFPlayer tidak terdeteksi!");
  }

  Serial.println("==========================");
  Serial.println("  SISTEM ABSENSI RFID");
  Serial.println("  CNC STORE BANDUNG");
  Serial.println("==========================");
  Serial.println("Silakan tempel kartu...");
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

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  // CEGAH PEMBACAAN BERULANG
  if (uid == uidTerakhir &&
      millis() - waktuTerakhir < jedaScan) {
    return;
  }

  uidTerakhir = uid;
  waktuTerakhir = millis();

  Serial.println("-----------------------");
  Serial.print("UID: ");
  Serial.println(uid);

  bool ditemukan = false;

  for (int i = 0; i < 5; i++) {

    if (uid == siswa[i].uid) {
      ditemukan = true;

      Serial.println("KARTU DIKENALI!");
      Serial.print("Nama  : ");
      Serial.println(siswa[i].nama);

      Serial.print("Kelas : ");
      Serial.println(siswa[i].kelas);

      if (suaraReady) {
        dfPlayer.playMp3Folder(
          siswa[i].nomorAudio
        );

        Serial.println("Memutar suara...");
      }

      Serial.println("Absensi terdeteksi!");
      break;
    }
  }

  if (!ditemukan) {
    Serial.println("KARTU TIDAK TERDAFTAR!");
  }

  Serial.println("-----------------------");
  Serial.println("Silakan tempel kartu...");
}
