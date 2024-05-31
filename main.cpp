#include "keypad.cpp"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

// Konfigurasi untuk I2C LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Atur alamat LCD ke 0x27 untuk layar 16 karakter dan 2 baris

// Konfigurasi untuk SoftwareSerial untuk sensor sidik jari
SoftwareSerial mySerial(2, 3);  // Pin RX, TX untuk komunikasi SoftwareSerial dengan sensor sidik jari
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);  // Membuat objek sensor sidik jari

// Inisialisasi keypad
KeypadController keypad;

// Mendefinisikan pin relay
const int relayPin = 12;  // Pin relay untuk mengontrol mekanisme kunci

// Deklarasi fungsi
void setupSystem();
void enrollFingerprint();
void deleteFingerprint();
void lockSystem();
void unlockSystem();
bool isFingerprintMatched();
void setLocked(bool locked);

bool isLocked = true;  // Status sistem kunci, awalnya terkunci

void setup() {
  Serial.begin(9600);  // Inisialisasi serial untuk debugging
  lcd.init();  // Inisialisasi LCD
  lcd.backlight();  // Menyalakan lampu latar LCD
  lcd.setCursor(0, 0);
  lcd.print("Door Lock System");  // Menampilkan pesan awal

  // Inisialisasi pin relay
  pinMode(relayPin, OUTPUT);
  setLocked(true);  // Pastikan sistem dimulai dalam keadaan terkunci

  // Inisialisasi sensor sidik jari
  mySerial.begin(57600);  // Memulai komunikasi serial dengan sensor sidik jari
  finger.begin(57600);  // Inisialisasi sensor sidik jari
  delay(1000);  // Tunggu sensor untuk inisialisasi
  
  Serial.println("Initializing fingerprint sensor...");
  lcd.setCursor(0, 1);
  if (finger.verifyPassword()) {  // Memeriksa apakah sensor sidik jari terhubung dengan benar
    lcd.print("Fingerprint OK");
    Serial.println("Fingerprint sensor initialized successfully.");
  } else {
    lcd.print("Finger Err");
    Serial.println("Error: Unable to initialize fingerprint sensor.");
    while (1) {
      delay(1);  // Loop tak terbatas untuk menghentikan eksekusi lebih lanjut jika inisialisasi sensor gagal
    }
  }
  delay(2000);
  lcd.clear();
  setupSystem();  // Menampilkan instruksi setup pada LCD
}

void loop() {
  char key = keypad.getKey();  // Mendapatkan penekanan tombol dari keypad
  if (key) {
    lcd.clear();
    switch (key) {
      case 'A':  // Mengubah dari '1' menjadi 'A'
        lcd.setCursor(0, 0);
        lcd.print("Input Fingerprint");
        enrollFingerprint();  // Mendaftarkan sidik jari baru
        break;
      case 'B':  // Mengubah dari '2' menjadi 'B'
        lockSystem();  // Mengunci sistem
        break;
      default:
        lcd.setCursor(0, 0);
        lcd.print("Invalid Option");  // Menangani penekanan tombol yang tidak valid
        delay(2000);
        lcd.clear();
        setupSystem();
        break;
    }
  }

  if (isFingerprintMatched() && isLocked) {  // Memeriksa apakah sidik jari cocok dan sistem terkunci
    unlockSystem();  // Membuka sistem
  }
}

void setupSystem() {
  lcd.setCursor(0, 0);
  lcd.print("Input A to start");  // Mengubah dari '1' menjadi 'A'
  if (isLocked) {
    lcd.setCursor(0, 1);
    lcd.print("Fingerprint to open");
  }
}

void enrollFingerprint() {
  int id = 1;  // ID tetap untuk kesederhanaan
  lcd.setCursor(0, 1);
  lcd.print("Place Finger");

  while (finger.getImage() != FINGERPRINT_OK);  // Tunggu gambar sidik jari yang valid
  if (finger.image2Tz(1) != FINGERPRINT_OK) {  // Mengonversi gambar ke template sidik jari
    lcd.setCursor(0, 1);
    lcd.print("Error");
    delay(2000);
    lcd.clear();
    setupSystem();
    return;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Remove Finger");
  delay(2000);

  while (finger.getImage() != FINGERPRINT_NOFINGER);  // Tunggu sidik jari dilepas
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Place Again");

  while (finger.getImage() != FINGERPRINT_OK);  // Tunggu gambar sidik jari yang valid lagi
  if (finger.image2Tz(2) != FINGERPRINT_OK) {  // Mengonversi gambar ke template sidik jari lagi
    lcd.setCursor(0, 1);
    lcd.print("Error");
    delay(2000);
    lcd.clear();
    setupSystem();
    return;
  }

  if (finger.createModel() != FINGERPRINT_OK) {  // Membuat model sidik jari
    lcd.setCursor(0, 1);
    lcd.print("Error");
    delay(2000);
    lcd.clear();
    setupSystem();
    return;
  }

  if (finger.storeModel(id) != FINGERPRINT_OK) {  // Menyimpan model sidik jari dalam sensor
    lcd.setCursor(0, 1);
    lcd.print("Error");
    delay(2000);
    lcd.clear();
    setupSystem();
    return;
  }

  lcd.setCursor(0, 1);
  lcd.print("Locked");
  lockSystem();  // Mengunci sistem setelah pendaftaran
  delay(2000);
  lcd.clear();
}

void deleteFingerprint() {
  int id = 1;  // ID tetap untuk kesederhanaan
  lcd.setCursor(0, 0);
  lcd.print("Deleting...");

  if (finger.deleteModel(id) == FINGERPRINT_OK) {  // Menghapus model sidik jari dari sensor
    lcd.setCursor(0, 1);
    lcd.print("Deleted");
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Error");
  }
  delay(2000);
  lcd.clear();
  setupSystem();
}

bool isFingerprintMatched() {
  int id = 1;  // ID tetap untuk kesederhanaan
  if (finger.getImage() != FINGERPRINT_OK) return false;  // Mendapatkan gambar sidik jari
  if (finger.image2Tz() != FINGERPRINT_OK) return false;  // Mengonversi gambar ke template
  if (finger.fingerFastSearch() != FINGERPRINT_OK) return false;  // Mencari sidik jari dalam memori sensor
  if (finger.fingerID == id) return true;  // Memeriksa apakah sidik jari cocok dengan template yang disimpan
  return false;
}

void setLocked(bool locked) {
  isLocked = locked;
  digitalWrite(relayPin, locked ? HIGH : LOW);  // Mengaktifkan relay (mengunci) jika terkunci, menonaktifkan jika terbuka
  lcd.setCursor(0, 0);
  if (locked) {
    lcd.print("System Locked  ");
    lcd.setCursor(0, 1);
    lcd.print("Fingerprint to open");
  } else {
    lcd.print("Unlocked       ");
    lcd.setCursor(0, 1);
    lcd.print("                ");  // Menghapus baris kedua
  }
}

void lockSystem() {
  setLocked(true);  // Mengatur status sistem menjadi terkunci
  delay(2000);
  lcd.clear();
  setupSystem();
}

void unlockSystem() {
  setLocked(false);  // Mengatur status sistem menjadi terbuka
  delay(2000);
  lcd.clear();
  setupSystem();
}
