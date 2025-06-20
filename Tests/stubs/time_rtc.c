#include "time_rtc.h"
#include <string.h>
static ds3231_time_t current_time = {12,0,0,16,6,2025};

void time_rtc_GetFormattedDateTime(char *buffer, size_t len){
    snprintf(buffer, len, "%04d-%02d-%02d %02d:%02d:%02d", current_time.year, current_time.month,
             current_time.day, current_time.hour, current_time.min, current_time.sec);
    if(len>0) buffer[len-1]='\0';
}

bool ds3231_get_datetime(ds3231_time_t* dt){
    if(!dt) return false;
    *dt = current_time;
    return true;
}

void stub_set_time(unsigned char hour, unsigned char min, unsigned char sec){
    current_time.hour = hour; current_time.min = min; current_time.sec = sec;
}

void stub_advance_seconds(unsigned int s){
    current_time.sec += s;
    while(current_time.sec >= 60){ current_time.sec -= 60; current_time.min++; }
    while(current_time.min >= 60){ current_time.min -= 60; current_time.hour++; }
    while(current_time.hour >= 24){ current_time.hour -= 24; }
}
