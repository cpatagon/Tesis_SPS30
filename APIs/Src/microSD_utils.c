/*
 * Nombre del archivo: microSD_utils.c
 * Descripción: Funciones auxiliares para operaciones de escritura en archivos en la tarjeta
 * microSD. Autor: lgomez Creado en: 24-05-2025 Derechos de Autor: (C) 2023 CESE Licencia: GNU
 * General Public License v3.0
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
 ** @brief Funciones auxiliares para escritura de líneas en archivos ubicados en rutas absolutas.
 **/

/* === Headers files inclusions =============================================================== */

#include "ff.h"
#include <string.h>
#include <stdbool.h>
#include "microSD_utils.h"
#include "uart.h" // Para funciones de impresión/debug opcionales
#include <stdio.h>

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */

/**
 * @brief Agrega una línea al final de un archivo CSV dado por su ruta absoluta.
 *
 * @param filepath Ruta absoluta del archivo (ej. "/2025/05/24/RAW_20250524.CSV")
 * @param line Línea de texto a escribir (debe terminar en '\n' si se requiere)
 * @return true si se escribió correctamente, false si hubo error
 */

bool microSD_appendLineAbsolute(const char * filepath, const char * line) {
    FIL file;
    FRESULT res;
    UINT bw;

    res = f_open(&file, filepath, FA_OPEN_ALWAYS | FA_WRITE);
    if (res != FR_OK) {
        print_fatfs_error(res);
        return false;
    }

    // Verificar si el archivo está vacío para escribir encabezado
    if (f_size(&file) == 0) {
        const char * header = "timestamp,sensor_id,pm1.0,pm2.5,pm4.0,pm10,temp,hum\n";
        res = f_write(&file, header, strlen(header), &bw);
        if (res != FR_OK || bw != strlen(header)) {
            f_close(&file);
            uart_print("Error al escribir encabezado CSV\r\n");
            return false;
        }
    }

    // Ir al final del archivo para agregar línea
    res = f_lseek(&file, f_size(&file));
    if (res != FR_OK) {
        f_close(&file);
        print_fatfs_error(res);
        return false;
    }

    res = f_write(&file, line, strlen(line), &bw);
    f_close(&file);

    return (res == FR_OK && bw == strlen(line));
}

void print_fatfs_error(FRESULT res) {
    char msg[64];
    snprintf(msg, sizeof(msg), "f_mount() error code: %d\r\n", res);
    uart_print(msg);

    switch (res) {
    case FR_OK:
        uart_print("FR_OK: Operacion exitosa\r\n");
        break;
    case FR_DISK_ERR:
        uart_print("FR_DISK_ERR: Error fisico en el disco\r\n");
        break;
    case FR_INT_ERR:
        uart_print("FR_INT_ERR: Error interno de FatFs\r\n");
        break;
    case FR_NOT_READY:
        uart_print("FR_NOT_READY: Disco no esta listo\r\n");
        break;
    case FR_NO_FILE:
        uart_print("FR_NO_FILE: Archivo no encontrado\r\n");
        break;
    case FR_NO_PATH:
        uart_print("FR_NO_PATH: Ruta no encontrada\r\n");
        break;
    case FR_INVALID_NAME:
        uart_print("FR_INVALID_NAME: Nombre inválido\r\n");
        break;
    case FR_DENIED:
        uart_print("FR_DENIED: Acceso denegado\r\n");
        break;
    case FR_EXIST:
        uart_print("FR_EXIST: Archivo ya existe\r\n");
        break;
    case FR_INVALID_OBJECT:
        uart_print("FR_INVALID_OBJECT: Objeto invalido\r\n");
        break;
    case FR_WRITE_PROTECTED:
        uart_print("FR_WRITE_PROTECTED: Tarjeta protegida contra escritura\r\n");
        break;
    case FR_INVALID_DRIVE:
        uart_print("FR_INVALID_DRIVE: Unidad invalida\r\n");
        break;
    case FR_NOT_ENABLED:
        uart_print("FR_NOT_ENABLED: FatFs no esta habilitado\r\n");
        break;
    case FR_NO_FILESYSTEM:
        uart_print("FR_NO_FILESYSTEM: No hay sistema de archivos FAT valido\r\n");
        break;
    case FR_TIMEOUT:
        uart_print("FR_TIMEOUT: Timeout de acceso\r\n");
        break;
    case FR_LOCKED:
        uart_print("FR_LOCKED: El archivo esta bloqueado\r\n");
        break;
    default:
        uart_print("Codigo de error desconocido\r\n");
        break;
    }
}

/* === End of documentation ==================================================================== */
