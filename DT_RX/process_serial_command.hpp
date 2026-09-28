#ifndef PROCESS_SERIAL_COMMAND_HPP
#define PROCESS_SERIAL_COMMAND_HPP

#include <ArduinoJson.h>
#include "FS.h"
#include "SD_MMC.h"
#include "SPI.h"
#include "SD_MMC.h"
#include <SPI.h>

const int pageSize = 10;    // Number of files per page
int currentPage = 1;       // Current page

bool useSDCard = true; // Flag to determine which card to use
bool shift_to_serial_mode = false, shift_to_rec = false, log_header = true;
void sendFileList(int page) {
  DynamicJsonDocument jsonBuffer(1024);
  jsonBuffer["s"] = "1";

  JsonArray dataArray = jsonBuffer.createNestedArray("d");

  if (!useSDCard) {
    File root = SD_MMC.open("/");
    File file = root.openNextFile();
    Serial.println(file);
    int fileCount = 0;
    int fileSkipped = (page - 1) * pageSize;
    while (file) {
      if (!file.isDirectory()) {
        if (fileCount >= fileSkipped && fileCount < fileSkipped + pageSize) {
          JsonObject fileObj = dataArray.createNestedObject();
          fileObj["file_name"] = String(file.name()).substring(1); // Removing the leading '/'
          fileObj["size"] = String(file.size() / 1024.0, 2) + " KB";
        }
        fileCount++;
      }
      file = root.openNextFile();
      Serial.println(file);
    }

    root.close();
  } else {
    File root = SD_MMC.open("/");
    File file = root.openNextFile();

    int fileCount = 0;
    int fileSkipped = (page - 1) * pageSize;
    while (file) {
      if (!file.isDirectory()) {
        if (fileCount >= fileSkipped && fileCount < fileSkipped + pageSize) {
          JsonObject fileObj = dataArray.createNestedObject();
          fileObj["file_name"] = String(file.name()).substring(1); // Removing the leading '/'
          fileObj["size"] = String(file.size() / 1024.0, 2) + " KB";
        }
        fileCount++;
      }
      file = root.openNextFile();
    }

    root.close();
  }

  String output;
  serializeJson(jsonBuffer, output);
  Serial.println(output);
}

void sendFileHeading(const char* fileName) {
  DynamicJsonDocument jsonBuffer(1024);
  jsonBuffer["s"] = "1";
  jsonBuffer["last"] = "1";
  jsonBuffer["title"] = "1";

  JsonArray dataArray = jsonBuffer.createNestedArray("d");
  JsonArray parametersArray = jsonBuffer.createNestedArray("parameters");

  String datafile = String("/") + fileName;

  if (!useSDCard) {
    File file = SD_MMC.open(datafile);
    if (!file) {
      Serial.println("File open failed");
      return;
    }

    String line;
    for (int i = 0; i < 10; i++) {
      if (file.available()) {
        line = file.readStringUntil('\n');
        line.trim();

        String jsonLine = line;

        // Splitting the line and adding to respective arrays
        String delimiter = ",";
        int delimiterIndex = line.indexOf(delimiter);
        while (delimiterIndex != -1) {
          String element = line.substring(0, delimiterIndex);
          element.trim();

          if (!element.isEmpty()) {
            if (i == 9) {
              dataArray.add(element);
            } else {
              parametersArray.add(element);
            }
          }

          line = line.substring(delimiterIndex + 1);
          delimiterIndex = line.indexOf(delimiter);
        }

        // Adding the last element after the last delimiter
        line.trim();
        if (!line.isEmpty()) {
          if (i == 9) {
            dataArray.add(line);
          } else {
            parametersArray.add(line);
          }
        }
      } else {
        break;
      }
    }

    file.close();
  } else {
    File file = SD_MMC.open(datafile);
    if (!file) {
      Serial.println("File open failed");
      return;
    }

    String line;
    for (int i = 0; i < 10; i++) {
      if (file.available()) {
        line = file.readStringUntil('\n');
        line.trim();

        String jsonLine = line;

        // Splitting the line and adding to respective arrays
        String delimiter = ",";
        int delimiterIndex = line.indexOf(delimiter);
        while (delimiterIndex != -1) {
          String element = line.substring(0, delimiterIndex);
          element.trim();

          if (!element.isEmpty()) {
            if (i == 9) {
              dataArray.add(element);
            } else {
              parametersArray.add(element);
            }
          }

          line = line.substring(delimiterIndex + 1);
          delimiterIndex = line.indexOf(delimiter);
        }

        // Adding the last element after the last delimiter
        line.trim();
        if (!line.isEmpty()) {
          if (i == 9) {
            dataArray.add(line);
          } else {
            parametersArray.add(line);
          }
        }
      } else {
        break;
      }
    }

    file.close();
  }

  jsonBuffer["fn"] = fileName;
  String output;
  serializeJson(jsonBuffer, output);
  Serial.println(output);
}

