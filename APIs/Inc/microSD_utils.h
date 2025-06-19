/*
 * Nombre del archivo: microSD_utils.h
 * Descripción: Declaraciones públicas para funciones de utilidad de escritura en microSD.
 * Autor: lgomez
 * Creado en: 24-05-2025
 * Derechos de Autor: (C) 2023 CESE
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
#ifndef INC_MICROSD_UTILS_H_
#define INC_MICROSD_UTILS_H_

/** @file
 ** @brief Declaración de funciones auxiliares para escritura en microSD.
 **/

/* === Headers files inclusions ================================================================ */

#include <stdbool.h>
#include "ff.h"

/* === Cabecera C++ ============================================================================ */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */

/* === Public data type declarations =========================================================== */

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

/**
 * @brief Escribe una línea de texto al final de un archivo específico en la microSD.
 *
 * @param filepath Ruta absoluta del archivo.
 * @param line Línea de texto a escribir (debe incluir \n si se desea salto de línea).
 * @return true si la operación fue exitosa, false en caso de error.
 */
bool microSD_appendLineAbsolute(const char * filepath, const char * line);

void print_fatfs_error(FRESULT res);

/* === End of documentation ==================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* INC_MICROSD_UTILS_H_ */
