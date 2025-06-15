/*
 * Nombre del archivo: DHT22.c
 * Descripción: Controlador para el sensor de temperatura y humedad DHT22
 * Autor: lgomez
 * Creado en: Jun 15, 2024
 * Derechos de Autor: (C) 2023 Luis Gómez CESE FiUBA
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
 ** @brief Controlador para el sensor de temperatura y humedad DHT22
 **/

/* === Headers files inclusions =============================================================== */

#include "DHT22.h"
#include "DWT_Delay.h"
#include "DHT22_Hardware.h"
#include <stdio.h> // Agrega esta línea

/* === Private macros definitions =============================================================== */

#define DELAY_T_BE              1000 // Host the start signal down time 0.8 1 20 m S
#define DELAY_T_GO              20   // Bus master has released time 20 30 200 µ S
#define DELAY_T_REL             80   // Response to low time 75 80 85 µ S
#define DELAY_T_REH             80   // In response to high time 75 80 85 µ S

#define NUM_BITS                5   // Número de bytes leídos del sensor DHT22 (40 bits)
#define INIT_BITS               0   // Valor inicial para los bits leídos
#define INIT_CHECKSUM           0   // Valor inicial para el checksum
#define INIT_TIMEOUT            100 // Valor de timeout inicial para las operaciones de espera
#define FIN_TIMEOUT             0   // Valor de timeout final para indicar expiración

#define DHT22_DATA_BITS         40 // Número total de bits de datos leídos del sensor DHT22
#define DHT22_DATA_BYTES        (DHT22_DATA_BITS / 8) // Número de bytes de datos (5 bytes)

#define DHT22_HUMIDITY_SHIFT    8   // Desplazamiento para los datos de humedad
#define DHT22_TEMPERATURE_SHIFT 8   // Desplazamiento para los datos de temperatura
#define DHT22_CONVERSION_FACTOR 0.1 // Factor de conversión para los datos leídos

#define DHT22_BYTE_BITS         8 // Número de bits en un byte

#define DHT22_TIMEOUT           100 // Valor de timeout para la lectura de bits individuales
#define DHT22_BIT_DELAY         30 // Retraso en microsegundos para asegurar una lectura precisa de bits

/* === Private function declarations ============================================================ */

/* === Public function definitions ============================================================ */

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
void DHT22_Init(DHT22_HandleTypeDef * dht, GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin) {
    DHT22_InitHardware(dht, GPIOx, GPIO_Pin);
    DWT_Init();
}

/**
 * @brief Lee los datos del sensor DHT22.
 *
 * Esta función lee la temperatura y la humedad del sensor DHT22 y almacena
 * los valores en la estructura proporcionada.
 *
 * @param[in] dht Puntero a la estructura DHT22_HandleTypeDef que contiene la configuración del
 * puerto y pin GPIO.
 * @param[out] data Puntero a la estructura DHT22_Data donde se almacenarán los datos leídos.
 *
 * @retval int Estado de la operación: DHT22_OK si es exitoso, código de error si falla.
 */
int DHT22_Read(DHT22_HandleTypeDef * dht, DHT22_Data * data) {
    uint8_t bits[NUM_BITS] = {INIT_BITS}; // Array para almacenar los bits leídos
    uint8_t checksum = INIT_CHECKSUM;     // Inicializar el checksum

    // Enviar señal de inicio
    DHT22_StartSignal(dht);

    // Esperar respuesta del sensor: DHT22 debe poner el pin en bajo por 80 µs y luego en alto por
    // 80 µs
    uint32_t timeout = INIT_TIMEOUT;
    while (HAL_GPIO_ReadPin(dht->GPIOx, dht->GPIO_Pin) == GPIO_PIN_SET) {
        if (--timeout == FIN_TIMEOUT) {
            return DHT22_ERROR_PIN_HIGH; // Error si el pin permanece en alto
        }
    }
    DWT_Delay(DELAY_T_REL); // Esperar 80 µs

    timeout = INIT_TIMEOUT;
    while (HAL_GPIO_ReadPin(dht->GPIOx, dht->GPIO_Pin) == GPIO_PIN_RESET) {
        if (--timeout == FIN_TIMEOUT) {
            return DHT22_ERROR_PIN_LOW; // Error si el pin permanece en bajo
        }
    }
    DWT_Delay(DELAY_T_REH); // Esperar 80 µs

    // Leer los 40 bits de datos (5 bytes)
    for (uint8_t i = 0; i < DHT22_DATA_BYTES; i++) {
        bits[i] = DHT22_ReadByte(dht);
    }

    // Calcular el checksum
    checksum = bits[0] + bits[1] + bits[2] + bits[3];
    if (checksum != bits[4]) {
        return DHT22_ERROR_CHECKSUM; // Error si el checksum no coincide
    }

    // Convertir los datos
    data->humedad =
        ((bits[0] << DHT22_HUMIDITY_SHIFT) | bits[1]) * DHT22_CONVERSION_FACTOR; // Calcular humedad
    data->temperatura = ((bits[2] << DHT22_TEMPERATURE_SHIFT) | bits[3]) *
                        DHT22_CONVERSION_FACTOR; // Calcular temperatura
    return DHT22_OK;                             // Operación exitosa
}

/* === End of documentation ==================================================================== */