void sendFileData(const char* fileName) {
  String datafile = String("/") + fileName;

  if (!useSDCard) {
    File file = SD_MMC.open(datafile);
    if (!file) {
      Serial.println("File open failed");
      return;
    }

    // Skip first 10 lines
    for (int i = 0; i < 10; i++) {
      if (file.available()) {
        file.readStringUntil('\n').trim();
      } else {
        break;
      }
    }

    while (file.available()) {
      DynamicJsonDocument jsonBuffer(1024);
      jsonBuffer["s"] = "1";
      jsonBuffer["last"] = "0";  // Set to 0 during data transfer
      jsonBuffer["title"] = "1";

      JsonArray dataArray = jsonBuffer.createNestedArray("d");  // Moved dataArray declaration here

      String line = file.readStringUntil('\n');
      line.trim();

      // Check for stop command
      if (Serial.available()) {
        String stopCmd = Serial.readStringUntil('\n');
        DynamicJsonDocument stopJson(1024);
        DeserializationError error = deserializeJson(stopJson, stopCmd);
        if (!error && stopJson["ct"] == "file_data_stop" && stopJson["fn"] == fileName) {
          file.close();
          if (!useSDCard) {
            if (SD_MMC.remove(datafile.c_str())) {
              DynamicJsonDocument jsonBuffer(1024);
              jsonBuffer["s"] = "1";
              jsonBuffer["ct"] = "file_data_stop";  // Set to 0 during data transfer
              String output;
              serializeJson(jsonBuffer, output);
              Serial.println(output);
              return;
            } else {
              DynamicJsonDocument jsonBuffer(1024);
              jsonBuffer["s"] = "0";
              jsonBuffer["ct"] = "file_data_stop";  // Set to 0 during data transfer
              String output;
              serializeJson(jsonBuffer, output);
              Serial.println(output);
            }
          } else {
            if (SD_MMC.remove(datafile.c_str())) {
              DynamicJsonDocument jsonBuffer(1024);
              jsonBuffer["s"] = "1";
              jsonBuffer["ct"] = "file_data_stop";  // Set to 0 during data transfer
              String output;
              serializeJson(jsonBuffer, output);
              Serial.println(output);
              return;
            } else {
              DynamicJsonDocument jsonBuffer(1024);
              jsonBuffer["s"] = "0";
              jsonBuffer["ct"] = "file_data_stop";  // Set to 0 during data transfer
              String output;
              serializeJson(jsonBuffer, output);
              Serial.println(output);
            }
          }
        }   
      }  

      // Splitting the line and adding to respective arrays
      String delimiter = ",";
      int delimiterIndex = line.indexOf(delimiter);
      while (delimiterIndex != -1) {
        String element = line.substring(0, delimiterIndex);
        element.trim();

        if (!element.isEmpty()) {
          dataArray.add(element);
        }

        line = line.substring(delimiterIndex + 1);
        delimiterIndex = line.indexOf(delimiter);
      }

      // Adding the last element after the last delimiter
      line.trim();
      if (!line.isEmpty()) {
        dataArray.add(line);
      }

      // Check if it is the last line of the file
      if (!file.available()) {
        jsonBuffer["last"] = "1";  // Set to 1 for the last line
      }

      jsonBuffer["fn"] = fileName;
      String output;
      serializeJson(jsonBuffer, output);
      Serial.println(output);

      delay(100); // Delay 100ms
    }

    file.close();
  } else {
    File file = SD_MMC.open(datafile);
    if (!file) {
      Serial.println("File open failed");
      return;
    }

    // Skip first 10 lines
    for (int i = 0; i < 10; i++) {
      if (file.available()) {
        file.readStringUntil('\n').trim();
      } else {
        break;
      }
    }

    while (file.available()) {
      DynamicJsonDocument jsonBuffer(1024);
      jsonBuffer["s"] = "1";
      jsonBuffer["last"] = "0";  // Set to 0 during data transfer
      jsonBuffer["title"] = "1";

      JsonArray dataArray = jsonBuffer.createNestedArray("d");  // Moved dataArray declaration here

      String line = file.readStringUntil('\n');
      line.trim();

      // Check for stop command
      if (Serial.available()) {
        String stopCmd = Serial.readStringUntil('\n');
        DynamicJsonDocument stopJson(1024);
        DeserializationError error = deserializeJson(stopJson, stopCmd);
        if (!error && stopJson["ct"] == "file_data_stop" && stopJson["fn"] == fileName) {
          file.close();
          if (!useSDCard) {
            if (SD_MMC.remove(datafile.c_str())) {
              DynamicJsonDocument jsonBuffer(1024);
              jsonBuffer["s"] = "1";
              jsonBuffer["ct"] = "file_data_stop";  // Set to 0 during data transfer
              String output;
              serializeJson(jsonBuffer, output);
              Serial.println(output);
              return;
            } else {
              DynamicJsonDocument jsonBuffer(1024);
              jsonBuffer["s"] = "0";
              jsonBuffer["ct"] = "file_data_stop";  // Set to 0 during data transfer
              String output;
              serializeJson(jsonBuffer, output);
              Serial.println(output);
            }
          } else {
            if (SD_MMC.remove(datafile.c_str())) {
              DynamicJsonDocument jsonBuffer(1024);
              jsonBuffer["s"] = "1";
              jsonBuffer["ct"] = "file_data_stop";  // Set to 0 during data transfer
              String output;
              serializeJson(jsonBuffer, output);
              Serial.println(output);
              return;
            } else {
              DynamicJsonDocument jsonBuffer(1024);
              jsonBuffer["s"] = "0";
              jsonBuffer["ct"] = "file_data_stop";  // Set to 0 during data transfer
              String output;
              serializeJson(jsonBuffer, output);
              Serial.println(output);
            }
          }
        }   
      }  

      // Splitting the line and adding to respective arrays
      String delimiter = ",";
      int delimiterIndex = line.indexOf(delimiter);
      while (delimiterIndex != -1) {
        String element = line.substring(0, delimiterIndex);
        element.trim();

        if (!element.isEmpty()) {
          dataArray.add(element);
        }

        line = line.substring(delimiterIndex + 1);
        delimiterIndex = line.indexOf(delimiter);
      }

      // Adding the last element after the last delimiter
      line.trim();
      if (!line.isEmpty()) {
        dataArray.add(line);
      }

      // Check if it is the last line of the file
      if (!file.available()) {
        jsonBuffer["last"] = "1";  // Set to 1 for the last line
      }

      jsonBuffer["fn"] = fileName;
      String output;
      serializeJson(jsonBuffer, output);
      Serial.println(output);

      delay(100); // Delay 100ms
    }

    file.close();
  }
}


