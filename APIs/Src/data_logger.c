/*
 * Nombre del archivo: data_logger.c
 * Descripción: Registro de datos de sensores SPS30 en buffers y microSD
 * Autor: lgomez
 * Creado en: May 10, 2025
 * Derechos de Autor: (C) 2023 lgomez
 * Licencia: GNU General Public License v3.0
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

/** @file
 ** @brief Módulo de almacenamiento y resumen de datos del sistema MP2.5
 */

/* === Headers files inclusions =============================================================== */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "fatfs.h"
#include "fatfs_sd.h"
#include "microSD.h"
#include "microSD_utils.h"

#include "rtc.h"
#include "data_logger.h"
#include "time_rtc.h"

#include "uart.h"
#ifdef UNIT_TESTING
#undef UNIT_TESTING
#include "ff_stub.h"
#define UNIT_TESTING
#else
#include "ff.h"
#endif

#include "rtc_ds3231_for_stm32_hal.h"

#include "ParticulateDataAnalyzer.h"

#include "mp_sensors_info.h"

#ifdef UNIT_TESTING
float calculateAverage(float data[], int n_data) {
    return 0.0f;
}
float findMaxValue(float data[], int n_data) {
    return 0.0f;
}
float findMinValue(float data[], int n_data) {
    return 0.0f;
}
float calculateStandardDeviation(float data[], int n) {
    return 0.0f;
}
void print_fatfs_error(FRESULT res) {
}
#endif
/* === Macros definitions ====================================================================== */

#define CSV_LINE_BUFFER_SIZE 128

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Variables estáticas === */
static MedicionMP buffer_alta_frec[BUFFER_HIGH_FREQ_SIZE];
static MedicionMP buffer_horario[BUFFER_HOURLY_SIZE];
static MedicionMP buffer_diario[BUFFER_DAILY_SIZE];

static FATFS fs;                // Sistema de archivos FAT32
static bool sd_mounted = false; // Bandera de estado para evitar montaje doble

static BufferCircular buffer_alta_frecuencia = {
    .datos = buffer_alta_frec, .capacidad = BUFFER_HIGH_FREQ_SIZE, .inicio = 0, .cantidad = 0};

static BufferCircular buffer_hora = {
    .datos = buffer_horario, .capacidad = BUFFER_HOURLY_SIZE, .inicio = 0, .cantidad = 0};

static BufferCircular buffer_dia = {
    .datos = buffer_diario, .capacidad = BUFFER_DAILY_SIZE, .inicio = 0, .cantidad = 0};

extern RTC_HandleTypeDef hrtc;

static TimeWindow current_window = {0};
static float hourly_avgs[AVG10_PER_HOUR] = {0};
static int hourly_index = 0;
static float daily_avgs[AVG1H_PER_DAY] = {0};
static int daily_index = 0;
static ds3231_time_t hourly_start_time = {0};
static ds3231_time_t daily_start_time = {0};
/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/**
 * @brief Añade una medición al buffer circular especificado
 *
 * @param buffer Buffer donde almacenar la medición
 * @param medicion Medición a almacenar
 */

static void buffer_circular_agregar(BufferCircular * buffer, const MedicionMP * medicion) {
    uint32_t indice;

    if (buffer->cantidad < buffer->capacidad) {
        // El buffer aún no está lleno
        indice = (buffer->inicio + buffer->cantidad) % buffer->capacidad;
        buffer->cantidad++;
    } else {
        // El buffer está lleno, sobrescribir el elemento más antiguo
        indice = buffer->inicio;
        buffer->inicio = (buffer->inicio + 1) % buffer->capacidad;
    }

    // Copiar la medición al buffer
    memcpy(&buffer->datos[indice], medicion, sizeof(MedicionMP));
}

/**
 * @brief Calcula la diferencia de tiempo en segundos entre dos instantes del día.
 *
 * Esta función calcula cuántos segundos han pasado entre `start` y `end`,
 * considerando un ciclo horario de 24 horas. Si `end` es anterior a `start`,
 * se asume que ocurrió un cambio de día.
 *
 * @param start Puntero a la estructura de tiempo inicial (`ds3231_time_t`).
 * @param end   Puntero a la estructura de tiempo final (`ds3231_time_t`).
 * @return Diferencia de tiempo en segundos (ajustada para rollover diario).
 */

static unsigned int time_diff_seconds(const ds3231_time_t * start, const ds3231_time_t * end);

/**
 * @brief Verifica si ha transcurrido un intervalo de 10 minutos desde el tiempo inicial del buffer
 * de 10min.
 *
 * Compara el tiempo actual `dt` con `current_window.start_time`, y determina si han transcurrido
 * al menos 600 segundos (10 minutos).
 *
 * @param dt Puntero a la estructura de tiempo actual (`ds3231_time_t`).
 * @return true si han pasado 10 minutos o más; false en caso contrario.
 */

static bool is_10min_boundary(const ds3231_time_t * dt) {
    return time_diff_seconds(&current_window.start_time, dt) >= 10 * 60;
}

