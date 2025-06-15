/*
 * Nombre del archivo: DWT_Delay.c
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

#include "DWT_Delay.h"

/* === Macros definitions ====================================================================== */

#define REINIT_COUNT     0
#define CLK_DIV_FREQ_RCC 1000000

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */

void DWT_Init(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // Habilitar el DWT
    DWT->CYCCNT = REINIT_COUNT;                     // Reiniciar el contador de ciclos
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;            // Habilitar el contador de ciclos
}

void DWT_Delay(uint32_t us) {
    uint32_t clk_cycle_start = DWT->CYCCNT;
    uint32_t clk_cycle_delay = us * (HAL_RCC_GetHCLKFreq() / CLK_DIV_FREQ_RCC);
    while ((DWT->CYCCNT - clk_cycle_start) < clk_cycle_delay)
        ;
}
/* === End of documentation ==================================================================== */
