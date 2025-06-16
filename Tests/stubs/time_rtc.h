#ifndef TIME_RTC_H
#define TIME_RTC_H
#include <stddef.h>
#include <stdbool.h>
typedef struct {unsigned char hour; unsigned char min; unsigned char sec; unsigned char day; unsigned char month; unsigned short year;} ds3231_time_t;
void time_rtc_GetFormattedDateTime(char *buffer, size_t len);
bool ds3231_get_datetime(ds3231_time_t* dt);
#endif
