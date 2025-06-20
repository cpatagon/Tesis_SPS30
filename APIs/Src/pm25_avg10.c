#include "pm25_avg10.h"
#include "ParticulateDataAnalyzer.h"
#include "microSD_utils.h"
#include "uart.h"
#ifdef UNIT_TESTING
#undef UNIT_TESTING
#include "ff_stub.h"
#define UNIT_TESTING
#include <stddef.h>
#include <stdbool.h>
float calculateAverage(float data[], int n){return 0.0f;}
float findMaxValue(float data[], int n){return 0.0f;}
float findMinValue(float data[], int n){return 0.0f;}
float calculateStandardDeviation(float data[], int n){return 0.0f;}
#else
#include "ff.h"
#endif
#include <stdio.h>
#include <string.h>

#define AVG10_BUFFER_SIZE 60

static float pm25_buffer[AVG10_BUFFER_SIZE];
static uint16_t pm25_count = 0;
bool flag_promedio_10min = false;
static int last_boundary_minute = -1;

void pm25_avg10_add_sample(float pm25){
    pm25_buffer[pm25_count % AVG10_BUFFER_SIZE] = pm25;
    if(pm25_count < AVG10_BUFFER_SIZE) pm25_count++;
}

static void clear_buffer(void){
    pm25_count = 0;
}

static void save_avg_csv(const ds3231_time_t *dt, float mean, uint16_t count,
                         float min, float max, float std){
    char dir[32];
    snprintf(dir, sizeof(dir), "/%04d/%02d/%02d", dt->year, dt->month, dt->day);
    f_mkdir(dir);

    char path[64];
    snprintf(path, sizeof(path), "%s/AVG10.csv", dir);

    char ts[32];
    snprintf(ts, sizeof(ts), "%04d-%02d-%02d %02d:%02d:%02d", dt->year, dt->month,
             dt->day, dt->hour, dt->min, dt->sec);

    char line[128];
    snprintf(line, sizeof(line), "%s,%.2f,%u,%.2f,%.2f,%.2f\n", ts, mean, count,
             min, max, std);
    microSD_appendLineAbsolute(path, line);
}

void pm25_avg10_process(void){
    ds3231_time_t dt;
    if(!ds3231_get_datetime(&dt)) return;

    if(dt.min % 10 == 0 && dt.min != last_boundary_minute){
        flag_promedio_10min = true;
        last_boundary_minute = dt.min;
    }

    if(flag_promedio_10min){
        uint16_t count = pm25_count;
        float mean = calculateAverage(pm25_buffer, count);
        float min = findMinValue(pm25_buffer, count);
        float max = findMaxValue(pm25_buffer, count);
        float std = calculateStandardDeviation(pm25_buffer, count);

        char ts[32];
        snprintf(ts, sizeof(ts), "%04d-%02d-%02d %02d:%02d:%02d", dt.year, dt.month,
                 dt.day, dt.hour, dt.min, dt.sec);
        uart_print("%s,%.2f,%u,%.2f,%.2f,%.2f\r\n", ts, mean, count, min, max, std);

        save_avg_csv(&dt, mean, count, min, max, std);
        clear_buffer();
        flag_promedio_10min = false;
    }
}

#if defined(UNIT_TEST) || defined(UNIT_TESTING)
uint16_t pm25_avg10_get_count(void){
    return pm25_count;
}
#endif
