/**
 * @file rtc_ds3231_for_stm32_hal.c
 * @brief Biblioteca HAL para el DS3231 con STM32: lectura de hora, fecha y temperatura.
 *
 * Esta implementación permite manejar el reloj en tiempo real DS3231 usando la biblioteca HAL de STM32,
 * incluyendo lectura de registros de tiempo, temperatura interna y verificación de presencia del dispositivo.
 *
 * Funciones incluidas:
 * - DS3231_Init: Inicializa el RTC con el handler I2C.
 * - DS3231_IsConnected: Verifica si el DS3231 responde.
 * - DS3231_GetDateTime: Obtiene la hora y fecha actual.
 * - DS3231_SetDateTime: Configura manualmente fecha y hora.
 * - DS3231_GetTemperature: Lee la temperatura interna del RTC.
 * - rtc_get_time: Obtiene fecha/hora formateada ISO 8601.
 * - rtc_set_time_from_uart: Configura el RTC a partir de una cadena recibida vía UART.
 * - rtc_set_test_time: Carga una fecha fija de prueba para verificar funcionamiento.
 *
 * Reemplaza la funcionalidad previa del DS1307.
 *
 * @author Luis Gómez
 * @date 2025-05-13
 * @license SPDX-License-Identifier: GPL-3.0-only
 */

#include "rtc_config.h"
#include "rtc_ds3231_for_stm32_hal.h"
#include "time_rtc.h"
#include "uart_printing.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>




static I2C_HandleTypeDef* _ds3231_i2c = NULL;

void DS3231_Init(I2C_HandleTypeDef *hi2c) {
    _ds3231_i2c = hi2c;
}

bool DS3231_IsConnected(void) {
    return HAL_I2C_IsDeviceReady(_ds3231_i2c, DS3231_I2C_ADDR << 1, 3, DS3231_TIMEOUT) == HAL_OK;
}

uint8_t DS3231_GetRegByte(uint8_t regAddr) {
    uint8_t val;
    HAL_I2C_Master_Transmit(_ds3231_i2c, DS3231_I2C_ADDR << 1, &regAddr, 1, DS3231_TIMEOUT);
    HAL_I2C_Master_Receive(_ds3231_i2c, DS3231_I2C_ADDR << 1, &val, 1, DS3231_TIMEOUT);
    return val;
}

void DS3231_SetRegByte(uint8_t regAddr, uint8_t val) {
    uint8_t data[2] = {regAddr, val};
    HAL_I2C_Master_Transmit(_ds3231_i2c, DS3231_I2C_ADDR << 1, data, 2, DS3231_TIMEOUT);
}

