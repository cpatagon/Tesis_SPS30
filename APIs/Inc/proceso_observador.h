/**
 * @file proceso_observador.h
 * @brief API del módulo observador para adquisición, validación y registro de datos de sensores
 * SPS30 y DHT22.
 *
 * Este módulo coordina la adquisición de datos ambientales en tiempo real, utilizando sensores
 * ópticos SPS30 (PM1.0, PM2.5, PM4.0, PM10) y sensores de temperatura/humedad DHT22.
 *
 * Las funciones disponibles permiten:
 * - Ejecutar ciclos de medición individuales (con o sin timestamp externo),
 * - Validar lecturas y almacenar registros crudos (raw) en microSD (CSV),
 * - Imprimir salidas por UART (debug/log),
 * - Alimentar los buffers circulares para promedios periódicos (10min, 1h, 24h).
 *
 * Parte del sistema embebido de medición de MP2.5 desarrollado como tesis en FIUBA.
 *
 * @author Luis Gómez
 * @date 04-05-2025
 * @copyright (C) 2023 Luis Gómez, CESE - FIUBA
 * @license GNU General Public License v3.0
 *
 * @see proceso_observador.c
 * @see data_logger.h
 * @see sps30_comm.h
 * @see DHT22.h
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef INC_PROCESO_OBSERVADOR_H_
#define INC_PROCESO_OBSERVADOR_H_

/** @file
 ** @brief Declaraciones públicas del proceso observador para SPS30
 **/

#include "sps30_comm.h"
#include "time_rtc.h"
#include "uart.h"
#include "observador_MEF.h"
#include <stdbool.h>
#include "config_sistema.h"
#include "config_mensaje.h"

#ifdef __cplusplus
extern "C" {
#endif

/* === Macros públicas === */
#define MSG_PM_FORMAT_WITH_TIME                                                                    \
    "[%s] SPS30 ID:%d | PM1.0: %.2f | PM2.5: %.2f | PM4.0: %.2f | PM10: %.2f | "                   \
    "ug/m3\n"

/* === Declaraciones de funciones públicas === */

/**
 * @brief Ejecuta un ciclo de adquisición desde un sensor SPS30 usando fecha/hora interna.
 *
 * @param sensor Puntero al objeto SPS30
 * @param sensor_id ID del sensor (1 a 3)
 * @param temp_amb Temperatura ambiente (°C)
 * @param hum_amb Humedad relativa ambiente (%)
 * @return true si la adquisición fue exitosa, false si falló tras reintentos
 */
bool proceso_observador(SPS30 * sensor, uint8_t sensor_id, float temp_amb, float hum_amb);

/**
 * @brief Ejecuta un ciclo de adquisición desde un sensor SPS30 con valores de 2 sensores DHT22.
 *
 * @param sensor Puntero al objeto SPS30
 * @param sensor_id ID del sensor (1 a 3)
 * @param datetime_str Cadena con timestamp formateado
 * @param temp_amb Temperatura ambiente (°C)
 * @param hum_amb Humedad relativa ambiente (%)
 * @param temp_cam Temperatura de cámara (°C)
 * @param hum_cam Humedad de cámara (%)
 * @return true si la lectura fue válida y almacenada, false si falló
 */
bool proceso_observador_3PM_2TH(SPS30 * sensor, uint8_t sensor_id, const char * datetime_str,
                                float temp_amb, float hum_amb, float temp_cam, float hum_cam);

/**
 * @brief Ejecuta un ciclo de adquisición SPS30 con timestamp externo.
 *
 * @param sensor Puntero al objeto SPS30
 * @param sensor_id ID del sensor (1 a 3)
 * @param datetime_str Timestamp formateado como cadena
 * @param temp_amb Temperatura ambiente (°C)
 * @param hum_amb Humedad relativa ambiente (%)
 * @return true si la adquisición fue exitosa, false si falló
 */
bool proceso_observador_with_time(SPS30 * sensor, uint8_t sensor_id, const char * datetime_str,
                                  float temp_amb, float hum_amb);

#ifdef __cplusplus
}
#endif

#endif /* INC_PROCESO_OBSERVADOR_H_ */
