#include "heltec.h"
#include <ArduinoJson.h>
#include "LoraData.h"

#define BAND 915E6  // Band set to 915MHz

// SSID and password
const char* ssid = "SMCTI";
const char* password = "pato@6080";

String serverName = "http://server.com:81/api/..";

// Instantiate LoraData object
LoraData lora;

void setup() {
  Heltec.begin(false /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);

  // Start internet connnection
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Receive data via LoRa
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;

    http.begin(client, serverName);

    // Send HTTP GET request
    int httpResponseCode = http.GET();
    if (httpResponseCode != NULL) {
      if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String httpRequestData = http.getString();
      lora.set_loraPacket(lora.jsonParse(httpRequestData));
      lora.send_data();
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
        Serial.println();
      }
    }
    
    http.end();
}