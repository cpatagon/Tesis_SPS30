/*
 * Nombre del archivo: proceso_observador.h
 * Descripción: Módulo para adquisición de datos de sensores SPS30
 * Autor: lgomez
 * Creado en: 04-05-2025 
 * Derechos de Autor: (C) 2023 [Tu nombre o institución]
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
 */

#ifndef INC_PROCESO_OBSERVADOR_H_
#define INC_PROCESO_OBSERVADOR_H_

/** @file
 ** @brief Declaraciones públicas del proceso observador para SPS30
 **/

/* === Inclusión de archivos ================================================================ */
#include "sps30_comm.h"
#include "uart_printing.h"
#include "time_rtc.h"
#include <stdbool.h>

/* === Cabecera C++ ======================================================================== */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */

// Formato para imprimir los datos con timestamp
#define MSG_PM_FORMAT_WITH_TIME "[%s] SPS30 ID:%d | PM1.0: %.2f | PM2.5: %.2f | PM4.0: %.2f | PM10: %.2f | ug/m3\n"

// Mensajes de error actualizados
#define MSG_ERROR_REINT     "**ERROR[SPS30_RETRY] Intento fallido, reintentando...\n"
#define MSG_ERROR_FALLO     "**ERROR[SPS30_FAIL][%s] Sensor ID:%d sin respuesta tras 3 intentos\n"

// Tamaños de buffer
#define BUFFER_SIZE_MSG_PM_FORMAT 256
#define BUFFER_SIZE_MSG_ERROR_FALLO 96

// Parámetros de configuración
#define NUM_REINT 3                     // Número máximo de intentos de lectura del sensor
#define CONC_MIN_PM 0.0f                // Valor mínimo considerado válido para concentraciones de PM
#define HAL_DELAY_SIGUIENTE_MEDICION 5000 // Delay entre inicio de medición y lectura en ms

#define CONC_MAX_PM 1000.0f // umbral máximo por seguridad

/* === Declaraciones de funciones públicas ================================================ */
/**
 * @brief Ejecuta una adquisición desde un sensor SPS30 e incluye timestamp
 *
 * @param sensor Puntero al objeto SPS30
 * @param uart Puntero al objeto UART_Printing para salida de datos
 * @param sensor_id Número identificador del sensor (ej. 1, 2, 3)
 * @return true si la adquisición fue exitosa; false si falló tras reintentos
 */
bool proceso_observador(SPS30* sensor, UART_Printing* uart, uint8_t sensor_id);

/**
 * @brief Ejecuta una adquisición desde un sensor SPS30 y guarda los datos con timestamp
 *
 * @param sensor Puntero al objeto SPS30
 * @param uart Puntero al objeto UART_Printing para salida de datos
 * @param sensor_id Número identificador del sensor (ej. 1, 2, 3)
 * @param datetime_str Cadena con la fecha y hora de la medición
 * @return true si la adquisición fue exitosa; false si falló tras reintentos
 */
bool proceso_observador_with_time(SPS30* sensor, UART_Printing* uart, uint8_t sensor_id, const char* datetime_str);


/* === Fin de cabecera C++ ================================================================ */
#ifdef __cplusplus
}
#endif

#endif /* INC_PROCESO_OBSERVADOR_H_ */
