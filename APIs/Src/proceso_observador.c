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

/* === Inclusión de archivos =============================================================== */
#include "proceso_observador.h"
#include "rtc_ds3231_for_stm32_hal.h" // para ds3231_get_datetime()
#include "data_logger.h"
#include "time_rtc.h"
#include <stdio.h>
#include <string.h>
#include "DHT22.h"
#include "sensors.h"

#include "ParticulateDataAnalyzer.h"

/* === Definición de funciones ============================================================= */
bool proceso_observador(SPS30 * sensor, uint8_t sensor_id) {
    // Obtener timestamp actual
    char datetime_buffer[32];
    time_rtc_GetFormattedDateTime(datetime_buffer, sizeof(datetime_buffer));

    return proceso_observador_with_time(sensor, sensor_id, datetime_buffer);
}

bool proceso_observador_with_time(SPS30 * sensor, uint8_t sensor_id, const char * datetime_str) {
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

            // ⏱ Obtener hora actual
            ds3231_time_t dt;

            // 🌡️ Leer temperatura y humedad del sensor asociado
            float temperatura = -99.9f;
            float humedad = -99.9f;

            if (sensor_id == 1) {
                DHT22_ReadSimple(&dhtA, &temperatura, &humedad); // Usa tu identificador real
            } else if (sensor_id == 2) {
                DHT22_ReadSimple(&dhtB, &temperatura, &humedad);
            }
            if (!ds3231_get_datetime(&dt)) {
                uart_print("Error leyendo hora del RTC\r\n");
                return false;
            }

            // 🧱 Formatear para UART
            char buffer[BUFFER_SIZE_MSG_PM_FORMAT];
            snprintf(buffer, sizeof(buffer), MSG_PM_FORMAT_WITH_TIME, datetime_str, sensor_id,
                     pm.pm1_0, pm.pm2_5, pm.pm4_0, pm.pm10);
            uart_print("%s", buffer);

            // 📦 Llenar estructura de datos para almacenamiento
            ParticulateData data = {.sensor_id = sensor_id,
                                    .pm1_0 = pm.pm1_0,
                                    .pm2_5 = pm.pm2_5,
                                    .pm4_0 = pm.pm4_0,
                                    .pm10 = pm.pm10,
                                    .temp = temperatura,
                                    .hum = humedad,
                                    .year = dt.year,
                                    .month = dt.month,
                                    .day = dt.day,
                                    .hour = dt.hour,
                                    .min = dt.min,
                                    .sec = dt.sec};

            data_logger_store_raw(&data); // 💾 Guardar como archivo RAW

            return true;
        }

        uart_print("%s", MSG_ERROR_REINT);
    }

    char error_msg[BUFFER_SIZE_MSG_ERROR_FALLO];
    snprintf(error_msg, sizeof(error_msg), MSG_ERROR_FALLO, datetime_str, sensor_id);
    uart_print("%s", error_msg);
    return false;
}
