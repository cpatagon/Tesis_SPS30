/*
 * Nombre del archivo: data_logger.h
 * Descripción: API para registrar datos de sensores en microSD en formato CSV
 * Autor: lgomez
 * Creado en: May 10, 2025
 * Derechos de Autor: (C) 2023 Luis Gómez CESE FiUBA
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

/* === Headers files inclusions
 * ================================================================ */

#include "ff.h"
#include "shdlc.h" // Para acceder a ConcentracionesPM
#include "uart.h"
#include <stdbool.h>
#include <stdint.h>

#include "ParticulateDataAnalyzer.h"
#include "rtc_ds3231_for_stm32_hal.h"
/* === Cabecera C++
 * ============================================================================
 */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions
 * =============================================================== */

// Tamaños de los buffers circulares
#define BUFFER_HIGH_FREQ_SIZE 60 // 60 muestras cada 10min = 10h
#define BUFFER_HOURLY_SIZE    24 // 24 muestras = 1 día
#define BUFFER_DAILY_SIZE     30 // 30 muestras = 1 mes

#define CSV_LINE_BUFFER_SIZE  128

/* === Public data type declarations
 * =========================================================== */

/**
 * @brief Guarda una línea de promedio de concentraciones en la microSD.
 *
 * @param pm1_0  Promedio de PM1.0 en ug/m3
 * @param pm2_5  Promedio de PM2.5 en ug/m3
 * @param pm4_0  Promedio de PM4.0 en ug/m3
 * @param pm10   Promedio de PM10 en ug/m3
 * @param temp   Promedio de temperatura en °C
 * @param hum    Promedio de humedad relativa en %
 * @return FRESULT Código de resultado de la operación FatFs
 */
FRESULT guardar_promedio_csv(float pm1_0, float pm2_5, float pm4_0, float pm10, float temp,
                             float hum);

/**
 * @brief Estructura para almacenar una medición de material particulado
 */
typedef struct {
    char timestamp[32];        // Formato ISO8601
    uint8_t sensor_id;         // ID del sensor
    ConcentracionesPM valores; // Valores de concentración
    float temperatura;         // Temperatura ambiente (opcional)
    float humedad;             // Humedad ambiente (opcional)
} MedicionMP;

/**
 * @brief Estructura para un buffer circular de datos
 */
typedef struct {
    MedicionMP * datos; // Array de mediciones
    uint32_t capacidad; // Tamaño máximo del buffer
    uint32_t inicio;    // Índice del elemento más antiguo
    uint32_t cantidad;  // Cantidad actual de elementos
} BufferCircular;

/* === Public variable declarations
 * ============================================================ */

/* === Public function declarations
 * ============================================================ */

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
 * @param temperatura Temperatura ambiente (opcional, usar -999 si no
 * disponible)
 * @param humedad Humedad ambiente (opcional, usar -999 si no disponible)
 * @return true si el almacenamiento fue exitoso
 */
bool data_logger_store_measurement(uint8_t sensor_id, ConcentracionesPM valores, float temperatura,
                                   float humedad);

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
void data_logger_print_summary(void);

/**
 * @brief Formatea una estructura de datos como línea CSV
 *
 * @param data Puntero a estructura con los datos
 * @param csv_line Cadena de salida donde se almacenará la línea CSV
 * @param max_len Tamaño máximo del búfer de salida
 * @return true si el formateo fue exitoso, false si hubo error de espacio
 */
bool format_csv_line(const ParticulateData * data, char * csv_line, size_t max_len);

bool build_csv_filepath_from_datetime(char * filepath, size_t max_len);

bool log_data_to_sd(const ParticulateData * data);

void print_fatfs_error(FRESULT res);

bool data_logger_write_csv_line(const ParticulateData * data);

bool data_logger_store_raw(const ParticulateData * data);

bool crear_directorio_fecha(const ds3231_time_t * dt);

bool escribir_linea_csv(const char * filepath, const char * linea);

bool obtener_ruta_archivo(const ds3231_time_t * dt, const char * nombre_archivo, char * filepath,
                          size_t len);

/* === End of documentation
 * ==================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* INC_DATA_LOGGER_H_ */
