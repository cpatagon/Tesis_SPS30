/*
 * Nombre del archivo: sistema_init.h
 * Descripción: [Breve descripción del archivo]
 * Autor: lgomez
 * Creado en: 15-06-2025
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
#ifndef INC_SISTEMA_INIT_H_
#define INC_SISTEMA_INIT_H_
/** @file
 ** @brief
 **/

/* === Headers files inclusions ================================================================ */

#include <stdbool.h> // Para tipo bool
#include <stdint.h>  // Para tipos enteros estándar

/* Si alguna de las funciones usa UART, RTC o necesita estructuras públicas, incluir aquí */
#include "uart.h"                     // Para funciones como uart_print()
#include "sps30_multi.h"              // Para acceso a sensores_sps30[]
#include "DHT22.h"                    // Si se usan funciones como DHT22_ReadSimple
#include "rtc_ds3231_for_stm32_hal.h" // Para verificación de RTC
#include "data_logger.h"              // Para inicialización de microSD

/* === Cabecera C++ ============================================================================ */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */

/* === Public data type declarations =========================================================== */

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

bool sistema_verificar_componentes(void);
void sistema_imprimir_datos_iniciales(void);

/* === End of documentation ==================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* INC_SISTEMA_INIT_H_ */