/**
 * @brief Verifica si ha transcurrido una hora desde el tiempo inicial del buffer horario.
 *
 * Compara el tiempo actual `dt` con `hourly_start_time`, y determina si han transcurrido
 * al menos 3600 segundos (1 hora).
 *
 * @param dt Puntero a la estructura de tiempo actual (`ds3231_time_t`).
 * @return true si ha pasado 1 hora o más; false en caso contrario.
 */

static bool is_1hour_boundary(const ds3231_time_t * dt) {
    return time_diff_seconds(&hourly_start_time, dt) >= 60 * 60;
}

/**
 * @brief Verifica si ha transcurrido un intervalo de 24 horas desde el tiempo inicial del buffer
 * diario.
 *
 * Compara el tiempo actual `dt` con `daily_start_time`, y determina si han transcurrido
 * al menos 86400 segundos (24 horas).
 *
 * @param dt Puntero a la estructura de tiempo actual (`ds3231_time_t`).
 * @return true si han pasado 24 horas o más; false en caso contrario.
 */

static bool is_24hour_boundary(const ds3231_time_t * dt) {
    return time_diff_seconds(&daily_start_time, dt) >= 24 * 3600;
}

/**
 * @brief Calcula la diferencia de tiempo en segundos entre dos instantes del mismo día o con
 * rollover.
 *
 * Esta función convierte dos estructuras de tiempo (`ds3231_time_t`) en segundos desde las 00:00:00
 * y calcula la diferencia entre ellas. Si `end` ocurre antes que `start` (por ejemplo, cuando hay
 * un cambio de día), ajusta la diferencia para considerar el rollover de 24 horas.
 *
 * @param start Puntero a la estructura `ds3231_time_t` que representa el tiempo inicial.
 * @param end   Puntero a la estructura `ds3231_time_t` que representa el tiempo final.
 *
 * @return Diferencia en segundos entre `end` y `start`. El resultado siempre es no negativo
 *         y está en el rango [0, 86399].
 *
 * @note Esta función asume que ambos tiempos corresponden a la misma fecha o que la diferencia
 *       no supera un día completo. No considera cambios de fecha (solo de hora).
 */

static unsigned int time_diff_seconds(const ds3231_time_t * start, const ds3231_time_t * end) {
    int start_s = start->hour * 3600 + start->min * 60 + start->sec;
    int end_s = end->hour * 3600 + end->min * 60 + end->sec;
    int diff = end_s - start_s;
    if (diff < 0)
        diff += 24 * 3600;
    return (unsigned int)diff;
}

/**
 * @brief Acumula una muestra de PM2.5 en la ventana temporal actual de 10 minutos.
 *
 * Esta función guarda el valor de la muestra en un buffer si hay espacio disponible.
 * Si es la primera muestra del ciclo, inicializa los tiempos de referencia para
 * los cálculos de promedios a 10 minutos, 1 hora y 24 horas.
 *
 * @param sample Muestra de concentración PM2.5.
 * @param dt     Puntero a la estructura `ds3231_time_t` con el timestamp de la muestra.
 */

static void accumulate_sample_in_current_window(float sample, const ds3231_time_t * dt) {
    if (current_window.count == 0) {
        current_window.start_time = *dt;
        if (hourly_index == 0)
            hourly_start_time = *dt;
        if (daily_index == 0)
            daily_start_time = *dt;
    }

    if (current_window.count < MAX_SAMPLES_PER_10MIN) {
        current_window.samples[current_window.count++] = sample;
    }
}

/**
 * @brief Calcula los estadísticos de la ventana de datos de 10 minutos y reinicia el buffer.
 *
 * Esta función genera un resumen estadístico (promedio, mínimo, máximo, desviación estándar)
 * de las muestras acumuladas y retorna una estructura `TimeSyncedAverage` con los resultados.
 * Luego, limpia el buffer para el siguiente intervalo de 10 minutos.
 *
 * @return Estructura `TimeSyncedAverage` con los valores calculados y timestamp inicial.
 */

static TimeSyncedAverage finalize_temporal_window(void) {
    TimeSyncedAverage avg = {0};

    avg.timestamp = current_window.start_time;
    avg.sample_count = current_window.count;
    if (current_window.count > 0) {
        avg.pm2_5_avg = calculateAverage(current_window.samples, current_window.count);
        avg.pm2_5_max = findMaxValue(current_window.samples, current_window.count);
        avg.pm2_5_min = findMinValue(current_window.samples, current_window.count);
        avg.pm2_5_std = calculateStandardDeviation(current_window.samples, current_window.count);
    }

    current_window.count = 0;
    return avg;
}

/**
 * @brief Crea los directorios base de almacenamiento para promedios si no existen.
 *
 * Verifica y crea (si es necesario) los directorios `/AVG10`, `/AVG60` y `/AVG24`
 * en la microSD. Esto asegura que existan antes de guardar archivos CSV.
 */

