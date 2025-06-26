/*
 * @file proceso_observador.c
 * @brief Módulo observador para lectura, validación y registro de mediciones SPS30
 *
 * Este módulo implementa la lógica de adquisición y verificación de datos de sensores de
 * material particulado (SPS30), incluyendo:
 * - Inicio/parada de medición,
 * - Validación de concentraciones,
 * - Registro en microSD (formato RAW),
 * - Acumulación para estadísticas y promedios temporales (cada 10min, 1h, 24h),
 * - Integración con sensores de temperatura/humedad (DHT22).
 *
 * Forma parte del sistema embebido de medición de MP2.5 para tesis FIUBA-UBA.
 *
 * @author Luis Gómez
 * @date 04-05-2025
 * @copyright (C) 2023 Luis Gómez
 * @license GNU General Public License v3.0
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

/* === Inclusión de archivos
 * =============================================================== */
#include <sensor.h>
#include "proceso_observador.h"
#include "DHT22.h"
#include "data_logger.h"
#include "rtc_ds3231_for_stm32_hal.h" // para ds3231_get_datetime()
#include "time_rtc.h"
#include <stdio.h>
#include <string.h>

#include "ParticulateDataAnalyzer.h"

/* === Definición de funciones
 * ============================================================= */

/**
 * @brief Realiza una lectura completa desde un sensor SPS30 y registra los datos si son válidos.
 *
 * Esta función ejecuta el ciclo de medición para un sensor de material particulado SPS30:
 * inicia la medición, espera un tiempo, lee los datos y la detiene. Verifica si las
 * concentraciones son válidas (dentro de un rango aceptable) y si es así:
 * - Obtiene el timestamp desde el RTC.
 * - Formatea e imprime los datos por UART.
 * - Los guarda en un archivo CSV (raw).
 * - Alimenta el sistema de análisis promedio con `registrar_lectura_pm25`.
 *
 * En caso de fallo, intenta un número limitado de reintentos.
 *
 * @param sensor Puntero a estructura del sensor SPS30.
 * @param sensor_id Identificador del sensor (1, 2 o 3).
 * @param datetime_str Cadena de fecha/hora ya formateada (usada para mensajes UART).
 * @param temp_amb Temperatura ambiente medida (°C).
 * @param hum_amb Humedad relativa ambiente medida (%).
 * @param temp_cam Temperatura interna o de cámara medida (°C).
 * @param hum_cam Humedad relativa interna o de cámara (%).
 * @param rtc_error_msg Mensaje a imprimir si falla la lectura del RTC.
 * @return `true` si los datos fueron válidos y se almacenaron correctamente, `false` en caso
 * contrario.
 */

static bool proceso_observador_base(SPS30 * sensor, uint8_t sensor_id, const char * datetime_str,
                                    float temp_amb, float hum_amb, float temp_cam, float hum_cam,
                                    const char * rtc_error_msg);

/**
 * @brief Ejecuta un ciclo de observación para el sensor SPS30 con temperatura y humedad ambiente.
 *
 * Esta función obtiene el timestamp actual del RTC y lo pasa al proceso base.
 * Internamente llama a `proceso_observador_with_time()`.
 *
 * @param sensor Puntero a estructura del sensor SPS30.
 * @param sensor_id Identificador del sensor (1, 2 o 3).
 * @param temp_amb Temperatura ambiente medida (°C).
 * @param hum_amb Humedad relativa ambiente medida (%).
 * @return `true` si se registraron correctamente los datos, `false` en caso de error.
 */

bool proceso_observador(SPS30 * sensor, uint8_t sensor_id, float temp_amb, float hum_amb) {
    char datetime_buffer[32];
    time_rtc_GetFormattedDateTime(datetime_buffer, sizeof(datetime_buffer));

    return proceso_observador_with_time(sensor, sensor_id, datetime_buffer, temp_amb, hum_amb);
}

/**
 * @brief Realiza un ciclo de observación con timestamp externo y lectura de sensores DHT.
 *
 * Esta función permite ejecutar el ciclo de observación con un timestamp externo ya formateado
 * y mide la temperatura y humedad de la cámara correspondiente (según el ID del sensor).
 * Luego, llama a `proceso_observador_base()`.
 *
 * @param sensor Puntero a estructura del sensor SPS30.
 * @param sensor_id ID del sensor (1 a 3).
 * @param datetime_str Timestamp ya formateado como cadena.
 * @param temp_amb Temperatura ambiente (°C).
 * @param hum_amb Humedad ambiente (%).
 * @return `true` si se logró registrar una lectura válida, `false` si falló.
 */

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

