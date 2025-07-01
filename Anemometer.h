#ifndef ANEMOMETER_H
#define ANEMOMETER_H

#include <Arduino.h>

// if true, skips using the actual sensor.
// a random value will be generated
// instead of reading the sensor output.
#define TEST_MODE true

// tolerance for approximate float comparisons
#define ZERO_TOLERANCE 0.05

// anemometer parameters
#define ANEMOMETER_SENSOR_PIN A0
#define ANEMOMETER_ANALOG_READ_RESOLUTION 14
#define ANEMOMETER_HIGH_THRESHOLD 200
#define ANEMOMETER_LOW_THRESHOLD 1
#define ANEMOMETER_TIMEOUT_MS 1000
#define REPORTING_INTERVAL_MS 2000
#define SAMPLING_INTERVAL_MS 100
#define IOT_CHECK_INTERVAL_MS 1000
#define AVG_WINDOW_LEN_MS 2000


bool approximatelyEqual(float value, float reference) {
  return (value <= reference + ZERO_TOLERANCE
    && value >= reference - ZERO_TOLERANCE);
}

#endif //ANEMOMETER_H