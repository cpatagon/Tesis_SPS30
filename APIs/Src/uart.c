/*
 * Nombre del archivo: uart.c
 * Descripción: Implementación de funciones personalizadas para la comunicación
 * UART. Autor: lgomez Derechos de Autor: (C) 2024 lgomez Licencia: GNU General
 * Public License v3.0
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

/** @file uart.c
 ** @brief Implementación de funciones personalizadas para la comunicación UART
 *en STM32.
 **/

/* === Inclusiones de archivos de cabecera
 * ===================================================== */
#include "uart.h"
#include "stm32f4xx_hal.h"
#include "usart.h"
#include <stdarg.h>
#include <stdio.h>  // Para usar snprintf
#include <string.h> // Para usar memset y strlen

/* === Definiciones de macros
 * ================================================================== */
// Aquí puedes definir cualquier macro relevante para este módulo.

/* === Declaraciones de tipos de datos privados
 * ================================================ */
// Aquí puedes definir estructuras, enumeraciones, u otros tipos de datos
// privados.

/* === Declaraciones de variables privadas
 * ===================================================== */
// Aquí puedes declarar variables que solo se utilizan en este módulo.

extern UART_HandleTypeDef
    huart3; // Asume que huart3 está definido en otro lugar.
extern SPI_HandleTypeDef hspi1;
UART_HandleTypeDef *uart_debug = NULL;

/* === Declaraciones de funciones privadas
 * ===================================================== */
// Aquí puedes declarar funciones que solo se utilizan en este módulo.

/* === Definiciones de variables públicas
 * ====================================================== */
// Aquí puedes definir variables que son accesibles desde otros módulos.

/* === Definiciones de variables privadas
 * ====================================================== */
// Aquí puedes definir variables que solo se utilizan en este módulo.

/* === Implementación de funciones privadas
 * ==================================================== */
// Aquí puedes implementar funciones que solo se utilizan en este módulo.

/* === Implementación de funciones públicas
 * ==================================================== */

/**
 * @brief Envía un mensaje a través de UART3.
 * @param message Mensaje a enviar.
 */
void uart_print(const char *format, ...) {
  char buffer[256];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  if (uart_debug != NULL) {
    if (HAL_UART_Transmit(uart_debug, (uint8_t *)buffer, strlen(buffer),
                          UART_TX_TIMEOUT_MS) != HAL_OK) {
      return; // Transmission failed or timed out
    }
  }
}
/**
 * @brief Imprime un vector de datos como hexadecimal a través de UART3.
 * @param data_len Longitud de los datos a imprimir.
 * @param data Datos a imprimir.
 */
void uart_vector_print(uint16_t data_len, const uint8_t *data) {
  char buffer[200];                  // Aumenta el tamaño si es necesario.
  memset(buffer, 0, sizeof(buffer)); // Limpia el buffer.
  char *ptr = buffer; // Puntero para la posición actual en el buffer.
  int remaining = sizeof(buffer); // Espacio restante en el buffer.
  for (uint16_t i = 0; i < data_len; ++i) {
    int written = snprintf(ptr, remaining, "%02X ", data[i]);
    ptr += written;       // Avanza el puntero.
    remaining -= written; // Decrementa el espacio restante.
    if (remaining <= 0)
      break; // Evita desbordamiento del búfer.
  }
  uart_print(buffer); // Imprimir todos los bytes formateados.
}

// Función para enviar comando al SPS30 y esperar una respuesta
void uart_send_command(const uint8_t *command, uint16_t commandSize) {
  HAL_UART_Transmit(&huart5, command, commandSize, 100);
}

// Función para iniciar la recepción de datos de forma asincrónica
void uart_receive_async(uint8_t *dataBuffer, uint16_t bufferSize) {
  HAL_UART_Receive(&huart5, dataBuffer, bufferSize, 100);
}
// Función para enviar comando al SPS30 y esperar una respuesta
void uart_send_receive(const uint8_t *command, uint16_t commandSize,
                       uint8_t *dataBuffer, uint16_t bufferSize) {
  HAL_UART_Transmit(&huart5, command, commandSize, 100);
  HAL_UART_Receive(&huart5, dataBuffer, bufferSize, 100);
}

void probar_spi_sd(void) {
  uint8_t tx = 0xFF;
  uint8_t rx = 0x00;

  uart_print("Probando SPI con microSD...\r\n");

  HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(&hspi1, &tx, &rx, 1, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_SET);

  char msg[64];
  snprintf(msg, sizeof(msg), "Respuesta SPI de SD: 0x%02X\r\n", rx);
  uart_print(msg);
}

/* === Fin de la documentación
 * ================================================================ */
