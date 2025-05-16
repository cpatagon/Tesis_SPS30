/*
 * Nombre del archivo: rtc_buildtime.c
 * Descripción: [Breve descripción del archivo]
 * Autor: lgomez
 * Creado en: May 15, 2025 
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

#include "rtc_buildtime.h"
#include "rtc_ds3231_for_stm32_hal.h"
#include <stdio.h>
#include <string.h>


/* === Macros definitions ====================================================================== */


/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */



void RTC_SetFromBuildTime(void) {
    DS3231_DateTime dt;
    char month_str[4];

    // Extraer fecha de compilación
    sscanf(__DATE__, "%3s %hhu %hu", month_str, &dt.day, &dt.year);

    // Convertir mes textual a número
    const char* months[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    for (uint8_t i = 0; i < 12; ++i) {
        if (strncmp(month_str, months[i], 3) == 0) {
            dt.month = i + 1;
            break;
        }
    }

    // Extraer hora de compilación
    sscanf(__TIME__, "%hhu:%hhu:%hhu", &dt.hours, &dt.minutes, &dt.seconds);

    DS3231_SetDateTime(&dt);
}


/* === End of documentation ==================================================================== */

