/*
 * Nombre del archivo: DHT22_Hardware.c
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
/** @file
 ** @brief
 **/

/* === Headers files inclusions =============================================================== */
#include "DHT22_Hardware.h"
// #include "DWT_Delay.h" // Archivo separado para la gestión de retrasos DWT
#include "stm32f4xx_hal.h" // Incluir HAL para tipos de datos y funciones

/* === Macros definitions ====================================================================== */

#define DHT22_BIT_DELAY 30
#define DHT22_TIMEOUT   100
#define DHT22_BYTE_BITS 8

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */

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
void DHT22_InitHardware(DHT22_HandleTypeDef * dht, GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin) {
    dht->GPIOx = GPIOx;
    dht->GPIO_Pin = GPIO_Pin;

    // Habilitar el reloj para el puerto GPIO específico
    if (GPIOx == GPIOA)
        __HAL_RCC_GPIOA_CLK_ENABLE(); // Habilitar reloj para puerto GPIOA
    else if (GPIOx == GPIOB)
        __HAL_RCC_GPIOB_CLK_ENABLE(); // Habilitar reloj para puerto GPIOB
    else if (GPIOx == GPIOC)
        __HAL_RCC_GPIOC_CLK_ENABLE(); // Habilitar reloj para puerto GPIOC
    else if (GPIOx == GPIOD)
        __HAL_RCC_GPIOD_CLK_ENABLE(); // Habilitar reloj para puerto GPIOD
    else if (GPIOx == GPIOE)
        __HAL_RCC_GPIOE_CLK_ENABLE(); // Habilitar reloj para puerto GPIOE
    else if (GPIOx == GPIOF)
        __HAL_RCC_GPIOF_CLK_ENABLE(); // Habilitar reloj para puerto GPIOF
    else if (GPIOx == GPIOG)
        __HAL_RCC_GPIOG_CLK_ENABLE(); // Habilitar reloj para puerto GPIOG
    else if (GPIOx == GPIOH)
        __HAL_RCC_GPIOH_CLK_ENABLE(); // Habilitar reloj para puerto GPIOH
    else if (GPIOx == GPIOI)
        __HAL_RCC_GPIOI_CLK_ENABLE(); // Habilitar reloj para puerto GPIOI

    // Configurar el pin como salida
    DHT22_SetPinOutput(dht);
    HAL_GPIO_WritePin(dht->GPIOx, dht->GPIO_Pin, GPIO_PIN_SET); // Poner el pin en alto
}

/**
 * @brief Configura el pin GPIO del DHT22 como salida.
 *
 * @param[in] dht Puntero a la estructura DHT22_HandleTypeDef que contiene la configuración del
 * puerto y pin GPIO.
 *
 * @retval None
 */
void DHT22_SetPinOutput(DHT22_HandleTypeDef * dht) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = dht->GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(dht->GPIOx, &GPIO_InitStruct); // Inicializar el GPIO como salida
}

/**
 * @brief Configura el pin GPIO del DHT22 como entrada.
 *
 * @param[in] dht Puntero a la estructura DHT22_HandleTypeDef que contiene la configuración del
 * puerto y pin GPIO.
 *
 * @retval None
 */
void DHT22_SetPinInput(DHT22_HandleTypeDef * dht) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = dht->GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(dht->GPIOx, &GPIO_InitStruct);
}

/**
 * @brief Envía la señal de inicio al sensor DHT22.
 *
 * Esta función configura el pin GPIO como salida, envía una señal de inicio
 * al DHT22 y luego lo configura como entrada para recibir la respuesta del sensor.
 *
 * @param[in] dht Puntero a la estructura DHT22_HandleTypeDef que contiene la configuración del
 * puerto y pin GPIO.
 *
 * @retval None
 */
void DHT22_StartSignal(DHT22_HandleTypeDef * dht) {
    DHT22_SetPinOutput(dht);                                      // Configurar el pin como salida
    HAL_GPIO_WritePin(dht->GPIOx, dht->GPIO_Pin, GPIO_PIN_RESET); // Enviar señal de inicio (bajo)
    DWT_Delay(1000);                                              // Esperar 1000 µs
    HAL_GPIO_WritePin(dht->GPIOx, dht->GPIO_Pin, GPIO_PIN_SET); // Terminar señal de inicio (alto)
    DWT_Delay(20);                                              // Esperar 20 µs
    DHT22_SetPinInput(dht);                                     // Configurar el pin como entrada
}

/**
 * @brief Lee un byte del sensor DHT22.
 *
 * Esta función lee 8 bits del pin GPIO del DHT22 y los combina en un solo byte.
 *
 * @param[in] dht Puntero a la estructura DHT22_HandleTypeDef que contiene la configuración del
 * puerto y pin GPIO.
 *
 * @retval uint8_t El byte leído del sensor.
 */
uint8_t DHT22_ReadByte(DHT22_HandleTypeDef * dht) {
    uint8_t result = 0;                             // Inicializa el resultado en 0
    for (uint8_t i = 0; i < DHT22_BYTE_BITS; i++) { // Itera 8 veces para leer cada bit
        result <<=
            1; // Desplaza el resultado a la izquierda para hacer espacio para el siguiente bit
        result |= DHT22_ReadBit(dht); // Lee un bit y lo añade al resultado
    }
    return result; // Devuelve el byte completo leído del sensor
}

/**
 * @brief Lee un bit del sensor DHT22.
 *
 * Esta función espera a que el pin GPIO cambie de estado y luego lee el valor del bit.
 *
 * @param[in] dht Puntero a la estructura DHT22_HandleTypeDef que contiene la configuración del
 * puerto y pin GPIO.
 *
 * @retval uint8_t El bit leído del sensor (0 o 1).
 */
uint8_t DHT22_ReadBit(DHT22_HandleTypeDef * dht) {
    uint32_t timeout = DHT22_TIMEOUT;

    // Esperar a que el pin cambie a alto
    while (HAL_GPIO_ReadPin(dht->GPIOx, dht->GPIO_Pin) == GPIO_PIN_RESET) {
        if (--timeout == 0) {
            return 0; // Timeout, devolver 0
        }
    }

    // Introducir un pequeño retraso para asegurar una lectura precisa del bit
    DWT_Delay(DHT22_BIT_DELAY); // Esperar 30 µs

    // Leer el bit (0 o 1) según el estado del pin
    uint8_t result = HAL_GPIO_ReadPin(dht->GPIOx, dht->GPIO_Pin);

    timeout = DHT22_TIMEOUT;

    // Esperar a que el pin vuelva a su estado bajo
    while (HAL_GPIO_ReadPin(dht->GPIOx, dht->GPIO_Pin) == GPIO_PIN_SET) {
        if (--timeout == 0) {
            break; // Timeout, salir del bucle
        }
    }

    // Devolver el bit leído
    return result;
}

/* === End of documentation ==================================================================== */
