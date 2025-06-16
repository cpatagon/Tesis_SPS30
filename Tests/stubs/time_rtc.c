#include "time_rtc.h"
#include <string.h>
void time_rtc_GetFormattedDateTime(char *buffer, size_t len){
    const char *fixed = "2025-06-16 12:00:00";
    strncpy(buffer, fixed, len);
    if(len>0) buffer[len-1]='\0';
}
bool ds3231_get_datetime(ds3231_time_t* dt){
    if(!dt) return false;
    dt->year=2025; dt->month=6; dt->day=16; dt->hour=12; dt->min=0; dt->sec=0;
    return true;
}
