/*
 * Nombre del archivo: DHT22.h
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
#ifndef INC_DHT22_H_
#define INC_DHT22_H_
/** @file
 ** @brief Controlador para el sensor de temperatura y humedad DHT22
 **/

/* === Headers files inclusions ================================================================ */
#include "stm32f4xx_hal.h"
#include "DHT22_Hardware.h"
// #include "gpio.h"

/* === Cabecera C++ ============================================================================ */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */

// Definir constantes de estado
#define DHT22_OK             0     // Indica que la operación fue exitosa
#define DHT22_ERROR          1     // Indica que ocurrió un error general
#define DHT22_ERROR_PIN_HIGH 99901 // Error: No hay respuesta del sensor (pin en estado alto)
#define DHT22_ERROR_PIN_LOW  99902 // Error: No hay respuesta del sensor (pin en estado bajo)
#define DHT22_ERROR_CHECKSUM 99903 // Error: Checksum incorrecto
#define DHT22_ERROR_RESPONSE 99904 // Error: Respuesta incorrecta del sensor

/* === Public data type declarations =========================================================== */

/**
 * @brief Estructura para almacenar los datos del sensor DHT22.
 *
 * Esta estructura contiene los datos de temperatura y humedad leídos desde el sensor DHT22.
 */
typedef struct {
    float temperatura; /**< Temperatura medida por el sensor, en grados Celsius. */
    float humedad;     /**< Humedad medida por el sensor, en porcentaje. */
} DHT22_Data;

/* === Public function declarations ============================================================ */

/**
 * @brief Inicializa el sensor DHT22.
 *
 * Configura el pin GPIO para el sensor DHT22 y habilita el reloj del puerto GPIO.
 *
 * @param dht Puntero a la estructura DHT22_HandleTypeDef.
 * @param GPIOx Puerto GPIO donde está conectado el DHT22.
 * @param GPIO_Pin Pin GPIO donde está conectado el DHT22.
 */
void DHT22_Init(DHT22_HandleTypeDef * dht, GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin);

/**
 * @brief Lee los datos del sensor DHT22.
 *
 * Lee la temperatura y la humedad del sensor DHT22 y almacena los valores en la estructura
 * proporcionada.
 *
 * @param dht Puntero a la estructura DHT22_HandleTypeDef.
 * @param data Puntero a la estructura DHT22_Data donde se almacenarán los datos leídos.
 * @return int Estado de la operación: DHT22_OK si es exitoso, código de error si falla.
 */
int DHT22_Read(DHT22_HandleTypeDef * dht, DHT22_Data * data);

/* === End of documentation ==================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* INC_DHT22_H_ */
