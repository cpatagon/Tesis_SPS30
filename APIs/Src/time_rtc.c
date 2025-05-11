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
/**
 * @file time_rtc.c
 * @brief Manejo automático de RTC interno y externo (DS1307/DS3231)
 */

#include "time_rtc.h"
#include "rtc_ds1307_for_stm32_hal.h"
#include "rtc.h"  // HAL RTC interno
#include <stdio.h>
#include <string.h>

RTC_Source active_rtc = RTC_SOURCE_INTERNAL;  // Por defecto

/**
 * @brief Verifica si el RTC externo responde por I2C
 */
static bool rtc_external_available(void) {
    return (HAL_I2C_IsDeviceReady(&hi2c2, DS1307_ADDRESS << 1, 3, 100) == HAL_OK);
}

/**
 * @brief Inicializa automáticamente el RTC disponible (externo si responde, interno si no)
 */
void rtc_auto_init(void) {
    if (rtc_external_available()) {
    	DS1307_Init(&hi2c2);
        active_rtc = RTC_SOURCE_EXTERNAL;
    } else {
        MX_RTC_Init();
        active_rtc = RTC_SOURCE_INTERNAL;
    }
}

/**
 * @brief Obtiene fecha y hora del RTC activo en formato ISO8601
 * @param buffer Buffer donde se escribirá la cadena de fecha
 * @param len Tamaño del buffer
 */
void rtc_get_time(char *buffer, size_t len) {
    if (active_rtc == RTC_SOURCE_EXTERNAL) {
        DS1307_DateTime dt;
        DS1307_GetTime(&dt);
        snprintf(buffer, len, "%04d-%02d-%02d %02d:%02d:%02d",
                 dt.year, dt.month, dt.day,
                 dt.hours, dt.minutes, dt.seconds);
    } else {
        RTC_TimeTypeDef sTime;
        RTC_DateTypeDef sDate;
        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
        snprintf(buffer, len, "20%02d-%02d-%02d %02d:%02d:%02d",
                 sDate.Year, sDate.Month, sDate.Date,
                 sTime.Hours, sTime.Minutes, sTime.Seconds);
    }
}

/**
 * @brief Función de compatibilidad con código previo
 */
void obtener_fecha_hora(char *fecha_hora_str) {
    rtc_get_time(fecha_hora_str, 32);
}

// Compatibilidad con nombres anteriores
void time_rtc_Init(void) {
    rtc_auto_init();
}

void time_rtc_GetFormattedDateTime(char *buffer, size_t len) {
    rtc_get_time(buffer, len);
}
