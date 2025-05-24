/**
 * @file rtc_ds3231_for_stm32_hal.h
 * @brief Interfaz HAL para el reloj en tiempo real DS3231 con STM32.
 *
 * Este archivo define funciones para comunicarse con el módulo DS3231 a través del bus I2C
 * utilizando la HAL de STM32. Permite inicializar el módulo, leer y configurar la fecha/hora,
 * obtener la temperatura interna del RTC y realizar operaciones de bajo nivel sobre registros.
 * También incluye funciones de prueba y configuración a través de UART.
 *
 * ### Funciones principales:
 * - DS3231_Init(): Inicializa el manejador I2C para el módulo DS3231.
 * - DS3231_GetDateTime(): Obtiene fecha y hora actuales.
 * - DS3231_SetDateTime(): Configura una nueva fecha/hora.
 * - DS3231_GetTemperature(): Lee la temperatura interna.
 *
 * ### Funciones de bajo nivel:
 * - DS3231_GetRegByte(): Lee un byte desde un registro específico.
 * - DS3231_SetRegByte(): Escribe un byte en un registro específico.
 * - DS3231_DecodeBCD(): Convierte un byte en formato BCD a decimal.
 * - DS3231_EncodeBCD(): Convierte un valor decimal a BCD.
 *
 * ### Funciones auxiliares:
 * - rtc_set_time_from_uart(): Configura la hora basada en una entrada UART del usuario.
 * - RTC_ReceiveTimeFromTerminal(): Solicita interactivamente una fecha/hora por UART.
 * - rtc_set_test_time(): Configura una fecha/hora predefinida para validación de funcionamiento.
 *
 * @author Luis Gómez
 * @date 2025-05-13
 * @license GNU General Public License v3.0 (SPDX-License-Identifier: GPL-3.0-only)
 */
#ifndef RTC_DS3231_FOR_STM32_HAL_H_
#define RTC_DS3231_FOR_STM32_HAL_H_
/** @file
 ** @brief 
 **/

/* === Headers files inclusions ================================================================ */
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
/* === Cabecera C++ ============================================================================ */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */
#define DS3231_I2C_ADDR     0x68

#define DS3231_REG_SECOND   0x00
#define DS3231_REG_MINUTE   0x01
#define DS3231_REG_HOUR     0x02
#define DS3231_REG_DAY      0x03
#define DS3231_REG_DATE     0x04
#define DS3231_REG_MONTH    0x05
#define DS3231_REG_YEAR     0x06
#define DS3231_REG_TEMP_MSB 0x11
#define DS3231_REG_TEMP_LSB 0x12
#define DS3231_TIMEOUT      100
/* === Public data type declarations =========================================================== */
/* === Estructura de fecha y hora ============================================================= */

typedef struct {
    uint8_t seconds;  /**< Segundos (0–59) */
    uint8_t minutes;  /**< Minutos (0–59) */
    uint8_t hours;    /**< Horas (0–23) */
    uint8_t day;      /**< Día del mes (1–31) */
    uint8_t month;    /**< Mes (1–12) */
    uint16_t year;    /**< Año (e.g. 2025) */
} DS3231_DateTime;


typedef struct {
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} ds3231_time_t;
/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */
/**
 * @brief Inicializa el DS3231 con el manejador I2C especificado.
 * @param hi2c Puntero al manejador I2C configurado.
 */
void DS3231_Init(I2C_HandleTypeDef *hi2c);

/**
 * @brief Verifica si el DS3231 responde en el bus I2C.
 * @retval true  Si el dispositivo está conectado.
 * @retval false Si no hay respuesta.
 */
bool DS3231_IsConnected(void);

/**
 * @brief Lee la fecha y hora desde el DS3231.
 * @param[out] dt Puntero a una estructura donde se almacenarán los datos.
 */

bool DS3231_GetDateTime(DS3231_DateTime *dt);


/**
*@brief actualizar fecha y hora
*/
bool DS3231_SetDateTime(const DS3231_DateTime *dt);

