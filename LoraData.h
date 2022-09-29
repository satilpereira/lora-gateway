#include <Arduino.h>
#include "heltec.h"
#include <WiFi.h>
#include <HTTPClient.h>
#define QTD_SENSORS 8


const String jsonKeys[] = {
  "{\"data_1\":\"",
  "\",\"data_2\":\"",
  "\",\"data_3\":\"",
  "\",\"data_4\":\"",
  "\",\"data_5\":\"",
  "\",\"data_6\":\"",
  "\",\"data_7\":\"",
  "\",\"data_8\":\"",
};
class LoraData {
private:
  String loraPacket;

public:
  void send_data();
  void set_loraPacket(String);
  String jsonParse(String);
};

void LoraData::set_loraPacket(String httpRequestData) {
  this->loraPacket = httpRequestData;
}

void LoraData::send_data() {
  Serial.print("Sending packet: ");
  Serial.println(this->loraPacket);

  // Send packet
  LoRa.beginPacket();

  /*
   * LoRa.setTxPower(txPower,RFOUT_pin);
   * txPower -- 0 ~ 20
   * RFOUT_pin could be RF_PACONFIG_PASELECT_PABOOST or RF_PACONFIG_PASELECT_RFO
   *   - RF_PACONFIG_PASELECT_PABOOST -- LoRa single output via PABOOST, maximum output 20dBm
   *   - RF_PACONFIG_PASELECT_RFO     -- LoRa single output via RFO_HF / RFO_LF, maximum output 14dBm
   */
  LoRa.setTxPower(20, RF_PACONFIG_PASELECT_PABOOST);
  LoRa.print(this->loraPacket);
  LoRa.endPacket();
}

String LoraData::jsonParse(String str) {
  String jsonData;
  StaticJsonDocument<0> filter;
  filter.set(true);

  StaticJsonDocument<1000> doc;

  DeserializationError error = deserializeJson(doc, str, DeserializationOption::Filter(filter));

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return "-1";
  }

  for (JsonObject item : doc.as<JsonArray>()) {

    const char* device_id = item["device_id"];          // "1", "2"
    const char* device_status = item["device_status"];  // "11", "20"

    jsonData += device_id;
    jsonData += ":";
    jsonData += device_status;
    jsonData += ",";
  }
  Serial.println(jsonData);
  return jsonData;
}

String LoRajsonEncode() {
  String received = "";
  uint32_t packetSize = LoRa.parsePacket();

  if (!packetSize) {
    return "";
  }

  Serial.print("Received packet '");
  // read packet
  while (LoRa.available()) {
    received += (char)LoRa.read();
  }
  Serial.println(received);

  uint8_t count = 0;
  String arr[QTD_SENSORS];

  for (uint32_t i = 0; i < packetSize; i++) {
    if ((char)received[i] == '/') {

      count++;
      continue;
    }
    arr[count] += received[i];
  }

  if (arr == NULL) {
    Serial.println("Lora data is null");
    return "";
  }
  Serial.println("Lora data not null");

  String httpRequestData = "";

  for (int i = 0; i < QTD_SENSORS; i++) {
    httpRequestData += jsonKeys[i];
    httpRequestData += arr[i];
  }

  httpRequestData += "}";

  Serial.print("JSON format: ");
  Serial.println(httpRequestData);

  return httpRequestData;
}