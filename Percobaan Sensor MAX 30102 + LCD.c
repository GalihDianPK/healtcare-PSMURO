#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <LiquidCrystal_I2C.h>

// Inisialisasi objek sensor dan LCD
MAX30105 particleSensor;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Konfigurasi untuk penghitungan BPM rata-rata
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing...");

  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();

  // Tampilan awal
  lcd.setCursor(0, 0);
  lcd.print(" WELCOME ");
  lcd.setCursor(0, 1);
  lcd.print(" TO ");
  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PSMURO");
  lcd.setCursor(0, 1);
  lcd.print("UNIV.GUNADARMA");
  delay(5000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MCU HEALTHCARE");
  lcd.setCursor(0, 1);
  lcd.print("HEART RATE");
  delay(5000);
  lcd.clear();

  // Inisialisasi sensor MAX30105
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 was not found. Please check wiring/power.");
    while (1); // Hentikan program jika sensor tidak ditemukan
  }

  Serial.println("Place your index finger on the sensor with steady pressure.");

  // Pengaturan default sensor
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);  // Red LED sebagai indikator
  particleSensor.setPulseAmplitudeGreen(0);   // Matikan green LED
}

void loop() {
  long irValue = particleSensor.getIR(); // Baca nilai IR

  if (checkForBeat(irValue)) {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;

      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++) {
        beatAvg += rates[x];
      }
      beatAvg /= RATE_SIZE;
    }
  }

  // Tampilkan di serial monitor
  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);

  // Tampilkan di LCD
  if (irValue < 50000) {
    Serial.print(" No finger?");
    lcd.setCursor(0, 0);
    lcd.print("No Finger ?    ");
    lcd.setCursor(0, 1);
    lcd.print("Letakan Jari!  ");
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BPM: ");
    lcd.print(beatsPerMinute);
    lcd.setCursor(0, 1);
    lcd.print("Avg BPM: ");
    lcd.print(beatAvg);
  }

  Serial.println();
  // delay(500); // Optional: gunakan untuk mengurangi flicker layar
}