/**
 * @brief Ejecuta un ciclo de observación con 3 variables PM y 2 de temperatura/humedad.
 *
 * Esta función es un envoltorio de conveniencia sobre `proceso_observador_base` que utiliza
 * un mensaje de error predefinido si falla la lectura del reloj RTC. Se utiliza en contextos
 * donde se realiza un monitoreo con un sensor SPS30, temperatura y humedad ambiente y de cámara.
 *
 * @param sensor Puntero al objeto `SPS30` correspondiente al sensor.
 * @param sensor_id Identificador numérico del sensor (1 a 3).
 * @param datetime_str Cadena de texto con la fecha y hora actual (formato legible).
 * @param temp_amb Temperatura ambiente en grados Celsius.
 * @param hum_amb Humedad relativa ambiente en porcentaje.
 * @param temp_cam Temperatura dentro del gabinete/cámara en grados Celsius.
 * @param hum_cam Humedad relativa dentro de la cámara en porcentaje.
 *
 * @return true si la lectura fue válida y se almacenó correctamente, false si falló.
 */

bool proceso_observador_3PM_2TH(SPS30 * sensor, uint8_t sensor_id, const char * datetime_str,
                                float temp_amb, float hum_amb, float temp_cam, float hum_cam) {
    return proceso_observador_base(sensor, sensor_id, datetime_str, temp_amb, hum_amb, temp_cam,
                                   hum_cam, "Error leyendo hora del RTC\r\n");
}

/**
 * @brief Realiza un ciclo de medición completo con un sensor SPS30.
 *
 * Esta función ejecuta un intento de medición del sensor SPS30, valida los datos,
 * los imprime por UART, los registra en microSD y ejecuta la lógica definida
 * por la máquina de estados del proceso observador.
 *
 * @param sensor Puntero al objeto `SPS30` correspondiente al sensor a leer.
 * @param sensor_id Identificador del sensor (1 a 3).
 * @param datetime_str Cadena con timestamp formateado para impresión.
 * @param temp_amb Temperatura ambiente medida (°C).
 * @param hum_amb Humedad relativa ambiente (%).
 * @param temp_cam Temperatura interna del gabinete o cámara (°C).
 * @param hum_cam Humedad relativa dentro de la cámara (%).
 * @param rtc_error_msg Mensaje de error personalizado si falla la lectura del RTC.
 *
 * @return true si la medición fue válida y procesada correctamente, false en caso contrario.
 */
static bool proceso_observador_base(SPS30 * sensor, uint8_t sensor_id, const char * datetime_str,
                                    float temp_amb, float hum_amb, float temp_cam, float hum_cam,
                                    const char * rtc_error_msg) {
    DEBUG_PRINT("[INFO] entra a  proceso_observador_base()\r\n");
    int reintentos = NUM_REINT;

    while (reintentos--) {
        sensor->start_measurement(sensor);
        HAL_Delay(2000); // ⏳ Espera crítica tras start_measurement()
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
            } else {
                DEBUG_PRINT(
                    "[WARN] RTC funcionando correctamente en  proceso_observador_base()\r\n");
            }

            char buffer[BUFFER_SIZE_MSG_PM_FORMAT];
            snprintf(buffer, sizeof(buffer), MSG_PM_FORMAT_WITH_TIME, datetime_str, sensor_id,
                     pm.pm1_0, pm.pm2_5, pm.pm4_0, pm.pm10);
            DEBUG_PRINT("%s", buffer);

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
            // registrar_lectura_pm25(sensor_id, pm.pm2_5);
            return true;
        }

        uart_print("%s", MSG_ERROR_REINT);
    }

    char error_msg[BUFFER_SIZE_MSG_ERROR_FALLO];
    snprintf(error_msg, sizeof(error_msg), MSG_ERROR_FALLO, datetime_str, sensor_id);
    uart_print("%s", error_msg);
    return false;
}