static void ensure_avg_directories(void) {
    const char * dirs[] = {"/AVG10", "/AVG60", "/AVG24"};
    for (unsigned int i = 0; i < sizeof(dirs) / sizeof(dirs[0]); i++) {
        if (f_stat(dirs[i], NULL) != FR_OK) {
            f_mkdir(dirs[i]);
        }
    }
}

/**
 * @brief Guarda una estructura `TimeSyncedAverage` en un archivo CSV de la microSD.
 *
 * Formatea los datos de la estructura `avg` en una línea CSV, detecta el tipo
 * de promedio según la ruta (AVG10, AVG60, AVG24), y guarda el resultado en
 * la ubicación especificada por `path`.
 *
 * @param avg  Puntero a la estructura `TimeSyncedAverage` con los datos procesados.
 * @param path Ruta absoluta al archivo CSV donde se escribirá la línea.
 */

static void save_temporal_average_to_csv(const TimeSyncedAverage * avg, const char * path) {
    ensure_avg_directories();
    const char * type = "UNK";
    if (strstr(path, "AVG10"))
        type = "avg10";
    else if (strstr(path, "AVG60"))
        type = "avg60";
    else if (strstr(path, "AVG24"))
        type = "avg24";

    char line[128];
    snprintf(line, sizeof(line), "%04d-%02d-%02d %02d:%02d:%02d,%s,%.2f,%u,%.2f,%.2f,%.2f\n",
             avg->timestamp.year, avg->timestamp.month, avg->timestamp.day, avg->timestamp.hour,
             avg->timestamp.min, avg->timestamp.sec, type, avg->pm2_5_avg, avg->sample_count,
             avg->pm2_5_min, avg->pm2_5_max, avg->pm2_5_std);

    microSD_appendLineAbsolute(path, line);
}

/* === Public function implementation ========================================================== */

/**
 * @brief Registra en la microSD los datos promediados de PM2.5 cada 10 minutos.
 *
 * Escribe una línea en el archivo `/AVG10/avg10.csv` con los valores estadísticos
 * de la concentración de PM2.5. También imprime el resumen por UART para monitoreo.
 *
 * @param avg Puntero a la estructura `PMDataAveraged` con los datos a registrar.
 */
void log_avg10_data(const PMDataAveraged * avg) {
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "%.2f,%.2f,%.2f,%.2f\n", avg->mean, avg->max, avg->min,
             avg->std);

    microSD_appendLineAbsolute("/AVG10/avg10.csv", buffer);

    uart_print("[PROMEDIO 10min] PM2.5 -> media: %.2f, max: %.2f, min: %.2f, std: %.2f\r\n",
               avg->mean, avg->max, avg->min, avg->std);
}

/**
 * @brief Registra en la microSD los datos promediados de PM2.5 cada 1 hora.
 *
 * Escribe una línea en el archivo `/AVG60/avg60.csv` con los valores estadísticos
 * del promedio horario de PM2.5. También imprime el resumen por UART.
 *
 * @param avg Puntero a la estructura `PMDataAveraged` con los datos a registrar.
 */
void log_avg1h_data(const PMDataAveraged * avg) {
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "%.2f,%.2f,%.2f,%.2f\n", avg->mean, avg->max, avg->min,
             avg->std);

    microSD_appendLineAbsolute("/AVG60/avg60.csv", buffer);

    uart_print("[PROMEDIO 1h] PM2.5 -> media: %.2f, max: %.2f, min: %.2f, std: %.2f\r\n", avg->mean,
               avg->max, avg->min, avg->std);
}

/**
 * @brief Registra en la microSD los datos promediados de PM2.5 cada 24 horas.
 *
 * Escribe una línea en el archivo `/AVG24/avg24.csv` con los valores estadísticos
 * del promedio diario de PM2.5. También imprime el resumen por UART.
 *
 * @param avg Puntero a la estructura `PMDataAveraged` con los datos a registrar.
 */

void log_avg24h_data(const PMDataAveraged * avg) {
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "%.2f,%.2f,%.2f,%.2f\n", avg->mean, avg->max, avg->min,
             avg->std);

    microSD_appendLineAbsolute("/AVG24/avg24.csv", buffer);

    uart_print("[PROMEDIO 24h] PM2.5 -> media: %.2f, max: %.2f, min: %.2f, std: %.2f\r\n",
               avg->mean, avg->max, avg->min, avg->std);
}

/**
 * @brief Función de compatibilidad para manejo de ciclos (actualmente no utilizada).
 *
 * Esta función existía para el control basado en conteo de muestras o ciclos,
 * pero ha sido reemplazada por el control temporal basado en RTC.
 */
void data_logger_increment_cycle(void) {
    // Ya no se utiliza el contador de ciclos
}

