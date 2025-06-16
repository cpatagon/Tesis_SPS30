/*
 * Nombre del archivo: mp_sensors_info.c
 * Descripción: Implementación de la inicialización y almacenamiento de metadatos de sensores SPS30.
 * Autor: lgomez
 * Creado en: 24-05-2025
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
 */

/** @file
 ** @brief Implementación del módulo de metadatos de sensores SPS30.
 **/

/* === Headers files inclusions =============================================================== */
#include "mp_sensors_info.h"
#include "sps30_multi.h"
#include "uart.h"
#include <string.h>

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */
MP_SensorInfo sensor_metadata[NUM_SENSORS_SPS30] = {
    [0] = {.serial_number = "0001", .location_name = "Cerrillos"},
    [1] = {.serial_number = "0002", .location_name = "Cerrillos"},
    [2] = {.serial_number = "0003", .location_name = "Cerrillos"},
};

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */

void mp_sensors_info_init(void) {
    for (int i = 0; i < sensores_disponibles; ++i) {
        SPS30 * sensor = &sensores_sps30[i].sensor;
        char serial[SENSOR_SERIAL_MAX_LEN] = {0};

        // Obtener serial dinámicamente
        if (sensor->serial_number(sensor, serial)) {
            // Guardar serial en metadatos
            strncpy(sensor_metadata[i].serial_number, serial, SENSOR_SERIAL_MAX_LEN - 1);
            sensor_metadata[i].serial_number[SENSOR_SERIAL_MAX_LEN - 1] = '\0';

            // Mensaje UART de registro
            uart_print("Sensor ID: %d -> Serial: %s\n", sensores_sps30[i].id, serial);
        } else {
            strncpy(sensor_metadata[i].serial_number, "UNKNOWN", SENSOR_SERIAL_MAX_LEN - 1);
            sensor_metadata[i].serial_number[SENSOR_SERIAL_MAX_LEN - 1] = '\0';
            uart_print("[WARN] Sensor ID: %d -> No se pudo obtener el número de serie\n",
                       sensores_sps30[i].id);
        }

        // Nombre de ubicación fijo (puede adaptarse por sensor si es necesario)
        strncpy(sensor_metadata[i].location_name, LOCATION_NAME,
                sizeof(sensor_metadata[i].location_name) - 1);
        sensor_metadata[i].location_name[sizeof(sensor_metadata[i].location_name) - 1] = '\0';
    }
}

/* === End of documentation ==================================================================== */
