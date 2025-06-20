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
#include "../APIs/Src/data_logger.c"

int main(void){
    stub_set_time(12,0,0);
    for(int i=0;i<61;i++){
        stub_advance_seconds(10);
        proceso_analisis_periodico(10.0f); // constant value
    }
    float *hbuf = get_avg1h_buffer();
    if(get_hourly_index()>0){
        for(int i=1;i<6;i++){
            for(int j=0;j<60;j++){ stub_advance_seconds(10); proceso_analisis_periodico(10.0f); }
        }
        if(get_daily_index()>0){ printf("PASS\n"); return 0; }
    }
    printf("hourly_index=%d daily_index=%d first=%f\n", get_hourly_index(), get_daily_index(), hbuf[0]);
    printf("FAIL\n");
    return 1;
}

// debugging
