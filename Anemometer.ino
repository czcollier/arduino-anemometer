#include "arduino_secrets.h"
#include <Arduino.h>
#include <time.h>

#include "ntp.h"
#include "frequencyCounter.h"
#include "slidingWindowAverage.h"
#include "thingProperties.h"
#include "Anemometer.h"

const int WINDOW_SIZE = AVG_WINDOW_LEN_MS / SAMPLING_INTERVAL_MS;

bool iotReady = false;

// maintain ms of last data push and sensor read
// for loop timers
unsigned long lastPush = 0;
unsigned long lastMonitor = 0;
unsigned long lastIotCheck = 0;

// maintain the previous reading so updates can be
// suppressed if the new reading is not significantly
// different from the last one.
float prevFreq = 0.0;

// long life object; memory released upon program exit
FrequencyCounter *anemometerFrequency = new FrequencyCounter(
  ANEMOMETER_HIGH_THRESHOLD,
  ANEMOMETER_LOW_THRESHOLD,
  ANEMOMETER_TIMEOUT_MS
);

// long life object; memory released upon program exit
SlidingWindowAverage *frequencyAverage = new SlidingWindowAverage(WINDOW_SIZE);

/*
* iot cloud connected callback. Set up ntp and clock
* because built-in iot seems to fail getting ntp time.
*/

float readSensor() {
  if (TEST_MODE) {
    return (float)random(800) / 100.0;
  }
  else {
    return analogRead(ANEMOMETER_SENSOR_PIN);
  }
}

void updateWindSpeed(float windSpeed) {
  wind_speed = windSpeed;
}

void onIotConnected() {
  //initializeNtp();
  //initializeRtc();
  Serial.println("~~~~~~~ IoT Connected ~~~~~~~");
  iotReady = true;
}

void setupSensor() {
  analogReadResolution(ANEMOMETER_ANALOG_READ_RESOLUTION);
  pinMode(ANEMOMETER_SENSOR_PIN, INPUT);
}

void setup() {
  Serial.begin(9600);
  delay(1500); 
  Serial.println("Anemometer Starting...");
  setupSensor();
  // arduino cloud setup
  Serial.print("using wifi ssid: ");
  Serial.println(SSID);
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  ArduinoCloud.addCallback(ArduinoIoTCloudEvent::CONNECT, onIotConnected);
  setDebugMessageLevel(4);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  unsigned long currTime = millis();

  if (currTime - lastIotCheck >= IOT_CHECK_INTERVAL_MS) {
    lastIotCheck = currTime;
    if (!iotReady) {
      Serial.println("IoT not ready. waiting...");
      return;
    }
  }
  
  if (currTime - lastMonitor >= SAMPLING_INTERVAL_MS) {
    lastMonitor = currTime;
    anemometerFrequency->update(millis(), readSensor());
    float instantaneousFreq = anemometerFrequency->getFrequency();
    frequencyAverage->addValue(instantaneousFreq); 
  } 

  if (currTime - lastPush >= REPORTING_INTERVAL_MS) {
    lastPush = currTime;
    
    float currFreq = frequencyAverage->getAverage();

    if (!approximatelyEqual(currFreq, prevFreq)) {
      Serial.print("sending wind speed value: ");
      Serial.println(currFreq);
      updateWindSpeed(currFreq);
    }
    
    prevFreq = currFreq; 
  }
}