/**
 * @brief Obtiene la temperatura interna del DS3231.
 * @return Temperatura en grados Celsius.
 */
float DS3231_GetTemperature(void);

/* === Funciones de bajo nivel (registro) ====================================================== */

/**
 * @brief Lee un byte desde un registro del DS3231.
 * @param regAddr Dirección del registro.
 * @return Byte leído.
 */
uint8_t DS3231_GetRegByte(uint8_t regAddr);

/**
 * @brief Escribe un byte en un registro del DS3231.
 * @param regAddr Dirección del registro.
 * @param val Valor a escribir.
 */
void DS3231_SetRegByte(uint8_t regAddr, uint8_t val);

/**
 * @brief Convierte un valor BCD a decimal.
 * @param bcd Valor BCD.
 * @return Valor decimal.
 */
uint8_t DS3231_DecodeBCD(uint8_t bcd);

/**
 * @brief Convierte un valor decimal a BCD.
 * @param dec Valor decimal.
 * @return Valor codificado en BCD.
 */
uint8_t DS3231_EncodeBCD(uint8_t dec);

/**
 * @brief Interfaz por UART para configurar la hora manualmente.
 * Espera un formato: YYYY-MM-DD HH:MM:SS\n
 */
void rtc_set_time_from_uart(const char *input_str);

/**
 * @brief Establece una fecha/hora fija predefinida en el RTC para propósitos de prueba.
 *
 * Se utiliza para verificar que la función de escritura sobre el RTC funciona correctamente
 * sin requerir interacción del usuario.
 */
void rtc_set_test_time(void);

/**
 * @brief Obtiene la fecha y hora de compilación del firmware.
 *
 * Esta función devuelve un objeto `DS3231_DateTime` con la fecha y hora
 * en la que se compiló el firmware, utilizando macros como `__DATE__` y `__TIME__`.
 * Es útil para inicializar el RTC con la fecha de firmware en ausencia de configuración previa.
 *
 * @return Estructura DS3231_DateTime con los valores extraídos de compilación.
 */
DS3231_DateTime rtc_get_compile_time(void);


/**
 * @brief Establece la fecha y hora en el RTC DS3231 utilizando estructuras HAL.
 *
 * Esta función convierte los valores desde las estructuras `RTC_DateTypeDef`
 * y `RTC_TimeTypeDef` (formato HAL de STM32) a la estructura interna
 * `DS3231_DateTime`, y luego los escribe en el RTC.
 *
 * @param[in] date Puntero a estructura HAL con la fecha.
 * @param[in] time Puntero a estructura HAL con la hora.
 *
 * @retval true Si la operación de escritura fue exitosa.
 * @retval false Si ocurrió un error al escribir en el RTC.
 */
bool RTC_DS3231_Set(RTC_DateTypeDef *date, RTC_TimeTypeDef *time);


/**
 * @brief Obtiene la fecha y hora actuales desde el RTC DS3231 en formato HAL.
 *
 * Esta función lee el RTC y llena las estructuras `RTC_DateTypeDef` y
 * `RTC_TimeTypeDef` con los valores actuales del reloj en formato STM32 HAL.
 *
 * @param[out] date Puntero a estructura donde se guardará la fecha.
 * @param[out] time Puntero a estructura donde se guardará la hora.
 *
 * @retval true Si la lectura fue exitosa.
 * @retval false Si ocurrió un error al leer desde el RTC.
 */
bool RTC_DS3231_Get(RTC_DateTypeDef *date, RTC_TimeTypeDef *time);


bool ds3231_get_time(uint8_t *hour, uint8_t *min, uint8_t *sec);

bool ds3231_get_date(uint8_t *day, uint8_t *month, uint16_t *year);

bool ds3231_get_datetime(ds3231_time_t *dt);




/* === End of documentation ==================================================================== */


#endif /* RTC_DS3231_FOR_STM32_HAL_H_ */
