/*
 * Nombre del archivo: rtc_ds1307_for_stm32_hal.h
 * Descripción: Este programa esta orientada a manejar el RTC ds1307
 *              este programa es una adaptación de @eepj www.github.com/eepj
 * Autor: Luis Gómez P.
 * Derechos de Autor: (C) 2024 Luis Gómez P. EIRL
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

/* An STM32 HAL library written for the DS1307 real-time clock IC. */
/* Library by @eepj www.github.com/eepj */

#ifndef RTC_DS1307_FOR_STM32_HAL_H
#define RTC_DS1307_FOR_STM32_HAL_H
//#include "main.h"

#include "i2c.h"

/*----------------------------------------------------------------------------*/
/** @file
 * @brief Definiciones y funciones para la gestión del RTC DS1307 en STM32 HAL.
 */

/* === Headers files inclusions ================================================================ */


/** @brief Dirección I2C del DS1307. */
#define DS1307_I2C_ADDR    0x68

/** @brief Dirección del registro de segundos del DS1307. */
#define DS1307_REG_SECOND  0x00

/** @brief Dirección del registro de minutos del DS1307. */
#define DS1307_REG_MINUTE  0x01

/** @brief Dirección del registro de horas del DS1307. */
#define DS1307_REG_HOUR    0x02

/** @brief Dirección del registro del día de la semana del DS1307. */
#define DS1307_REG_DOW     0x03

/** @brief Dirección del registro de fecha (día del mes) del DS1307. */
#define DS1307_REG_DATE    0x04

/** @brief Dirección del registro del mes del DS1307. */
#define DS1307_REG_MONTH   0x05

/** @brief Dirección del registro del año del DS1307. */
#define DS1307_REG_YEAR    0x06

/** @brief Dirección del registro de control del DS1307. */
#define DS1307_REG_CONTROL 0x07

/** @brief Dirección del registro de la hora UTC del DS1307. */
#define DS1307_REG_UTC_HR  0x08

/** @brief Dirección del registro de minutos UTC del DS1307. */
#define DS1307_REG_UTC_MIN 0x09

/** @brief Dirección del registro de siglo del DS1307. */
#define DS1307_REG_CENT    0x10

/** @brief Dirección del registro de RAM del DS1307. */
#define DS1307_REG_RAM     0x11

/** @brief Tiempo de espera para las operaciones de comunicación I2C con DS1307. */
#define DS1307_TIMEOUT     1000

/* === Cabecera C++ ============================================================================ */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */

/* === Public data type declarations =========================================================== */

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

/**
 * @file rtc_ds1307_for_stm32_hal.h
 * @brief Definiciones de interfaz para el control del RTC DS1307 con HAL de STM32.
 */

/**
 * @brief Puntero global al manejador de I2C utilizado por el DS1307.
 *
 * Este puntero se debe inicializar en la aplicación antes de llamar a DS1307_Init para
 * asegurar que las funciones de este módulo puedan comunicarse correctamente con el hardware del DS1307.
 */
extern I2C_HandleTypeDef * _ds1307_ui2c;

/**
 * @brief Enumeración de las tasas de interrupción disponibles para la salida de onda cuadrada del DS1307.
 *
 * Estos valores configuran la frecuencia de la onda cuadrada generada por el DS1307, la cual puede ser
 * útil para tareas de temporización externa o para generar interrupciones periódicas.
 */
typedef enum DS1307_Rate {
    DS1307_1HZ,     /**< Onda cuadrada a 1 Hz */
    DS1307_4096HZ,  /**< Onda cuadrada a 4096 Hz */
    DS1307_8192HZ,  /**< Onda cuadrada a 8192 Hz */
    DS1307_32768HZ  /**< Onda cuadrada a 32768 Hz */
} DS1307_Rate;

/**
 * @brief Enumeración para habilitar o deshabilitar la salida de onda cuadrada del DS1307.
 *
 * Estos valores permiten controlar si la salida de onda cuadrada está habilitada o no. La onda
 * cuadrada se emite a través del pin SQW del DS1307 y puede configurarse a diferentes frecuencias
 * usando DS1307_Rate.
 */
typedef enum DS1307_SquareWaveEnable {
    DS1307_DISABLED, /**< Salida de onda cuadrada deshabilitada */
    DS1307_ENABLED   /**< Salida de onda cuadrada habilitada */
} DS1307_SquareWaveEnable;


/**
 * @brief Inicializa el módulo DS1307.
 *
 * Configura el módulo DS1307 para empezar a funcionar activando el reloj si está detenido.
 * @param hi2c Puntero a la estructura de manejo I2C que se utiliza para la comunicación.
 */
