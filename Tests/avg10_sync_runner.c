#include <stdio.h>
#define UNIT_TESTING
#include "stubs/fatfs_stub.h"
#include "stubs/ff_stub.h"
#include "stubs/fatfs.h"
#include "stubs/fatfs_sd.h"
#include "stubs/microSD_stub.h"
#include "stubs/microSD_utils.h"
#include "stubs/rtc.h"
#include "stubs/time_rtc.h"
#include "stubs/uart.h"
#include "stubs/rtc_ds3231_for_stm32_hal.h"
#include "stubs/ParticulateDataAnalyzer.h"
#include "stubs/mp_sensors_info.h"
#include "stubs/main.h"
#include "stubs/usart.h"
#include "../APIs/Src/pm25_avg10.c"

int main(void){
    stub_set_time(15,8,0);
    for(int i=0;i<12;i++){ // two minutes of samples every 10s
        pm25_avg10_add_sample(10.0f);
        stub_advance_seconds(10);
        pm25_avg10_process();
    }
    // advance to 15:10 exact
    stub_set_time(15,10,0);
    pm25_avg10_process();

    if(pm25_avg10_get_count()==0 && !flag_promedio_10min){
        printf("PASS\n");
        return 0;
    }else{
        printf("FAIL count=%u flag=%d\n", pm25_avg10_get_count(), flag_promedio_10min);
        return 1;
    }
}