/**
 * @brief Verifica si ha transcurrido el tiempo necesario para consolidar promedios de PM2.5.
 *
 * Esta función compara el tiempo actual (`dt`) con el inicio de la ventana de 10 minutos.
 * Si han pasado al menos 600 segundos, consolida los datos en:
 * - Promedio de 10 minutos
 * - Promedio de 1 hora (si se completan 6 bloques de 10 minutos)
 * - Promedio de 24 horas (si se completan 24 bloques de 1 hora)
 *
 * Cada promedio se registra en microSD y se imprime vía UART.
 *
 * @param dt Puntero a la estructura `ds3231_time_t` con la hora actual.
 */
static void data_logger_check_time_averages(const ds3231_time_t * dt) {
    bool finalize10 = false;
    if (current_window.count > 0) {
        unsigned int diff = time_diff_seconds(&current_window.start_time, dt);
        if (diff >= 600 || is_10min_boundary(dt)) {
            finalize10 = true;
        }
    }

    if (finalize10) {
        TimeSyncedAverage avg10 = finalize_temporal_window();
        hourly_avgs[hourly_index % AVG10_PER_HOUR] = avg10.pm2_5_avg;
        hourly_index++;

        PMDataAveraged to_print = {avg10.pm2_5_avg, avg10.pm2_5_max, avg10.pm2_5_min,
                                   avg10.pm2_5_std};
        log_avg10_data(&to_print);
        save_temporal_average_to_csv(&avg10, "/AVG10/avg10.csv");

        if (hourly_index % AVG10_PER_HOUR == 0) {
            int count = AVG10_PER_HOUR;
            PMDataAveraged avg1h;
            avg1h.mean = calculateAverage(hourly_avgs, count);
            avg1h.max = findMaxValue(hourly_avgs, count);
            avg1h.min = findMinValue(hourly_avgs, count);
            avg1h.std = calculateStandardDeviation(hourly_avgs, count);

            log_avg1h_data(&avg1h);

            daily_avgs[daily_index % AVG1H_PER_DAY] = avg1h.mean;
            daily_index++;

            TimeSyncedAverage ta = {.timestamp = *dt,
                                    .pm2_5_avg = avg1h.mean,
                                    .sample_count = count,
                                    .pm2_5_min = avg1h.min,
                                    .pm2_5_max = avg1h.max,
                                    .pm2_5_std = avg1h.std};
            save_temporal_average_to_csv(&ta, "/AVG60/avg60.csv");

            if (daily_index % AVG1H_PER_DAY == 0) {
                int dcount = AVG1H_PER_DAY;
                PMDataAveraged avg24;
                avg24.mean = calculateAverage(daily_avgs, dcount);
                avg24.max = findMaxValue(daily_avgs, dcount);
                avg24.min = findMinValue(daily_avgs, dcount);
                avg24.std = calculateStandardDeviation(daily_avgs, dcount);

                log_avg24h_data(&avg24);

                TimeSyncedAverage da = {.timestamp = *dt,
                                        .pm2_5_avg = avg24.mean,
                                        .sample_count = dcount,
                                        .pm2_5_min = avg24.min,
                                        .pm2_5_max = avg24.max,
                                        .pm2_5_std = avg24.std};
                save_temporal_average_to_csv(&da, "/AVG24/avg24.csv");
            }
        }
    }
}

/**
 * @brief Función principal para análisis y registro periódico de PM2.5.
 *
 * Debe ser llamada con cada nueva muestra de PM2.5 obtenida.
 * Acumula la muestra en la ventana temporal actual y verifica si corresponde
 * consolidar estadísticas (10 min, 1 h, 24 h).
 *
 * @param pm25_actual Valor actual de PM2.5 medido.
 */
void proceso_analisis_periodico(float pm25_actual) {
    ds3231_time_t dt;
    if (!ds3231_get_datetime(&dt)) {
        return;
    }

    accumulate_sample_in_current_window(pm25_actual, &dt);
    data_logger_check_time_averages(&dt);
}

/**
 * @brief Inicializa el sistema de almacenamiento y crea directorios base si es necesario.
 *
 * Monta la tarjeta microSD, imprime el estado por UART y asegura que existan los
 * directorios `/AVG10`, `/AVG60` y `/AVG24` para almacenar promedios.
 *
 * @return `true` si la inicialización fue exitosa, `false` si hubo error al montar la SD.
 */
bool data_logger_init(void) {
    // Inicializar buffers
    FRESULT res = f_mount(&fs, "", 1);
    if (res != FR_OK) {
        print_fatfs_error(res); // ⬅️ nueva línea aquí
        sd_mounted = false;
        return false;
    }

    uart_print("[OK] microSD montada correctamente\r\n");
    ensure_avg_directories();
    sd_mounted = true;
    return true;
}

/**
 * @brief Almacena una medición puntual de un sensor en los buffers internos.
 *
 * Utiliza la hora actual del RTC para crear un timestamp, y guarda los valores
 * (PM, temperatura y humedad) en los buffers de alta frecuencia, hora y día.
 *
 * @param sensor_id   Identificador del sensor que realizó la medición.
 * @param valores     Concentraciones PM medidas por el sensor.
 * @param temperatura Temperatura en grados Celsius.
 * @param humedad     Humedad relativa en porcentaje.
 *
 * @return `true` si la medición fue almacenada exitosamente.
 */
