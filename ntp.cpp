#include <NTPClient.h>
#include <WiFiUdp.h>
#include <RTC.h>

#include "ntp.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_SERVER, 0, 60000);

void initializeNtp() {
  Serial.println("starting ntp...");
  timeClient.begin();
}

void initializeRtc() {
  Serial.println("starting rtc...");
  RTC.begin();

  Serial.println("getting time from ntp...");
  unsigned long unixTime = getNtpTime();
  Serial.print("Unix time = ");
  Serial.println(unixTime);

  RTCTime timeToSet = RTCTime(static_cast<time_t>(unixTime));
  RTC.setTime(timeToSet);

  RTCTime currentTime;
  RTC.getTime(currentTime); 
  Serial.println("The RTC was just set to: " + String(currentTime));
}

unsigned long getNtpTime() {
  timeClient.update();
  return timeClient.getEpochTime() + (TIMEZONE_OFFSET_HOURS * 3600);
}