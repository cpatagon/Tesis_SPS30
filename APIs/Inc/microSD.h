/*
 * Nombre del archivo: microSD.h
 * Descripción: Funciones para operar la tarjeta microSD usando FatFs
 * Autor: lgomez
 * Creado en: 12-06-2024
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
#ifndef INC_MICROSD_H_
#define INC_MICROSD_H_
/** @file
 ** @brief
 **/

/* === Headers files inclusions
 * ================================================================ */

#include "fatfs.h" // Asegúrate de incluir esta cabecera si estás utilizando FatFs
#include "usart.h" // Incluye esta cabecera si estás utilizando la UART
#include "config_mensaje.h"
#include <stdint.h>

/* === Cabecera C++
 * ============================================================================
 */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions
 * =============================================================== */
#define SIZE_FILE_NAME 128 /**< Tamaño máximo del nombre del archivo */
#define SIZE_DIRECTORY_NAME                                                                        \
    128                       /**< Tamaño máximo del nombre del directorio                       \
                               */
#define SIZE_BUFFER       256 /**< Tamaño del buffer utilizado para almacenamiento temporal */

#define RESERVED_CLUSTERS 2   /**< Número de clústeres reservados */
#define BYTES_PER_SECTOR  512 /**< Tamaño de un sector en bytes */
#define SECTORS_TO_KILOBYTES_CONVERSION_FACTOR                                                     \
    0.5 /**< Factor de conversión de sectores a kilobytes */

#define UART_TIMEOUT_MS                                                                            \
    2000 /**< Tiempo de espera máximo para la transmisión UART en milisegundos                   \
          */

#define FORCE_UNMOUNT 1 /**< Opción para forzar el desmontaje del sistema de archivos */

#define LOG_FILE      "log.txt" /**< Nombre del archivo de log */
#define NEW_LINE      "\n"      /**< Carácter de nueva línea */
#define CHAR_VACIO    '\0'      /**< Carácter vacío */
#define TXT_STRING    "%s%s"    /**< Formato de cadena para combinar dos cadenas */

/* === Public data type declarations
 * =========================================================== */

/**
 * @brief Estructura que representa una tarjeta microSD y su configuración.
 */
typedef struct {
    UART_HandleTypeDef * huart;          /**< Puntero a la estructura de manejo de UART. */
    char filename[SIZE_FILE_NAME];       /**< Nombre del archivo a ser manejado en la
                                            tarjeta microSD. */
    char directory[SIZE_DIRECTORY_NAME]; /**< Directorio de montaje de la tarjeta
                                            microSD. */
    FATFS fs;                 /**< Estructura FATFS que representa el sistema de archivos. */
    FIL fil;                  /**< Estructura FIL que representa un archivo abierto. */
    FRESULT fresult;          /**< Resultado de las operaciones en la tarjeta microSD. */
    FATFS * pfs;              /**< Puntero a la estructura FATFS utilizada para obtener
                                 información del sistema      de archivos. */
    DWORD fre_clust;          /**< Número de clústeres libres en la tarjeta microSD. */
    char buffer[SIZE_BUFFER]; /**< Buffer para almacenamiento temporal de datos. */
} MicroSD;

/* === Public variable declarations
 * ============================================================ */

/**
 * @brief Crea y monta el sistema de archivos en la tarjeta microSD.
 *
 * @param huart Puntero a la estructura de manejo de UART.
 * @param filename Nombre del archivo a ser manejado en la tarjeta microSD.
 * @param directory Directorio de montaje de la tarjeta microSD.
 * @return Puntero a la estructura MicroSD creada. Retorna NULL si hay un error.
 */
MicroSD * microSD_create(UART_HandleTypeDef * huart, const char * filename, const char * directory);

/**
 * @brief Desmonta el sistema de archivos y libera la memoria de la estructura
 * MicroSD.
 *
 * @param sd Puntero a la estructura MicroSD a ser destruida.
 */
void microSD_destroy(MicroSD * sd);

/**
 * @brief Obtiene y envía el tamaño total y el espacio libre de la tarjeta
 * microSD.
 *
 * @param sd Puntero a la estructura MicroSD.
 */
void microSD_getSize(MicroSD * sd);

/**
 * @brief Escribe datos en el archivo especificado en la tarjeta microSD.
 *
 * @param sd Puntero a la estructura MicroSD.
 * @param data Datos a ser escritos en el archivo.
 */
void microSD_write(MicroSD * sd, const char * data);

/**
 * @brief Lee datos del archivo especificado en la tarjeta microSD y los envía a
 * través de UART.
 *
 * @param sd Puntero a la estructura MicroSD.
 */
void microSD_read(MicroSD * sd);

/**
 * @brief Agrega una nueva línea al archivo especificado en la tarjeta microSD.
 *
 * @param sd Puntero a la estructura MicroSD.
 * @param line Línea de texto a ser agregada al archivo.
 */
void microSD_appendLine(MicroSD * sd, const char * line);

/**
 * @brief Establece el directorio de montaje para la tarjeta microSD.
 *
 * @param sd Puntero a la estructura MicroSD.
 * @param directory Nuevo directorio de montaje.
 */
void microSD_setDirectory(MicroSD * sd, const char * directory);

/**
 * @brief Registra un mensaje de error en un archivo de log en la tarjeta
 * microSD.
 *
 * @param sd Puntero a la estructura MicroSD.
 * @param error_message Mensaje de error a ser registrado.
 */
void log_error(MicroSD * sd, const char * error_message);

/**
 * @brief Limpia el contenido de un buffer.
 *
 * @param buf Puntero al buffer a ser limpiado.
 * @param len Longitud del buffer.
 */
void bufclear(char * buf, uint16_t len);

/**
 * @brief Devuelve un mensaje de texto correspondiente a un código de resultado
 * FRESULT.
 *
 * @param fr Código de resultado FRESULT.
 * @return Cadena de caracteres con el mensaje correspondiente al código de
 * resultado.
 */
const char * get_fresult_message(FRESULT fr);

/* === End of documentation
 * ==================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* INC_MICROSD_H_ */
