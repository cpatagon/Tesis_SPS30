/**
 * @file data_logger.h
 * @brief API para registrar y organizar datos de material particulado (PM) en microSD usando
 * estructura temporal.
 *
 * Este módulo gestiona el almacenamiento eficiente y ordenado de datos de concentración de material
 * particulado (PM1.0, PM2.5, PM4.0 y PM10), temperatura y humedad en una tarjeta microSD, usando
 * formato CSV y rutas estructuradas por fecha (/YYYY/MM/DD/).
 *
 * Incluye:
 * - Registro de mediciones crudas y estadísticas promediadas (10 min, 1 h, 24 h).
 * - Gestión de buffers circulares y ventanas temporales sincronizadas con el reloj RTC.
 * - Cálculo automático de estadísticas (media, mínimo, máximo, desviación estándar).
 * - Formateo y escritura de líneas CSV con timestamp.
 * - Control automático de ciclos y directorios para promedios acumulados.
 *
 * Este módulo está diseñado para operar en sistemas embebidos STM32 sin RTOS,
 * utilizando periféricos como UART, RTC y microSD vía FATFS.
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
#undef UNIT_TESTING
#include "ff_stub.h"
#define UNIT_TESTING
#else
#include "ff.h"
#endif

#include "config_sistema.h"
#include "data_types.h"
#include "buffers_config.h"

#include "shdlc.h" // Para acceder a ConcentracionesPM
#include "uart.h"
#include "pm25_buffer.h"
#include "data_logger.h"
#include "sensor.h"

#include <stdbool.h>
#include <stdint.h>

#include "ParticulateDataAnalyzer.h"
#include <rtc_ds3231_for_stm32_hal.h>
/* === Cabecera C++
 * ============================================================================
 */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions
 * =============================================================== */

/* === Public data type declarations
 * =========================================================== */

/* === Public variable declarations
 * ============================================================ */

// --- Buffers por sensor (asumiendo sensor_id ∈ {1, 2, 3}) ---

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
float data_logger_get_average_pm25_id(uint8_t sensor_id, uint32_t num_mediciones);

/** @brief Proceso periódico de análisis y almacenamiento de promedios. */
void proceso_analisis_periodico(float pm25_actual);

/** @brief Incrementa el contador de ciclos completados. */
void data_logger_increment_cycle(void);

/** Imprime los valores almacenado  en el buffer**/
void data_logger_print_value(void);

/** @brief Verifica si corresponde calcular promedios según el contador. */
void data_logger_check_cycle_averages(void);

/** @brief Imprime resumen general vía UART. */
void data_logger_print_summary(void);

/** @brief Log de promedio de 10 minutos. */
void log_avg10_data(const PMDataAveraged * avg);

/** @brief Log de promedio de 1 hora. */
void log_avg1h_data(const PMDataAveraged * avg);

/** @brief Log de promedio de 24 horas. */
void log_avg24h_data(const PMDataAveraged * avg);

/**
 * @brief Devuelve la cantidad de mediciones almacenadas para un sensor.
 *
 * @param sensor_id ID del sensor
 * @return Número de muestras válidas
 */
uint8_t data_logger_get_count(uint8_t sensor_id);

/**
 * @brief Versión de acceso directo a datos desde data_logger (valor crudo).
 *
 * @param sensor_id ID del sensor
 * @param index Índice en el buffer
 * @return Puntero a estructura `MedicionMP` o NULL si no existe
 */
const MedicionMP * data_logger_get_medicion(uint8_t sensor_id, uint8_t index);

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

/**
 * @brief Registra datos promediados cada 10 minutos en la microSD.
 *
 * Esta función escribe una línea en el archivo CSV correspondiente al promedio
 * de PM2.5 cada 10 minutos. La ruta del archivo es generada automáticamente
 * con base en la fecha y ubicación `/YYYY/MM/DD/AVG10.csv`.
 *
 * @param avg Puntero a la estructura `PMDataAveraged` que contiene los datos procesados.
 */
void log_avg10_data(const PMDataAveraged * avg);

/**
 * @brief Registra datos promediados por hora en la microSD.
 *
 * Escribe en el archivo `/YYYY/MM/DD/AVG60.csv` una línea con los datos estadísticos
 * de concentración de PM2.5 para el intervalo horario correspondiente.
 *
 * @param avg Puntero a la estructura `PMDataAveraged` con los datos promedio horarios.
 */
