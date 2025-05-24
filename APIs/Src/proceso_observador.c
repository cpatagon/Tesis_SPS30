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
#include "data_logger.h"
#include "time_rtc.h"
#include <stdio.h>
#include <string.h>

/* === Definición de funciones ============================================================= */
bool proceso_observador(SPS30* sensor, uint8_t sensor_id) {
    // Obtener timestamp actual
    char datetime_buffer[32];
    time_rtc_GetFormattedDateTime(datetime_buffer, sizeof(datetime_buffer));

    return proceso_observador_with_time(sensor,  sensor_id, datetime_buffer);
}

bool proceso_observador_with_time(SPS30* sensor, uint8_t sensor_id, const char* datetime_str) {
    int reintentos = NUM_REINT;

    while (reintentos--) {
        sensor->start_measurement(sensor);
        HAL_Delay(HAL_DELAY_SIGUIENTE_MEDICION);

        ConcentracionesPM pm = sensor->get_concentrations(sensor);
        sensor->stop_measurement(sensor);

        if ((pm.pm1_0 > CONC_MIN_PM && pm.pm1_0 < CONC_MAX_PM) ||
            (pm.pm2_5 > CONC_MIN_PM && pm.pm2_5 < CONC_MAX_PM) ||
            (pm.pm4_0 > CONC_MIN_PM && pm.pm4_0 < CONC_MAX_PM) ||
            (pm.pm10  > CONC_MIN_PM && pm.pm10  < CONC_MAX_PM)) {

            // Formatear mensaje para UART
            char buffer[BUFFER_SIZE_MSG_PM_FORMAT];
            snprintf(buffer, sizeof(buffer), MSG_PM_FORMAT_WITH_TIME,
                     datetime_str, sensor_id, pm.pm1_0, pm.pm2_5, pm.pm4_0, pm.pm10);
            uart_print("%s", buffer);

            // ⬇️ Crear estructura de datos para almacenamiento
            ParticulateData data = {
                .sensor_id = sensor_id,
                .pm1_0 = pm.pm1_0,
                .pm2_5 = pm.pm2_5,
                .pm4_0 = pm.pm4_0,
                .pm10 = pm.pm10,
                .temp = 0.0,     // Aquí podrías integrar sensor DHT22 si ya está disponible
                .hum = 0.0
            };
            strncpy(data.timestamp, datetime_str, sizeof(data.timestamp));

            log_data_to_sd(&data);  // <== Guardar en microSD

            return true;
        }

        uart_print("%s", MSG_ERROR_REINT);
    }

    char error_msg[BUFFER_SIZE_MSG_ERROR_FALLO];
    snprintf(error_msg, sizeof(error_msg), MSG_ERROR_FALLO, datetime_str, sensor_id);
    uart_print("%s", error_msg);
    return false;
}