void HANDLE_SERIAL() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');

    StaticJsonDocument<200> jsonBuffer;
    DeserializationError error = deserializeJson(jsonBuffer, input);

    if (error) {
      Serial.println("Error parsing JSON");
      return;
    }

    const char* ct = jsonBuffer["ct"];

    Serial.print("Received Command: ");
    Serial.println(ct);

    if (strcmp(ct, "files") == 0) {
      int page = jsonBuffer["page"];
      Serial.print("Page: ");
      Serial.println(page);
      sendFileList(page);
    } else if (strcmp(ct, "file_data") == 0) {
      const char* fileName = jsonBuffer["fn"];
      int title = jsonBuffer["title"];
      if (title == 1){
        sendFileHeading(fileName);
      } else {
        sendFileData(fileName);
      }
    }

    if (strcmp(ct, "stop") == 0) {
      shift_to_serial_mode = false;
    }

    if (strcmp(ct, "file_delete") == 0) {
      const char* fileName = jsonBuffer["fn"];
      String datafile = String("/") + String(fileName);
      if (!useSDCard) {
        if (SD_MMC.remove(datafile.c_str())) {
          Serial.println("File deleted successfully.");
          DynamicJsonDocument response(1024);
          response["s"] = "1";
          response["ct"] = "file_delete";
          String resp;
          serializeJson(response, resp);
          Serial.println(resp);
        } else {
          DynamicJsonDocument response(1024);
          response["s"] = "0";
          response["ct"] = "file_delete";
          String resp;
          serializeJson(response, resp);
          Serial.println(resp);
        }
      } else {
        if (SD_MMC.remove(datafile.c_str())) {
          Serial.println("File deleted successfully.");
          DynamicJsonDocument response(1024);
          response["s"] = "1";
          response["ct"] = "file_delete";
          String resp;
          serializeJson(response, resp);
          Serial.println(resp);
        } else {
          DynamicJsonDocument response(1024);
          response["s"] = "0";
          response["ct"] = "file_delete";
          String resp;
          serializeJson(response, resp);
          Serial.println(resp);
        }
      }
      return;
    }

    if (strcmp(ct, "clear_memory") == 0) {
      if (!useSDCard) {
        if (SD_MMC.begin()) {
          File root = SD_MMC.open("/");
          while (true) {
            File entry = root.openNextFile();
            if (!entry) {
              break;
            }
            SD_MMC.remove(entry.name());
            entry.close();
          }
          root.close();
          Serial.println("Memory cleared successfully.");
          DynamicJsonDocument response(1024);
          response["s"] = "1";
          response["ct"] = "clear_memory";
          String resp;
          serializeJson(response, resp);
          Serial.println(resp);
        } else {
          DynamicJsonDocument response(1024);
          response["s"] = "0";
          response["ct"] = "clear_memory";
          String resp;
          serializeJson(response, resp);
          Serial.println(resp);
        }
      } else {
        if (SD_MMC.begin()) {
          File root = SD_MMC.open("/");
          while (true) {
            File entry = root.openNextFile();
            if (!entry) {
              break;
            }
            SD_MMC.remove(entry.name());
            entry.close();
          }
          root.close();
          Serial.println("Memory cleared successfully.");
          DynamicJsonDocument response(1024);
          response["s"] = "1";
          response["ct"] = "clear_memory";
          String resp;
          serializeJson(response, resp);
          Serial.println(resp);
        } else {
          DynamicJsonDocument response(1024);
          response["s"] = "0";
          response["ct"] = "clear_memory";
          String resp;
          serializeJson(response, resp);
          Serial.println(resp);
        }
      }
      return;
    }
  }
}


#endif // DATE_CHECK_FUNC_HPP
