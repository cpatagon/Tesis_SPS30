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

#include "config_global.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "fatfs.h"
#include "fatfs_sd.h"
#include "microSD.h"
#include "microSD_utils.h"

#include "rtc.h"
#include "data_logger.h"
#include "time_rtc.h"
#include "sensor.h"
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

BufferCircularSensor buffers_10min[MAX_SENSORES_SPS30];

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

/*
static bool is_1hour_boundary(const ds3231_time_t * dt) {
    return time_diff_seconds(&hourly_start_time, dt) >= 60 * 60;
}*/

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
/*
static bool is_24hour_boundary(const ds3231_time_t * dt) {
    return time_diff_seconds(&daily_start_time, dt) >= 24 * 3600;
}*/

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

/*
void log_avg10_data(const PMDataAveraged * avg) {
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "%.2f,%.2f,%.2f,%.2f\n", avg->mean, avg->max, avg->min,
             avg->std);

    microSD_appendLineAbsolute("/AVG10/avg10.csv", buffer);

    uart_print("[PROMEDIO 10min] PM2.5 -> media: %.2f, max: %.2f, min: %.2f, std: %.2f\r\n",
               avg->mean, avg->max, avg->min, avg->std);
}*/

/**
 * @brief Registra en la microSD los datos promediados de PM2.5 cada 1 hora.
 *
 * Escribe una línea en el archivo `/AVG60/avg60.csv` con los valores estadísticos
 * del promedio horario de PM2.5. También imprime el resumen por UART.
 *
 * @param avg Puntero a la estructura `PMDataAveraged` con los datos a registrar.
 */

/*
void log_avg1h_data(const PMDataAveraged * avg) {
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "%.2f,%.2f,%.2f,%.2f\n", avg->mean, avg->max, avg->min,
             avg->std);

    microSD_appendLineAbsolute("/AVG60/avg60.csv", buffer);

    uart_print("[PROMEDIO 1h] PM2.5 -> media: %.2f, max: %.2f, min: %.2f, std: %.2f\r\n", avg->mean,
               avg->max, avg->min, avg->std);
}*/

/**
 * @brief Registra en la microSD los datos promediados de PM2.5 cada 24 horas.
 *
 * Escribe una línea en el archivo `/AVG24/avg24.csv` con los valores estadísticos
 * del promedio diario de PM2.5. También imprime el resumen por UART.
 *
 * @param avg Puntero a la estructura `PMDataAveraged` con los datos a registrar.
 */

/*
void log_avg24h_data(const PMDataAveraged * avg) {
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "%.2f,%.2f,%.2f,%.2f\n", avg->mean, avg->max, avg->min,
             avg->std);

    microSD_appendLineAbsolute("/AVG24/avg24.csv", buffer);

    uart_print("[PROMEDIO 24h] PM2.5 -> media: %.2f, max: %.2f, min: %.2f, std: %.2f\r\n",
               avg->mean, avg->max, avg->min, avg->std);
}
*/

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
static void registrar_promedio_10min(const TimeSyncedAverage * avg10) {
    EstadisticaPM25 resumen = {.sensor_id = 0, // promedio general
                               .year = avg10->timestamp.year,
                               .month = avg10->timestamp.month,
                               .day = avg10->timestamp.day,
                               .hour = avg10->timestamp.hour,
                               .min = avg10->timestamp.min,
                               .sec = avg10->timestamp.sec,
                               .bloque_10min = (avg10->timestamp.min / 10),
                               .pm2_5_promedio = avg10->pm2_5_avg,
                               .pm2_5_min = avg10->pm2_5_min,
                               .pm2_5_max = avg10->pm2_5_max,
                               .pm2_5_std = avg10->pm2_5_std,
                               .num_validos = avg10->sample_count};

    data_logger_store_avg10_csv(&resumen);
    uart_print("[AVG10] PM2.5 = %.2f ug/m3 (%u muestras)\r\n", resumen.pm2_5_promedio,
               resumen.num_validos);
}

