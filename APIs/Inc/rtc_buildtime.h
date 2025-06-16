/*
 * Nombre del archivo: rtc_buildtime.h
 * Descripción: Utilidades para obtener fecha y hora de compilación
 * Autor: lgomez
 * Creado en: May 15, 2025
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
#ifndef INC_RTC_BUILDTIME_H_
#define INC_RTC_BUILDTIME_H_
/** @file
 ** @brief
 **/

/* === Headers files inclusions
 * ================================================================ */
#include <stdint.h>

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

/* === Public variable declarations
 * ============================================================ */

/* === Public function declarations
 * ============================================================ */

/**
 * @brief Establece el RTC con la fecha y hora de compilación (__DATE__,
 * __TIME__).
 */
void RTC_SetFromBuildTime(void);

/* === End of documentation
 * ==================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* INC_RTC_BUILDTIME_H_ */
