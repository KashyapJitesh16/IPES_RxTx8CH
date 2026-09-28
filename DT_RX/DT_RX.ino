#include <WiFi.h>              //Built-in
#include <ESP32WebServer.h>    //https://github.com/Pedroalbuquerque/ESP32WebServer download and place in your Libraries folder
#include <ESPmDNS.h>
ESP32WebServer server(80);

#define servername "myserver" //Define the name to your server... 
//#define SD_pin 5 //G16 in my case

bool   SD_present = true; //Controls if the SD card is present or not


#include <esp_now.h>
#include <WiFi.h>
#include <SPI.h>
//#include <TaskScheduler.h>

#include "RTClib.h"
RTC_PCF8563 rtc;

#include "process_serial_command.hpp"
#include "CSS.h"
#include "FS.h"
#include "SD_MMC.h"
#include "SPI.h"
#include <Adafruit_ADS1X15.h>

// Global Variables
#include <ADS1256.h>
float clockMHZ = 7.68; // crystal frequency used on ADS1256
float vRef = 2.5; // voltage reference
// Construct and init ADS1256 object
ADS1256 adc(clockMHZ, vRef, false); // RESETPIN is permanently tied to 3.3v
//ADS1256 A(26, 12, 16, 5, 2.500);

float x1 = 0.0, x2 = 0.0, x3 = 0.0, x4 = 0.0;
volatile int pulseCount1 = 0, pulseCount2 = 0, pulseCount3 = 0, pulseCount4 = 0;
float distance = 0;
float t1, t2, t3, t4, t5, t6, t7, t8;
String fnme;

// FIX 1: Initialize arrays to 0 to prevent NaN on first read
float rdataa[25] = {0};
float dataa[25] = {0};                              // LAT, LNG, ALT, SPEED, DIST

int minp[25], maxp[25], minv[25], maxv[25], offset[25], zeroo[25];
int pulse_type = 0;

// Constants
#define pulse_pin1 25
#define pulse_pin2 33
#define pulse_pin3 32
#define pulse_pin4 35

//#define adc_pin1 32
//#define adc_pin2 35
unsigned long lastMillis = 0;
unsigned long calculationInterval = 10;

float gpsspeed = 0, gpsalt = 0, gpslng = 0, gpslat = 0;
float  prex1 = 0.0;
float  prex2 = 0.0;

// Date and Time variables for incoming ESP-NOW data
uint16_t rec_year = 0;
uint8_t rec_month = 0;
uint8_t rec_day = 0;
uint8_t rec_hour = 0;
uint8_t rec_min = 0;
uint8_t rec_sec = 0;

// Function Prototypes
void handleInterrupt1();
void handleInterrupt2();
void handleInterrupt3();
void handleInterrupt4();
void onDataReceived(const uint8_t *mac, const uint8_t *data, int len);
void readFile(fs::FS &fs, const char *path);
void writeFile(fs::FS &fs, const char *path, const char *message);
void appendFile(fs::FS &fs, const char *path, const char *message);
//static void smartDelay(unsigned long ms);
//static void printFloat(float val, bool valid, int len, int prec);
#define EARTH_RADIUS 6371000
String logd;
unsigned long previousMillis = 0;

