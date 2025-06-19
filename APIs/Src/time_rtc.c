/*
 * Nombre del archivo: time_rtc.c
 * Descripción: Implementación de funciones para la gestión del RTC DS1307 en un entorno STM32.
 *              Este archivo proporciona funciones para inicializar el RTC, configurar la hora
 * inicial, y obtener la fecha y hora formateada. Además, se incluye una función para imprimir la
 * fecha y hora en formato ISO 8601. Autor: lgomez Creado en: 08-06-2024 Derechos de Autor: (C) 2023
 * [Tu nombre o el de tu organización] Licencia: GNU General Public License v3.0
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
#include "rtc_buildtime.h"
#include "rtc_ds3231_for_stm32_hal.h"
#include "rtc.h" // HAL RTC interno
#include "rtc_buildtime.h"
#include "rtc_config.h"
#include "usart.h"
#include "uart.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

/* === Configuraciones por define =================================================== */

extern I2C_HandleTypeDef hi2c2;

extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart1;

RTC_Source active_rtc = RTC_SOURCE_INTERNAL; // Por defecto

/* === Función interna ============================================================= */

/* === Función interna ============================================================= */
/**
 * @brief Verifica si el RTC externo responde por I2C
 */
static bool rtc_external_available(void) {
    return (HAL_I2C_IsDeviceReady(&RTC_I2C_HANDLER, RTC_I2C_ADDRESS, RTC_RETRIES, RTC_TIMEOUT_MS) ==
            HAL_OK);
}

/**
 * @brief Inicializa automáticamente el RTC (externo o interno), y si está habilitado,
 *        configura la hora con el timestamp de compilación en caso de ser necesario.
 */

bool rtc_auto_init(void) {
    if (rtc_external_available()) {
        DS3231_Init(&RTC_I2C_HANDLER);
        active_rtc = RTC_SOURCE_EXTERNAL;
#if RTC_USE_BUILD_TIME_IF_NO_SOURCE
        DS3231_DateTime now;
        DS3231_GetDateTime(&now);

        if (now.year < RTC_YEAR_MIN || now.year > RTC_YEAR_MAX) {
            uart_print(MSM_RTC_HORA_NO_VALIDA);
            DS3231_DateTime build_time = rtc_get_compile_time();
            DS3231_SetDateTime(&build_time);
            return false; // hora inválida
        }
#endif
        return true;
    } else {
        MX_RTC_Init();
        active_rtc = RTC_SOURCE_INTERNAL;
        return true; // aunque sea interno, se considera OK
    }
}

/* === Interfaz de usuario por UART ================================================== */

bool RTC_ReceiveTimeFromTerminal(UART_HandleTypeDef * huart) {
    char rx_buffer[UART_INPUT_BUFFER_LEN] = {0};
    char debug_buf[UART_DEBUG_BUFFER_LEN];
    uint8_t idx = 0;
    uint32_t start_tick = HAL_GetTick();
    HAL_StatusTypeDef status;

    uart_print(RTC_PROMPT_MSG);

    while ((HAL_GetTick() - start_tick) < UART_INPUT_TIMEOUT_MS) {
        uint8_t ch;
        status = HAL_UART_Receive(huart, &ch, 1, UART_TIMEOUT_MS);
        if (status == HAL_OK) {
            if (ch == ';') {
                rx_buffer[idx] = '\0';
                break;
            }
            if (idx < UART_INPUT_BUFFER_LEN - 1) {
                rx_buffer[idx++] = ch;
            }
        }
    }

    if (idx < 14) {
        uart_print(RTC_PARSE_FAIL_MSG);
        return false;
    }

    snprintf(debug_buf, sizeof(debug_buf), "[DEBUG] Entrada recibida: %s\r\n", rx_buffer);
    uart_print(debug_buf);

    rx_buffer[strcspn(rx_buffer, "\r\n;")] = '\0';

    char year_str[5] = {0}, month_str[3] = {0}, day_str[3] = {0};
    char hour_str[3] = {0}, min_str[3] = {0}, sec_str[3] = {0};

    strncpy(year_str, rx_buffer + 0, 4);
    strncpy(month_str, rx_buffer + 4, 2);
    strncpy(day_str, rx_buffer + 6, 2);
    strncpy(hour_str, rx_buffer + 8, 2);
    strncpy(min_str, rx_buffer + 10, 2);
    strncpy(sec_str, rx_buffer + 12, 2);

    uint16_t year = atoi(year_str);
    RTC_DateTypeDef date = {0};
    RTC_TimeTypeDef time = {0};

    date.Month = atoi(month_str);
    date.Date = atoi(day_str);
    time.Hours = atoi(hour_str);
    time.Minutes = atoi(min_str);
    time.Seconds = atoi(sec_str);

    snprintf(debug_buf, sizeof(debug_buf), "Fecha parseada: %04u-%02u-%02u %02u:%02u:%02u\r\n",
             year, date.Month, date.Date, time.Hours, time.Minutes, time.Seconds);
    uart_print(debug_buf);

    if (year < RTC_YEAR_MIN || year > RTC_YEAR_MAX) {
        uart_print(RTC_YEAR_ERROR_MSG);
        return false;
    }

    date.Year = (uint8_t)(year - 2000);
    date.WeekDay = 1;

    if (!RTC_DS3231_Set(&date, &time)) {
        uart_print(RTC_SET_ERROR_MSG);
        return false;
    }

    uart_print(RTC_SUCCESS_MSG);
    return true;
}

/* === Wrappers de compatibilidad =================================================== */

void obtener_fecha_hora(char * fecha_hora_str) {
    rtc_get_time(fecha_hora_str, 32);
}

void time_rtc_Init(void) {
    rtc_auto_init();
}

void time_rtc_GetFormattedDateTime(char * buffer, size_t len) {
    rtc_get_time(buffer, len);
}
