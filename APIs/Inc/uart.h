/*
 * Nombre del archivo: uart.h
 * Descripción: Define las interfaces para las operaciones UART, incluyendo
 *              funciones para imprimir mensajes, enviar comandos y recibir
 *              datos de forma asíncrona.
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

#ifndef INC_UART_H_
#define INC_UART_H_

/** @file
 ** @brief Interfaces para operaciones UART.
 **/

#include <stdint.h> // Para usar tipos de datos estándar

#include "main.h"
#include "usart.h"



extern UART_HandleTypeDef *uart_debug;

/**
 * @brief Envia un mensaje a través de UART3.
 *
 * @param message Cadena de caracteres para enviar.
 */
void uart_print(const char *format, ...);

/**
 * @brief Imprime un vector de datos como una cadena de hexadecimales a través de UART3.
 *
 * @param data_len Longitud del vector de datos.
 * @param data Puntero al vector de datos a imprimir.
 */
void uart_vector_print(uint16_t data_len, const uint8_t* data);

/**
 * @brief Envía un comando a través de UART5 y espera una respuesta.
 *
 * @param command Puntero al comando a enviar.
 * @param commandSize Tamaño del comando a enviar.
 */
void uart_send_command(const uint8_t *command, uint16_t commandSize);

/**
 * @brief Inicia la recepción de datos de forma asíncrona a través de UART5.
 *
 * @param dataBuffer Buffer para almacenar los datos recibidos.
 * @param bufferSize Tamaño del buffer de datos.
 */
void uart_receive_async(uint8_t *dataBuffer, uint16_t bufferSize);


void uart_send_receive(const uint8_t *command, uint16_t commandSize, uint8_t *dataBuffer, uint16_t bufferSize);


void probar_spi_sd(void);

#endif /* INC_UART_H_ */