void DS1307_Init(I2C_HandleTypeDef * hi2c);

/**
 * @brief Establece el estado de parada del reloj del DS1307.
 *
 * Permite iniciar o detener el reloj del DS1307 mediante la configuración del bit de parada.
 * @param halt Valor que determina el estado del reloj: 0 para iniciar, 1 para detener.
 */
void DS1307_SetClockHalt(uint8_t halt);

/**
 * @brief Obtiene el estado actual del bit de parada del reloj del DS1307.
 *
 * @return Estado del bit de parada del reloj: 0 si el reloj está en funcionamiento, 1 si está detenido.
 */
uint8_t DS1307_GetClockHalt(void);

/**
 * @brief Escribe un byte en un registro específico del DS1307.
 *
 * Esta función permite modificar el contenido de un registro del DS1307.
 * @param regAddr Dirección del registro a modificar.
 * @param val Valor a escribir en el registro.
 */
void DS1307_SetRegByte(uint8_t regAddr, uint8_t val);

/**
 * @brief Lee un byte de un registro específico del DS1307.
 *
 * Permite leer el contenido actual de cualquier registro del DS1307.
 * @param regAddr Dirección del registro que se desea leer.
 * @return Valor actual almacenado en el registro especificado.
 */
uint8_t DS1307_GetRegByte(uint8_t regAddr);

/**
 * @brief Habilita o deshabilita la salida de onda cuadrada del DS1307.
 *
 * Configura el pin SQW/OUT para emitir una onda cuadrada o para permanecer bajo.
 * @param mode Modo de la onda cuadrada: DS1307_DISABLED para desactivar, DS1307_ENABLED para activar.
 */
void DS1307_SetEnableSquareWave(DS1307_SquareWaveEnable mode);

/**
 * @brief Establece la frecuencia de la onda cuadrada de salida del DS1307.
 *
 * Ajusta la frecuencia de la onda cuadrada generada por el DS1307.
 * @param rate Frecuencia de la onda cuadrada a configurar.
 */
void DS1307_SetInterruptRate(DS1307_Rate rate);


/**
 * @brief Obtiene el día actual de la semana del DS1307.
 *
 * El día de la semana es retornado como un número del 0 al 6, donde 0 representa el domingo.
 * @return Número del día de la semana, de 0 (domingo) a 6 (sábado).
 */
uint8_t DS1307_GetDayOfWeek(void);

/**
 * @brief Obtiene el día del mes actual del DS1307.
 *
 * Esta función devuelve el día del mes del DS1307, que varía entre 1 y 31 dependiendo del mes.
 * @return Día del mes actual, en el rango de 1 a 31.
 */
uint8_t DS1307_GetDate(void);

/**
 * @brief Obtiene el mes actual del DS1307.
 *
 * Esta función devuelve el mes actual almacenado en el DS1307, con 1 siendo enero y 12 diciembre.
 * @return Mes actual, en el rango de 1 (enero) a 12 (diciembre).
 */
uint8_t DS1307_GetMonth(void);

/**
 * @brief Obtiene el año actual del DS1307.
 *
 * Devuelve el año completo actual almacenado en el DS1307. La función combina el registro de centuria
 * y el registro de año para formar un número de cuatro dígitos.
 * @return Año actual, que abarca desde 2000 hasta 2099.
 */
uint16_t DS1307_GetYear(void);


/**
 * @brief Obtiene la hora actual en formato de 24 horas del DS1307.
 *
 * Esta función devuelve la hora actual almacenada en el DS1307. El formato retornado es de 24 horas,
 * oscilando entre 0 y 23.
 * @return Hora actual, en el rango de 0 a 23.
 */
uint8_t DS1307_GetHour(void);

/**
 * @brief Obtiene los minutos actuales del DS1307.
 *
 * Esta función devuelve los minutos actuales almacenados en el DS1307, los cuales varían de 0 a 59.
 * @return Minutos actuales, en el rango de 0 a 59.
 */
uint8_t DS1307_GetMinute(void);

/**
 * @brief Obtiene los segundos actuales del DS1307.
 *
 * Devuelve los segundos actuales del DS1307, que varían de 0 a 59. Esta función excluye el bit de parada del reloj.
 * @return Segundos actuales, en el rango de 0 a 59.
 */
uint8_t DS1307_GetSecond(void);

/**
 * @brief Obtiene la hora de la zona horaria UTC almacenada en el DS1307.
 *
 * Devuelve la diferencia en horas entre la hora local y la hora UTC configurada en el DS1307.
 * El rango de valores va de -12 a +12, representando las zonas horarias alrededor del mundo.
 * @return Diferencia horaria UTC, en el rango de -12 a +12.
 */
