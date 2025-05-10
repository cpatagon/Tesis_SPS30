/*
 * Nombre del archivo: time_rtc.h
 * Descripción: Este archivo proporciona funciones para inicializar el RTC, configurar la hora inicial,
 * y obtener la fecha y hora formateada. Además, se incluye una función para imprimir
 * la fecha y hora en formato ISO 8601.
 *
 * Funciones incluidas:
 * - getFormattedDateTime: Obtiene la fecha y hora formateadas como cadena.
 * - printFormattedDateTime: Imprime la fecha y hora formateadas.
 * - DS1307_SetInitialTime: Configura la hora inicial del RTC.
 *
 * Futuras ampliaciones pueden incluir:
 * - Funciones para ajustar la alarma del RTC.
 * - Funciones para leer y escribir en la SRAM del RTC.
 * - Integración con otros módulos de tiempo o sincronización.
 * Autor: lgomez
 * Creado en: 08-06-2024 
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
#ifndef INC_TIME_RTC_H_
#define INC_TIME_RTC_H_
/** @file time_rtc.h
 ** @brief   Implementación de funciones para la gestión del RTC DS1307 en un entorno STM32.
 **/

/* === Headers files inclusions ================================================================ */

#include <stddef.h> // Incluir el encabezado necesario para size_t
#include "rtc_ds1307_for_stm32_hal.h"

/* === Cabecera C++ ============================================================================ */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */

/* === Public data type declarations =========================================================== */

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

/**
 * @brief Inicializa el RTC DS1307 y configura el puerto I2C.
 *
 * Esta función inicializa el RTC DS1307 y configura el puerto I2C que se utilizará
 * para la comunicación.
 *
 * @param[in] hi2c  Puntero al manejador I2C.
 */
void time_rtc_Init(I2C_HandleTypeDef *hi2c);

/**
 * @brief Obtiene la fecha y hora formateadas desde el RTC DS1307.
 *
 * Esta función obtiene la fecha y hora actuales del RTC DS1307 y las formatea
 * como una cadena en el formato ISO 8601.
 *
 * @param[out] buffer       El buffer donde se almacenará la cadena formateada.
 * @param[in]  buffer_size  El tamaño del buffer.
 */
void time_rtc_GetFormattedDateTime(char *buffer, size_t buffer_size);

/**
 * @brief Imprime la fecha y hora formateadas desde el RTC DS1307.
 *
 * Esta función obtiene la fecha y hora actuales del RTC DS1307 y las imprime
 * directamente en la consola en el formato ISO 8601.
 */
void time_rtc_PrintFormattedDateTime(void);

/**
 * @brief Configura la hora inicial del RTC DS1307.
 *
 * Esta función inicializa el RTC DS1307 y configura la fecha y hora inicial,
 * incluyendo la zona horaria.
 *
 * @param[in] hi2c           Puntero al manejador I2C.
 * @param[in] timeZoneHour   La hora de la zona horaria (ejemplo: -3 para UTC-3).
 * @param[in] timeZoneMin    Los minutos de la zona horaria.
 * @param[in] date           El día del mes.
 * @param[in] month          El mes.
 * @param[in] year           El año.
 * @param[in] dayOfWeek      El día de la semana.
 * @param[in] hour           La hora.
 * @param[in] minute         Los minutos.
 * @param[in] second         Los segundos.
 */
void DS1307_SetInitialTime(I2C_HandleTypeDef *hi2c, int8_t timeZoneHour, uint8_t timeZoneMin, uint8_t date, uint8_t month, uint16_t year, uint8_t dayOfWeek, uint8_t hour, uint8_t minute, uint8_t second);

/* === End of documentation ==================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* INC_TIME_RTC_H_ */
