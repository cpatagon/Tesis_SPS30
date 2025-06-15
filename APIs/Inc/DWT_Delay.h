/*
 * Nombre del archivo: DWT_Delay.h
 * Descripción: Proporciona funciones para la inicialización y el uso del contador de ciclos DWT
 * para crear retrasos precisos. Autor: lgomez Creado en: 19-06-2024 Derechos de Autor: (C) 2023
 * lgomez CESE FiUBA Licencia: GNU General Public License v3.0
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
#ifndef INC_DWT_DELAY_H_
#define INC_DWT_DELAY_H_
/** @file
 ** @brief Proporciona funciones para la inicialización y el uso del contador de ciclos DWT para
 *crear retrasos precisos.
 **/

/* === Headers files inclusions ================================================================ */

/* === Cabecera C++ ============================================================================ */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */

#include "stm32f4xx_hal.h"

/* === Public data type declarations =========================================================== */

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

/**
 * @brief Inicializa el contador de ciclos del Data Watchpoint and Trace (DWT).
 *
 * Esta función habilita el contador de ciclos del DWT y lo reinicia. El DWT es
 * un componente del ARM Cortex-M4 que permite la medición precisa de ciclos
 * de reloj, útil para implementar retrasos precisos.
 *
 * @note Esta función debe ser llamada antes de utilizar DWT_Delay.
 *
 * @retval None
 */
void DWT_Init(void);

/**
 * @brief Introduce un retraso en microsegundos utilizando el contador de ciclos del DWT.
 *
 * Esta función utiliza el contador de ciclos del DWT para introducir un retraso
 * preciso en microsegundos. El DWT debe estar inicializado previamente llamando
 * a DWT_Init.
 *
 * @param[in] us Cantidad de microsegundos a esperar.
 *
 * @note La precisión del retraso depende de la frecuencia del reloj del sistema.
 *
 * @retval None
 */
void DWT_Delay(uint32_t us);

/* === End of documentation ==================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* INC_DWT_DELAY_H_ */
