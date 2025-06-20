#ifndef PM25_AVG10_H
#define PM25_AVG10_H

#include <stdbool.h>
#include <stdint.h>
#include "rtc_ds3231_for_stm32_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

void pm25_avg10_add_sample(float pm25);
void pm25_avg10_process(void);

extern bool flag_promedio_10min;

#if defined(UNIT_TEST) || defined(UNIT_TESTING)
uint16_t pm25_avg10_get_count(void);
#endif

#ifdef __cplusplus
}
#endif

#endif // PM25_AVG10_H
