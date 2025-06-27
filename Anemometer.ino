#include "arduino_secrets.h"
#include <FirebaseClient.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <NTPClient.h>
#include <Arduino.h>

#include "wifi.h"
#include "firebase.h"
#include "ntp.h"
#include "frequencyCounter.h"
#include "slidingWindowAverage.h"

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

bool isArmed = false;
float windSpeed = 0.0;

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

void setup() {
  Serial.begin(9600);
  analogReadResolution(14);
  pinMode(ANEMOMETER_SENSOR_PIN, INPUT);
  setupWifi();
  initializeNtp();
  delay(4000);
  initializeRtc();
  setupFirebase();
}

float prevFreq = 0.0;

void loop() {
  unsigned long currTime = millis();
  firebaseLoopStart();


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
      pushRealtime(currFreq);
    }
    
    prevFreq = currFreq; 
  }

  firebaseLoopEnd();
}
