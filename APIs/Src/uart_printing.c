/*
 * Nombre del archivo: uart_printing.c
 * Descripción: Implementación de funciones para la impresión a través de UART.
 * Autor: lgomez
 * Creado en: Jun 23, 2024 
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

#include "uart_printing.h"
#include <string.h> // Para usar memset y strlen
#include <stdio.h>  // Para usar snprintf

void uart_print(UART_Printing *self, const char *message) {
    HAL_UART_Transmit(self->huart, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
}

void uart_vector_print(UART_Printing *self, uint16_t data_len, const uint8_t *data) {
    char buffer[data_len * 3 + 1];     // Dinamizar tamaño del buffer basado en data_len.
    memset(buffer, 0, sizeof(buffer)); // Limpia el buffer.
    char * ptr = buffer;               // Puntero para la posición actual en el buffer.
    int remaining = sizeof(buffer);    // Espacio restante en el buffer.
    for (uint16_t i = 0; i < data_len; ++i) {
        int written = snprintf(ptr, remaining, "%02X ", data[i]);
        ptr += written;       // Avanza el puntero.
        remaining -= written; // Decrementa el espacio restante.
        if (remaining <= 0)
            break; // Evita desbordamiento del buffer.
    }
    uart_print(self, buffer); // Imprimir todos los bytes formateados.
}


void uart_print_debug(UART_HandleTypeDef *huart, const char *msg) {
    HAL_UART_Transmit(huart, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}


static void uart_default_print(UART_Printing *self, const char *message) {
    HAL_UART_Transmit(self->huart, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
}

static void uart_default_vector_print(UART_Printing *self, uint16_t data_len, const uint8_t *data) {
    HAL_UART_Transmit(self->huart, (uint8_t *)data, data_len, HAL_MAX_DELAY);
}

UART_Printing uart_logger;

void UART_Printing_init(UART_Printing *self, UART_HandleTypeDef *huart) {
    self->huart = huart;
    self->print = uart_default_print;
    self->vector_print = uart_default_vector_print;
}

