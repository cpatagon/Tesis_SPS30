/*
 * Nombre del archivo: pm25_buffer.h
 * Descripción: manejo de buffers circulares de PM2.5 por sensor
 * Autor: lgomez
 * Creado en: 22-06-2025
 * Derechos de Autor: (C) 2023 [Tu nombre o el de tu organización]
 * Licencia: GNU General Public License v3.0
 *
 * Este módulo permite almacenar y procesar lecturas de concentración PM2.5
 * en buffers circulares independientes por sensor. Se incluyen funciones
 * para agregar datos, obtener conteos, acceder a valores individuales,
 * limpiar buffers y calcular estadísticas como promedio, mínimo, máximo
 * y desviación estándar.
 *
 * Este programa es software libre: puedes redistribuirlo y/o modificarlo
 * bajo los términos de la Licencia Pública General GNU publicada por
 * la Free Software Foundation, ya sea la versión 3 de la Licencia, o
 * (a tu elección) cualquier versión posterior.
 *
 * Este programa se distribuye con la esperanza de que sea útil,
 * pero SIN NINGUNA GARANTÍA; sin siquiera la garantía implícita
 * de COMERCIABILIDAD o APTITUD PARA UN PROPÓSITO PARTICULAR. Ver la
 * Licencia Pública General GNU para más detalles.
 *
 * Deberías haber recibido una copia de la Licencia Pública General GNU
 * junto con este programa. Si no es así, visita <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-only
 *
 */
#ifndef INC_PM25_BUFFER_H_
#define INC_PM25_BUFFER_H_
/**  @file pm25_buffer.h
 * @brief API para manejo de buffers circulares de PM2.5 por sensor.
 *
 * Este módulo permite almacenar y procesar lecturas de concentración PM2.5
 * en buffers circulares independientes por sensor. Se incluyen funciones
 * para agregar datos, obtener conteos, acceder a valores individuales,
 * limpiar buffers y calcular estadísticas como promedio, mínimo, máximo
 * y desviación estándar.
 **/

/* === Headers files inclusions ================================================================ */

#include <stdint.h>
#include <stddef.h>
#include "data_logger.h"

/* === Cabecera C++ ============================================================================ */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */

/* === Public data type declarations =========================================================== */

/* === Public variable declarations ============================================================ */

/**
 * @brief Estructura de estadísticas de PM2.5 calculadas desde el buffer.
 */
typedef struct {
    float pm2_5_promedio;    /**< Promedio de las concentraciones */
    float min;               /**< Valor mínimo registrado */
    float max;               /**< Valor máximo registrado */
    float std;               /**< Desviación estándar */
    uint8_t n_datos_validos; /**< Cantidad de datos utilizados */
} EstadisticasPM;

/* === Public function declarations ============================================================ */

/**
 * @brief Devuelve un valor específico de PM2.5 para un sensor desde data_logger.
 *
 * @param sensor_id ID del sensor
 * @param index Índice del valor (0 a count-1)
 * @return Valor de PM2.5 o -1.0f si está fuera de rango
 */
float pm25_buffer_get_value(uint8_t sensor_id, uint8_t index);

/**
 * @brief Devuelve la cantidad de muestras válidas de PM2.5 disponibles para un sensor.
 *
 * @param sensor_id ID del sensor
 * @return Número de mediciones válidas
 */
uint8_t pm25_buffer_get_count(uint8_t sensor_id);

/**
 * @brief Calcula estadísticas (promedio, min, max, std) para PM2.5 de un sensor.
 *
 * @param sensor_id ID del sensor
 * @param stats Puntero a estructura donde se almacenarán los resultados
 */
void pm25_buffer_calcular_estadisticas(uint8_t sensor_id, EstadisticasPM * stats);

/**
 * @brief Reinicia el estado del buffer de un sensor. (Stub: delega en data_logger si aplica)
 *
 * @param sensor_id ID del sensor a limpiar
 */
void pm25_buffer_reset(uint8_t sensor_id);

/**
 * @brief Devuelve la cantidad de mediciones válidas registradas para un sensor.
 *
 * @param sensor_id ID del sensor
 * @return Cantidad de muestras válidas disponibles
 */
uint8_t data_logger_get_count(uint8_t sensor_id);

/* === End of documentation ==================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* INC_PM25_BUFFER_H_ */
