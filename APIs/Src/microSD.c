/*
 * Nombre del archivo: microSD.c
 * Descripción: Implementación de funciones para la operación de una microSD
 * 				utilizando FatFs y UART en un sistema basado en STM32.
 * Autor: lgomez
 * Creado en: 12-06-2024 
 * Derechos de Autor: (C) 2023 lgomez
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
/** @file microSD.c
 *  @brief Implementación de funciones para la operación de una microSD.
 *
 *  Este archivo contiene las implementaciones de las funciones necesarias para
 *  inicializar, leer y escribir en una tarjeta microSD utilizando el sistema
 *  de archivos FatFs y la comunicación UART en un microcontrolador STM32.
 */

/* === Headers files inclusions =============================================================== */

#include "microSD.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "fatfs.h"
#include "usart.h"

/* === Macros definitions ====================================================================== */

#define SAFE_STRCPY(dest, src, size) do { \
    strncpy((dest), (src), (size) - 1); \
    (dest)[(size) - 1] = CHAR_VACIO; \
} while(0)

#define CHECK_ERROR(fresult, sd, msg) do { \
    if ((fresult) != FR_OK) { \
        send_uart((sd), (msg)); \
        send_uart((sd), get_fresult_message((fresult))); \
        log_error((sd), (msg)); \
        free((sd)); \
        return NULL; \
    } \
} while(0)

#define FILE_CHECK_ERROR(fresult, sd, msg) do { \
    if ((fresult) != FR_OK) { \
        send_uart((sd), (msg)); \
        send_uart((sd), get_fresult_message((fresult))); \
        log_error((sd), (msg)); \
        return; \
    } \
} while(0)


#define BUFFER_CLEAR(buf) do { \
    memset((buf), 0, sizeof((buf))); \
} while(0)

#define SEND_UART(sd, msg) do { \
    uint8_t len = strlen((msg)); \
    HAL_UART_Transmit((sd)->huart, (uint8_t*)(msg), len, UART_TIMEOUT_MS); \
} while(0)


/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */


/* === Private variable definitions ============================================================ */


/* === Private function implementation ========================================================= */

// Declaración de la función send_uart antes de su uso
static void send_uart(MicroSD *sd, const char *string);


static void send_uart(MicroSD *sd, const char *string) {
    uint8_t len = strlen(string);
    HAL_UART_Transmit(sd->huart, (uint8_t*) string, len, 2000);
}

const char* get_fresult_message(FRESULT fr) {
    switch (fr) {
        case FR_OK: return FR_OK_MSG;
        case FR_DISK_ERR: return FR_DISK_ERR_MSG;
        case FR_INT_ERR: return FR_INT_ERR_MSG;
        case FR_NOT_READY: return FR_NOT_READY_MSG;
        case FR_NO_FILE: return FR_NO_FILE_MSG;
        case FR_NO_PATH: return FR_NO_PATH_MSG;
        case FR_INVALID_NAME: return FR_INVALID_NAME_MSG;
        case FR_DENIED: return FR_DENIED_MSG;
        case FR_EXIST: return FR_EXIST_MSG;
        case FR_INVALID_OBJECT: return FR_INVALID_OBJECT_MSG;
        case FR_WRITE_PROTECTED: return FR_WRITE_PROTECTED_MSG;
        case FR_INVALID_DRIVE: return FR_INVALID_DRIVE_MSG;
        case FR_NOT_ENABLED: return FR_NOT_ENABLED_MSG;
        case FR_NO_FILESYSTEM: return FR_NO_FILESYSTEM_MSG;
        case FR_MKFS_ABORTED: return FR_MKFS_ABORTED_MSG;
        case FR_TIMEOUT: return FR_TIMEOUT_MSG;
        case FR_LOCKED: return FR_LOCKED_MSG;
        case FR_NOT_ENOUGH_CORE: return FR_NOT_ENOUGH_CORE_MSG;
        case FR_TOO_MANY_OPEN_FILES: return FR_TOO_MANY_OPEN_FILES_MSG;
        default: return FR_UNKNOWN_ERROR_MSG;
    }
}

/* === Public function implementation ========================================================== */

void log_error(MicroSD *sd, const char *error_message) {
    FIL log_file;
    FRESULT fr = f_open(&log_file, LOG_FILE, FA_OPEN_APPEND | FA_WRITE);
    if (fr != FR_OK) {
        SEND_UART(sd, LOG_FILE_OPEN_FAILURE);
        return;
    }
    f_puts(error_message, &log_file);
    f_puts(NEW_LINE, &log_file);
    f_close(&log_file);
}

MicroSD* microSD_create(UART_HandleTypeDef *huart, const char *filename, const char *directory) {
    MicroSD *sd = (MicroSD*)malloc(sizeof(MicroSD));
    if (sd == NULL) {
        return NULL;
    }

    sd->huart = huart;
    SAFE_STRCPY(sd->filename, filename, sizeof(sd->filename));
    SAFE_STRCPY(sd->directory, directory, sizeof(sd->directory));

    sd->fresult = f_mount(&sd->fs, sd->directory, 1);
    CHECK_ERROR(sd->fresult, sd, MOUNT_FAILURE);

    SEND_UART(sd, MOUNT_SUCCESS);
    return sd;
}