int8_t DS1307_GetTimeZoneHour(void);

/**
 * @brief Obtiene los minutos de la zona horaria UTC almacenada en el DS1307.
 *
 * Devuelve los minutos de ajuste para la zona horaria UTC configurada en el DS1307.
 * Esto puede ser útil para ajustes de zona horaria que incluyen minutos, como la zona horaria de Nepal (UTC+5:45).
 * @return Minutos de ajuste de la zona horaria UTC, en el rango de 0 a 59.
 */
uint8_t DS1307_GetTimeZoneMin(void);


/**
 * @brief Establece el día de la semana en el DS1307.
 *
 * Configura el día de la semana en el DS1307, donde 0 representa el domingo y 6 el sábado.
 * @param dow Día de la semana a establecer, en el rango de 0 (domingo) a 6 (sábado).
 */
void DS1307_SetDayOfWeek(uint8_t dow);

/**
 * @brief Establece el día del mes en el DS1307.
 *
 * Configura el día del mes en el DS1307, que puede variar de 1 a 31 dependiendo del mes y el año.
 * @param date Día del mes a establecer, en el rango de 1 a 31.
 */
void DS1307_SetDate(uint8_t date);

/**
 * @brief Establece el mes en el DS1307.
 *
 * Configura el mes en el DS1307, donde 1 corresponde a enero y 12 a diciembre.
 * @param month Mes a establecer, en el rango de 1 (enero) a 12 (diciembre).
 */
void DS1307_SetMonth(uint8_t month);

/**
 * @brief Establece el año en el DS1307.
 *
 * Configura el año en el DS1307. El año se debe proporcionar en formato completo de cuatro dígitos.
 * @param year Año a establecer, en el rango de 2000 a 2099.
 */
void DS1307_SetYear(uint16_t year);


/**
 * @brief Establece la hora en el DS1307.
 *
 * Configura la hora actual del DS1307 en formato de 24 horas. El rango válido es de 0 a 23.
 * @param hour_24mode Hora a establecer, en formato de 24 horas, rango de 0 a 23.
 */
void DS1307_SetHour(uint8_t hour_24mode);

/**
 * @brief Establece los minutos en el DS1307.
 *
 * Configura los minutos actuales del DS1307. El rango válido es de 0 a 59.
 * @param minute Minutos a establecer, en el rango de 0 a 59.
 */
void DS1307_SetMinute(uint8_t minute);

/**
 * @brief Establece los segundos en el DS1307.
 *
 * Configura los segundos actuales del DS1307. El rango válido es de 0 a 59.
 * @param second Segundos a establecer, en el rango de 0 a 59.
 */
void DS1307_SetSecond(uint8_t second);

/**
 * @brief Establece la zona horaria UTC en el DS1307.
 *
 * Configura la compensación de la zona horaria UTC en el DS1307. Esto es útil para ajustar el reloj
 * a la hora local considerando la diferencia con la hora UTC. La hora se puede ajustar desde -12 hasta +12,
 * y los minutos desde 0 hasta 59.
 * @param hr Diferencia de horas respecto a UTC, puede ser negativa o positiva, rango de -12 a 12.
 * @param min Diferencia de minutos respecto a UTC, rango de 0 a 59.
 */
void DS1307_SetTimeZone(int8_t hr, uint8_t min);


/**
 * @brief Decodifica un valor en formato BCD a decimal.
 *
 * Esta función convierte un valor almacenado en formato BCD (Binary-Coded Decimal) a su equivalente decimal.
 * El formato BCD permite representar cada dígito de un número decimal en un nibble (4 bits), facilitando operaciones
 * aritméticas en sistemas que manipulan datos digitales.
 * @param bin Valor en formato BCD que se desea convertir.
 * @return Valor decimal equivalente al número BCD proporcionado.
 */
uint8_t DS1307_DecodeBCD(uint8_t bin);

/**
 * @brief Codifica un valor decimal a formato BCD.
 *
 * Esta función convierte un número decimal a su representación en formato BCD (Binary-Coded Decimal).
 * Este formato es útil para la comunicación con ciertos dispositivos de hardware, como el DS1307, que
 * requieren que los datos numéricos sean enviados en formato BCD.
 * @param dec Número decimal que se desea convertir a BCD.
 * @return Valor en formato BCD equivalente al número decimal proporcionado.
 */
uint8_t DS1307_EncodeBCD(uint8_t dec);

/* === End of documentation ==================================================================== */
#endif