static void registrar_promedio_1h(const ds3231_time_t * dt) {
    const int count = AVG10_PER_HOUR;
    float promedio = calculateAverage(hourly_avgs, count);
    float min = findMinValue(hourly_avgs, count);
    float max = findMaxValue(hourly_avgs, count);
    float std = calculateStandardDeviation(hourly_avgs, count);

    TimeSyncedAverage avg1h = {.timestamp = *dt,
                               .pm2_5_avg = promedio,
                               .sample_count = count,
                               .pm2_5_min = min,
                               .pm2_5_max = max,
                               .pm2_5_std = std};

    save_temporal_average_to_csv(&avg1h, "/AVG60/avg60.csv");
    uart_print("[AVG60] PM2.5 = %.2f ug/m3\r\n", promedio);

    daily_avgs[daily_index % AVG1H_PER_DAY] = promedio;
    daily_index++;

    if (daily_index % AVG1H_PER_DAY == 0) {
        registrar_promedio_24h(dt);
    }
}

void registrar_promedio_24h(const ds3231_time_t * dt) {
    const int count = AVG1H_PER_DAY;
    float promedio = calculateAverage(daily_avgs, count);
    float min = findMinValue(daily_avgs, count);
    float max = findMaxValue(daily_avgs, count);
    float std = calculateStandardDeviation(daily_avgs, count);

    TimeSyncedAverage avg24 = {.timestamp = *dt,
                               .pm2_5_avg = promedio,
                               .sample_count = count,
                               .pm2_5_min = min,
                               .pm2_5_max = max,
                               .pm2_5_std = std};

    save_temporal_average_to_csv(&avg24, "/AVG24/avg24.csv");
    uart_print("[AVG24] PM2.5 = %.2f ug/m3\r\n", promedio);
}

