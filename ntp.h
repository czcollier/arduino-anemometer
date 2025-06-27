#ifndef NTP_H
#define NTP_H

#define TIMEZONE_OFFSET_HOURS 0
#define NTP_SERVER "pool.ntp.org"
#define NTP_TIMEZONE "CET-1CEST,M3.5.0/02,M10.5.0/03"

void initializeNtp();
void initializeRtc();
unsigned long getNtpTime();

#endif //NTP_H