#include "frequencyCounter.h"

FrequencyCounter::FrequencyCounter(int high, int low, unsigned long timeoutMs) {
  this->highThreshold = high;
  this->lowThreshold = low;
  this->timeoutMs = timeoutMs;

  // Initialize all state variables
  this->isArmed = false;
  this->hasStarted = false;
  this->lastEventTime = 0;
  this->currentFrequency = 0.0;
}

void FrequencyCounter::update(unsigned long currentMillis, int sensorValue) {
  // 1. Arming condition
  if (sensorValue < this->lowThreshold) {
    this->isArmed = true;
  }

  // 2. Trigger condition
  if (this->isArmed && sensorValue > this->highThreshold) {
    if (this->hasStarted) {
      unsigned long period = currentMillis - this->lastEventTime;
      if (period == 0.0) {
        this->currentFrequency = 0.0;
      }
      else {
        this->currentFrequency = 1000.0 / period;
      }
    }
    this->lastEventTime = currentMillis;
    this->isArmed = false;
    this->hasStarted = true;
  }
  
  // 3. Timeout condition
  if (this->hasStarted && (currentMillis - this->lastEventTime > this->timeoutMs)) {
    this->currentFrequency = 0.0;
    this->hasStarted = false; // Reset to require a new "first" event
  }
}

float FrequencyCounter::getFrequency() {
  return this->currentFrequency;
}