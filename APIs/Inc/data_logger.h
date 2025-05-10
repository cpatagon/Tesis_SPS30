/*
 * Nombre del archivo: data_logger.h
 * Descripción: [Breve descripción del archivo]
 * Autor: lgomez
 * Creado en: May 10, 2025 
 * Derechos de Autor: (C) 2023 [Tu nombre o el de tu organización]
 * Licencia: GNU General Public License v3.0
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
#ifndef INC_DATA_LOGGER_H_
#define INC_DATA_LOGGER_H_
/** @file
 ** @brief 
 **/

/* === Headers files inclusions ================================================================ */

#include "shdlc.h"  // Para acceder a ConcentracionesPM
#include <stdint.h>
#include <stdbool.h>
#include "uart_printing.h"
/* === Cabecera C++ ============================================================================ */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */

// Tamaños de los buffers circulares
#define BUFFER_HIGH_FREQ_SIZE 60  // 60 muestras cada 10min = 10h
#define BUFFER_HOURLY_SIZE    24  // 24 muestras = 1 día
#define BUFFER_DAILY_SIZE     30  // 30 muestras = 1 mes


/* === Public data type declarations =========================================================== */

/**
 * @brief Estructura para almacenar una medición de material particulado
 */
typedef struct {
    char timestamp[32];          // Formato ISO8601
    uint8_t sensor_id;           // ID del sensor
    ConcentracionesPM valores;   // Valores de concentración
    float temperatura;           // Temperatura ambiente (opcional)
    float humedad;               // Humedad ambiente (opcional)
} MedicionMP;

/**
 * @brief Estructura para un buffer circular de datos
 */
typedef struct {
    MedicionMP* datos;          // Array de mediciones
    uint32_t capacidad;         // Tamaño máximo del buffer
    uint32_t inicio;            // Índice del elemento más antiguo
    uint32_t cantidad;          // Cantidad actual de elementos
} BufferCircular;


/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

/**
 * @brief Inicializa el sistema de almacenamiento de datos
 *
 * @return true si la inicialización fue exitosa
 */
bool data_logger_init(void);

/**
 * @brief Almacena una nueva medición en los buffers
 *
 * @param sensor_id ID del sensor que realizó la medición
 * @param valores Valores de concentración medidos
 * @param temperatura Temperatura ambiente (opcional, usar -999 si no disponible)
 * @param humedad Humedad ambiente (opcional, usar -999 si no disponible)
 * @return true si el almacenamiento fue exitoso
 */
bool data_logger_store_measurement(uint8_t sensor_id, ConcentracionesPM valores,
                                   float temperatura, float humedad);

/**
 * @brief Obtiene el promedio de PM2.5 de las últimas N mediciones
 *
 * @param sensor_id ID del sensor (0 para todos los sensores)
 * @param num_mediciones Número de mediciones a promediar
 * @return float Valor promedio de PM2.5
 */
float data_logger_get_average_pm25(uint8_t sensor_id, uint32_t num_mediciones);

/**
 * @brief Imprime resumen de datos almacenados
 *
 * @param uart Objeto UART para imprimir los datos
 */
void data_logger_print_summary(UART_Printing* uart);

/* === End of documentation ==================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* INC_DATA_LOGGER_H_ */
