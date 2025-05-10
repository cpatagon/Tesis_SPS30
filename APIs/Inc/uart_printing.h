/*
 * Nombre del archivo: uart_printing.h
 * Descripción: Define las interfaces para las operaciones UART, incluyendo
 *              funciones para imprimir mensajes y vectores de datos en formato
 *              hexadecimal.
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

#ifndef INC_UART_PRINTING_H_
#define INC_UART_PRINTING_H_

/** @file
 ** @brief Interfaces para operaciones UART de impresión de datos.
 **/

#include <stdint.h> // Para usar tipos de datos estándar
#include "stm32f4xx_hal.h"

typedef struct UART_Printing UART_Printing;




struct UART_Printing {
    UART_HandleTypeDef *huart;  // Manejador de UART

    // Métodos
    void (*print)(UART_Printing *self, const char *message);
    void (*vector_print)(UART_Printing *self, uint16_t data_len, const uint8_t *data);
};

// Prototipos de funciones para inicializar el objeto UART_Printing
void UART_Printing_init(UART_Printing *self, UART_HandleTypeDef *huart);


#endif // INC_UART_PRINTING_H_