bool data_logger_store_measurement(uint8_t sensor_id, ConcentracionesPM valores, float temperatura,
                                   float humedad) {
    char timestamp[32];
    MedicionMP nueva_medicion;

    // Obtener timestamp actual
    time_rtc_GetFormattedDateTime(timestamp, sizeof(timestamp));

    // Completar estructura de medición
    strcpy(nueva_medicion.timestamp, timestamp);
    nueva_medicion.sensor_id = sensor_id;
    nueva_medicion.valores = valores;
    nueva_medicion.temperatura = temperatura;
    nueva_medicion.humedad = humedad;

    // Almacenar en buffer de alta frecuencia
    buffer_circular_agregar(&buffer_alta_frecuencia, &nueva_medicion);

    // También almacenar en los buffers de hora y día
    buffer_circular_agregar(&buffer_hora, &nueva_medicion);
    buffer_circular_agregar(&buffer_dia, &nueva_medicion);

    return true;
}
/**
 * @brief Calcula el promedio de PM2.5 a partir de las últimas N mediciones disponibles.
 *
 * Esta función recorre el buffer circular de alta frecuencia para calcular
 * el valor promedio de PM2.5, opcionalmente filtrando por sensor_id.
 *
 * @param sensor_id ID del sensor a considerar (0 = todos los sensores).
 * @param num_mediciones Número de muestras más recientes a promediar.
 * @return Promedio calculado de PM2.5 o 0 si no hay datos válidos.
 */

float data_logger_get_average_pm25(uint8_t sensor_id, uint32_t num_mediciones) {
    float suma = 0.0f;
    uint32_t contador = 0;

    // Limitar la cantidad de mediciones a usar
    if (num_mediciones > buffer_alta_frecuencia.cantidad) {
        num_mediciones = buffer_alta_frecuencia.cantidad;
    }

    // Si no hay mediciones, retornar 0
    if (num_mediciones == 0) {
        return 0.0f;
    }

    // Calcular promedio de las últimas 'num_mediciones'
    for (uint32_t i = 0; i < num_mediciones; i++) {
        uint32_t indice =
            (buffer_alta_frecuencia.inicio + buffer_alta_frecuencia.cantidad - i - 1) %
            buffer_alta_frecuencia.capacidad;

        // Filtrar por sensor_id si es necesario
        if (sensor_id == 0 || buffer_alta_frecuencia.datos[indice].sensor_id == sensor_id) {
            suma += buffer_alta_frecuencia.datos[indice].valores.pm2_5;
            contador++;
        }
    }

    // Retornar promedio o 0 si no hay datos
    return (contador > 0) ? (suma / contador) : 0.0f;
}

/**
 * @brief Imprime un resumen por UART del estado actual de los buffers de datos.
 *
 * Muestra:
 * - Cantidad de muestras almacenadas en cada buffer (alta frecuencia, horario, diario).
 * - Las 3 últimas mediciones almacenadas con su timestamp, sensor ID y PM2.5.
 */
void data_logger_print_summary() {
    char buffer[256];

    // Imprimir encabezado
    snprintf(buffer, sizeof(buffer),
             "\n--- Resumen de Datos Almacenados ---\n"
             "Buffer alta frecuencia: %lu/%lu muestras\n"
             "Buffer horario: %lu/%lu muestras\n"
             "Buffer diario: %lu/%lu muestras\n",
             buffer_alta_frecuencia.cantidad, buffer_alta_frecuencia.capacidad,
             buffer_hora.cantidad, buffer_hora.capacidad, buffer_dia.cantidad,
             buffer_dia.capacidad);

    uart_print("%s", buffer);

    // Imprimir últimas mediciones si hay datos
    if (buffer_alta_frecuencia.cantidad > 0) {
        uart_print("\nÚltimas 3 mediciones:\n");

        for (uint32_t i = 0; i < 3 && i < buffer_alta_frecuencia.cantidad; i++) {
            uint32_t indice =
                (buffer_alta_frecuencia.inicio + buffer_alta_frecuencia.cantidad - i - 1) %
                buffer_alta_frecuencia.capacidad;

            MedicionMP * medicion = &buffer_alta_frecuencia.datos[indice];

            snprintf(buffer, sizeof(buffer), "[%s] Sensor %d: PM2.5=%.2f ug/m3\n",
                     medicion->timestamp, medicion->sensor_id, medicion->valores.pm2_5);

            uart_print("%s", buffer);
        }
    }
}

