/**
 * @file rtc_ds1307_for_stm32_hal.c
 * @brief Implementación de la biblioteca STM32 HAL para el circuito integrado de reloj en tiempo real DS1307.
 * @author Luis Gómez P.
 * @copyright (C) 2023 Luis Gómez P.
 * @license GNU General Public License v3.0
 *
 * Este programa es software libre: puedes redistribuirlo y/o modificarlo bajo los términos de la Licencia
 * Pública General GNU publicada por la Free Software Foundation, ya sea la versión 3 de la Licencia, o
 * (a tu elección) cualquier versión posterior.
 *
 * Este programa se distribuye con la esperanza de que sea útil, pero SIN NINGUNA GARANTÍA; sin siquiera
 * la garantía implícita de COMERCIABILIDAD o APTITUD PARA UN PROPÓSITO PARTICULAR. Ver la Licencia Pública
 * General GNU para más detalles.
 *
 * Deberías haber recibido una copia de la Licencia Pública General GNU junto con este programa.
 * Si no es así, visita <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

/* === Headers files inclusions =============================================================== */
#include "rtc_ds1307_for_stm32_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* === Macros definitions ====================================================================== */
// No specific macros defined for this module

/* === Private data type declarations ========================================================== */
// No specific private data types

/* === Private variable declarations =========================================================== */
I2C_HandleTypeDef * _ds1307_ui2c;  ///< Manejador de I2C para el DS1307.

/* === Private function declarations =========================================================== */
// No private functions

/* === Public variable definitions ============================================================= */
// No public variables

/* === Private variable definitions ============================================================ */
// No private variables

/* === Private function implementation ========================================================= */
// No private functions

/* === Public function implementation ========================================================== */

void DS1307_Init(I2C_HandleTypeDef * hi2c) {
    _ds1307_ui2c = hi2c;
    DS1307_SetClockHalt(0);
}

void DS1307_SetClockHalt(uint8_t halt) {
    uint8_t ch = (halt ? 1 << 7 : 0);
    uint8_t currentSeconds = DS1307_GetRegByte(DS1307_REG_SECOND);
    DS1307_SetRegByte(DS1307_REG_SECOND, ch | (currentSeconds & 0x7F));
}

uint8_t DS1307_GetClockHalt(void) {
    return (DS1307_GetRegByte(DS1307_REG_SECOND) & 0x80) >> 7;
}



/**
 * @brief Escribe un valor en un registro específico del DS1307.
 *
 * Utiliza la comunicación I2C para transmitir un valor a la dirección de registro
 * especificada del DS1307. La dirección del dispositivo se desplaza para
 * el protocolo de comunicación I2C.
 *
 * @param regAddr Dirección del registro en el DS1307 donde escribir.
 * @param val Valor a escribir en el registro, en el rango de 0 a 255.
 */
void DS1307_SetRegByte(uint8_t regAddr, uint8_t val) {
    uint8_t bytes[2] = {regAddr, val};
    HAL_I2C_Master_Transmit(_ds1307_ui2c, DS1307_I2C_ADDR << 1, bytes, 2, DS1307_TIMEOUT);
}

/**
 * @brief Lee un byte del registro especificado del DS1307.
 *
 * Envía la dirección del registro a través de I2C y luego recibe el byte almacenado
 * en ese registro. La dirección del dispositivo también se desplaza para
 * cumplir con el protocolo I2C.
 *
 * @param regAddr Dirección del registro del DS1307 a leer.
 * @return El byte almacenado en el registro especificado, en el rango de 0 a 255.
 */
uint8_t DS1307_GetRegByte(uint8_t regAddr) {
    uint8_t val;
    HAL_I2C_Master_Transmit(_ds1307_ui2c, DS1307_I2C_ADDR << 1, &regAddr, 1, DS1307_TIMEOUT);
    HAL_I2C_Master_Receive(_ds1307_ui2c, DS1307_I2C_ADDR << 1, &val, 1, DS1307_TIMEOUT);
    return val;
}

/**
 * @brief Configura la salida de onda cuadrada en el pin SQW/OUT del DS1307.
 *
 * Activa o desactiva la salida de onda cuadrada mediante la modificación del
 * registro de control. El bit 4 del registro controla la activación de la salida,
 * siendo manipulado aquí según el modo deseado.
 *
 * @param mode Estado deseado para la salida de onda cuadrada: DS1307_ENABLED (1) o
 * DS1307_DISABLED (0).
 */
void DS1307_SetEnableSquareWave(DS1307_SquareWaveEnable mode) {
    uint8_t controlReg = DS1307_GetRegByte(DS1307_REG_CONTROL);
    uint8_t newControlReg = (controlReg & ~(1 << 4)) | ((mode & 1) << 4);
    DS1307_SetRegByte(DS1307_REG_CONTROL, newControlReg);
}

