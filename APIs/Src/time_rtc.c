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
#include "rtc_buildtime.h"
//#include "rtc_ds1307_for_stm32_hal.h"
#include "rtc_ds3231_for_stm32_hal.h"
#include "rtc.h"  // HAL RTC interno
#include "rtc_buildtime.h"
#include "usart.h"
#include "uart_printing.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>


#define UART_BUFFER_SIZE 64


extern I2C_HandleTypeDef hi2c2;

extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart1;

RTC_Source active_rtc = RTC_SOURCE_INTERNAL;  // Por defecto

/**
 * @brief Verifica si el RTC externo responde por I2C
 */
static bool rtc_external_available(void) {
    return (HAL_I2C_IsDeviceReady(&hi2c2, DS3231_I2C_ADDR << 1, 3, 100) == HAL_OK);
}


/**
 * @brief Inicializa automáticamente el RTC (externo o interno), y si está habilitado,
 *        configura la hora con el timestamp de compilación en caso de ser necesario.
 */
void rtc_auto_init(void) {

    if (rtc_external_available()) {
        DS3231_Init(&hi2c2);
        active_rtc = RTC_SOURCE_EXTERNAL;

#if RTC_USE_BUILD_TIME_IF_NO_SOURCE
        DS3231_DateTime now;
        DS3231_GetDateTime(&now);

        // Si la hora no es válida, usar hora de compilación
        if (now.year < 2023 || now.year > 2099) {
            uart_logger.print(&uart_logger, "[RTC] Hora no válida. Usando hora de compilación.\r\n");

            DS3231_DateTime build_time = rtc_get_compile_time();
            DS3231_SetDateTime(&build_time);
        }
#endif

    } else {
           MX_RTC_Init(); // RTC interno
           active_rtc = RTC_SOURCE_INTERNAL;

           // Puedes agregar lógica similar aquí si quieres aplicar fallback también al RTC interno.
       }
   }



/**
 * @brief Solicita fecha y hora por terminal UART y actualiza el RTC.
 *
 * Esta función permite al usuario ingresar manualmente la fecha y hora
 * en formato `YYYYMMDDHHMMSS;` a través de la UART (terminal). Una vez
 * recibida la cadena completa, se procesa para actualizar el reloj en tiempo real (RTC).
 *
 * Requiere que `rtc_set_time_from_uart()` esté implementada para interpretar
 * la cadena y establecer la hora del RTC.
 *
 * El usuario debe terminar la entrada con un punto y coma `;`.
 * Se hace eco de cada carácter recibido para confirmación por el terminal.
 *
 * @note UART3 se utiliza para recibir datos y UART1 para eco de salida.
 * @note Utiliza uart_logger para mostrar instrucciones al usuario.
 */
bool RTC_ReceiveTimeFromTerminal(UART_HandleTypeDef *huart)
{
    char rx_buffer[32] = {0};
    char debug_buf[128];
    uint16_t year;

    HAL_StatusTypeDef status;
    uint32_t start_tick = HAL_GetTick();
    uint8_t idx = 0;

    uart_print_debug(huart, "Ingrese fecha y hora [YYYYMMDDHHMMSS;]:\r\n");

    while ((HAL_GetTick() - start_tick) < 30000) {
        uint8_t ch;
        status = HAL_UART_Receive(huart, &ch, 1, 100);
        if (status == HAL_OK) {
            if (ch == ';') {
                rx_buffer[idx] = '\0';
                break;
            }
            if (idx < sizeof(rx_buffer) - 1) {
                rx_buffer[idx++] = ch;
            }
        }
    }

    if (idx < 14) {
        uart_print_debug(huart, "Tiempo expirado o entrada incompleta. Se mantiene hora RTC actual.\r\n");
        return false;
    }

    snprintf(debug_buf, sizeof(debug_buf), "[DEBUG] Entrada recibida: %s\r\n", rx_buffer);
    uart_print_debug(huart, debug_buf);

    // Limpiar caracteres como '\r', '\n' o ';'
    rx_buffer[strcspn(rx_buffer, "\r\n;")] = '\0';

    // Parseo manual
    char year_str[5] = {0}, month_str[3] = {0}, day_str[3] = {0};
    char hour_str[3] = {0}, min_str[3] = {0}, sec_str[3] = {0};

    strncpy(year_str,  rx_buffer +  0, 4);
    strncpy(month_str, rx_buffer +  4, 2);
    strncpy(day_str,   rx_buffer +  6, 2);
    strncpy(hour_str,  rx_buffer +  8, 2);
    strncpy(min_str,   rx_buffer + 10, 2);
    strncpy(sec_str,   rx_buffer + 12, 2);

    year = atoi(year_str);
    RTC_DateTypeDef date = {0};
    RTC_TimeTypeDef time = {0};

    date.Month     = atoi(month_str);
    date.Date      = atoi(day_str);
    time.Hours     = atoi(hour_str);
    time.Minutes   = atoi(min_str);
    time.Seconds   = atoi(sec_str);

    snprintf(debug_buf, sizeof(debug_buf),
             "Fecha parseada: %04u-%02u-%02u %02u:%02u:%02u\r\n",
             year, date.Month, date.Date,
             time.Hours, time.Minutes, time.Seconds);
    uart_print_debug(huart, debug_buf);

    if (year < 2000 || year > 2099) {
        uart_print_debug(huart, "Año fuera de rango\r\n");
        return false;
    }

    date.Year = (uint8_t)(year - 2000);
    date.WeekDay = 1;

    if (!RTC_DS3231_Set(&date, &time)) {
        uart_print_debug(huart, "Error al configurar el RTC.\r\n");
        return false;
    }

    uart_print_debug(huart, "RTC actualizado exitosamente.\r\n");
    return true;
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