/**
 * @brief Guarda una línea con promedios multivariable en un archivo CSV con timestamp único.
 *
 * Esta función crea un archivo individual para cada promedio usando la hora actual
 * como parte del nombre de archivo. Registra: PM1.0, PM2.5, PM4.0, PM10, temperatura, humedad.
 *
 * @param pm1_0  Promedio de PM1.0
 * @param pm2_5  Promedio de PM2.5
 * @param pm4_0  Promedio de PM4.0
 * @param pm10   Promedio de PM10
 * @param temp   Temperatura ambiente promedio
 * @param hum    Humedad relativa promedio
 * @return Código `FRESULT` del sistema de archivos FatFs.
 */
FRESULT guardar_promedio_csv(float pm1_0, float pm2_5, float pm4_0, float pm10, float temp,
                             float hum) {
    FIL archivo;
    FRESULT res;
    char nombre_archivo[64];
    char linea[128];
    char timestamp[32];

    // Obtener fecha/hora actual para el nombre del archivo
    time_rtc_GetFormattedDateTime(timestamp, sizeof(timestamp));
    snprintf(nombre_archivo, sizeof(nombre_archivo), "/%s_avg10min.csv",
             timestamp); // p.ej: /20250518T134000_avg10min.csv

    // Abrir archivo en modo append o crear si no existe
    res = f_open(&archivo, nombre_archivo, FA_OPEN_APPEND | FA_WRITE);
    if (res != FR_OK) {
        uart_print("Error abriendo archivo: %s\r\n", nombre_archivo);
        return res;
    }

    // Formato: timestamp, PM1.0, PM2.5, PM4.0, PM10, Temp, Hum
    snprintf(linea, sizeof(linea), "%s,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\r\n", timestamp, pm1_0, pm2_5,
             pm4_0, pm10, temp, hum);

    UINT escritos;
    res = f_write(&archivo, linea, strlen(linea), &escritos);
    f_sync(&archivo);
    f_close(&archivo);

    if (res == FR_OK && escritos > 0) {
        uart_print("Promedio guardado: %s", linea);
    } else {
        uart_print("Error escribiendo promedio a SD\r\n");
    }

    if (f_size(&archivo) == 0) {
        const char * encabezado = "timestamp,PM1.0,PM2.5,PM4.0,PM10,temp,humidity\n";
        f_write(&archivo, encabezado, strlen(encabezado), &escritos);
    }

    return res;
}

/**
 * @brief Formatea una estructura de datos como línea CSV
 *
 * @param data Puntero a estructura con los datos
 * @param csv_line Cadena de salida donde se almacenará la línea CSV
 * @param max_len Tamaño máximo del búfer de salida
 * @return true si el formateo fue exitoso, false si hubo error de espacio
 */
bool format_csv_line(const ParticulateData * data, char * csv_line, size_t max_len) {
    char timestamp[32];
    build_iso8601_timestamp(timestamp, sizeof(timestamp), data);

    int written =
        snprintf(csv_line, max_len, "%s,%d,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f\n", timestamp,
                 data->sensor_id, data->pm1_0, data->pm2_5, data->pm4_0, data->pm10, data->temp_amb,
                 data->hum_amb, data->temp_cam, data->hum_cam);

    return (written > 0 && (size_t)written < max_len);
}

/**
 * @brief Convierte una estructura `ParticulateData` en una línea CSV con timestamp.
 *
 * Esta función genera una cadena con formato CSV que incluye:
 * timestamp ISO8601, sensor_id, concentraciones PM, temperatura y humedad.
 *
 * @param data      Estructura con los datos a registrar.
 * @param csv_line  Buffer de salida para la línea formateada.
 * @param max_len   Longitud máxima del buffer.
 * @return `true` si la línea fue generada correctamente, `false` si hubo error de espacio.
 */
bool build_csv_filepath_from_datetime(char * filepath, size_t max_len) {
    ds3231_time_t dt;

    if (!ds3231_get_datetime(&dt)) {
        uart_print("Error: No se pudo leer el DS3231\r\n");
        return false;
    }

    char debug[64];
    snprintf(debug, sizeof(debug), "RTC DS3231: %04d/%02d/%02d %02d:%02d:%02d\r\n", dt.year,
             dt.month, dt.day, dt.hour, dt.min, dt.sec);
    uart_print(debug);

    int written = snprintf(filepath, max_len, "/%04d/%02d/%02d/DATA_%02d%02d%02d.CSV", dt.year,
                           dt.month, dt.day, dt.hour, dt.min, dt.sec);

    return (written > 0 && (size_t)written < max_len);
}

/**
 * @brief Crea la estructura de carpetas `/YYYY/MM/DD/` en la microSD si no existe.
 *
 * Esta función asegura que existan los directorios anidados según fecha actual.
 *
 * @param dt Puntero a la fecha actual obtenida del RTC.
 * @return `true` si todos los directorios fueron creados o ya existían.
 */
