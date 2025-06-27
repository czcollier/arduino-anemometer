#include <RTC.h>
#include <time.h>
#include <string.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#include "wifi.h"
#include "ntp.h"
#include "arduino_secrets.h"

int status = WL_IDLE_STATUS;     // the WiFi radio's status

void printMacAddress(byte mac[]) {
  for (int i = 0; i < 6; i++) {
    if (i > 0) {
      Serial.print(":");
    }
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
  }
  Serial.println();
}


void printWifiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void printWifiInfo() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void setupWifi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    return;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
  
  Serial.print("Attempting to connect to wifi network. SSID: ");
  Serial.println(SECRET_WIFI_SSID);
  
  // Connect to WPA/WPA2 network:
  if (strcmp(SECRET_WIFI_PASSWORD, "") == 0) {
    status = WiFi.begin(SECRET_WIFI_SSID);
  } else {
    status = WiFi.begin(SECRET_WIFI_SSID, SECRET_WIFI_PASSWORD);
  }

  while (status != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }

  // you're connected now, so print out the data:
  Serial.println("we are connected to the network");
  printWifiInfo();
  printWifiData();

}

