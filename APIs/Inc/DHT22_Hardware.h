/*
 * Nombre del archivo: DHT22_Hardware.h
 * Descripción: [Breve descripción del archivo]
 * Autor: lgomez
 * Creado en: 19-06-2024
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
#ifndef INC_DHT22_HARDWARE_H_
#define INC_DHT22_HARDWARE_H_
/** @file
 ** @brief
 **/
#include "stm32f4xx_hal.h"
// #include "DHT22.h"  // Asegúrate de incluir DHT22.h para que se conozcan los tipos

/* === Headers files inclusions ================================================================ */

/* === Cabecera C++ ============================================================================ */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */

/* === Public data type declarations =========================================================== */
/**
 * @brief Estructura para manejar el estado del DHT22.
 *
 * Esta estructura contiene la configuración del puerto y pin GPIO donde está conectado el sensor
 * DHT22.
 */
typedef struct {
    GPIO_TypeDef * GPIOx; /**< Puerto GPIO donde está conectado el DHT22. */
    uint16_t GPIO_Pin;    /**< Pin GPIO donde está conectado el DHT22. */
} DHT22_HandleTypeDef;
/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

/**
 * @brief Inicializa el hardware necesario para el sensor DHT22.
 *
 * Esta función configura el pin GPIO al que está conectado el sensor DHT22
 * y habilita el reloj para el puerto GPIO correspondiente.
 *
 * @param[in] dht Puntero a la estructura DHT22_HandleTypeDef que contiene la configuración del
 * puerto y pin GPIO.
 * @param[in] GPIOx Puerto GPIO al que está conectado el DHT22.
 * @param[in] GPIO_Pin Pin GPIO al que está conectado el DHT22.
 *
 * @retval None
 */
void DHT22_InitHardware(DHT22_HandleTypeDef * dht, GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin);

/**
 * @brief Configura el pin GPIO como salida.
 *
 * Esta función establece el pin GPIO al que está conectado el sensor DHT22
 * en modo de salida para enviar señales al sensor.
 *
 * @param[in] dht Puntero a la estructura DHT22_HandleTypeDef que contiene la configuración del
 * puerto y pin GPIO.
 *
 * @retval None
 */
void DHT22_SetPinOutput(DHT22_HandleTypeDef * dht);

/**
 * @brief Configura el pin GPIO como entrada.
 *
 * Esta función establece el pin GPIO al que está conectado el sensor DHT22
 * en modo de entrada para recibir datos del sensor.
 *
 * @param[in] dht Puntero a la estructura DHT22_HandleTypeDef que contiene la configuración del
 * puerto y pin GPIO.
 *
 * @retval None
 */
void DHT22_SetPinInput(DHT22_HandleTypeDef * dht);

/**
 * @brief Envía la señal de inicio al sensor DHT22.
 *
 * Esta función configura el pin GPIO como salida, envía la señal de inicio
 * y luego lo configura como entrada para recibir datos del sensor.
 *
 * @param[in] dht Puntero a la estructura DHT22_HandleTypeDef que contiene la configuración del
 * puerto y pin GPIO.
 *
 * @retval None
 */
void DHT22_StartSignal(DHT22_HandleTypeDef * dht);

/**
 * @brief Lee un byte de datos del sensor DHT22.
 *
 * Esta función lee 8 bits de datos del sensor DHT22 y los devuelve como un byte.
 *
 * @param[in] dht Puntero a la estructura DHT22_HandleTypeDef que contiene la configuración del
 * puerto y pin GPIO.
 *
 * @retval uint8_t El byte leído del sensor.
 */
uint8_t DHT22_ReadByte(DHT22_HandleTypeDef * dht);

/**
 * @brief Lee un bit de datos del sensor DHT22.
 *
 * Esta función lee un bit de datos del sensor DHT22.
 *
 * @param[in] dht Puntero a la estructura DHT22_HandleTypeDef que contiene la configuración del
 * puerto y pin GPIO.
 *
 * @retval uint8_t El bit leído del sensor (0 o 1).
 */
uint8_t DHT22_ReadBit(DHT22_HandleTypeDef * dht);
/* === End of documentation ==================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* INC_DHT22_HARDWARE_H_ */
