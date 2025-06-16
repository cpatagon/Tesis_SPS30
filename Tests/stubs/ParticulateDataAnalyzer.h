#ifndef PARTICULATEDATAANALYZER_H
#define PARTICULATEDATAANALYZER_H
#include <stdint.h>
typedef struct {
    uint8_t sensor_id;
    float pm1_0;
    float pm2_5;
    float pm4_0;
    float pm10;
    float temp_amb;
    float hum_amb;
    float temp_cam;
    float hum_cam;
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} ParticulateData;
#endif
