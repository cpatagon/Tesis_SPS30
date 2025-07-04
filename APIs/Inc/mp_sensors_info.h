/*
 * Nombre del archivo: mp_sensors_info.h
 * Descripción: Cabecera para la gestión de información y metadatos de sensores
 * de MP (SPS30).
 * Autor: lgomez
 * Creado en: 24-05-2025
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
 */

#ifndef INC_MP_SENSORS_INFO_H_
#define INC_MP_SENSORS_INFO_H_

/** @file
 ** @brief Cabecera para la obtención de seriales y metadatos de sensores SPS30
 **/

/* === Headers files inclusions
 * ================================================================ */
#include <stdint.h>
#include "config_sistema.h"

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

/**
 * @brief Información de identificación y ubicación del sensor de MP
 */
typedef struct {
    char serial_number[SENSOR_SERIAL_MAX_LEN + 1]; ///< Número de serie del sensor
    char location_name[64];                        ///< Nombre del lugar de instalación
} MP_SensorInfo;

/* === Public variable declarations
 * ============================================================ */

/**
 * @brief Arreglo global con la información de los sensores conectados
 */
extern MP_SensorInfo sensor_metadata[NUM_SENSORS_SPS30];

/* === Public function declarations
 * ============================================================ */

/**
 * @brief Inicializa el arreglo de metadatos con los seriales reales de cada
 * sensor.
 *
 * Requiere que `inicializar_sensores_sps30()` ya se haya ejecutado.
 */
void mp_sensors_info_init(void);

/* === End of documentation
 * ==================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* INC_MP_SENSORS_INFO_H_ */
