/*
 * Nombre del archivo: sensor.c
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
/** @file
 ** @brief
 **/

/* === Headers files inclusions =============================================================== */

#include "sensors.h"
#include "usart.h"

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

SPS30 sps30_A;
SPS30 sps30_B;
SPS30 sps30_C;

DHT22_HandleTypeDef dhtA;
DHT22_HandleTypeDef dhtB;
DHT22_HandleTypeDef dhtC;

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */

/* === Función de inicialización ============================================================ */

void sensors_init_all(void) {
    // Inicializar sensores SPS30
    inicializar_sensores_sps30(); // <-- esta función sigue en sps30_multi.c

    // Inicialización de sensores DHT22 (definir los pines según tu hardware)
    DHT22_Init(&dhtA, GPIOB, GPIO_PIN_11); // Sensor A
    DHT22_Init(&dhtB, GPIOB, GPIO_PIN_12); // Sensor B
}

/* === End of documentation ==================================================================== */