void Handle_inputs(void *parameter) {
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(1));

    adc.setChannel(0);
    adc.waitDRDY(); // wait for DRDY to go low before next register read
    rdataa[0] = adc.readCurrentChannel(); // read as voltage according to gain and vref
    //  Serial.print("ch 1. : "); Serial.println(volts0 , 10); // Print as decimal, 10 decimal places

    adc.setChannel(1);
    adc.waitDRDY(); // wait for DRDY to go low before next register read
    rdataa[1] = adc.readCurrentChannel(); // read as voltage according to gain and vref
    //  Serial.print("ch 2. : "); Serial.println(volts1 , 10); // Print as decimal, 10 decimal places

    adc.setChannel(2);
    adc.waitDRDY(); // wait for DRDY to go low before next register read
    rdataa[2] = adc.readCurrentChannel(); // read as voltage according to gain and vref
    //  Serial.print("ch 3. : "); Serial.println(volts2 , 10); // Print as decimal, 10 decimal places

    adc.setChannel(3);
    adc.waitDRDY(); // wait for DRDY to go low before next register read
    rdataa[3] = adc.readCurrentChannel(); // read as voltage according to gain and vref
    //  Serial.print("ch 4. : "); Serial.println(volts3 , 10); // Print as decimal, 10 decimal places

    adc.setChannel(4);
    adc.waitDRDY(); // wait for DRDY to go low before next register read
    rdataa[4] = adc.readCurrentChannel(); // read as voltage according to gain and vref
    //  Serial.print("ch 5. : "); Serial.println(volts4 , 10); // Print as decimal, 10 decimal places

    adc.setChannel(5);
    adc.waitDRDY(); // wait for DRDY to go low before next register read
    rdataa[5] = adc.readCurrentChannel(); // read as voltage according to gain and vref
    //  Serial.print("ch 6. : "); Serial.println(volts5 , 10); // Print as decimal, 10 decimal places

    adc.setChannel(6);
    adc.waitDRDY(); // wait for DRDY to go low before next register read
    rdataa[6] = adc.readCurrentChannel(); // read as voltage according to gain and vref
    //  Serial.print("ch 7. : "); Serial.println(volts6 , 10); // Print as decimal, 10 decimal places

    adc.setChannel(7);
    adc.waitDRDY(); // wait for DRDY to go low before next register read
    rdataa[7] = adc.readCurrentChannel(); // read as voltage according to gain and vref
    //  Serial.print("ch 8. : "); Serial.println(volts7 , 10); // Print as decimal, 10 decimal places
    //  Serial.println();

    //    for (int i = 0; i < 8; i++)
    //    {
    //      rdataa[i] = A.convertToVoltage(A.cycleSingle());
    ////      Serial.print(dataa[i], 4); //print the converted single-ended results with 4 digits
    ////      Serial.print("\t");//tab separator to separate the 4 conversions shown in the same line
    //    }
    ////    Serial.println();
    //        if (shift_to_rec) {
    //      //      if (log_header) {
    //      //        log_header = false;
    //      //
    //      //
    //      //      }
    //      const char* cfnme = fnme.c_str();
    //      const char* clogd = logd.c_str();
    //      appendFile(SD_MMC, cfnme, clogd);
    //    }
    // Print Data
    Serial.print("V1_");   Serial.print(dataa[0], 3);
    Serial.print("&V2_");  Serial.print(dataa[1], 3);
    Serial.print("&V3_");  Serial.print(dataa[2], 3);
    Serial.print("&V4_");  Serial.print(dataa[3], 3);
    Serial.print("&V5_");  Serial.print(dataa[4], 3);
    Serial.print("&V6_");  Serial.print(dataa[5], 3);
    Serial.print("&V7_");  Serial.print(dataa[6], 3);
    Serial.print("&V8_");  Serial.print(dataa[7], 3);
    Serial.print("&KTEMP1_"); Serial.print(dataa[8], 3);
    Serial.print("&KTEMP2_");  Serial.print(dataa[9], 3);
    Serial.print("&KTEMP3_"); Serial.print(dataa[10], 3);
    Serial.print("&KTEMP4_");  Serial.print(dataa[11], 3);
    Serial.print("&KTEMP5_"); Serial.print(dataa[12], 3);
    Serial.print("&KTEMP6_");  Serial.print(dataa[13], 3);
    Serial.print("&KTEMP7_"); Serial.print(dataa[14], 3);
    Serial.print("&KTEMP8_");  Serial.print(dataa[15], 3);
    Serial.print("&PULSE1_"); Serial.print(dataa[16]);
    Serial.print("&PULSE2_"); Serial.print(dataa[17]);
    Serial.print("&PULSE3_"); Serial.print(dataa[18]);
    Serial.print("&PULSE4_"); Serial.print(dataa[19]);
    Serial.print("&LAT_");  Serial.print(dataa[20], 6);
    Serial.print("&LNG_");  Serial.print(dataa[21], 6);
    Serial.print("&ALT_");  Serial.print(dataa[22], 3);
    Serial.print("&SPEED_");  Serial.print(dataa[23], 3);
    Serial.print("&DIST_"); Serial.print(dataa[24], 3);
    
    // Print Date and Time
    Serial.print("&DATE_"); 
    Serial.print(rec_year); Serial.print("-");
    if(rec_month < 10) Serial.print("0"); Serial.print(rec_month); Serial.print("-");
    if(rec_day < 10) Serial.print("0"); Serial.print(rec_day);
    
    Serial.print("&TIME_");
    if(rec_hour < 10) Serial.print("0"); Serial.print(rec_hour); Serial.print(":");
    if(rec_min < 10) Serial.print("0"); Serial.print(rec_min); Serial.print(":");
    if(rec_sec < 10) Serial.print("0"); Serial.print(rec_sec);

    int available_bytes = SD_MMC.totalBytes() - SD_MMC.usedBytes();
    Serial.print("&AM_"); Serial.print(available_bytes);
    Serial.print("&TM_"); Serial.print(SD_MMC.totalBytes()); Serial.println("&");
    server.handleClient();
  }
}

TaskHandle_t Task1;

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
};

