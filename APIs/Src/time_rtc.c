/*
 * Nombre del archivo: time_rtc.c
 * Descripción: Implementación de funciones para la gestión del RTC DS1307 en un entorno STM32.
 *              Este archivo proporciona funciones para inicializar el RTC, configurar la hora inicial,
 *              y obtener la fecha y hora formateada. Además, se incluye una función para imprimir
 *              la fecha y hora en formato ISO 8601.
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
/** @file
 ** @brief 
 **/

/* === Headers files inclusions =============================================================== */

#include "time_rtc.h"
#include <stdio.h>
#include <stddef.h>

/* === Macros definitions ====================================================================== */


/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */

void time_rtc_Init(I2C_HandleTypeDef *hi2c) {
    DS1307_Init(hi2c);
}

// Función para obtener la fecha y hora formateada como cadena
void time_rtc_GetFormattedDateTime(char *buffer, size_t buffer_size) {
    uint8_t date = DS1307_GetDate();
    uint8_t month = DS1307_GetMonth();
    uint16_t year = DS1307_GetYear();
    uint8_t hour = DS1307_GetHour();
    uint8_t minute = DS1307_GetMinute();
    uint8_t second = DS1307_GetSecond();
    int8_t zone_hr = DS1307_GetTimeZoneHour();
    uint8_t zone_min = DS1307_GetTimeZoneMin();

    snprintf(buffer, buffer_size, "%04d-%02d-%02dT%02d:%02d:%02d%+03d:%02d\n", year, month, date, hour, minute, second, zone_hr, zone_min);
}

// Función para imprimir la fecha y hora formateada
void time_rtc_PrintFormattedDateTime() {
    char buffer[100] = {0};
    time_rtc_GetFormattedDateTime(buffer, sizeof(buffer));
    printf("%s", buffer);
}

// Función para configurar la hora inicial del RTC
void DS1307_SetInitialTime(I2C_HandleTypeDef *hi2c, int8_t timeZoneHour, uint8_t timeZoneMin, uint8_t date, uint8_t month, uint16_t year, uint8_t dayOfWeek, uint8_t hour, uint8_t minute, uint8_t second) {
    DS1307_Init(hi2c);
    DS1307_SetTimeZone(timeZoneHour, timeZoneMin);
    DS1307_SetDate(date);
    DS1307_SetMonth(month);
    DS1307_SetYear(year);
    DS1307_SetDayOfWeek(dayOfWeek);
    DS1307_SetHour(hour);
    DS1307_SetMinute(minute);
    DS1307_SetSecond(second);
}


/* === End of documentation ==================================================================== */

