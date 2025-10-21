#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

// ================== Konfigurasi ==================

// Pin data untuk sensor suhu DS18B20
#define ONE_WIRE_BUS D5

// Kredensial WiFi
const char* ssid = "UGMURO-INET";
const char* password = "Gepuk15000";

// Konfigurasi ThingSpeak
unsigned long myChannelNumber = 3112191;           // Ganti dengan Channel ID Anda
const char* myWriteAPIKey = "44YK16EQJBKBRK8Z";             // Ganti dengan Write API Key Anda

// ================== Inisialisasi ==================

MAX30105 particleSensor;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Array untuk menghitung rata-rata BPM
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;

WiFiClient client;

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing...");

  // Inisialisasi sensor suhu
  sensors.begin();

  // Koneksi WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");

  // Inisialisasi ThingSpeak
  ThingSpeak.begin(client);

  // Inisialisasi sensor MAX30105
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 was not found. Please check wiring/power.");
    while (1);  // Berhenti jika sensor tidak terdeteksi
  }

  Serial.println("Place your index finger on the sensor with steady pressure.");

  // Konfigurasi sensor
  particleSensor.setup();                     // Pengaturan default
  particleSensor.setPulseAmplitudeRed(0x0A);  // Red LED sebagai indikator
  particleSensor.setPulseAmplitudeGreen(0);   // Green LED dimatikan
}

void loop() {
  // Baca nilai inframerah dari sensor MAX30105
  long irValue = particleSensor.getIR();

  // Deteksi detak jantung
  if (checkForBeat(irValue)) {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    // Validasi nilai BPM yang masuk akal
    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;

      // Hitung rata-rata BPM
      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++) {
        beatAvg += rates[x];
      }
      beatAvg /= RATE_SIZE;
    }
  }

  // Baca suhu dari sensor DS18B20
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);

  // Tampilkan data di Serial Monitor
  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);
  Serial.print(", Temp=");
  Serial.print(temperatureC);
  Serial.println("°C");

  // Kirim data ke ThingSpeak
  ThingSpeak.setField(1, beatsPerMinute);
  ThingSpeak.setField(2, beatAvg);
  ThingSpeak.setField(3, temperatureC);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  // Delay untuk menghindari update terlalu cepat (ThingSpeak: minimal 15 detik)
  delay(20000);
}