void microSD_destroy(MicroSD *sd) {
    if (sd != NULL) {
        sd->fresult = f_mount(NULL, sd->directory, FORCE_UNMOUNT);
        if (sd->fresult != FR_OK) {
            SEND_UART(sd, UNMOUNT_FAILURE);
        } else {
            SEND_UART(sd, UNMOUNT_SUCCESS);
        }
        free(sd);
    }
}

void microSD_setDirectory(MicroSD *sd, const char *directory) {
    SAFE_STRCPY(sd->directory, directory, sizeof(sd->directory));
}

/**
 * @brief Obtiene y envía el tamaño total y el espacio libre de la tarjeta microSD.
 *
 * @param sd Puntero a la estructura MicroSD.
 */
void microSD_getSize(MicroSD *sd) {
    // Obtiene el número de clústeres libres y el número total de clústeres en el sistema de archivos.
    sd->fresult = f_getfree(sd->directory, &sd->fre_clust, &sd->pfs);
    // Verifica si hubo un error al obtener la información del sistema de archivos.
    FILE_CHECK_ERROR(sd->fresult, sd, GET_SIZE_FAILURE);

    // Calcula el tamaño total de la tarjeta microSD en kilobytes.
    uint32_t total = (uint32_t)((sd->pfs->n_fatent - RESERVED_CLUSTERS) * sd->pfs->csize * SECTORS_TO_KILOBYTES_CONVERSION_FACTOR);
    // Formatea el tamaño total en una cadena de caracteres y la almacena en el buffer.
    snprintf(sd->buffer, sizeof(sd->buffer), MSN_SD_TOTAL_SIZE, total);
    // Envía la cadena formateada a través de UART.
    SEND_UART(sd, sd->buffer);
    // Limpia el contenido del buffer.
    BUFFER_CLEAR(sd->buffer);

    // Calcula el espacio libre en la tarjeta microSD en kilobytes.
    uint32_t freeSpace = (uint32_t)(sd->fre_clust * sd->pfs->csize * SECTORS_TO_KILOBYTES_CONVERSION_FACTOR);
    // Formatea el espacio libre en una cadena de caracteres y la almacena en el buffer.
    snprintf(sd->buffer, sizeof(sd->buffer), MSN_SD_FREE_SIZE, freeSpace);
    // Envía la cadena formateada a través de UART.
    SEND_UART(sd, sd->buffer);
    // Limpia el contenido del buffer.
    BUFFER_CLEAR(sd->buffer);
}

void microSD_write(MicroSD *sd, const char *data) {
    sd->fresult = f_open(&sd->fil, sd->filename, FA_OPEN_ALWAYS | FA_WRITE);
    FILE_CHECK_ERROR(sd->fresult, sd, FILE_WRITE_FAILURE);

    f_puts(data, &sd->fil);
    sd->fresult = f_close(&sd->fil);
    FILE_CHECK_ERROR(sd->fresult, sd, FILE_CLOSE_FAILURE);

    snprintf(sd->buffer, sizeof(sd->buffer), TXT_STRING, sd->filename, FILE_WRITE_SUCCESS);
    SEND_UART(sd, sd->buffer);
    BUFFER_CLEAR(sd->buffer);
}

void microSD_read(MicroSD *sd) {
    sd->fresult = f_open(&sd->fil, sd->filename, FA_READ);
    FILE_CHECK_ERROR(sd->fresult, sd, FILE_READ_FAILURE);

    f_gets(sd->buffer, sizeof(sd->buffer), &sd->fil);
    char msg[200];
    snprintf(msg, sizeof(msg), TXT_STRING, sd->filename, FILE_READ_HEADER);
    SEND_UART(sd, msg);
    SEND_UART(sd, sd->buffer);
    SEND_UART(sd, NEW_LINE);
    f_close(&sd->fil);
    BUFFER_CLEAR(sd->buffer);
}

void microSD_appendLine(MicroSD *sd, const char *line) {
    sd->fresult = f_open(&sd->fil, sd->filename, FA_OPEN_APPEND | FA_WRITE);
    FILE_CHECK_ERROR(sd->fresult, sd, FILE_APPEND_FAILURE);

    f_puts(line, &sd->fil);
    f_puts(NEW_LINE, &sd->fil);
    sd->fresult = f_close(&sd->fil);
    FILE_CHECK_ERROR(sd->fresult, sd, FILE_CLOSE_FAILURE);

    snprintf(sd->buffer, sizeof(sd->buffer), TXT_STRING, FILE_APPEND_SUCCESS, sd->filename);
    SEND_UART(sd, sd->buffer);
    BUFFER_CLEAR(sd->buffer);
}

// Definición de bufclear
void bufclear(char *buf, uint16_t len) {
    for (int i = 0; i < len; i++) {
        buf[i] = CHAR_VACIO;
    }
}

/* === End of documentation ==================================================================== */

