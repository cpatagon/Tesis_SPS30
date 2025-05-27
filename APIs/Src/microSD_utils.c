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
#include "uart.h" // Para funciones de impresión/debug opcionales

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

/* === End of documentation ==================================================================== */