/**
 * @brief Establece la frecuencia de la salida de onda cuadrada en el DS1307.
 *
 * Configura la frecuencia de la salida de onda cuadrada en el pin SQW/OUT del DS1307.
 * Permite seleccionar entre varias frecuencias, configurando los dos bits menos
 * significativos del registro de control.
 *
 * @param rate Frecuencia de la onda cuadrada, puede ser:
 *             - DS1307_1HZ (0b00): 1 Hz
 *             - DS1307_4096HZ (0b01): 4096 Hz
 *             - DS1307_8192HZ (0b10): 8192 Hz
 *             - DS1307_32768HZ (0b11): 32768 Hz
 */
void DS1307_SetInterruptRate(DS1307_Rate rate) {
    uint8_t controlReg = DS1307_GetRegByte(DS1307_REG_CONTROL);
    uint8_t newControlReg = (controlReg & ~0x03) | rate;
    DS1307_SetRegByte(DS1307_REG_CONTROL, newControlReg);
}

/**
 * @brief Obtiene el día actual de la semana del DS1307.
 *
 * Lee y decodifica el día de la semana desde el registro correspondiente. Los días
 * están numerados desde 0 (domingo) hasta 6 (sábado).
 *
 * @return Día de la semana como un número entero entre 0 y 6.
 */
uint8_t DS1307_GetDayOfWeek(void) {
    return DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_DOW));
}

/**
 * @brief Obtiene el día actual del mes del DS1307.
 *
 * Lee y decodifica el día del mes desde el registro correspondiente. Los días
 * varían de 1 a 31 dependiendo del mes actual.
 *
 * @return Día del mes como un número entero entre 1 y 31.
 */
uint8_t DS1307_GetDate(void) {
    return DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_DATE));
}

/**
 * @brief Obtiene el mes actual del DS1307.
 *
 * Lee y decodifica el mes desde el registro correspondiente. Los meses están
 * numerados de 1 (enero) a 12 (diciembre).
 *
 * @return Mes actual como un número entero entre 1 y 12.
 */
uint8_t DS1307_GetMonth(void) {
    return DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_MONTH));
}
/**
 * @brief Obtiene el año actual configurado en el DS1307.
 *
 * Lee y combina los valores de los registros de siglo y año para formar el año completo.
 * El siglo se multiplica por 100 y se suma al año, ambos decodificados de BCD.
 *
 * @return El año actual, en el rango de 2000 a 2099.
 */
uint16_t DS1307_GetYear(void) {
    uint16_t cen = DS1307_GetRegByte(DS1307_REG_CENT) * 100;
    return DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_YEAR)) + cen;
}

/**
 * @brief Obtiene la hora actual en formato de 24 horas del DS1307.
 *
 * Lee y decodifica el valor del registro de la hora, eliminando cualquier influencia
 * del bit de parada del reloj.
 *
 * @return La hora en formato de 24 horas, en el rango de 0 a 23.
 */
uint8_t DS1307_GetHour(void) {
    return DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_HOUR) & 0x3f);
}

/**
 * @brief Obtiene los minutos actuales del DS1307.
 *
 * Lee y decodifica el valor del registro de los minutos, proporcionando una medida precisa
 * del tiempo actual.
 *
 * @return Los minutos, en el rango de 0 a 59.
 */
uint8_t DS1307_GetMinute(void) {
    return DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_MINUTE));
}

/**
 * @brief Obtiene los segundos actuales del DS1307.
 *
 * Lee y decodifica el valor del registro de segundos, excluyendo el bit de parada del reloj
 * para asegurar una medida precisa del tiempo actual.
 *
 * @return Los segundos, en el rango de 0 a 59.
 */
uint8_t DS1307_GetSecond(void) {
    return DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_SECOND) & 0x7f);
}

/**
 * @brief Obtiene la compensación de hora UTC almacenada en el DS1307.
 *
 * Recupera el ajuste de hora UTC que no se actualiza automáticamente, necesario para
 * configuraciones manuales del reloj respecto a la hora UTC.
 *
 * @note La compensación UTC no se actualiza automáticamente.
 * @return Compensación de la hora UTC, en el rango de -12 a 12 horas.
 */
int8_t DS1307_GetTimeZoneHour(void) {
    return DS1307_GetRegByte(DS1307_REG_UTC_HR);
}
/**
 * @brief Obtiene el desplazamiento de minutos UTC almacenado en el DS1307.
 *
 * Este valor representa los minutos de ajuste respecto a la hora UTC, y debe ser configurado manualmente.
 * No se actualiza automáticamente con cambios de zona horaria.
 *
 * @note La compensación de minutos UTC no se actualiza automáticamente.
 * @return Minutos de desplazamiento de la zona horaria UTC, en el rango de 0 a 59.
 */
uint8_t DS1307_GetTimeZoneMin(void) {
    return DS1307_GetRegByte(DS1307_REG_UTC_MIN);
}

/**
 * @brief Establece el día de la semana en el DS1307.
 *
 * Configura el día de la semana, donde 0 representa el domingo y 6 el sábado.
 * El valor se codifica en BCD antes de ser escrito en el registro correspondiente.
 *
 * @param dayOfWeek Día de la semana desde el último domingo, en el rango de 0 a 6.
 */
void DS1307_SetDayOfWeek(uint8_t dayOfWeek) {
    DS1307_SetRegByte(DS1307_REG_DOW, DS1307_EncodeBCD(dayOfWeek));
}