void log_avg1h_data(const PMDataAveraged * avg);

/**
 * @brief Registra datos promediados cada 24 horas en la microSD.
 *
 * Esta función escribe una línea en `/YYYY/MM/DD/AVG24.csv` con los datos agregados
 * de PM2.5 diarios. El formato del CSV incluye fecha, promedio, mínimo, máximo,
 * desviación estándar y cantidad de datos válidos.
 *
 * @param avg Puntero a la estructura `PMDataAveraged` con los datos diarios.
 */
void log_avg24h_data(const PMDataAveraged * avg);

/**
 * @brief Incrementa el estado interno del ciclo de almacenamiento de promedios.
 *
 * Esta función debe ser llamada después de almacenar un promedio de 10 minutos,
 * y permite que el sistema lleve control sobre los acumuladores horarios y diarios.
 */
void data_logger_increment_cycle(void);

/**
 * @brief Verifica si corresponde generar promedios horarios o diarios y los guarda si es necesario.
 *
 * Esta función compara la hora actual (obtenida del RTC) con los timestamps de inicio de los ciclos
 * horarios y diarios, y decide si corresponde consolidar y guardar dichos promedios. Llama
 * internamente a `log_avg1h_data()` o `log_avg24h_data()` si corresponde.
 */
void data_logger_check_cycle_averages(void);

void data_logger_print_value(void);

/**
 * @brief Guarda los datos de un sensor en el buffer circular de 10 minutos.
 *
 * Esta función recibe una estructura `MedicionMP` completa, que incluye
 * identificador del sensor, datos de PM, temperatura, humedad y timestamp,
 * y lo inserta en el buffer correspondiente.
 *
 * @param buffer Puntero al buffer circular destino.
 * @param data Estructura con los datos del sensor.
 * @return true si se guardaron correctamente, false si ocurrió un error (e.g. overflow).
 */
bool buffer_guardar(BufferCircular * buffer);

/**
 * @brief Guarda múltiples mediciones desde un arreglo temporal en los buffers circulares.
 *
 * @param temp_data Arreglo de datos de sensores (uno por sensor).
 * @param num_mediciones Número de elementos en el arreglo.
 * @param buffers_destino Arreglo de buffers circulares, uno por sensor.
 * @return true si todas las mediciones se guardaron correctamente, false si alguna falló.
 */

bool data_logger_store_sensor_data(const MedicionMP * temp_data, size_t num_mediciones,
                                   BufferCircularSensor * buffers_destino);

/**
 * @brief Calcula estadísticas de PM2.5 sobre un buffer de datos.
 *
 * @param buffer Puntero al buffer circular con datos recientes.
 * @param resultado Puntero a la estructura donde se guardará la estadística calculada.
 * @return true si se pudo calcular la estadística, false si no había datos suficientes.
 */

bool data_logger_estadistica_10min_pm25(const BufferCircularSensor * buffers,
                                        EstadisticaPM25 * resultado);

/**
 * @brief Guarda una estadística de 10 minutos en la microSD en formato CSV.
 *
 * @param data Estructura de estadística ya calculada.
 * @return true si se escribió correctamente en la microSD, false si ocurrió un error.
 */
bool data_logger_store_avg10_csv(const EstadisticaPM25 * data);

/**
 * @brief Limpia todos los buffers circulares asignados a sensores.
 *
 * @param buffers Arreglo de buffers circulares, uno por sensor.
 */
// void data_logger_buffer_limpiar_todos(BufferCircular * buffers);
void data_logger_buffer_limpiar_todos(BufferCircularSensor * buffers);

void registrar_promedio_24h(const ds3231_time_t * dt);

/* === End of documentation
 * ==================================================================== */

#ifdef UNIT_TEST
BufferCircular * get_buffer_high_freq(void);
BufferCircular * get_buffer_hourly(void);
BufferCircular * get_buffer_daily(void);
TimeWindow * get_time_window(void);
float * get_avg1h_buffer(void);
float * get_avg24h_buffer(void);
int get_hourly_index(void);
int get_daily_index(void);
#endif

#ifdef __cplusplus
}
#endif
#endif /* INC_DATA_LOGGER_H_ */
