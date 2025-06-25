/*
 * Nombre del archivo: sps30_comm.h
 * Descripción: Interfaz de comunicación con el sensor SPS30 mediante SHDLC
 * Autor: lgomez
 * Creado en: Jun 23, 2024
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
#ifndef INC_SPS30_COMM_H_
#define INC_SPS30_COMM_H_
/** @file
 ** @brief
 **/

/* === Headers files inclusions
 * ================================================================ */

#include "shdlc.h"
#include "stm32f4xx_hal.h"
#include "uart.h"
#include <stdbool.h>
#include <stdint.h>
#include "sps30_config.h"

/* === Cabecera C++
 * ============================================================================
 */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions
 * =============================================================== */

/* === Public data type declarations
 * =========================================================== */

// Declaración de la estructura SPS30
typedef struct SPS30 SPS30;

// Declara externamente el manejador de UART, asumiendo que se define en otro
// lugar Estructura que representa el objeto SPS30
typedef struct SPS30 {
    UART_HandleTypeDef * huart;
    char serial_buf[SERIAL_BUFFER_LEN]; // Buffer para guardar número de serie

    // Métodos
    void (*send_command)(struct SPS30 * self, const uint8_t * command, uint16_t commandSize);
    void (*receive_async)(struct SPS30 * self, uint8_t * dataBuffer, uint16_t bufferSize);

    // Cambiado de void a bool
    bool (*send_receive)(struct SPS30 * self, const uint8_t * command, uint16_t commandSize,
                         uint8_t * dataBuffer, uint16_t bufferSize);

    void (*start_measurement)(struct SPS30 * self);
    void (*stop_measurement)(struct SPS30 * self);
    void (*sleep)(struct SPS30 * self);
    void (*read_data)(struct SPS30 * self);

    // También declarada correctamente como bool
    bool (*serial_number)(struct SPS30 * self, char * out_serial);

    void (*wake_up)(struct SPS30 * self);
    ConcentracionesPM (*get_concentrations)(struct SPS30 * self);
} SPS30;
/* === Public variable declarations
 * ============================================================ */

/* === Public function declarations
 * ============================================================ */

// Prototipos de funciones para inicializar el objeto SPS30
void SPS30_init(SPS30 * self, UART_HandleTypeDef * huart);

bool sps30_serial_number(SPS30 * self, char * out_serial);

/* === End of documentation
 * ==================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* INC_SPS30_COMM_H_ */
