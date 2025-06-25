/**
 * @file time_rtc.h
 * @brief Funciones para gestión automática del RTC en STM32 (DS1307/DS3231).
 *
 * Este archivo proporciona interfaces para inicializar, configurar y leer la hora desde un RTC
 * externo (como el DS3231) o el RTC interno del STM32F429ZI. Permite obtener la hora actual
 * formateada, actualizar el RTC desde UART y realizar pruebas de validación.
 *
 * ### Funciones incluidas:
 * - rtc_auto_init(): Inicializa el RTC disponible (externo si responde, interno si no).
 * - rtc_get_time(): Obtiene la fecha y hora en formato ISO 8601.
 * - obtener_fecha_hora(): Alias para compatibilidad con código anterior.
 * - time_rtc_Init(): Inicializa el módulo RTC (interno o externo).
 * - time_rtc_GetFormattedDateTime(): Devuelve cadena formateada con fecha y hora.
 * - RTC_ReceiveTimeFromTerminal(): Solicita al usuario la fecha y hora por UART y la establece.
 * - rtc_set_test_time(): Carga una hora de prueba fija para validar el funcionamiento del RTC.
 *
 * ### Posibles ampliaciones futuras:
 * - Funciones para configurar alarmas en RTC.
 * - Manejo de SRAM integrada en módulos como el DS3231.
 * - Sincronización con fuentes externas (GPS, NTP).
 *
 * @author Luis Gómez
 * @date 2024-06-08
 * @copyright (C) 2024 Luis Gómez
 * @license GNU General Public License v3.0 (SPDX-License-Identifier: GPL-3.0-only)
 */

#ifndef TIME_RTC_H
#define TIME_RTC_H

#include <stdbool.h>
#include <stddef.h>
#include "usart.h"

/**
 * @brief Fuente de tiempo utilizada actualmente.
 */
typedef enum {
    RTC_SOURCE_INTERNAL, /**< RTC interno del STM32 */
    RTC_SOURCE_EXTERNAL  /**< RTC externo (DS3231) */
} RTC_Source;

/**
 * @brief Variable global para saber qué fuente de tiempo está activa.
 */
extern RTC_Source active_rtc;

/**
 * @brief Inicializa automáticamente el RTC activo (interno o externo).
 */
bool rtc_auto_init(void);

/**
 * @brief Indica si el RTC (interno o externo) está operativo.
 * @return true si el RTC está funcionando correctamente.
 */
bool rtc_esta_activo(void);

/**
 * @brief Obtiene la hora actual formateada desde el RTC activo.
 * @param buffer Puntero al buffer donde se guardará la cadena de fecha/hora.
 * @param len Longitud máxima del buffer.
 */
void rtc_get_time(char * buffer, size_t len);

/**
 * @brief Alias para rtc_get_time() con nombre legado.
 * @param fecha_hora_str Cadena donde se almacena la fecha/hora.
 */
void obtener_fecha_hora(char * fecha_hora_str);

/**
 * @brief Inicializa el módulo RTC completo.
 */
void time_rtc_Init(void);

/**
 * @brief Devuelve la hora actual del sistema en formato ISO 8601.
 * @param buffer Puntero al buffer donde se almacena el resultado.
 * @param len Tamaño máximo del buffer.
 */
void time_rtc_GetFormattedDateTime(char * buffer, size_t len);

/**
 * @brief Solicita fecha y hora desde terminal UART e intenta configurarla.
 */
bool RTC_ReceiveTimeFromTerminal(UART_HandleTypeDef * huart);

/**
 * @brief Establece una fecha y hora fija para pruebas del RTC.
 */
void rtc_set_test_time(void);

/**
 * @brief Evalúa la hora actual y cambia el estado del sistema según límites de tiempo (cada 10min,
 * hora, día).
 */
void time_rtc_ActualizarEstadoPorTiempo(void);

bool time_rtc_hay_cambio_bloque(void);

#endif /* TIME_RTC_H */
