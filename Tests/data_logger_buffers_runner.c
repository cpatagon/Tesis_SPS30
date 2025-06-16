#include <stdio.h>

// Definir la macro antes que todo
#define UNIT_TESTING

#ifdef UNIT_TESTING
// Incluye solo stubs en entorno de pruebas
#include "stubs/fatfs_stub.h"
#include "stubs/ff_stub.h"
#include "stubs/fatfs.h"
#include "stubs/fatfs_sd.h"
#include "stubs/microSD_stub.h" // << Usa esta versión que no incluya ff.h real
#include "stubs/microSD_utils.h"
#include "stubs/rtc.h"
#include "stubs/time_rtc.h"
#include "stubs/uart.h"
#include "stubs/rtc_ds3231_for_stm32_hal.h"
#include "stubs/ParticulateDataAnalyzer.h"
#include "stubs/mp_sensors_info.h"
#include "stubs/main.h"
#include "stubs/usart.h"
#else
// No deberías entrar a esta rama en pruebas unitarias
#include "ff.h"
#include "microSD.h"
#include "microSD_utils.h"
#endif

// Incluir el .c del módulo bajo prueba sólo en test
#include "../APIs/Src/data_logger.c"

// Declaración de buffers que quieras inspeccionar
BufferCircular * get_buffer_high_freq(void);
BufferCircular * get_buffer_hourly(void);
BufferCircular * get_buffer_daily(void);

int main(void) {
    ConcentracionesPM val = {1.0f, 2.0f, 3.0f, 4.0f};

    if (!data_logger_store_measurement(2, val, 25.0f, 50.0f))
        return 1;

    BufferCircular * hf = get_buffer_high_freq();
    BufferCircular * h = get_buffer_hourly();
    BufferCircular * d = get_buffer_daily();

    if (hf->cantidad == 1 && h->cantidad == 1 && d->cantidad == 1 && hf->datos[0].sensor_id == 2 &&
        h->datos[0].sensor_id == 2 && d->datos[0].sensor_id == 2) {
        printf("PASS\n");
        return 0;
    } else {
        printf("FAIL\n");
        return 1;
    }
}