float floatmap(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void setup() {
  // Initialize Serial and SoftwareSerial
  Serial.begin(115200);
  //  ss.begin(9600); 
  Serial.println("Starting ADC");
  // start the ADS1256 with data rate of 15 SPS and gain x1
  adc.begin(ADS1256_DRATE_500SPS, ADS1256_GAIN_1, false);
  //  A.InitializeADC();
  Serial.println("ADC Started");

  // Initialize WiFi and ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(onDataReceived);

  // Print MAC Address
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  // Initialize Interrupts
  pinMode(2, INPUT_PULLUP);
  delay(2000);
  pinMode(pulse_pin1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pulse_pin1), handleInterrupt1, FALLING);
  pinMode(pulse_pin2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pulse_pin2), handleInterrupt2, FALLING);
  pinMode(pulse_pin3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pulse_pin3), handleInterrupt3, FALLING);
  pinMode(pulse_pin4, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pulse_pin4), handleInterrupt4, FALLING);

  // Initialize ADC
  //  pinMode(adc_pin1, INPUT_PULLUP);
  //  pinMode(adc_pin2, INPUT_PULLUP);
  //  if (!ads.begin()) {
  //    Serial.println("Failed to initialize ADS.");
  //    while (1);
  //  }

  pinMode(2, INPUT_PULLUP);
  WiFi.softAP("IPES File Manager", "12345678"); //Network and password for the access point genereted by ESP32

  //Set your preferred server name, if you use "mcserver" the address would be http://myserver.local/
  if (!MDNS.begin(servername))
  {
    Serial.println(F("Error setting up MDNS responder!"));
    ESP.restart();
  }