static void data_logger_check_time_averages(const ds3231_time_t * dt) {
    if (current_window.count == 0)
        return;

    if (time_diff_seconds(&current_window.start_time, dt) < 600 && !is_10min_boundary(dt))
        return;

    TimeSyncedAverage avg10 = finalize_temporal_window();
    hourly_avgs[hourly_index % AVG10_PER_HOUR] = avg10.pm2_5_avg;
    hourly_index++;

    registrar_promedio_10min(&avg10);

    if (hourly_index % AVG10_PER_HOUR == 0) {
        registrar_promedio_1h(dt);
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
bool data_logger_store_measurement(uint8_t sensor_id, ConcentracionesPM valores, float temp_amb,
                                   float hum_amb) {
    ds3231_time_t now;
    // if (!rtc_ds3231_get_time(&now)) return false;
    if (!ds3231_get_datetime(&now))
        return false;

    MedicionMP nueva;

    nueva.timestamp = now;
    nueva.bloque_10min = now.min / 10;
    nueva.sensor_id = sensor_id;

    nueva.pm1_0 = valores.pm1_0;
    nueva.pm2_5 = valores.pm2_5;
    nueva.pm4_0 = valores.pm4_0;
    nueva.pm10 = valores.pm10;

    nueva.temp_amb = temp_amb;
    nueva.hum_amb = hum_amb;
    nueva.temp_cam = 0.0f; // opcional si aún no se mide
    nueva.hum_cam = 0.0f;

    buffer_circular_agregar(&buffer_alta_frecuencia, &nueva);
    buffer_circular_agregar(&buffer_hora, &nueva);
    buffer_circular_agregar(&buffer_dia, &nueva);

    return true;
}
/**
 * @brief Calcula el promedio de PM2.5 para un sensor específico.
 *
 * Recorre las últimas `num_mediciones` almacenadas en el buffer de alta frecuencia
 * y calcula el promedio de PM2.5 para el sensor especificado. Si `sensor_id == 0`,
 * incluye todos los sensores.
 *
 * @param sensor_id       ID lógico del sensor (0 para todos).
 * @param num_mediciones  Número máximo de mediciones a considerar.
 * @return Promedio PM2.5 calculado o 0.0f si no hay datos válidos.
 */
float data_logger_get_average_pm25_id(uint8_t sensor_id, uint32_t num_mediciones) {
    float suma = 0.0f;
    uint32_t contador = 0;

    // Asegurar que no se pidan más mediciones de las disponibles
    if (num_mediciones > buffer_alta_frecuencia.cantidad) {
        num_mediciones = buffer_alta_frecuencia.cantidad;
    }

    if (num_mediciones == 0) {
        return 0.0f;
    }

    // Recorrer desde la última medición hacia atrás
    for (uint32_t i = 0; i < num_mediciones; i++) {
        uint32_t indice =
            (buffer_alta_frecuencia.inicio + buffer_alta_frecuencia.cantidad - i - 1) %
            buffer_alta_frecuencia.capacidad;

        MedicionMP * m = &buffer_alta_frecuencia.datos[indice];

        if (sensor_id == 0 || m->sensor_id == sensor_id) {
            suma += m->pm2_5;
            contador++;
        }
    }

    return (contador > 0) ? (suma / contador) : 0.0f;
}

#include <math.h>
#include <string.h> // para memset si se quiere limpiar antes

/**
 * @brief Calcula estadísticas combinadas de PM2.5 para los buffers de 10 minutos de todos los
 * sensores.
 *
 * @param buffers Arreglo de buffers circulares por sensor (uno por cada SPS30).
 * @param resultado Puntero a estructura donde se almacenará el resultado estadístico.
 * @return true si se calcularon datos válidos, false si no hubo mediciones.
 */
bool data_logger_estadistica_10min_pm25(const BufferCircularSensor * buffers,
                                        EstadisticaPM25 * resultado) {
    if (!buffers || !resultado)
        return false;

    float suma = 0.0f, min = 10000.0f, max = -10000.0f;
    float valores[BUFFER_10MIN_SIZE * MAX_SENSORES_SPS30];
    uint16_t n = 0;

    for (uint8_t i = 0; i < MAX_SENSORES_SPS30; ++i) {
        const BufferCircularSensor * buf = &buffers[i];
        if (buf->count == 0)
            continue;

        for (uint8_t j = 0; j < buf->count; ++j) {
            uint8_t idx = (buf->head + j) % BUFFER_10MIN_SIZE;
            float val = buf->buffer[idx].pm2_5;

            valores[n++] = val;
            suma += val;
            if (val < min)
                min = val;
            if (val > max)
                max = val;
        }
    }

    if (n == 0)
        return false;

    float promedio = suma / n;

    float suma_cuadrados = 0.0f;
    for (uint16_t i = 0; i < n; ++i) {
        float diff = valores[i] - promedio;
        suma_cuadrados += diff * diff;
    }
    float stddev = (n > 1) ? sqrtf(suma_cuadrados / (n - 1)) : 0.0f;

    // Buscar última muestra válida para obtener el timestamp
    for (int8_t i = MAX_SENSORES_SPS30 - 1; i >= 0; --i) {
        const BufferCircularSensor * buf = &buffers[i];
        if (buf->count > 0) {
            uint8_t idx_ultimo = (buf->head + buf->count - 1) % BUFFER_10MIN_SIZE;
            const MedicionMP * muestra = &buf->buffer[idx_ultimo];

            *resultado = (EstadisticaPM25){.sensor_id = 0, // Combinado
                                           .year = muestra->timestamp.year,
                                           .month = muestra->timestamp.month,
                                           .day = muestra->timestamp.day,
                                           .hour = muestra->timestamp.hour,
                                           .min = muestra->timestamp.min,
                                           .sec = muestra->timestamp.sec,
                                           .bloque_10min = muestra->bloque_10min,
                                           .pm2_5_promedio = promedio,
                                           .pm2_5_min = min,
                                           .pm2_5_max = max,
                                           .pm2_5_std = stddev,
                                           .num_validos = n};
            return true;
        }
    }

    return false;
}

/**
 * @brief Imprime un resumen por UART del estado actual de los buffers de datos.
 *
 * Muestra:
 * - Cantidad de muestras almacenadas en cada buffer (alta frecuencia, horario, diario).
 * - Las 3 últimas mediciones almacenadas con su timestamp, sensor ID y PM2.5.
 */
void data_logger_print_summary(void) {
    char buffer[128];

    // Imprimir encabezado
    snprintf(buffer, sizeof(buffer),
             "\n--- Resumen de Datos Almacenados ---\n"
             "Buffer alta frecuencia: %u/%u muestras\n"
             "Buffer horario:         %u/%u muestras\n"
             "Buffer diario:          %u/%u muestras\n",
             (unsigned int)buffer_alta_frecuencia.cantidad,
             (unsigned int)buffer_alta_frecuencia.capacidad, (unsigned int)buffer_hora.cantidad,
             (unsigned int)buffer_hora.capacidad, (unsigned int)buffer_dia.cantidad,
             (unsigned int)buffer_dia.capacidad);
    uart_print("%s", buffer);

    // Mostrar las últimas 3 mediciones si hay datos
    if (buffer_alta_frecuencia.cantidad > 0) {
        uart_print("\nÚltimas 3 mediciones (PM2.5):\n");

        for (uint8_t i = 0; i < 3 && i < buffer_alta_frecuencia.cantidad; i++) {
            uint16_t idx =
                (buffer_alta_frecuencia.inicio + buffer_alta_frecuencia.cantidad - 1 - i) %
                buffer_alta_frecuencia.capacidad;

            MedicionMP * med = &buffer_alta_frecuencia.datos[idx];

            snprintf(buffer, sizeof(buffer),
                     "[%04u-%02u-%02u %02u:%02u:%02u] Sensor %d: %.2f µg/m³\n", med->timestamp.year,
                     med->timestamp.month, med->timestamp.day, med->timestamp.hour,
                     med->timestamp.min, med->timestamp.sec, med->sensor_id, med->pm2_5);
            uart_print("%s", buffer);
        }
    }
}

/**
 * @brief Imprime un resumen detallado por UART del estado de `buffer_alta_frecuencia`.
 */
void data_logger_print_value(void) {
    char buffer[256];
    const BufferCircular * buf = &buffer_alta_frecuencia;

    // Imprimir encabezado
    snprintf(buffer, sizeof(buffer),
             "\n--- Resumen de Datos Almacenados ---\n"
             "Buffer alta frecuencia: %u/%u muestras\n",
             buf->cantidad, buf->capacidad);
    uart_print("%s", buffer);

    // Mostrar últimas mediciones
    if (buf->cantidad > 0) {
        uart_print("\nÚltimas 3 mediciones:\n");

        for (uint8_t i = 0; i < 3 && i < buf->cantidad; i++) {
            uint16_t idx = (buf->inicio + buf->cantidad - 1 - i) % buf->capacidad;
            const MedicionMP * m = &buf->datos[idx];

            snprintf(buffer, sizeof(buffer),
                     "[%04u-%02u-%02u %02u:%02u:%02u] Sensor %u: PM2.5 = %.2f µg/m³\n",
                     m->timestamp.year, m->timestamp.month, m->timestamp.day, m->timestamp.hour,
                     m->timestamp.min, m->timestamp.sec, m->sensor_id, m->pm2_5);
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

bool data_logger_store_avg10_csv(const EstadisticaPM25 * data) {
    if (!data || !sd_mounted) {
        if (!data_logger_init())
            return false;
    }

    char filepath[128];
    char csv_line[256];
    char dirpath[64];

    // Crear carpetas
    snprintf(dirpath, sizeof(dirpath), "/%04d", data->year);
    f_mkdir(dirpath);

    snprintf(dirpath, sizeof(dirpath), "/%04d/%02d", data->year, data->month);
    f_mkdir(dirpath);

    snprintf(dirpath, sizeof(dirpath), "/%04d/%02d/%02d", data->year, data->month, data->day);
    f_mkdir(dirpath);

    // Nombre del archivo de promedios cada 10 min
    snprintf(filepath, sizeof(filepath), "/%04d/%02d/%02d/AVG10_%04d%02d%02d.CSV", data->year,
             data->month, data->day, data->year, data->month, data->day);

    // Abrir archivo
    FIL file;
    FRESULT res = f_open(&file, filepath, FA_OPEN_ALWAYS | FA_WRITE);
    if (res != FR_OK) {
        uart_print("[ERROR] No se pudo abrir AVG10 para escribir\r\n");
        print_fatfs_error(res);
        return false;
    }

    // Escribir encabezado si está vacío
    if (f_size(&file) == 0) {
        const char * header =
            "# Formato: timestamp, pm2.5_promedio, pm2.5_min, pm2.5_max, pm2.5_std, muestras\n";
        UINT bw;
        f_write(&file, header, strlen(header), &bw);
    }
    f_close(&file);

    // Construir línea CSV
    snprintf(csv_line, sizeof(csv_line), "%04d-%02d-%02d %02d:%02d:%02d,%.2f,%.2f,%.2f,%.2f,%u\r\n",
             data->year, data->month, data->day, data->hour, data->min, data->sec,
             data->pm2_5_promedio, data->pm2_5_min, data->pm2_5_max, data->pm2_5_std,
             data->num_validos);

    // Escribir línea CSV
    return microSD_appendLineAbsolute(filepath, csv_line);
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

/**
 * @brief Devuelve un puntero a la medición PM almacenada para un sensor específico.
 *
 * Esta función permite acceder directamente a una estructura `MedicionMP` desde el buffer
 * de alta frecuencia, usando un índice relativo dentro del subconjunto de datos del sensor.
 *
 * La función busca la posición lógica del índice solicitado en el buffer circular,
 * y verifica que el `sensor_id` de la medición coincida con el solicitado.
 *
 * @param sensor_id ID del sensor (0 a NUM_SENSORS_SPS30-1)
 * @param index Índice dentro de las mediciones válidas del sensor
 * @return Puntero a `MedicionMP` si existe una medición válida; `NULL` si no hay coincidencia.
 */

const MedicionMP * data_logger_get_medicion(uint8_t sensor_id, uint8_t index) {
    if (sensor_id >= NUM_SENSORS_SPS30 || index >= buffer_alta_frecuencia.cantidad) {
        return NULL;
    }

    uint32_t i = (buffer_alta_frecuencia.inicio + index) % buffer_alta_frecuencia.capacidad;
    const MedicionMP * m = &buffer_alta_frecuencia.datos[i];
    return (m->sensor_id == sensor_id) ? m : NULL;
}

/**
 * @brief Devuelve la cantidad de mediciones almacenadas para un sensor específico.
 *
 * Esta función recorre el buffer circular de alta frecuencia (`buffer_alta_frecuencia`)
 * y cuenta cuántas mediciones corresponden al sensor indicado por `sensor_id`.
 *
 * Es útil para acceder al número de muestras válidas disponibles para análisis estadístico.
 *
 * @param sensor_id ID del sensor (0 a NUM_SENSORS_SPS30-1)
 * @return Número de mediciones válidas asociadas al sensor; 0 si no hay datos o si el ID es
 * inválido.
 */

uint8_t data_logger_get_count(uint8_t sensor_id) {
    if (sensor_id >= NUM_SENSORS_SPS30) {
        return 0;
    }

    uint8_t count = 0;
    for (uint32_t i = 0; i < buffer_alta_frecuencia.cantidad; i++) {
        uint32_t idx = (buffer_alta_frecuencia.inicio + i) % buffer_alta_frecuencia.capacidad;
        if (buffer_alta_frecuencia.datos[idx].sensor_id == sensor_id) {
            count++;
        }
    }

    return count;
}

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

/**
 * @brief Guarda los datos del buffer general en los buffers por sensor.
 *
 * Esta función distribuye los datos de `buffer` en los buffers individuales
 * `buffers_10min` en función del `sensor_id`.
 *
 * @param buffer Puntero al buffer circular con datos a guardar.
 * @return true si al menos un dato se almacenó correctamente, false en caso de error.
 */
bool buffer_guardar(BufferCircular * buffer) {
    if (!buffer || buffer->cantidad == 0)
        return false;

    bool al_menos_uno_guardado = false;

    for (uint16_t i = 0; i < buffer->cantidad; ++i) {
        const MedicionMP * data = &buffer->datos[i];

        if (data->sensor_id == 0 || data->sensor_id > MAX_SENSORES_SPS30) {
            uart_print("[ERROR] buffer_guardar: ID de sensor fuera de rango (%d)\r\n",
                       data->sensor_id);
            continue;
        }

        uint8_t idx = data->sensor_id - 1;
        BufferCircularSensor * buf = &buffers_10min[idx];

        buf->buffer[buf->head] = *data;
        buf->head = (buf->head + 1) % BUFFER_10MIN_SIZE;

        if (buf->count < BUFFER_10MIN_SIZE) {
            buf->count++;
        } else {
            uart_print("[WARN] buffer_guardar: sobreescritura en buffer sensor %d\r\n",
                       data->sensor_id);
        }

        al_menos_uno_guardado = true;
    }

    // Opcional: limpiar el buffer de entrada luego de transferir
    buffer->cantidad = 0;
    buffer->inicio = 0;

    return al_menos_uno_guardado;
}
/*
bool data_logger_store_sensor_data(const SensorData * data) {
    if (!data)
        return false;

    if (data->sensor_id == 0 || data->sensor_id > MAX_SENSORES_SPS30) {
        uart_print("[ERROR] ID de sensor inválido: %d\r\n", data->sensor_id);
        return false;
    }

    uint8_t idx = data->sensor_id - 1;
    BufferCircularSensor * buf = &buffers_10min[idx];

    buf->buffer[buf->head] = *data;
    buf->head = (buf->head + 1) % BUFFER_10MIN_SIZE;

    if (buf->count < BUFFER_10MIN_SIZE) {
        buf->count++;
    } else {
        uart_print("[WARN] buffer sensor %d sobreescribiendo datos\r\n", data->sensor_id);
    }

    return true;
}*/

/**
 * @brief Guarda múltiples mediciones en los buffers por sensor.
 *
 * @param temp_data        Puntero al arreglo de mediciones (`MedicionMP`).
 * @param num_mediciones   Cantidad de elementos válidos en el arreglo.
 * @param buffers_destino  Arreglo de buffers circulares por sensor.
 * @return true si se almacenaron todas correctamente, false si hubo al menos un error.
 */
bool data_logger_store_sensor_data(const MedicionMP * temp_data, size_t num_mediciones,
                                   BufferCircularSensor * buffers_destino) {
    if (!temp_data || !buffers_destino || num_mediciones == 0) {
        uart_print("[ERROR] Parámetros inválidos en data_logger_store_sensor_data()\r\n");
        return false;
    }

    bool todo_ok = true;

    for (size_t i = 0; i < num_mediciones; ++i) {
        const MedicionMP * d = &temp_data[i];

        if (d->sensor_id == 0 || d->sensor_id > MAX_SENSORES_SPS30) {
            uart_print("[ERROR] ID de sensor inválido (%d) en medición #%u\r\n", d->sensor_id,
                       (unsigned)i);
            todo_ok = false;
            continue;
        }

        uint8_t idx = d->sensor_id - 1;
        BufferCircularSensor * dest = &buffers_destino[idx];

        dest->buffer[dest->head] = *d;
        dest->head = (dest->head + 1) % BUFFER_10MIN_SIZE;

        if (dest->count < BUFFER_10MIN_SIZE) {
            dest->count++;
        } else {
            uart_print("[WARN] Sobreescritura en buffer de sensor %d\r\n", d->sensor_id);
        }
    }

    return todo_ok;
}

/**
 * @brief Limpia todos los buffers circulares por sensor.
 *
 * @param buffers Arreglo de buffers, uno por sensor.
 */
void data_logger_buffer_limpiar_todos(BufferCircularSensor * buffers) {
    if (!buffers)
        return;

    for (uint8_t i = 0; i < MAX_SENSORES_SPS30; ++i) {
        buffers[i].head = 0;
        buffers[i].count = 0;
        memset(buffers[i].buffer, 0, sizeof(buffers[i].buffer));
    }
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