/**
 * @brief Establece el día del mes en el DS1307.
 *
 * Configura el día del mes, que debe ser un valor entre 1 y 31, dependiendo del mes actual.
 * El valor se codifica en BCD antes de ser escrito en el registro correspondiente.
 *
 * @param date Día del mes a establecer, en el rango de 1 a 31.
 */
void DS1307_SetDate(uint8_t date) {
    DS1307_SetRegByte(DS1307_REG_DATE, DS1307_EncodeBCD(date));
}

/**
 * @brief Establece el mes actual en el DS1307.
 *
 * Configura el mes actual, que debe ser un valor entre 1 (enero) y 12 (diciembre).
 * El valor se codifica en BCD antes de ser escrito en el registro correspondiente.
 *
 * @param month Mes a establecer, en el rango de 1 a 12.
 */
void DS1307_SetMonth(uint8_t month) {
    DS1307_SetRegByte(DS1307_REG_MONTH, DS1307_EncodeBCD(month));
}

/**
 * @brief Establece el año actual en el DS1307.
 *
 * Divide el año en siglo y década para configurar correctamente el reloj.
 * El siglo se almacena en un registro separado y la década se codifica en BCD.
 *
 * @param year Año a establecer, en el rango de 2000 a 2099.
 */
void DS1307_SetYear(uint16_t year) {
    DS1307_SetRegByte(DS1307_REG_CENT, year / 100);
    DS1307_SetRegByte(DS1307_REG_YEAR, DS1307_EncodeBCD(year % 100));
}

/**
 * @brief Establece la hora actual en formato de 24 horas.
 *
 * Codifica la hora en formato BCD y asegura que el bit de parada del reloj no
 * afecte el valor al escribirlo en el registro.
 *
 * @param hour_24mode Hora a establecer, en el rango de 0 a 23.
 */
void DS1307_SetHour(uint8_t hour_24mode) {
    DS1307_SetRegByte(DS1307_REG_HOUR, DS1307_EncodeBCD(hour_24mode & 0x3f));
}

/**
 * @brief Establece los minutos actuales.
 *
 * Codifica los minutos en BCD y actualiza el registro correspondiente del DS1307.
 *
 * @param minute Minuto a establecer, en el rango de 0 a 59.
 */
void DS1307_SetMinute(uint8_t minute) {
    DS1307_SetRegByte(DS1307_REG_MINUTE, DS1307_EncodeBCD(minute));
}

/**
 * @brief Establece los segundos actuales.
 *
 * Codifica los segundos en BCD y escribe el valor en el registro,
 * manteniendo el estado del bit de parada del reloj.
 *
 * @param second Segundo a establecer, en el rango de 0 a 59.
 */
void DS1307_SetSecond(uint8_t second) {
    uint8_t ch = DS1307_GetClockHalt();
    DS1307_SetRegByte(DS1307_REG_SECOND, DS1307_EncodeBCD(second | ch));
}

/**
 * @brief Establece la compensación de zona horaria UTC.
 *
 * Configura los registros de hora y minutos de UTC para ajustar la hora local
 * en relación con la hora UTC. Los ajustes no se actualizan automáticamente.
 *
 * @param hr Diferencial de hora UTC, en el rango de -12 a 12.
 * @param min Diferencial de minuto UTC, en el rango de 0 a 59.
 * @note La compensación UTC no se actualiza automáticamente.
 */
void DS1307_SetTimeZone(int8_t hr, uint8_t min) {
    DS1307_SetRegByte(DS1307_REG_UTC_HR, hr);
    DS1307_SetRegByte(DS1307_REG_UTC_MIN, min);
}

/**
 * @brief Decodes a BCD (Binary-Coded Decimal) value to a decimal format.
 *
 * This function converts a BCD value into its decimal equivalent. BCD values represent
 * each decimal digit with its own byte for simplicity in digital systems handling.
 * The function extracts the tens and units from the BCD value and combines them to form the decimal value.
 *
 * @param bin The BCD value retrieved from a register, range from 0 to 255.
 * @return The decimal value decoded from the BCD value.
 */
uint8_t DS1307_DecodeBCD(uint8_t bin) {
    // Extract tens by shifting right 4 bits, then multiply by 10 to get the full value
    // Add units directly extracted from the lower nibble
    return (((bin & 0xf0) >> 4) * 10) + (bin & 0x0f);
}

/**
 * @brief Encodes a decimal number to a BCD (Binary-Coded Decimal) format for storage in registers.
 *
 * This function converts a decimal value into a BCD format, suitable for storage in digital systems that handle BCD.
 * The decimal value is split into tens and units, which are then encoded into a single byte.
 *
 * @param dec The decimal number to encode, range from 0 to 99.
 * @return The BCD value encoded from the decimal number.
 */
uint8_t DS1307_EncodeBCD(uint8_t dec) {
    // Calculate tens by dividing by 10 and shift left by 4 bits to place in the higher nibble
    // Calculate units by modulus 10 and add to tens in the BCD format
    return ((dec / 10) << 4) | (dec % 10);
}


/* === End of documentation ==================================================================== */

#ifdef __cplusplus
}
#endif