//  Serial.print(F("Initializing SD card..."));
//
//  if (!SD_MMC.begin()) {
//    Serial.println("Card Mount Failed");
//    return;
//  }
//  uint8_t cardType = SD_MMC.cardType();
//
//  if (cardType == CARD_NONE) {
//    Serial.println("No SD_MMC card attached");
//    return;
//  }
//
//  Serial.print("SD_MMC Card Type: ");
//  if (cardType == CARD_MMC) {
//    Serial.println("MMC");
//  } else if (cardType == CARD_SD) {
//    Serial.println("SDSC");
//  } else if (cardType == CARD_SDHC) {
//    Serial.println("SDHC");
//  } else {
//    Serial.println("UNKNOWN");
//  }
//
//  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
//  Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);
  /********* Server Commands  **********/
  server.on("/",         SD_dir);
  server.on("/upload",   File_Upload);
  server.on("/serialon",   serialon);
  server.on("/serialoff",   serialoff);
  server.on("/fupload",  HTTP_POST, []() {
    server.send(200);
  }, handleFileUpload);

  server.begin();

  Serial.println("HTTP server started");
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if ((rtc.lostPower())) {
    Serial.println("RTC is NOT initialized, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    //    rtc.adjust(DateTime(2024, 4, 19, 14, 37, 0));
    //
    // Note: allow 2 seconds after inserting battery or applying external power
    // without battery before calling adjust(). This gives the PCF8523's
    // crystal oscillator time to stabilize. If you call adjust() very quickly
    // after the RTC is powered, lostPower() may still return true.
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  //  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  // When the RTC was stopped and stays connected to the battery, it has
  // to be restarted by clearing the STOP bit. Let's do this to ensure
  // the RTC is running.
  rtc.start();

  DateTime now = rtc.now();
  String dt = String(now.day(), DEC) + "/" + String(now.month(), DEC) + "/" + String(now.year(), DEC);
  String tt = String(now.hour(), DEC) + ":" + String(now.minute(), DEC) + ":" + String(now.second(), DEC);

  Serial.print("Current time: ");
  Serial.println(dt + " : " + tt);

  xTaskCreatePinnedToCore(Handle_inputs, "Handle_inputs", 10000, NULL, 2, &Task1, 0);



}
String senddh = "";
//int c=0;
void loop() {
  // GPS Data
  if ( (!shift_to_serial_mode)) {//(millis() - lastMillis >= 1) &&
    //    lastMillis = millis();
    //    printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
    //    printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
    //    printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
    //    printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);

    //{"ct":"header","d":"Date,Time, Ch.1-Humidity (Percentage), Ch.2-Temp (C),Ch.9-K Temp 1 (C),Ch.10-K Temp 2 (C),Ch.21-GPS Latitude,Ch.22-GPS Longitude,Ch.23-GPS Altitude (m),Ch.24-Speed (m/s),Ch.25-Distance (m),"}



    DateTime now = rtc.now();
    String dt = String(now.day(), DEC) + "/" + String(now.month(), DEC) + "/" + String(now.year(), DEC);
    String tt = String(now.hour(), DEC) + ":" + String(now.minute(), DEC) + ":" + String(now.second(), DEC);

    // Log Data
    //    int16_t adc1 = ads.readADC_SingleEnded(0);
    //    int16_t adc2 = ads.readADC_SingleEnded(1);
    //    rdataa[0] = ads.computeVolts(adc1);
    //    rdataa[1] = ads.computeVolts(adc2);

    //calculations
    //analog setting
    for (int i = 0; i < 8; i++) {
      // FIX 2: Corrected condition — only call floatmap when maxp is valid (non-zero and not equal to minp)
      // Previously was (maxp[i] == 0) which caused division-by-zero → NaN
      if (maxp[i] != 0 && maxp[i] != minp[i]) {
        dataa[i] = floatmap(rdataa[i], minp[i], maxp[i], minv[i], maxv[i]);
      } else {
        dataa[i] = rdataa[i];
      }
      dataa[i] = dataa[i] + offset[i];
      dataa[i] = dataa[i] - zeroo[i];
    }

    //k type setting
    for (int i = 8; i < 16; i++) {
      dataa[i] = rdataa[i] + offset[i];
      dataa[i] = dataa[i] - zeroo[i];
    }

    //pulse setting
    rdataa[16] = pulseCount1;
    rdataa[17] = pulseCount2;
    rdataa[18] = pulseCount3;
    rdataa[19] = pulseCount4;


    for (int i = 16; i < 20; i++) {
      //      if (zeroo[i] == 1) {//incremental
      dataa[i] = rdataa[i];
      //        Serial.print("i data: ");   Serial.println(i);
      //        Serial.print("data");  Serial.println(dataa[i]);
      //      }
      //      if (zeroo[i] == 2) {//on/off
      //        unsigned long currentMillis = millis();
      //        if (currentMillis - previousMillis >= 5000) { // Adjust 1000 for your desired delay in milliseconds
      //          // Code to be executed every second
      //          previousMillis = currentMillis;  // Update for next check
      //          if (rdataa[i] > 6) {
      //            dataa[i] = 1;
      //          } else {
      //            dataa[i] = 0;
      //          }
      //          pulseCount1 = 3;
      //          pulseCount2 = 3;
      //          pulseCount3 = 3;
      //          pulseCount4 = 3;
      //        }
      //      }
      //if (zeroo[i] == 2) { // RPM
      //        dataa[i] = rdataa[i] / offset[i];
      //      } if (zeroo[i] == 3) {//speed
      //        dataa[i] = rdataa[i] * offset[i];
      //      } if (zeroo[i] == 4) {//distance
      //        dataa[i] = 2 * (22 / 7) * offset[i] * rdataa[i];
      //      } if (zeroo[i] = 0){//invslid
      //        dataa[i] = -1;
      //        Serial.print("invalid channel: ");   Serial.println(i);
      ////        Serial.print("data");  Serial.println(dataa[i]);
      //      }
    }

    //    for (int i = 16; i < 20; i++) {
    //      if (offset[i] > 0) {
    //        dataa[i] = rdataa[i];
    //      } else {
    //        if (rdataa[i] > 0) {
    //          rdataa[i] = 1111111;
    //          dataa[i] = 1111111;
    //        } else {
    //          rdataa[i] = 999999;
    //          dataa[i] = 999999;
    //        }
    //      }
    //      if (zeroo[i] > 0) {
    //        rdataa[i] = 0;
    //        dataa[i] = 0;
    //        zeroo[i] = 0;
    //      }
    //    }


    //
    //    pulseCount1 = dataa[16];
    //    pulseCount2 = dataa[17];
    //    pulseCount3 = dataa[18];
    //    pulseCount4 = dataa[19];



    //gps setting
    for (int i = 20; i < 25; i++) {
      dataa[i] = rdataa[i];
      if (zeroo[i] > 0) {
        rdataa[i] = 0;
        dataa[i] = 0;
        zeroo[i] = 0;
      }// only for distance
    }

    //log data
    logd = String(dt) + "," + String(tt) + "," + String(dataa[0], 6) + "," + String(dataa[1], 6) + "," + String(dataa[2], 6) + "," + String(dataa[3], 6) + "," + String(dataa[4], 6) + "," + String(dataa[5], 6) + "," + String(dataa[6], 6) + "," + String(dataa[7], 6) + "," + String(dataa[8]) + "," + String(dataa[9]) + "," + String(dataa[16]) + "," + String(rdataa[20], 6 ) + "," + String(rdataa[21], 6) + "," + String(rdataa[22], 3) + "," + String(rdataa[23], 3) + "," + String(rdataa[24], 3) + "\n";
    //    logd += "," + String(volts1, 6) + "," + String(volts2, 6) + "," + String(pulseCount1) + "," + String(pulseCount2) + "\n";
    if (shift_to_rec) {
      //      if (log_header) {
      //        log_header = false;
      //
      //
      //      }
      const char* cfnme = fnme.c_str();
      const char* clogd = logd.c_str();
      appendFile(SD_MMC, cfnme, clogd);
    }
    // Print Data
    //    Serial.print("V1_");   Serial.print(dataa[0], 3);
    //    Serial.print("&V2_");  Serial.print(dataa[1], 3);
    //    Serial.print("&V3_");  Serial.print(dataa[2], 3);
    //    Serial.print("&V4_");  Serial.print(dataa[3], 3);
    //    Serial.print("&V5_");  Serial.print(dataa[4], 3);
    //    Serial.print("&V6_");  Serial.print(dataa[5], 3);
    //    Serial.print("&V7_");  Serial.print(dataa[6], 3);
    //    Serial.print("&V8_");  Serial.print(dataa[7], 3);
    //    Serial.print("&KTEMP1_"); Serial.print(dataa[8], 3);
    //    Serial.print("&KTEMP2_");  Serial.print(dataa[9], 3);
    //    Serial.print("&KTEMP3_"); Serial.print(dataa[10], 3);
    //    Serial.print("&KTEMP4_");  Serial.print(dataa[11], 3);
    //    Serial.print("&KTEMP5_"); Serial.print(dataa[12], 3);
    //    Serial.print("&KTEMP6_");  Serial.print(dataa[13], 3);
    //    Serial.print("&KTEMP7_"); Serial.print(dataa[14], 3);
    //    Serial.print("&KTEMP8_");  Serial.print(dataa[15], 3);
    //    Serial.print("&PULSE1_"); Serial.print(dataa[16]);
    //    Serial.print("&PULSE2_"); Serial.print(dataa[17]);
    //    Serial.print("&PULSE3_"); Serial.print(dataa[18]);
    //    Serial.print("&PULSE4_"); Serial.print(dataa[19]);
    //    Serial.print("&LAT_");  Serial.print(dataa[20], 6);
    //    Serial.print("&LNG_");  Serial.print(dataa[21], 6);
    //    Serial.print("&ALT_");  Serial.print(dataa[22], 3);
    //    Serial.print("&SPEED_");  Serial.print(dataa[23], 3);
    //    Serial.print("&DIST_"); Serial.print(dataa[24], 3); Serial.println("_&");
    //

    // Serial Mode
    if (Serial.available() > 0) {
      String input = Serial.readStringUntil('\n');
      Serial.println(input);
      StaticJsonDocument<2048> jsonBuffer;
      DeserializationError error = deserializeJson(jsonBuffer, input);
      if (error) {
        Serial.print("Error parsing JSON: ");
        Serial.println(error.c_str());
      } else {
        if (input.indexOf("ct") > 0) {
          const char* ct = jsonBuffer["ct"];
          Serial.print("value :");
          Serial.println(ct);
          if (strcmp(ct, "span") == 0) {
            int cn =  jsonBuffer["cn"].as<int>();
            minp[cn] =  jsonBuffer["minp"].as<int>();
            maxp[cn] =  jsonBuffer["maxp"].as<int>();
            minv[cn] =  jsonBuffer["minv"].as<int>();
            maxv[cn] =  jsonBuffer["maxv"].as<int>();
            //        dataa[0] = map(rdataa[0], minp, maxp, minv, maxv);
          }

          if (strcmp(ct, "offset") == 0) {
            int cn =  jsonBuffer["cn"].as<int>();
            offset[cn] =  jsonBuffer["v"].as<int>();

            //        dataa[0] = map(rdataa[0], minp, maxp, minv, maxv);
          }

          if (strcmp(ct, "zero") == 0) {
            int cn =  jsonBuffer["cn"].as<int>();
            zeroo[cn] =  dataa[0];
          }

          if (strcmp(ct, "pulse_cal") == 0) {

            int cn =  jsonBuffer["cn"].as<int>();
            if (cn > 15) {
              if (cn < 21) {
                zeroo[cn] =  jsonBuffer["t"].as<int>();

                offset[cn] =  jsonBuffer["v"].as<int>();
                for (int i = 16; i < 20; i++) {
                  if (offset[i] == 0) {
                    dataa[i] = 0;
                  }
                }
              }
              //        dataa[0] = map(rdataa[0], minp, maxp, minv, maxv);
            }
          }

          if (strcmp(ct, "start") == 0) {
            shift_to_serial_mode = true;
            Serial.println(" started");
          }
          if (strcmp(ct, "rec_start") == 0) {
            shift_to_rec = true;
            //        log_header = true;
            Serial.println("Recording started");
          }
          if (strcmp(ct, "rec_stop") == 0) {
            shift_to_rec = false;
            //        log_header = false;
            Serial.println("Recording stoped");
          }
          if (strcmp(ct, "header") == 0) {  //{"ct":"header","d":"Date,Time, Ch.1-Humidity (Percentage),Ch.2-V2,Ch.3-V3,Ch.4-V4,Ch.5-V5,Ch.6-V6,Ch.7-V7,Ch.8-V8, Ch.9-KTEMP1 (C),Ch.10-KTEMP2 (C),Ch.11-KTEMP3 (C),Ch.12-KTEMP4 (C),Ch.13-KTEMP5 (C),Ch.14-KTEMP6 (C),Ch.15-KTEMP7 (C),Ch.16-KTEMP8 (C),Ch.17-ALT (m),Ch.24-SPEED (m/h),Ch.25-DIST (m)"}
            senddh = jsonBuffer["d"].as<String>() + "\n";
            Serial.print("Heafer Recived: ");
            Serial.println(senddh);
          }
          if (strcmp(ct, "ex_data") == 0) {
            String tbuffer = jsonBuffer["ekm"].as<String>();
            if (tbuffer == "") {
              // Create log file
              DateTime now = rtc.now();
              String fnn = String(now.day(), DEC) + "-" + String(now.month(), DEC) + "-" + String(now.year(), DEC) + "_" + String(now.hour(), DEC) + "-" + String(now.minute(), DEC) + "-" + String(now.second(), DEC);

              //          fnme = "/k_log_" + String(millis())  + ".csv";
              fnme = "/k_log" + fnn + ".csv";
              const char* cfnme = fnme.c_str();
              String sendd2 = "K type Gps Datalogger \n";
              const char* csendd2 = sendd2.c_str();
              appendFile(SD_MMC, cfnme, csendd2);
              Serial.println("here");


              String buffer = "Test Date : " + jsonBuffer["d"].as<String>() + "\n"; // Allocate a fixed-size buffer for the date string
              const char* data = buffer.c_str();
              appendFile(SD_MMC, cfnme, data);

              buffer = "Test Name : " + jsonBuffer["n"].as<String>() + "\n"; // Allocate a fixed-size buffer for the date string
              data = buffer.c_str();
              appendFile(SD_MMC, cfnme, data);

              buffer = "Test Place : " + jsonBuffer["p"].as<String>() + "\n"; // Allocate a fixed-size buffer for the date string
              data = buffer.c_str();
              appendFile(SD_MMC, cfnme, data);

              buffer = "Operator Name : " + jsonBuffer["on"].as<String>() + "\n"; // Allocate a fixed-size buffer for the date string
              data = buffer.c_str();
              appendFile(SD_MMC, cfnme, data);

              buffer = "Operator Contact No : " + jsonBuffer["ocn"].as<String>() + "\n"; // Allocate a fixed-size buffer for the date string
              data = buffer.c_str();
              appendFile(SD_MMC, cfnme, data);

              buffer = "Operator email ID : " + jsonBuffer["oe"].as<String>() + "\n"; // Allocate a fixed-size buffer for the date string
              data = buffer.c_str();
              appendFile(SD_MMC, cfnme, data);

              buffer = "Test Start KM : " + jsonBuffer["skm"].as<String>() + "\n"; // Allocate a fixed-size buffer for the date string
              data = buffer.c_str();
              appendFile(SD_MMC, cfnme, data);

              buffer = "Test End KM : " + jsonBuffer["ekm"].as<String>() + "\n"; // Allocate a fixed-size buffer for the date string
              data = buffer.c_str();
              appendFile(SD_MMC, cfnme, data);

              //        buffer = " \n"; // Allocate a fixed-size buffer for the date string
              //        data = buffer.c_str();
              //        appendFile(SD_MMC, cfnme, data);

              //        String sendd = jsonBuffer["header"].as<String>();
              //            String sendd = "Date,Time,Temperature1,Temperature2,Latitude,Longitude,Altitude,Speed,Distance,ADC1,ADC2,Pulse1,Pulse2\n";
              //          senddh = senddh + "\n";
              const char* csendd = senddh.c_str();
              //            const char* csendd = sendd.c_str();
              appendFile(SD_MMC, cfnme, csendd);


            }
            else {
              String buffer = "Test End KM : " + jsonBuffer["ekm"].as<String>() + "\n"; // Allocate a fixed-size buffer for the date string
              const char* data = buffer.c_str();
              const char* cfnme = fnme.c_str();
              appendFile(SD_MMC, cfnme, data);
            }
          }
        }
      }
    }
  }
  // Serial Mode Handling
  while (shift_to_serial_mode) {
    //    HANDLE_SERIAL();
    server.handleClient();
  }
}

// Interrupt Handlers
void IRAM_ATTR handleInterrupt1() {
  pulseCount1++;
}

void IRAM_ATTR handleInterrupt2() {
  pulseCount2++;
}

void IRAM_ATTR handleInterrupt3() {
  pulseCount3++;
}

void IRAM_ATTR handleInterrupt4() {
  pulseCount4++;
}

// ESP-NOW Data Handler
void onDataReceived(const uint8_t *mac, const uint8_t *data, int len) {
  if (len == sizeof(DataPacket)) {
    DataPacket *receivedData = (DataPacket *)data;
    rdataa[8] = receivedData->temperature1;
    rdataa[9] = receivedData->temperature2;
    rdataa[10] = receivedData->temperature3;
    rdataa[11] = receivedData->temperature4;
    rdataa[12] = receivedData->temperature5;
    rdataa[13] = receivedData->temperature6;
    rdataa[14] = receivedData->temperature7;
    rdataa[15] = receivedData->temperature8;
    rdataa[20] = receivedData->lati;
    rdataa[21] = receivedData->longi;
    rdataa[22] = receivedData->alti;
    rdataa[23] = receivedData->spped;
    rdataa[24] = receivedData->dist;

    // Capture incoming Date and Time
    rec_year = receivedData->year;
    rec_month = receivedData->month;
    rec_day = receivedData->day;
    rec_hour = receivedData->hour;
    rec_min = receivedData->minute;
    rec_sec = receivedData->second;

  } else {
    Serial.println("Received data size mismatch");
  }
}

// File Operations
void readFile(fs::FS & fs, const char *path) {
  File file = fs.open(path);
  if (file) {
    while (file.available()) {
      Serial.write(file.read());
    }
    file.close();
  } else {
    Serial.println("Failed to open file for reading");
  }
}

void writeFile(fs::FS & fs, const char *path, const char *message) {
  File file = fs.open(path, FILE_WRITE);
  if (file) {
    if (file.print(message)) {
      //      Serial.println("File written");
    } else {
      Serial.println("Write failed");
    }
    file.close();
  } else {
    Serial.println("Failed to open file for writing");
  }
}

void appendFile(fs::FS & fs, const char *path, const char *message) {
  File file = fs.open(path, FILE_APPEND);
  if (file) {
    if (file.print(message)) {
      //      Serial.println("Message appended");
    } else {
      Serial.println("Append failed");
    }
    file.close();
  } else {
    Serial.println("Failed to open file for appending");
  }
}


/********* FUNCTIONS  **********/
//Initial page of the server web, list directory and give you the chance of deleting and uploading
void SD_dir()
{
  if (SD_present)
  {
    //Action acording to post, dowload or delete, by MC 2022
    if (server.args() > 0 ) //Arguments were received, ignored if there are not arguments
    {
      Serial.println(server.arg(0));

      String Order = server.arg(0);
      Serial.println(Order);

      if (Order.indexOf("download_") >= 0)
      {
        Order.remove(0, 9);
        SD_file_download(Order);
        Serial.println(Order);
      }

      if ((server.arg(0)).indexOf("delete_") >= 0)
      {
        Order.remove(0, 7);
        SD_file_delete(Order);
        Serial.println(Order);
      }
    }

    File root = SD_MMC.open("/");
    if (root) {
      root.rewindDirectory();
      SendHTML_Header();
      webpage += F("<table align='center'>");
      webpage += F("<tr><th>Name/Type</th><th style='width:20%'>Type File/Dir</th><th>File Size</th></tr>");
      printDirectory("/", 0);
      webpage += F("</table>");
      SendHTML_Content();
      root.close();
    }
    else
    {
      SendHTML_Header();
      webpage += F("<h3>No Files Found</h3>");
    }
//    append_page_footer();
    SendHTML_Content();
    SendHTML_Stop();   //Stop is needed because no content length was sent
  } else ReportSDNotPresent();
}

//Upload a file to the SD
void File_Upload()
{
  append_page_header();
  webpage += F("<h3>Select File to Upload</h3>");
  webpage += F("<FORM action='/fupload' method='post' enctype='multipart/form-data'>");
  webpage += F("<input class='buttons' style='width:25%' type='file' name='fupload' id = 'fupload' value=''>");
  webpage += F("<button class='buttons' style='width:10%' type='submit'>Upload File</button><br><br>");
  webpage += F("<a href='/'>[Back]</a><br><br>");
//  append_page_footer();
  server.send(200, "text/html", webpage);
}

void serialon()
{
  shift_to_serial_mode = true;
  server.send(200, "text/html", "ok");
}

void serialoff()
{
  shift_to_serial_mode = false;
  server.send(200, "text/html", "ok");
}
//Prints the directory, it is called in void SD_dir()
void printDirectory(const char * dirname, uint8_t levels)
{

  File root = SD_MMC.open(dirname);

  if (!root) {
    return;
  }
  if (!root.isDirectory()) {
    return;
  }
  File file = root.openNextFile();

  int i = 0;
  while (file) {
    if (webpage.length() > 1000) {
      SendHTML_Content();
    }
    if (file.isDirectory()) {
      webpage += "<tr><td>" + String(file.isDirectory() ? "Dir" : "File") + "</td><td>" + String(file.name()) + "</td><td></td></tr>";
      printDirectory(file.name(), levels - 1);
    }
    else
    {
      webpage += "<tr><td>" + String(file.name()) + "</td>";
      webpage += "<td>" + String(file.isDirectory() ? "Dir" : "File") + "</td>";
      int bytes = file.size();
      int tm_byte = bytes/30000;
      if (tm_byte < 0){
       tm_byte = 1; 
      }
      int loadingTime = 9 + tm_byte;
      String fsize = "";
      if (bytes < 1024)                    fsize = String(bytes) + " B";
      else if (bytes < (1024 * 1024))        fsize = String(bytes / 1024.0, 3) + " KB";
      else if (bytes < (1024 * 1024 * 1024)) fsize = String(bytes / 1024.0 / 1024.0, 3) + " MB";
      else                                  fsize = String(bytes / 1024.0 / 1024.0 / 1024.0, 3) + " GB";
      webpage += "<td>" + fsize + "</td>";
      webpage += "<td>";
      webpage += F("<FORM action='/' method='post'>");
      webpage += F("<button type='submit' id='download_btn' onclick='showLoading(");
      webpage += String(loadingTime)+ F(")' name='download'");
      webpage += F("' value='"); webpage += "download_" + String(file.name()); webpage += F("'>Download</button>");
      webpage += "</td>";
      webpage += "<td>";
      webpage += F("<FORM action='/' method='post'>");
      webpage += F("<button type='submit' name='delete'");
      webpage += F("' value='"); webpage += "delete_" + String(file.name()); webpage += F("'>Delete</button>");
      webpage += "</td>";
      webpage += "</tr>";

    }
    file = root.openNextFile();
    i++;
  }
  file.close();


}

//Download a file from the SD, it is called in void SD_dir()
void SD_file_download(String filename)
{
  if (SD_present)
  {
    File download = SD_MMC.open("/" + filename);
    if (download)
    {
      server.sendHeader("Content-Type", "text/text");
      server.sendHeader("Content-Disposition", "attachment; filename=" + filename);
      server.sendHeader("Connection", "close");
      server.streamFile(download, "application/octet-stream");
      download.close();
    } else ReportFileNotPresent("download");
  } else ReportSDNotPresent();
}

//Handles the file upload a file to the SD
File UploadFile;
//Upload a new file to the Filing system
void handleFileUpload()
{
  HTTPUpload& uploadfile = server.upload(); //See https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer/srcv
  //For further information on 'status' structure, there are other reasons such as a failed transfer that could be used
  if (uploadfile.status == UPLOAD_FILE_START)
  {
    String filename = uploadfile.filename;
    if (!filename.startsWith("/")) filename = "/" + filename;
    Serial.print("Upload File Name: "); Serial.println(filename);
    SD_MMC.remove(filename);                         //Remove a previous version, otherwise data is appended the file again
    UploadFile = SD_MMC.open(filename, FILE_WRITE);  //Open the file for writing in SD (create it, if doesn't exist)
    filename = String();
  }
  else if (uploadfile.status == UPLOAD_FILE_WRITE)
  {
    if (UploadFile) UploadFile.write(uploadfile.buf, uploadfile.currentSize); // Write the received bytes to the file
  }
  else if (uploadfile.status == UPLOAD_FILE_END)
  {
    if (UploadFile)         //If the file was successfully created
    {
      UploadFile.close();   //Close the file again
      Serial.print("Upload Size: "); Serial.println(uploadfile.totalSize);
      webpage = "";
      append_page_header();
      webpage += F("<h3>File was successfully uploaded</h3>");
      webpage += F("<h2>Uploaded File Name: "); webpage += uploadfile.filename + "</h2>";
      webpage += F("<h2>File Size: "); webpage += file_size(uploadfile.totalSize) + "</h2><br><br>";
      webpage += F("<a href='/'>[Back]</a><br><br>");
//      append_page_footer();
      server.send(200, "text/html", webpage);
    }
    else
    {
      ReportCouldNotCreateFile("upload");
    }
  }
}

//Delete a file from the SD, it is called in void SD_dir()
void SD_file_delete(String filename)
{
  if (SD_present) {
    SendHTML_Header();
    File dataFile = SD_MMC.open("/" + filename, FILE_READ); //Now read data from SD Card
    if (dataFile)
    {
      if (SD_MMC.remove("/" + filename)) {
        Serial.println(F("File deleted successfully"));
        webpage += "<h3>File '" + filename + "' has been erased</h3>";
        webpage += F("<a href='/'>[Back]</a><br><br>");
      }
      else
      {
        webpage += F("<h3>File was not deleted - error</h3>");
        webpage += F("<a href='/'>[Back]</a><br><br>");
      }
    } else ReportFileNotPresent("delete");
//    append_page_footer();
    SendHTML_Content();
    SendHTML_Stop();
  } else ReportSDNotPresent();
}

//SendHTML_Header
void SendHTML_Header()
{
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", ""); //Empty content inhibits Content-length header so we have to close the socket ourselves.
  append_page_header();
  server.sendContent(webpage);
  webpage = "";
}

//SendHTML_Content
void SendHTML_Content()
{
  server.sendContent(webpage);
  webpage = "";
}

//SendHTML_Stop
void SendHTML_Stop()
{
  server.sendContent("");
  server.client().stop(); //Stop is needed because no content length was sent
}

//ReportSDNotPresent
void ReportSDNotPresent()
{
  SendHTML_Header();
  webpage += F("<h3>No SD Card present</h3>");
  webpage += F("<a href='/'>[Back]</a><br><br>");
//  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}

//ReportFileNotPresent
void ReportFileNotPresent(String target)
{
  SendHTML_Header();
  webpage += F("<h3>File does not exist</h3>");
  webpage += F("<a href='/"); webpage += target + "'>[Back]</a><br><br>";
//  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}

//ReportCouldNotCreateFile
void ReportCouldNotCreateFile(String target)
{
  SendHTML_Header();
  webpage += F("<h3>Could Not Create Uploaded File (write-protected?)</h3>");
  webpage += F("<a href='/"); webpage += target + "'>[Back]</a><br><br>";
//  append_page_footer();
  SendHTML_Content();
}

//File size conversion
String file_size(int bytes)
{
  String fsize = "";
  if (bytes < 1024)                 fsize = String(bytes) + " B";
  else if (bytes < (1024 * 1024))      fsize = String(bytes / 1024.0, 3) + " KB";
  else if (bytes < (1024 * 1024 * 1024)) fsize = String(bytes / 1024.0 / 1024.0, 3) + " MB";
  else                              fsize = String(bytes / 1024.0 / 1024.0 / 1024.0, 3) + " GB";
  return fsize;
}