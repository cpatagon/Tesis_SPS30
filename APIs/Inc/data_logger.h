/**
 * @file data_logger.h
 * @brief API para registrar datos de sensores SPS30 en microSD en formato CSV.
 *
 * Este módulo permite almacenar datos crudos y promedios de concentraciones de material particulado
 * (PM1.0, PM2.5, PM4.0 y PM10) así como temperatura y humedad, en una tarjeta microSD usando
 * formato CSV. Incluye funciones para gestionar buffers circulares y generar reportes promediados.
 *
 * @author Luis Gómez
 * @date 10 May 2025
 * @copyright (C) 2023 Luis Gómez, CESE - FIUBA
 * @license GNU General Public License v3.0 <http://www.gnu.org/licenses/>
 */
#ifndef INC_DATA_LOGGER_H_
#define INC_DATA_LOGGER_H_
/** @file
 ** @brief
 **/

/* === Headers files inclusions
 * ================================================================ */

#ifdef UNIT_TESTING
#include "ff_stub.h"
#else
#include "ff.h"
#endif
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
#define BUFFER_HIGH_FREQ_SIZE 60  /**< 60 muestras = 10 minutos con frecuencia 10s */
#define BUFFER_HOURLY_SIZE    24  /**< 24 muestras = 1 por cada 10 minutos en una hora */
#define BUFFER_DAILY_SIZE     30  /**< 30 muestras = 1 cada hora durante 30 horas (aprox. 1 día) */
#define CSV_LINE_BUFFER_SIZE  128 /**< Tamaño máximo para formatear una línea CSV */

/* === Public data type declarations
 * =========================================================== */

/* === Public variable declarations
 * ============================================================ */

/**
 * @struct MedicionMP
 * @brief Estructura para almacenar una medición puntual de PM
 */
typedef struct {
    char timestamp[32];        /**< Timestamp en formato ISO8601 */
    uint8_t sensor_id;         /**< ID del sensor */
    ConcentracionesPM valores; /**< Concentraciones PM medidas */
    float temperatura;         /**< Temperatura en °C */
    float humedad;             /**< Humedad relativa en % */
} MedicionMP;

/**
 * @struct BufferCircular
 * @brief Estructura para manejar buffers circulares de mediciones
 */
typedef struct {
    MedicionMP * datos; /**< Arreglo de mediciones */
    uint32_t capacidad; /**< Tamaño máximo del buffer */
    uint32_t inicio;    /**< Índice del elemento más antiguo */
    uint32_t cantidad;  /**< Cantidad actual de elementos */
} BufferCircular;

/**
 * @struct PMDataAveraged
 * @brief Estructura para almacenar estadísticas de PM2.5
 */
typedef struct {
    float mean; /**< Media */
    float max;  /**< Máximo */
    float min;  /**< Mínimo */
    float std;  /**< Desviación estándar */
} PMDataAveraged;

/* === Public function declarations
 * ============================================================ */

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

/** @brief Inicializa el sistema de almacenamiento (microSD, buffers, etc.). */
bool data_logger_init(void);

/**
 * @brief Guarda una medición en los buffers internos.
 * @param sensor_id ID del sensor
 * @param valores Valores de concentración
 * @param temperatura Temperatura medida
 * @param humedad Humedad medida
 */
bool data_logger_store_measurement(uint8_t sensor_id, ConcentracionesPM valores, float temperatura,
                                   float humedad);

/**
 * @brief Obtiene el promedio de PM2.5 de las últimas N mediciones.
 * @param sensor_id ID del sensor (0 = todos)
 * @param num_mediciones Número de muestras a promediar
 */
float data_logger_get_average_pm25(uint8_t sensor_id, uint32_t num_mediciones);

/** @brief Proceso periódico de análisis y almacenamiento de promedios. */
void proceso_analisis_periodico(float pm25_actual);

/** @brief Imprime resumen general vía UART. */
void data_logger_print_summary(void);

/** @brief Log de promedio de 10 minutos. */
void log_avg10_data(const PMDataAveraged * avg);

/** @brief Log de promedio de 1 hora. */
void log_avg1h_data(const PMDataAveraged * avg);

/** @brief Log de promedio de 24 horas. */
void log_avg24h_data(const PMDataAveraged * avg);

/**
 * @brief Formatea una línea CSV a partir de los datos
 * @param data Estructura con los datos
 * @param csv_line Cadena de salida
 * @param max_len Longitud máxima permitida
 */
bool format_csv_line(const ParticulateData * data, char * csv_line, size_t max_len);

/** @brief Escribe línea CSV en ruta basada en timestamp. */
bool data_logger_write_csv_line(const ParticulateData * data);

/** @brief Construye ruta de archivo en base a timestamp actual. */
bool build_csv_filepath_from_datetime(char * filepath, size_t max_len);

/** @brief Crea directorio en base a fecha actual. */
bool crear_directorio_fecha(const ds3231_time_t * dt);

/**
 * @brief Obtiene ruta con timestamp y nombre de archivo
 * @param dt Fecha/hora base
 * @param nombre_archivo Nombre del archivo
 * @param filepath Buffer de salida para ruta
 * @param len Longitud máxima del buffer
 */
bool obtener_ruta_archivo(const ds3231_time_t * dt, const char * nombre_archivo, char * filepath,
                          size_t len);

/** @brief Escribe línea de texto en un archivo CSV. */
bool escribir_linea_csv(const char * filepath, const char * linea);

/** @brief Guarda los datos crudos en microSD. */
bool log_data_to_sd(const ParticulateData * data);

/** @brief Almacena mediciones sin procesar en microSD. */
bool data_logger_store_raw(const ParticulateData * data);

/* === End of documentation
 * ==================================================================== */

#ifdef UNIT_TEST
BufferCircular * get_buffer_high_freq(void);
BufferCircular * get_buffer_hourly(void);
BufferCircular * get_buffer_daily(void);
#endif

#ifdef __cplusplus
}
#endif
#endif /* INC_DATA_LOGGER_H_ */
