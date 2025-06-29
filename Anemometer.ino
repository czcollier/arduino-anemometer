#include "arduino_secrets.h"
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <Arduino.h>
#include <time.h>

#include "wifi.h"
#include "ntp.h"
#include "frequencyCounter.h"
#include "slidingWindowAverage.h"
#include "thingProperties.h"

const int ANEMOMETER_SENSOR_PIN = A0;
const int ZERO_TOLERANCE = 0.05;

const int ANEMOMETER_HIGH_THRESHOLD = 200;
const int ANEMOMETER_LOW_THRESHOLD = 1; 

const int ANEMOMETER_TIMEOUT_MS = 1000;
const int REPORTING_INTERVAL_MS = 2000;
const int SAMPLING_INTERVAL_MS = 100;
const int AVG_WINDOW_LEN_MS = 2000;

const int WINDOW_SIZE = AVG_WINDOW_LEN_MS / SAMPLING_INTERVAL_MS;

unsigned long lastAnemoCheck = 0;
unsigned long lastAnemoCross = 0;
unsigned long lastPush = 0;
unsigned long lastMonitor = 0;

FrequencyCounter anemometer(
  ANEMOMETER_HIGH_THRESHOLD,
  ANEMOMETER_LOW_THRESHOLD,
  ANEMOMETER_TIMEOUT_MS
);

SlidingWindowAverage frequencyAverage(WINDOW_SIZE);

void setup();
void loop();

bool approximatelyEqual(float value, float reference) {
  return (value <= reference + ZERO_TOLERANCE
    && value >= reference - ZERO_TOLERANCE);
}

void updateWindSpeed(float windSpeed) {
  wind_speed = windSpeed;
}

void onIotConnected() {
  ArduinoCloud.printDebugInfo();
  initializeNtp();
  delay(1500);
  initializeRtc();

  Serial.println("end of setup");
}

void setup() {
  Serial.println("Anemometer Starting...");
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 
  analogReadResolution(14);
  pinMode(ANEMOMETER_SENSOR_PIN, INPUT);
  // get debug info for network and IoT Cloud connection
  // and errors - higher numbers more granular
  // The default is 0 (only errors), maximum is 4
  //setDebugMessageLevel(4);
 // Defined in thingProperties.h
  initProperties();
  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  ArduinoCloud.addCallback(ArduinoIoTCloudEvent::CONNECT, onIotConnected);
}

float prevFreq = 0.0;

void loop() {
  unsigned long currTime = millis();
  
  if (currTime - lastMonitor >= SAMPLING_INTERVAL_MS) {
    lastMonitor = currTime;
    anemometer.update(millis(), analogRead(ANEMOMETER_SENSOR_PIN));
    float instantaneousFreq = anemometer.getFrequency();
    frequencyAverage.addValue(instantaneousFreq); 
  } 

  if (currTime - lastPush >= REPORTING_INTERVAL_MS) {
    lastPush = currTime;
    
    float currFreq = frequencyAverage.getAverage();

    if (!approximatelyEqual(currFreq, prevFreq)) {
      updateWindSpeed(currFreq);
      Serial.print("sending wind speed value: ");
      Serial.println(currFreq);
    }
    
    prevFreq = currFreq; 
  }
}
