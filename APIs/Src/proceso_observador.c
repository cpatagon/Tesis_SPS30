/*
 * Nombre del archivo: proceso_observador.c
 * Descripción: Implementación del proceso observador para sensores SPS30
 * Autor: lgomez
 * Creado en: 04-05-2025
 * Derechos de Autor: (C) 2023 [Tu nombre o institución]
 * Licencia: GNU General Public License v3.0
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

/* === Inclusión de archivos
 * =============================================================== */
#include "proceso_observador.h"
#include "DHT22.h"
#include "data_logger.h"
#include "rtc_ds3231_for_stm32_hal.h" // para ds3231_get_datetime()
#include "sensors.h"
#include "time_rtc.h"
#include <stdio.h>
#include <string.h>

#include "ParticulateDataAnalyzer.h"

static float pm25_sensores[3];
static int sensores_recibidos = 0;

/* === Definición de funciones
 * ============================================================= */

static bool proceso_observador_base(SPS30 * sensor, uint8_t sensor_id, const char * datetime_str,
                                    float temp_amb, float hum_amb, float temp_cam, float hum_cam,
                                    const char * rtc_error_msg);

bool proceso_observador(SPS30 * sensor, uint8_t sensor_id, float temp_amb, float hum_amb) {
    char datetime_buffer[32];
    time_rtc_GetFormattedDateTime(datetime_buffer, sizeof(datetime_buffer));

    return proceso_observador_with_time(sensor, sensor_id, datetime_buffer, temp_amb, hum_amb);
}

bool proceso_observador_with_time(SPS30 * sensor, uint8_t sensor_id, const char * datetime_str,
                                  float temp_amb, float hum_amb) {
    float temp_cam = -99.9f;
    float hum_cam = -99.9f;

    if (sensor_id == 1) {
        DHT22_ReadSimple(&dhtA, &temp_cam, &hum_cam);
    } else if (sensor_id == 2) {
        DHT22_ReadSimple(&dhtB, &temp_cam, &hum_cam);
    }

    return proceso_observador_base(sensor, sensor_id, datetime_str, temp_amb, hum_amb, temp_cam,
                                   hum_cam, "Error leyendo hora del RTC\r\n");
}

bool proceso_observador_3PM_2TH(SPS30 * sensor, uint8_t sensor_id, const char * datetime_str,
                                float temp_amb, float hum_amb, float temp_cam, float hum_cam) {
    return proceso_observador_base(sensor, sensor_id, datetime_str, temp_amb, hum_amb, temp_cam,
                                   hum_cam, "⚠️ Error leyendo hora del RTC\r\n");
}

static bool proceso_observador_base(SPS30 * sensor, uint8_t sensor_id, const char * datetime_str,
                                    float temp_amb, float hum_amb, float temp_cam, float hum_cam,
                                    const char * rtc_error_msg) {
    int reintentos = NUM_REINT;

    while (reintentos--) {
        sensor->start_measurement(sensor);
        HAL_Delay(HAL_DELAY_SIGUIENTE_MEDICION);

        ConcentracionesPM pm = sensor->get_concentrations(sensor);
        sensor->stop_measurement(sensor);

        if ((pm.pm1_0 > CONC_MIN_PM && pm.pm1_0 < CONC_MAX_PM) ||
            (pm.pm2_5 > CONC_MIN_PM && pm.pm2_5 < CONC_MAX_PM) ||
            (pm.pm4_0 > CONC_MIN_PM && pm.pm4_0 < CONC_MAX_PM) ||
            (pm.pm10 > CONC_MIN_PM && pm.pm10 < CONC_MAX_PM)) {

            ds3231_time_t dt;
            if (!ds3231_get_datetime(&dt)) {
                uart_print("%s", rtc_error_msg);
                return false;
            }

            char buffer[BUFFER_SIZE_MSG_PM_FORMAT];
            snprintf(buffer, sizeof(buffer), MSG_PM_FORMAT_WITH_TIME, datetime_str, sensor_id,
                     pm.pm1_0, pm.pm2_5, pm.pm4_0, pm.pm10);
            uart_print("%s", buffer);

            ParticulateData data = {
                .sensor_id = sensor_id,
                .pm1_0 = pm.pm1_0,
                .pm2_5 = pm.pm2_5,
                .pm4_0 = pm.pm4_0,
                .pm10 = pm.pm10,
                .temp_amb = temp_amb,
                .hum_amb = hum_amb,
                .temp_cam = temp_cam,
                .hum_cam = hum_cam,
                .year = dt.year,
                .month = dt.month,
                .day = dt.day,
                .hour = dt.hour,
                .min = dt.min,
                .sec = dt.sec,
            };

            data_logger_store_raw(&data);
            registrar_lectura_pm25(sensor_id, pm.pm2_5);
            return true;
        }

        uart_print("%s", MSG_ERROR_REINT);
    }

    char error_msg[BUFFER_SIZE_MSG_ERROR_FALLO];
    snprintf(error_msg, sizeof(error_msg), MSG_ERROR_FALLO, datetime_str, sensor_id);
    uart_print("%s", error_msg);
    return false;
}

void registrar_lectura_pm25(uint8_t sensor_id, float pm25) {
    if (sensor_id >= 1 && sensor_id <= 3) {
        pm25_sensores[sensor_id - 1] = pm25;
        sensores_recibidos++;

        if (sensores_recibidos == 3) {
            // Ciclo completo
            float promedio_ciclo =
                (pm25_sensores[0] + pm25_sensores[1] + pm25_sensores[2]) / 3.0f;

            data_logger_increment_cycle();
            proceso_analisis_periodico(promedio_ciclo); // ✅ alimenta buffers

            sensores_recibidos = 0; // reiniciar para próximo ciclo
        }
    }
}