uint8_t DS3231_DecodeBCD(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

uint8_t DS3231_EncodeBCD(uint8_t dec) {
    return ((dec / 10) << 4) | (dec % 10);
}

bool DS3231_GetDateTime(DS3231_DateTime *dt)
{
    uint8_t raw;

    raw = DS3231_GetRegByte(DS3231_REG_SECOND);
    if (raw == 0xFF) return false;
    dt->seconds = DS3231_DecodeBCD(raw);

    raw = DS3231_GetRegByte(DS3231_REG_MINUTE);
    if (raw == 0xFF) return false;
    dt->minutes = DS3231_DecodeBCD(raw);

    raw = DS3231_GetRegByte(DS3231_REG_HOUR);
    if (raw == 0xFF) return false;
    dt->hours = DS3231_DecodeBCD(raw & 0x3F); // Formato 24h, sin bit AM/PM

    raw = DS3231_GetRegByte(DS3231_REG_DATE);
    if (raw == 0xFF) return false;
    dt->day = DS3231_DecodeBCD(raw);

    raw = DS3231_GetRegByte(DS3231_REG_MONTH);
    if (raw == 0xFF) return false;
    dt->month = DS3231_DecodeBCD(raw & 0x1F); // Bit 7 = siglo (ignorado)

    raw = DS3231_GetRegByte(DS3231_REG_YEAR);
    if (raw == 0xFF) return false;
    dt->year = 2000 + DS3231_DecodeBCD(raw); // Año 20xx

    return true;
}

bool DS3231_SetDateTime(const DS3231_DateTime *dt) {
    uint8_t buffer[7];

    buffer[0] = DS3231_EncodeBCD(dt->seconds);
    buffer[1] = DS3231_EncodeBCD(dt->minutes);
    buffer[2] = DS3231_EncodeBCD(dt->hours);
    buffer[3] = 1;  // Día de la semana fijo
    buffer[4] = DS3231_EncodeBCD(dt->day);
    buffer[5] = DS3231_EncodeBCD(dt->month);
    buffer[6] = DS3231_EncodeBCD(dt->year % 100);

    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(
        _ds3231_i2c,
        DS3231_I2C_ADDR << 1,
        DS3231_REG_SECOND,
        I2C_MEMADD_SIZE_8BIT,
        buffer,
        sizeof(buffer),
        HAL_MAX_DELAY
    );

    if (status != HAL_OK) {
        uart_logger.print(&uart_logger, "[RTC] Error al escribir en DS3231\r\n");
        return false;
    }
    return true;
}

float DS3231_GetTemperature(void) {
    int8_t msb = DS3231_GetRegByte(DS3231_REG_TEMP_MSB);
    uint8_t lsb = DS3231_GetRegByte(DS3231_REG_TEMP_LSB);
    return msb + ((lsb >> 6) * 0.25f);
}

void rtc_get_time(char *buffer, size_t len) {
    DS3231_DateTime dt;
    DS3231_GetDateTime(&dt);
    snprintf(buffer, len, "%04d-%02d-%02d %02d:%02d:%02d",
             dt.year, dt.month, dt.day,
             dt.hours, dt.minutes, dt.seconds);
}


/**
 * @brief Parsea una cadena con formato YYYYMMDDHHMMSS y actualiza el RTC.
 *
 * Esta función interpreta una cadena de texto recibida desde UART (o cualquier fuente)
 * con formato fijo `YYYYMMDDHHMMSS`, valida sus componentes (año, mes, día, hora, minutos y segundos)
 * y actualiza el RTC externo DS3231 con esos valores.
 *
 * Se reportan mensajes de depuración y errores mediante `uart_logger`.
 *
 * @param[in] input_str Cadena con la fecha y hora en formato YYYYMMDDHHMMSS;
 *                      Ejemplo válido: "20250515102530"
 */

#include <ctype.h>  // Para isdigit()

void rtc_set_time_from_uart(const char *input_str) {
    DS3231_DateTime dt;
    char debug_buf[RTC_DEBUG_BUFFER_SIZE];

    // Limpieza del string: verificar longitud y que todos sean dígitos
    size_t len = strlen(input_str);
    if (len != 14) {
        uart_logger.print(&uart_logger, "[ERROR] Longitud incorrecta. Debe tener 14 dígitos (YYYYMMDDHHMMSS).\r\n");
        return;
    }

    for (size_t i = 0; i < len; ++i) {
        if (!isdigit((unsigned char)input_str[i])) {
            uart_logger.print(&uart_logger, "[ERROR] El string contiene caracteres no numéricos.\r\n");
            return;
        }
    }

#if RTC_DEBUG_ENABLED
    snprintf(debug_buf, sizeof(debug_buf), RTC_MSG_INPUT_DEBUG, input_str);
    uart_logger.print(&uart_logger, debug_buf);
#endif

    // Parseo del string

    int y, mo, d, h, mi, s;
    int parsed = sscanf(input_str, "%4d%2d%2d%2d%2d%2d", &y, &mo, &d, &h, &mi, &s);

    if (parsed != 6) {
        snprintf(debug_buf, sizeof(debug_buf), "[ERROR] sscanf falló. Valores parseados: %d\r\n", parsed);
        uart_logger.print(&uart_logger, debug_buf);
        uart_logger.print(&uart_logger, RTC_MSG_PARSE_ERROR);
        return;
    }

    // Conversión segura
    dt.year    = (uint16_t)y;
    dt.month   = (uint8_t)mo;
    dt.day     = (uint8_t)d;
    dt.hours   = (uint8_t)h;
    dt.minutes = (uint8_t)mi;
    dt.seconds = (uint8_t)s;

    if (parsed != 6) {
        snprintf(debug_buf, sizeof(debug_buf), "[ERROR] sscanf falló. Valores parseados: %d\r\n", parsed);
        uart_logger.print(&uart_logger, debug_buf);
        uart_logger.print(&uart_logger, RTC_MSG_PARSE_ERROR);
        return;
    }

    // Validación de campos
    if (dt.year < RTC_YEAR_MIN || dt.year > RTC_YEAR_MAX ||
        dt.month < 1 || dt.month > 12 ||
        dt.day < 1 || dt.day > 31 ||
        dt.hours > 23 || dt.minutes > 59 || dt.seconds > 59) {

#if RTC_DEBUG_ENABLED
        snprintf(debug_buf, sizeof(debug_buf), RTC_MSG_DATE_INVALID,
                 dt.year, dt.month, dt.day, dt.hours, dt.minutes, dt.seconds);
        uart_logger.print(&uart_logger, debug_buf);
#endif
        return;
    }

#if RTC_DEBUG_ENABLED
    snprintf(debug_buf, sizeof(debug_buf), RTC_MSG_DATE_PARSED,
             dt.year, dt.month, dt.day, dt.hours, dt.minutes, dt.seconds);
    uart_logger.print(&uart_logger, debug_buf);
#endif

    // Actualizar RTC
    if (DS3231_SetDateTime(&dt)) {
        uart_logger.print(&uart_logger, RTC_MSG_SET_SUCCESS);
    } else {
        uart_logger.print(&uart_logger, RTC_MSG_SET_FAIL);
    }
}


void rtc_set_test_time(void) {
#if RTC_TEST_ENABLED
    DS3231_DateTime dt = {
        .year = 2025,
        .month = 5,
        .day = 14,
        .hours = 21,
        .minutes = 45,
        .seconds = 0
    };

    dt = rtc_get_compile_time();

    uart_logger.print(&uart_logger, "[TEST] Intentando configurar RTC con fecha fija...\r\n");

    if (DS3231_SetDateTime(&dt)) {
        uart_logger.print(&uart_logger, "[TEST] RTC configurado con exito.\r\n");

        DS3231_DateTime verif;
        DS3231_GetDateTime(&verif);

        char buffer[64];
        snprintf(buffer, sizeof(buffer),
                 "[TEST] Verificacion: %04d-%02d-%02d %02d:%02d:%02d\r\n",
                 verif.year, verif.month, verif.day,
                 verif.hours, verif.minutes, verif.seconds);
        uart_logger.print(&uart_logger, buffer);


        verif = rtc_get_compile_time();

        snprintf(buffer, sizeof(buffer),
                      "[TEST] Verificacion fecha compilacion: %04d-%02d-%02d %02d:%02d:%02d\r\n",
                      verif.year, verif.month, verif.day,
                      verif.hours, verif.minutes, verif.seconds);
             uart_logger.print(&uart_logger, buffer);


    } else {
        uart_logger.print(&uart_logger, "[TEST] Error al configurar RTC.\r\n");
    }
#endif
}


/**
 * @brief Devuelve una estructura DS3231_DateTime con la fecha y hora de compilación.
 *
 * @return DS3231_DateTime con campos cargados desde __DATE__ y __TIME__.
 */
DS3231_DateTime rtc_get_compile_time(void) {
    DS3231_DateTime dt = {0};
    char month_str[4] = {0};
    int day, year;

    // __DATE__ = "May 15 2025"
    if (sscanf(__DATE__, "%3s %d %d", month_str, &day, &year) == 3) {
        dt.day = (uint8_t)day;
        dt.year = (uint16_t)year;

        const char* months[] = {
            "Jan", "Feb", "Mar", "Apr", "May", "Jun",
            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
        };

        for (uint8_t i = 0; i < 12; ++i) {
            if (strncmp(month_str, months[i], 3) == 0) {
                dt.month = i + 1;
                break;
            }
        }
    }

    int h, m, s;
    if (sscanf(__TIME__, "%d:%d:%d", &h, &m, &s) == 3) {
        dt.hours = (uint8_t)h;
        dt.minutes = (uint8_t)m;
        dt.seconds = (uint8_t)s;
    }

    return dt;
}


bool RTC_DS3231_Set(RTC_DateTypeDef *date, RTC_TimeTypeDef *time)
{
    DS3231_DateTime dt;

    dt.year    = 2000 + date->Year;
    dt.month   = date->Month;
    dt.day     = date->Date;
    dt.hours   = time->Hours;
    dt.minutes = time->Minutes;
    dt.seconds = time->Seconds;

    bool resultado = DS3231_SetDateTime(&dt);

    if (!resultado) {
        uart_print_debug(&huart3, "Error al escribir al DS3231\r\n");
    }

    return resultado;
}

bool RTC_DS3231_Get(RTC_DateTypeDef *date, RTC_TimeTypeDef *time)
{
    DS3231_DateTime dt;

    if (!DS3231_GetDateTime(&dt)) {
        uart_print_debug(&huart3, "Error al leer desde el DS3231\r\n");
        return false;
    }

    // Convertir a estructuras HAL
    date->Year    = (uint8_t)(dt.year - 2000);  // HAL espera [0..99]
    date->Month   = dt.month;
    date->Date    = dt.day;
    date->WeekDay = 1;  // por defecto, o calcular si lo deseas

    time->Hours   = dt.hours;
    time->Minutes = dt.minutes;
    time->Seconds = dt.seconds;

    return true;
}



#ifdef __cplusplus
}
#endif
