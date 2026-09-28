#include <TinyGPS++.h>
#include <esp_now.h>
#include <WiFi.h>
#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include <time.h> // Included for accurate epoch time calculations

// ── GPS ───────────────────────────────────────────────────────────────────
HardwareSerial gpsSerial(2);
#define GPS_RX   16
#define GPS_TX   17
#define GPS_BAUD 9600

TinyGPSPlus gps;

float gpsspeed = 0, gpsalt = 0, gpslng = 0, gpslat = 0;
float distance = 0;
float prex1 = 0.0;
float prex2 = 0.0;
// ─────────────────────────────────────────────────────────────────────────

// ---------------- ESP NOW ----------------
uint8_t receiverMacAddress[] = {0x78, 0x21, 0x84, 0x6C, 0xD7, 0x18};

struct __attribute__((packed)) DataPacket {
  float temperature1;
  float temperature2;
  float temperature3;
  float temperature4;
  float temperature5;
  float temperature6;
  float temperature7;
  float temperature8;
  float lati;
  float longi;
  float alti;
  float spped;
  float dist;
  uint16_t year;   
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint16_t millisecond; // 2 bytes for 0-999 milliseconds
  uint8_t padding[3];   // 3 empty bytes to force perfect 64-byte Wi-Fi alignment
};

// SPI pins
#define MAXCS1   32
#define MAXCS2   33
#define MAXCS3   25
#define MAXCS4   26
#define MAXCS5   27
#define MAXCS6   5
#define MAXCS7   23
#define MAXCS8   0

#define SPI_SCK   18
#define SPI_MISO  19
#define SPI_MOSI  4

Adafruit_MAX31855 thermocouple1(MAXCS1);
Adafruit_MAX31855 thermocouple2(MAXCS2);
Adafruit_MAX31855 thermocouple3(MAXCS3);
Adafruit_MAX31855 thermocouple4(MAXCS4);
Adafruit_MAX31855 thermocouple5(MAXCS5);
Adafruit_MAX31855 thermocouple6(MAXCS6);
Adafruit_MAX31855 thermocouple7(MAXCS7);
Adafruit_MAX31855 thermocouple8(MAXCS8);

float t1 = 0.0, t2 = 0.0, t3 = 0.0, t4 = 0.0;
float t5 = 0.0, t6 = 0.0, t7 = 0.0, t8 = 0.0;

double calculateDistance(float lat1, float lon1, float lat2, float lon2) {
  double delta  = radians(lon1 - lon2);
  double sdlong = sin(delta);
  double cdlong = cos(delta);
  lat1 = radians(lat1);
  lat2 = radians(lat2);
  double slat1 = sin(lat1);
  double clat1 = cos(lat1);
  double slat2 = sin(lat2);
  double clat2 = cos(lat2);
  delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
  delta = sq(delta);
  delta += sq(clat2 * sdlong);
  delta = sqrt(delta);
  double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
  delta = atan2(delta, denom);
  return delta * 6372795;
}

TaskHandle_t Task1;

void Handle_inputs(void *parameter) {
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(1));

    while (gpsSerial.available() > 0) {
      if (gps.encode(gpsSerial.read())) {

        if (gps.location.isValid()) {
          gpslat = gps.location.lat();
          gpslng = gps.location.lng();
        }

        if (gps.altitude.isValid())
          gpsalt = gps.altitude.meters();

        if (gps.speed.isValid())
          gpsspeed = gps.speed.kmph();

        if ((gpslat != 0) && (gpslng != 0)) {
          distance += (calculateDistance(gpslat, gpslng, prex1, prex2) / 1000);
          if (distance > 8000) distance = 0;
          prex1 = gpslat;
          prex2 = gpslng;
        }
      }
    }
  }
}

void setup() {
  // Set ESP32 Timezone environment to UTC so mktime behaves predictably
  setenv("TZ", "UTC", 1);
  tzset();

  Serial.begin(9600);
  delay(500);

  // GPS on Serial2
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX, GPS_TX);

  // SPI
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

  int csPins[] = {32, 33, 25, 26, 27, 5,   23, 0};
  for (int i = 0; i < 8; i++) {
    pinMode(csPins[i], OUTPUT);
    digitalWrite(csPins[i], HIGH);
  }

  Serial.println("MAX31855 test");
  Serial.print("Initializing sensors...IPES_TxFixed.ino running");

  if (!thermocouple1.begin()) { Serial.println("ERROR sensor 1"); while (1) delay(10); }
  if (!thermocouple2.begin()) { Serial.println("ERROR sensor 2"); while (1) delay(10); }
  if (!thermocouple3.begin()) { Serial.println("ERROR sensor 3"); while (1) delay(10); }
  if (!thermocouple4.begin()) { Serial.println("ERROR sensor 4"); while (1) delay(10); }
  if (!thermocouple5.begin()) { Serial.println("ERROR sensor 5"); while (1) delay(10); }
  if (!thermocouple6.begin()) { Serial.println("ERROR sensor 6"); while (1) delay(10); }
  if (!thermocouple7.begin()) { Serial.println("ERROR sensor 7"); while (1) delay(10); }
  if (!thermocouple8.begin()) { Serial.println("ERROR sensor 8"); while (1) delay(10); }

  WiFi.mode(WIFI_STA);

  xTaskCreatePinnedToCore(Handle_inputs, "Handle_inputs", 10000, NULL, 2, &Task1, 0);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMacAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("\nInitialization complete. Ready to send data.");
}