bool crear_directorio_fecha(const ds3231_time_t * dt) {
    char dirpath[64];

    snprintf(dirpath, sizeof(dirpath), "/%04d", dt->year);
    if (f_mkdir(dirpath) != FR_OK && f_stat(dirpath, NULL) != FR_OK)
        return false;

    snprintf(dirpath, sizeof(dirpath), "/%04d/%02d", dt->year, dt->month);
    if (f_mkdir(dirpath) != FR_OK && f_stat(dirpath, NULL) != FR_OK)
        return false;

    snprintf(dirpath, sizeof(dirpath), "/%04d/%02d/%02d", dt->year, dt->month, dt->day);
    if (f_mkdir(dirpath) != FR_OK && f_stat(dirpath, NULL) != FR_OK)
        return false;

    return true;
}

/**
 * @brief Construye una ruta absoluta hacia un archivo específico dentro de la carpeta de fecha.
 *
 * Formato resultante: `/YYYY/MM/DD/<nombre_archivo>`.
 *
 * @param dt              Fecha base para construir la ruta.
 * @param nombre_archivo  Nombre del archivo (ej. `"RAW.csv"`).
 * @param filepath        Buffer de salida con la ruta completa.
 * @param len             Tamaño máximo del buffer.
 * @return `true` si la ruta fue construida correctamente.
 */
bool obtener_ruta_archivo(const ds3231_time_t * dt, const char * nombre_archivo, char * filepath,
                          size_t len) {
    int n =
        snprintf(filepath, len, "/%04d/%02d/%02d/%s", dt->year, dt->month, dt->day, nombre_archivo);
    return n > 0 && (size_t)n < len;
}

/**
 * @brief Escribe una línea de texto al final de un archivo CSV existente o nuevo.
 *
 * Abre (o crea) el archivo y posiciona el puntero al final para agregar la línea.
 *
 * @param filepath Ruta completa del archivo donde escribir.
 * @param linea    Cadena de texto a escribir.
 * @return `true` si la operación fue exitosa, `false` en caso de error.
 */

bool escribir_linea_csv(const char * filepath, const char * linea) {
    FIL file;
    FRESULT res = f_open(&file, filepath, FA_OPEN_ALWAYS | FA_WRITE);
    if (res != FR_OK)
        return false;

    f_lseek(&file, f_size(&file));

    UINT written;
    res = f_write(&file, linea, strlen(linea), &written);
    f_close(&file);

    return (res == FR_OK && written == strlen(linea));
}

/**
 * @brief Guarda una medición cruda de PM en la microSD con timestamp y estructura de carpetas.
 *
 * Crea la ruta `/YYYY/MM/DD/RAW.csv`, escribe encabezado si el archivo es nuevo
 * y registra los datos en formato CSV.
 *
 * @param data Puntero a la estructura `ParticulateData` con los datos crudos.
 * @return `true` si la operación fue exitosa.
 */
bool log_data_to_sd(const ParticulateData * data) {
    if (!sd_mounted) {
        if (!data_logger_init())
            return false;
    }

    ds3231_time_t dt;
    if (!ds3231_get_datetime(&dt)) {
        uart_print("Error al obtener la hora del DS3231\r\n");
        return false;
    }

    if (!crear_directorio_fecha(&dt)) {
        uart_print("Error al crear carpetas por fecha\r\n");
        return false;
    }

    char filepath[64];
    if (!obtener_ruta_archivo(&dt, "RAW.csv", filepath, sizeof(filepath))) {
        uart_print("Error al generar nombre de archivo\r\n");
        return false;
    }

    char csv_line[CSV_LINE_BUFFER_SIZE];
    if (!format_csv_line(data, csv_line, sizeof(csv_line))) {
        uart_print("Error al generar línea CSV\r\n");
        return false;
    }

    if (!escribir_linea_csv(filepath, csv_line)) {
        uart_print("Fallo al escribir en microSD\r\n");
        return false;
    }

    uart_print("Línea escrita en SD:\r\n");
    uart_print(csv_line);
    return true;
}

/**
 * @brief Escribe una medición formateada como línea CSV en una ruta generada automáticamente.
 *
 * Utiliza `build_csv_filepath_from_datetime()` para generar el nombre del archivo.
 *
 * @param data Estructura con los datos a registrar.
 * @return `true` si la línea fue escrita correctamente.
 */

bool data_logger_write_csv_line(const ParticulateData * data) {
    char line[CSV_LINE_BUFFER_SIZE];

    if (!format_csv_line(data, line, sizeof(line))) {
        uart_print("Error formateando línea CSV\r\n");
        return false;
    }

    // Ruta de archivo
    char path[128];
    if (!build_csv_filepath_from_datetime(path, sizeof(path))) {
        uart_print("Error generando ruta de archivo CSV\r\n");
        return false;
    }

    // Crear carpeta si es necesario
    f_mkdir(path); // o f_mkdir_recursive si la tienes implementada

    FIL file;
    FRESULT res = f_open(&file, path, FA_OPEN_APPEND | FA_WRITE);
    if (res != FR_OK) {
        uart_print("No se pudo abrir el archivo CSV\r\n");
        return false;
    }

    UINT bytes_written;
    f_write(&file, line, strlen(line), &bytes_written);
    f_write(&file, "\r\n", 2, &bytes_written);
    f_close(&file);

    uart_print("Línea escrita correctamente en CSV\r\n");
    return true;
}

