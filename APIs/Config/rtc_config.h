/*
 * Nombre del archivo: rtc_config.h
 * Descripción: Configuración inicial del RTC externo del sistema
 * Autor: lgomez
 * Creado en: May 15, 2025
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
#ifndef CONFIG_RTC_CONFIG_H_
#define CONFIG_RTC_CONFIG_H_
/** @file
 ** @brief
 **/

/* === Headers files inclusions
 * ================================================================ */

/* === Cabecera C++
 * ============================================================================
 */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions
 * =============================================================== */

/* === Macros de compilación: fecha y hora
 * =========================================== */
#define COMPILE_DATE_STRING __DATE__ // Ej: "May 15 2025"
#define COMPILE_TIME_STRING __TIME__ // Ej: "14:33:21"

#define DATE_PARSE_FORMAT "%3s %d %d" // formato para sscanf de __DATE__
#define TIME_PARSE_FORMAT "%d:%d:%d"  // formato para sscanf de __TIME__

#define COMPILE_MONTH_STR_LEN 3 // "Jan", "Feb", ...
#define COMPILE_MONTHS_COUNT 12

#define DATE_DEFAULT_DAY 1
#define DATE_DEFAULT_YEAR 2024
#define TIME_DEFAULT_HOUR 12
#define TIME_DEFAULT_MIN 0
#define TIME_DEFAULT_SEC 0

/** @brief Permite usar la hora de compilación si no se establece otra fuente */
#define RTC_USE_BUILD_TIME_IF_NO_SOURCE 0
#define RTC_TEST_ENABLED 0

/** @brief Tamaños de buffers */
#define RTC_INPUT_MAX_LENGTH 20
#define RTC_DEBUG_BUFFER_SIZE 128

/** @brief Rango de fechas válidas */
#define RTC_YEAR_MIN 2000
#define RTC_YEAR_MAX 2099

/** @brief Formato de entrada (sin separadores) */
#define RTC_INPUT_FORMAT_STRING "%4hu%2hhu%2hhu%2hhu%2hhu%2hhu"
#define RTC_GET_FORMAT_DATETIME "%04d-%02d-%02d %02d:%02d:%02d"
#define RTC_SET_FORMAT_DATETIME "%4d%2d%2d%2d%2d%2d"

/** @brief Control de mensajes de depuración */
#define RTC_DEBUG_ENABLED 0

/** @brief Mensajes */
#define RTC_MSG_INPUT_PROMPT "Ingrese fecha y hora [YYYYMMDDHHMMSS;]:\r\n"
#define RTC_MSG_INPUT_DEBUG "[DEBUG] Entrada recibida: %s\r\n"

/* === Mensajes de error para RTC DS3231
 * ============================================ */
#define RTC_DS3231_WRITE_ERROR_MSG "Error al escribir al DS3231\r\n"
#define RTC_DS3231_READ_ERROR_MSG "Error al leer desde el DS3231\r\n"

#define RTC_MSG_PARSE_ERROR "[ERROR] Formato invalido. Use YYYYMMDDHHMMSS;\r\n"
#define RTC_MSG_DATE_INVALID                                                   \
  "[ERROR] Fecha invalida: %04hu-%02hhu-%02hhu %02hhu:%02hhu:%02hhu\r\n"
#define RTC_MSG_ERROR_LONG_INVALID                                             \
  "[ERROR] Longitud incorrecta. Debe tener 14 dígitos (YYYYMMDDHHMMSS).\r\n"
#define RTC_MSG_ERROR_CARACTER_INVALID                                         \
  "[ERROR] El string contiene caracteres no numéricos.\r\n"
#define RTC_MSG_ERROR_VALUE_INVALID                                            \
  "[ERROR] sscanf fallO. Valores parseados: %d\r\n"

#define RTC_MSG_DATE_PARSED                                                    \
  "[DEBUG] Fecha/hora parseada: %04u-%02u-%02u %02u:%02u:%02u\r\n"
#define RTC_MSG_SET_SUCCESS "[RTC] Fecha/hora actualizada correctamente\r\n"
#define RTC_MSG_SET_FAIL "[RTC] Error al escribir en el RTC\r\n"
#define MSM_RTC_HORA_NO_VALIDA                                                 \
  "[RTC] Hora no válida. Usando hora de compilación.\r\n"

#define RTC_TEST_INIT_MSG                                                      \
  "[TEST] Intentando configurar RTC con fecha fija...\r\n"
#define RTC_TEST_SUCCESS_MSG "[TEST] RTC configurado con exito.\r\n"
#define RTC_TEST_FAILURE_MSG "[TEST] Error al configurar RTC.\r\n"
#define RTC_TEST_VERIF_MSG_FMT                                                 \
  "[TEST] Verificacion: %04d-%02d-%02d %02d:%02d:%02d\r\n"
#define RTC_TEST_COMPILE_TIME_FMT                                              \
  "[TEST] Verificacion fecha compilacion: %04d-%02d-%02d %02d:%02d:%02d\r\n"

/** @brief Carácter de terminación de entrada */
#define RTC_INPUT_TERMINATOR ';'

#define RTC_I2C_HANDLER hi2c2
#define RTC_I2C_ADDRESS (DS3231_I2C_ADDR << 1)
#define RTC_RETRIES 3
#define RTC_TIMEOUT_MS 100

#define RTC_YEAR_MAX 2099

#define UART_TIMEOUT_MS 100
#define UART_INPUT_TIMEOUT_MS 30000
#define UART_INPUT_BUFFER_LEN 32
#define UART_DEBUG_BUFFER_LEN 128

#define RTC_PROMPT_MSG "Ingrese fecha y hora [YYYYMMDDHHMMSS;]:\r\n"
#define RTC_PARSE_FAIL_MSG                                                     \
  "Tiempo expirado o entrada incompleta. Se mantiene hora RTC actual.\r\n"
#define RTC_YEAR_ERROR_MSG "Ano fuera de rango\r\n"
#define RTC_SET_ERROR_MSG "Error al configurar el RTC.\r\n"
#define RTC_SUCCESS_MSG "RTC actualizado exitosamente.\r\n"

#define UART_BUFFER_SIZE 64

/* === Public data type declarations
 * =========================================================== */

/* === Public variable declarations
 * ============================================================ */

/* === Public function declarations
 * ============================================================ */

/* === End of documentation
 * ==================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* CONFIG_RTC_CONFIG_H_ */
