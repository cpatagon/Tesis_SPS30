/*
 * Nombre del archivo: config_mensaje.h
 * Descripción: [Breve descripción del archivo]
 * Autor: lgomez
 * Creado en: 25-06-2025
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
#ifndef CONFIG_CONFIG_MENSAJE_H_
#define CONFIG_CONFIG_MENSAJE_H_
/** @file
 ** @brief
 **/

/* === Headers files inclusions ================================================================ */

/* === Cabecera C++ ============================================================================ */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */

#define MSG_ERROR_REINT "**ERROR[SPS30_RETRY] Intento fallido, reintentando...\n"
#define MSG_ERROR_FALLO "**ERROR[SPS30_FAIL][%s] Sensor ID:%d sin respuesta tras 3 intentos\n"

/* === MicroSD == */

#define MOUNT_SUCCESS                                                                              \
    "microSD CARD montada con exito\n" /**< Mensaje de éxito al montar la                         \
                                          tarjeta microSD */
#define MOUNT_FAILURE                                                                              \
    "ERROR !!! montaje sd fallido ... \n" /**< Mensaje de error al montar la                       \
                                             tarjeta microSD */
#define UNMOUNT_SUCCESS                                                                            \
    "microSD CARD desmontada con exito\n" /**< Mensaje de éxito al desmontar la                   \
                                             tarjeta microSD */
#define UNMOUNT_FAILURE                                                                            \
    "ERROR !!! desmontaje sd fallido ... \n" /**< Mensaje de error al desmontar                    \
                                              * la tarjeta microSD                                 \
                                              */
#define FILE_WRITE_SUCCESS                                                                         \
    " creado y los datos fueron escritos\n" /**< Mensaje de éxito al escribir en                  \
                                               el archivo */
#define FILE_WRITE_FAILURE                                                                         \
    "ERROR !!! escribir en archivo fallido ... \n" /**< Mensaje de error al                        \
                                                    * escribir en el archivo                       \
                                                    */
#define FILE_CLOSE_FAILURE                                                                         \
    "ERROR !!! cerrar archivo después de escribir fallido ... \n" /**< Mensaje                     \
                                                                     de error al                   \
                                                                     cerrar el                     \
                                                                     archivo                       \
                                                                     después de                   \
                                                                     escribir */
#define FILE_APPEND_FAILURE                                                                        \
    "ERROR !!! abrir archivo para agregar línea fallido ... \n" /**< Mensaje de                    \
                                                                   error al                        \
                                                                   abrir el                        \
                                                                   archivo para                    \
                                                                   agregar línea                  \
                                                                 */
#define FILE_READ_HEADER                                                                           \
    " fue abierto y contiene los siguientes datos:\n" /**< Encabezado de mensaje                   \
                                                         al leer el archivo */
#define FILE_READ_FAILURE                                                                          \
    "ERROR !!! abrir archivo para leer fallido ... \n" /**< Mensaje de error al                    \
                                                          abrir el archivo para                    \
                                                          leer */
#define FILE_APPEND_SUCCESS                                                                        \
    "\n Nueva linea agregada al archivo " /**< Mensaje de éxito al agregar una                    \
                                             línea al archivo */
#define GET_SIZE_FAILURE                                                                           \
    "ERROR !!! obtener tamaño de la SD fallido ... \n" /**< Mensaje de error al                    \
                                                          obtener el tamaño de                    \
                                                          la SD */
#define MSN_SD_TOTAL_SIZE                                                                          \
    "SD CARD tamano Total: \t%luKB\n" /**< Mensaje con el tamaño total de la SD                   \
                                       */
#define MSN_SD_FREE_SIZE                                                                           \
    "SD CARD Free Space: \t%luKB\n\n" /**< Mensaje con el espacio libre de la SD                   \
                                       */
#define LOG_FILE_OPEN_FAILURE                                                                      \
    "ERROR !!! abrir archivo de registro fallido ... \n" /**< Mensaje de error                     \
                                                            al abrir el archivo                    \
                                                            de registro */

// Define for FRESULT messages
#define FR_OK_MSG           "Success"        /**< Mensaje de éxito para FR_OK */
#define FR_DISK_ERR_MSG     "Disk error"     /**< Mensaje de error de disco para FR_DISK_ERR */
#define FR_INT_ERR_MSG      "Internal error" /**< Mensaje de error interno para FR_INT_ERR */
#define FR_NOT_READY_MSG    "Disk not ready" /**< Mensaje de disco no listo para FR_NOT_READY */
#define FR_NO_FILE_MSG      "File not found" /**< Mensaje de archivo no encontrado para FR_NO_FILE */
#define FR_NO_PATH_MSG      "Path not found" /**< Mensaje de ruta no encontrada para FR_NO_PATH */
#define FR_INVALID_NAME_MSG "Invalid name"  /**< Mensaje de nombre inválido para FR_INVALID_NAME */
#define FR_DENIED_MSG       "Access denied" /**< Mensaje de acceso denegado para FR_DENIED */
#define FR_EXIST_MSG        "File exists"   /**< Mensaje de archivo existente para FR_EXIST */
#define FR_INVALID_OBJECT_MSG                                                                      \
    "Invalid object" /**< Mensaje de objeto inválido para FR_INVALID_OBJECT */
#define FR_WRITE_PROTECTED_MSG                                                                     \
    "Write protected" /**< Mensaje de protección contra escritura para                            \
                         FR_WRITE_PROTECTED */
#define FR_INVALID_DRIVE_MSG                                                                       \
    "Invalid drive"                      /**< Mensaje de unidad inválida para FR_INVALID_DRIVE */
#define FR_NOT_ENABLED_MSG "Not enabled" /**< Mensaje de no habilitado para FR_NOT_ENABLED */
#define FR_NO_FILESYSTEM_MSG                                                                       \
    "No filesystem"                        /**< Mensaje de sistema de archivos no encontrado para  \
                                              FR_NO_FILESYSTEM */
#define FR_MKFS_ABORTED_MSG "MKFS aborted" /**< Mensaje de MKFS abortado para FR_MKFS_ABORTED */
#define FR_TIMEOUT_MSG      "Timeout"      /**< Mensaje de tiempo agotado para FR_TIMEOUT */
#define FR_LOCKED_MSG       "Locked"       /**< Mensaje de bloqueado para FR_LOCKED */
#define FR_NOT_ENOUGH_CORE_MSG                                                                     \
    "Not enough core" /**< Mensaje de núcleo insuficiente para                                    \
                         FR_NOT_ENOUGH_CORE */
#define FR_TOO_MANY_OPEN_FILES_MSG                                                                 \
    "Too many open files" /**< Mensaje de demasiados archivos abiertos para                        \
                           * FR_TOO_MANY_OPEN_FILES                                                \
                           */
#define FR_UNKNOWN_ERROR_MSG                                                                       \
    "Unknown error" /**< Mensaje de error desconocido para casos no                                \
                       especificados */

/* === Public data type declarations =========================================================== */

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

/* === End of documentation ==================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* CONFIG_CONFIG_MENSAJE_H_ */
