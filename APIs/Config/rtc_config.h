/*
 * Nombre del archivo: rtc_config.h
 * Descripción: [Breve descripción del archivo]
 * Autor: lgomez
 * Creado en: May 15, 2025 
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
#ifndef CONFIG_RTC_CONFIG_H_
#define CONFIG_RTC_CONFIG_H_
/** @file
 ** @brief 
 **/

/* === Headers files inclusions ================================================================ */

/* === Cabecera C++ ============================================================================ */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */

/** @brief Permite usar la hora de compilación si no se establece otra fuente */
#define RTC_USE_BUILD_TIME_IF_NO_SOURCE  0
#define RTC_TEST_ENABLED  0

/** @brief Tamaños de buffers */
#define RTC_INPUT_MAX_LENGTH       20
#define RTC_DEBUG_BUFFER_SIZE      128

/** @brief Rango de fechas válidas */
#define RTC_YEAR_MIN               2000
#define RTC_YEAR_MAX               2099

/** @brief Formato de entrada (sin separadores) */
#define RTC_INPUT_FORMAT_STRING    "%4hu%2hhu%2hhu%2hhu%2hhu%2hhu"

/** @brief Control de mensajes de depuración */
#define RTC_DEBUG_ENABLED          0

/** @brief Mensajes */
#define RTC_MSG_INPUT_PROMPT       "Ingrese fecha y hora [YYYYMMDDHHMMSS;]:\r\n"
#define RTC_MSG_INPUT_DEBUG        "[DEBUG] Entrada recibida: %s\r\n"
#define RTC_MSG_PARSE_ERROR        "[ERROR] Formato inválido. Use YYYYMMDDHHMMSS;\r\n"
#define RTC_MSG_DATE_INVALID       "[ERROR] Fecha inválida: %04hu-%02hhu-%02hhu %02hhu:%02hhu:%02hhu\r\n"
#define RTC_MSG_DATE_PARSED        "[DEBUG] Fecha/hora parseada: %04u-%02u-%02u %02u:%02u:%02u\r\n"
#define RTC_MSG_SET_SUCCESS        "[RTC] Fecha/hora actualizada correctamente\r\n"
#define RTC_MSG_SET_FAIL           "[RTC] Error al escribir en el RTC\r\n"

/** @brief Carácter de terminación de entrada */
#define RTC_INPUT_TERMINATOR       ';'





/* === Public data type declarations =========================================================== */

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

/* === End of documentation ==================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* CONFIG_RTC_CONFIG_H_ */