void loop() {
  t1 = thermocouple1.readCelsius();
  t2 = thermocouple2.readCelsius();
  t3 = thermocouple3.readCelsius();
  t4 = thermocouple4.readCelsius();
  t5 = thermocouple5.readCelsius();
  t6 = thermocouple6.readCelsius();
  t7 = thermocouple7.readCelsius();
  t8 = thermocouple8.readCelsius();

  Serial.print("Thermocouple 1 : "); Serial.print(t1); Serial.println(" C");
  Serial.print("Thermocouple 2 : "); Serial.print(t2); Serial.println(" C");
  Serial.print("Thermocouple 3 : "); Serial.print(t3); Serial.println(" C");
  Serial.print("Thermocouple 4 : "); Serial.print(t4); Serial.println(" C");
  Serial.print("Thermocouple 5 : "); Serial.print(t5); Serial.println(" C");
  Serial.print("Thermocouple 6 : "); Serial.print(t6); Serial.println(" C");
  Serial.print("Thermocouple 7 : "); Serial.print(t7); Serial.println(" C");
  Serial.print("Thermocouple 8 : "); Serial.print(t8); Serial.println(" C");

  Serial.print("lat : ");       Serial.print(gpslat, 6);
  Serial.print(" long : ");     Serial.print(gpslng, 6);
  Serial.print(" speed : ");    Serial.print(gpsspeed, 3);
  Serial.print(" distance : "); Serial.print(distance, 3);

  // --- Date and Time Extraction & UTC to IST Conversion ---
  uint16_t ist_year = 0, ist_millisecond = 0;
  uint8_t ist_month = 0, ist_day = 0, ist_hour = 0, ist_min = 0, ist_sec = 0;

  if (gps.date.isValid() && gps.time.isValid() && gps.date.year() > 2000) {
    struct tm t = {0};
    t.tm_year = gps.date.year() - 1900; 
    t.tm_mon = gps.date.month() - 1;    
    t.tm_mday = gps.date.day();
    t.tm_hour = gps.time.hour();
    t.tm_min = gps.time.minute();
    t.tm_sec = gps.time.second();
    t.tm_isdst = 0;

    // Convert GPS UTC time to Epoch
    time_t utc_epoch = mktime(&t);
    
    // Add 5 hours and 30 minutes (19800 seconds) for IST
    utc_epoch += 19800;

    // Convert back to structured time
    struct tm *ist = gmtime(&utc_epoch);

    ist_year = ist->tm_year + 1900;
    ist_month = ist->tm_mon + 1;
    ist_day = ist->tm_mday;
    ist_hour = ist->tm_hour;
    ist_min = ist->tm_min;
    ist_sec = ist->tm_sec;
    
    // Extract centiseconds and convert to milliseconds
    ist_millisecond = gps.time.centisecond() * 10; 
  }

  // --- Strict ISO 8601 JSON Formatting ---
  char isoBuffer[40];
  snprintf(isoBuffer, sizeof(isoBuffer), " \"t\":\"%04d-%02d-%02dT%02d:%02d:%02d.%03d\"", 
           ist_year, ist_month, ist_day, ist_hour, ist_min, ist_sec, ist_millisecond);
  
  Serial.println(isoBuffer);

  DataPacket data;
  data.temperature1 = t1;
  data.temperature2 = t2;
  data.temperature3 = t3;
  data.temperature4 = t4;
  data.temperature5 = t5;
  data.temperature6 = t6;
  data.temperature7 = t7;
  data.temperature8 = t8;
  data.lati  = gpslat;
  data.longi = gpslng;
  data.alti  = gpsalt;
  data.spped = gpsspeed;
  data.dist  = distance;
  
  // Pack converted IST time
  data.year   = ist_year;
  data.month  = ist_month;
  data.day    = ist_day;
  data.hour   = ist_hour;
  data.minute = ist_min;
  data.second = ist_sec;
  data.millisecond = ist_millisecond;
  
  // Initialize padding bytes to prevent memory garbage
  data.padding[0] = 0;
  data.padding[1] = 0;
  data.padding[2] = 0;

  if (esp_now_send(receiverMacAddress, (uint8_t*)&data, sizeof(data)) != ESP_OK) {
    Serial.println("Error sending data over ESP-NOW");
    return;
  }

  Serial.println("Data sent successfully.");

  delay(10);
}