/**
 * @brief Almacena una medición cruda de un sensor, incluyendo metadatos y encabezado del archivo.
 *
 * Crea la carpeta `/YYYY/MM/DD` si no existe y un archivo individual por sensor y fecha
 * (`RAW_<ID>_YYYYMMDD.CSV`). Agrega encabezado detallado con información del sensor.
 *
 * @param data Puntero a la estructura `ParticulateData` con los datos medidos.
 * @return `true` si la línea fue escrita exitosamente.
 */
bool data_logger_store_raw(const ParticulateData * data) {
    if (!sd_mounted) {
        if (!data_logger_init())
            return false;
    }

    char filepath[128];
    char csv_line[CSV_LINE_BUFFER_SIZE];
    char dirpath[64];

    // Crear carpetas: /YYYY/MM/DD
    snprintf(dirpath, sizeof(dirpath), "/%04d", data->year);
    f_mkdir(dirpath);

    snprintf(dirpath, sizeof(dirpath), "/%04d/%02d", data->year, data->month);
    f_mkdir(dirpath);

    snprintf(dirpath, sizeof(dirpath), "/%04d/%02d/%02d", data->year, data->month, data->day);
    f_mkdir(dirpath);

    // Crear nombre del archivo
    snprintf(filepath, sizeof(filepath), "/%04d/%02d/%02d/RAW_%02d_%04d%02d%02d.CSV", data->year,
             data->month, data->day, data->sensor_id, data->year, data->month, data->day);

    // Abrir archivo
    FIL file;
    FRESULT res = f_open(&file, filepath, FA_OPEN_ALWAYS | FA_WRITE);
    if (res != FR_OK) {
        uart_print("No se pudo abrir archivo para escribir\r\n");
        print_fatfs_error(res);
        return false;
    }

    // Verificar si el archivo está vacío para agregar encabezado
    if (f_size(&file) == 0) {
        char header[512];
        snprintf(header, sizeof(header),
                 "# Sensor ID: %d\n"
                 "# Serial: %s\n"
                 "# Ubicación: %s\n"
                 "# Coordenadas: %s\n"
                 "# Unidades:\n"
                 "#  - PM1.0, PM2.5, PM4.0, PM10 en ug/m3\n"
                 "#  - Temp_amb y Temp_cam en °C\n"
                 "#  - Hum_amb y Hum_cam en %%RH\n"
                 "# Formato:\n"
                 "#  timestamp, sensor_id, pm1.0, pm2.5, pm4.0, pm10, temp_amb, hum_amb, temp_cam, "
                 "hum_cam\n",
                 data->sensor_id, sensor_metadata[data->sensor_id - 1].serial_number,
                 sensor_metadata[data->sensor_id - 1].location_name, LOCATION_COORDS);

        UINT bw_header;
        f_write(&file, header, strlen(header), &bw_header);
    }

    // Cerrar archivo después de escribir cabecera
    f_close(&file);

    // Crear línea CSV
    if (!format_csv_line(data, csv_line, sizeof(csv_line))) {
        uart_print("Error al generar línea CSV\r\n");
        return false;
    }

    // Escribir la línea CSV usando utilidad
    bool ok = microSD_appendLineAbsolute(filepath, csv_line);

    if (ok) {
        uart_print("RAW escrito: ");
        uart_print(csv_line);
    } else {
        uart_print("Fallo al escribir en RAW\r\n");
    }

    return ok;
}

/**
 * @brief Construye una cadena con formato de timestamp ISO8601 a partir de una estructura de datos.
 *
 * Formato: `YYYY-MM-DDTHH:MM:SSZ`
 *
 * @param buffer Buffer de salida para el timestamp.
 * @param len    Longitud máxima del buffer.
 * @param data   Datos con la fecha y hora a convertir.
 */

void build_iso8601_timestamp(char * buffer, size_t len, const ParticulateData * data) {
    snprintf(buffer, len, "%04u-%02u-%02uT%02u:%02u:%02uZ", data->year, data->month, data->day,
             data->hour, data->min, data->sec);
}

/* === Función principal: cálculo periódico basado en RTC ===================================== */

#if defined(UNIT_TEST) || defined(UNIT_TESTING)
BufferCircular * get_buffer_high_freq(void) {
    return &buffer_alta_frecuencia;
}
BufferCircular * get_buffer_hourly(void) {
    return &buffer_hora;
}
BufferCircular * get_buffer_daily(void) {
    return &buffer_dia;
}
TimeWindow * get_time_window(void) {
    return &current_window;
}
float * get_avg1h_buffer(void) {
    return hourly_avgs;
}
float * get_avg24h_buffer(void) {
    return daily_avgs;
}
int get_hourly_index(void) {
    return hourly_index;
}
int get_daily_index(void) {
    return daily_index;
}
#endif

/* === End of documentation ==================================================================== */
