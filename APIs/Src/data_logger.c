/*
 * Nombre del archivo: data_logger.c
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
/** @file
 ** @brief
 **/

/* === Headers files inclusions =============================================================== */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "fatfs.h"
#include "fatfs_sd.h"
#include "microSD.h"

#include "rtc.h"
#include "data_logger.h"
#include "time_rtc.h"

#include "uart.h"
#include "ff.h"

#include "rtc_ds3231_for_stm32_hal.h"

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

/* === Public function implementation ========================================================== */

bool data_logger_init(void) {
    // Inicializar buffers
    FRESULT res = f_mount(&fs, "", 1);
    if (res != FR_OK) {
        print_fatfs_error(res); // ⬅️ nueva línea aquí
        sd_mounted = false;
        return false;
    }

    uart_print("[OK] microSD montada correctamente\r\n");
    sd_mounted = true;
    return true;
}

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

    // Actualizar buffers de hora y día según corresponda
    // (implementación pendiente)

    return true;
}

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
    int written = snprintf(csv_line, max_len, "%s,%d,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f\n",
                           data->timestamp, data->sensor_id, data->pm1_0, data->pm2_5, data->pm4_0,
                           data->pm10, data->temp, data->hum);

    return (written > 0 && (size_t)written < max_len);
}

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

bool log_data_to_sd(const ParticulateData * data) {

    if (!sd_mounted) {
        if (!data_logger_init())
            return false;
    }

    FRESULT res;
    FIL file;
    char filepath[64];
    char csv_line[CSV_LINE_BUFFER_SIZE];
    char dirpath[32];
    ds3231_time_t dt;

    if (!ds3231_get_datetime(&dt)) {
        uart_print("Error al obtener la hora del DS3231\r\n");
        return false;
    }

    // Crear ruta de directorio
    snprintf(dirpath, sizeof(dirpath), "/%04d", dt.year);
    f_mkdir(dirpath);

    snprintf(dirpath, sizeof(dirpath), "/%04d/%02d", dt.year, dt.month);
    f_mkdir(dirpath);

    snprintf(dirpath, sizeof(dirpath), "/%04d/%02d/%02d", dt.year, dt.month, dt.day);
    f_mkdir(dirpath);

    // Ruta final de archivo
    snprintf(filepath, sizeof(filepath), "/%04d/%02d/%02d/DATA_%02d%02d%02d.CSV", dt.year, dt.month,
             dt.day, dt.hour, dt.min, dt.sec);

    // Generar línea CSV
    if (!format_csv_line(data, csv_line, sizeof(csv_line))) {
        uart_print("Error al generar línea CSV\r\n");
        return false;
    }

    // Abrir archivo
    res = f_open(&file, filepath, FA_OPEN_ALWAYS | FA_WRITE);
    if (res != FR_OK) {
        uart_print("No se pudo abrir archivo para escribir\r\n");
        return false;
    }

    // Mover al final
    f_lseek(&file, f_size(&file));

    // Escribir
    UINT written;
    res = f_write(&file, csv_line, strlen(csv_line), &written);
    if (res != FR_OK || written != strlen(csv_line)) {
        uart_print("Fallo al escribir en microSD\r\n");
        f_close(&file);
        return false;
    }

    f_close(&file);
    uart_print("Línea escrita en SD:\r\n");
    uart_print(csv_line);
    return true;
}

void print_fatfs_error(FRESULT res) {
    char msg[64];
    snprintf(msg, sizeof(msg), "f_mount() error code: %d\r\n", res);
    uart_print(msg);

    switch (res) {
    case FR_OK:
        uart_print("✔ FR_OK: Operación exitosa\r\n");
        break;
    case FR_DISK_ERR:
        uart_print("❌ FR_DISK_ERR: Error físico en el disco\r\n");
        break;
    case FR_INT_ERR:
        uart_print("❌ FR_INT_ERR: Error interno de FatFs\r\n");
        break;
    case FR_NOT_READY:
        uart_print("❌ FR_NOT_READY: Disco no está listo\r\n");
        break;
    case FR_NO_FILE:
        uart_print("❌ FR_NO_FILE: Archivo no encontrado\r\n");
        break;
    case FR_NO_PATH:
        uart_print("❌ FR_NO_PATH: Ruta no encontrada\r\n");
        break;
    case FR_INVALID_NAME:
        uart_print("❌ FR_INVALID_NAME: Nombre inválido\r\n");
        break;
    case FR_DENIED:
        uart_print("❌ FR_DENIED: Acceso denegado\r\n");
        break;
    case FR_EXIST:
        uart_print("❌ FR_EXIST: Archivo ya existe\r\n");
        break;
    case FR_INVALID_OBJECT:
        uart_print("❌ FR_INVALID_OBJECT: Objeto inválido\r\n");
        break;
    case FR_WRITE_PROTECTED:
        uart_print("❌ FR_WRITE_PROTECTED: Tarjeta protegida contra escritura\r\n");
        break;
    case FR_INVALID_DRIVE:
        uart_print("❌ FR_INVALID_DRIVE: Unidad inválida\r\n");
        break;
    case FR_NOT_ENABLED:
        uart_print("❌ FR_NOT_ENABLED: FatFs no está habilitado\r\n");
        break;
    case FR_NO_FILESYSTEM:
        uart_print("❌ FR_NO_FILESYSTEM: No hay sistema de archivos FAT válido\r\n");
        break;
    case FR_TIMEOUT:
        uart_print("❌ FR_TIMEOUT: Timeout de acceso\r\n");
        break;
    case FR_LOCKED:
        uart_print("❌ FR_LOCKED: El archivo está bloqueado\r\n");
        break;
    default:
        uart_print("❌ Código de error desconocido\r\n");
        break;
    }
}

bool data_logger_write_csv_line(const ParticulateData * data) {
    char line[CSV_LINE_BUFFER_SIZE];

    if (!format_csv_line(data, line, sizeof(line))) {
        uart_print("❌ Error formateando línea CSV\r\n");
        return false;
    }

    // Ruta de archivo
    char path[128];
    if (!build_csv_filepath_from_datetime(path, sizeof(path))) {
        uart_print("❌ Error generando ruta de archivo CSV\r\n");
        return false;
    }

    // Crear carpeta si es necesario
    f_mkdir(path); // o f_mkdir_recursive si la tienes implementada

    FIL file;
    FRESULT res = f_open(&file, path, FA_OPEN_APPEND | FA_WRITE);
    if (res != FR_OK) {
        uart_print("❌ No se pudo abrir el archivo CSV\r\n");
        return false;
    }

    UINT bytes_written;
    f_write(&file, line, strlen(line), &bytes_written);
    f_write(&file, "\r\n", 2, &bytes_written);
    f_close(&file);

    uart_print("✅ Línea escrita correctamente en CSV\r\n");
    return true;
}

/* === End of documentation ==================================================================== */
