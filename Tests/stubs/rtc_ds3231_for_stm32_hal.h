#ifndef RTC_DS3231_FOR_STM32_HAL_H_
#define RTC_DS3231_FOR_STM32_HAL_H_
#include <stdint.h>
typedef struct {uint8_t seconds;uint8_t minutes;uint8_t hours;uint8_t day;uint8_t month;uint16_t year;} DS3231_DateTime;
#endif
