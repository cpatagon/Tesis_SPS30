/*
 * Nombre del archivo: test_format_csv.c
 * Descripción: [Breve descripción del archivo]
 * Autor: lgomez
 * Creado en: 21-05-2025
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
#include "data_logger.h"
#include <stdio.h>
#include <string.h>
#include <stddef.h>

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */
ParticulateData ejemplo = {.sensor_id = 1,
                           .pm1_0 = 10.0,
                           .pm2_5 = 20.5,
                           .pm4_0 = 25.0,
                           .pm10 = 30.5,
                           .temp_amb = 21.0, // Temperatura ambiente medida al inicio del ciclo
                           .hum_amb = 45.0,  // Humedad ambiente medida al inicio del ciclo
                           .temp_cam = 25.2, // Temperatura interna de la cámara, si se mide
                           .hum_cam = 48.1,  // Humedad dentro de la cámara, si se mide
                           .year = 2025,
                           .month = 5,
                           .day = 22,
                           .hour = 14,
                           .min = 20,
                           .sec = 0};

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */

/**
 * @brief Prueba la función format_csv_line() con datos simulados
 */
void test_format_csv_line(void) {
    ParticulateData test_data = {.year = 2025,
                                 .month = 5,
                                 .day = 21,
                                 .hour = 21,
                                 .min = 30,
                                 .sec = 0,
                                 .sensor_id = 1,
                                 .pm1_0 = 3.2,
                                 .pm2_5 = 5.6,
                                 .pm4_0 = 6.7,
                                 .pm10 = 7.2,
                                 .temp_amb = 23.4,
                                 .hum_amb = 42.1,
                                 .temp_cam = 24.8,
                                 .hum_cam = 46.7};

    char buffer[CSV_LINE_BUFFER_SIZE];

    if (format_csv_line(&test_data, buffer, sizeof(buffer))) {
        uart_print("Línea CSV generada:\n%s\n", buffer);
    } else {
        uart_print("Error: Buffer insuficiente para la línea CSV.\n");
    }

    char filepath[64];
    build_csv_filepath_from_datetime(filepath, sizeof(filepath));
    uart_print("Ruta CSV generada:\r\n");
    uart_print(filepath);
    uart_print("\r\n");

    log_data_to_sd(&test_data); // Ahora usa los datos correctos
}

/* === End of documentation ==================================================================== */
