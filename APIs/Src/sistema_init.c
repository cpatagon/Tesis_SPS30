/*
 * Nombre del archivo: sistema_init.c
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

#include "sistema_init.h"
#include "data_logger.h"
#include "rtc_ds3231_for_stm32_hal.h"
#include "sps30_multi.h"
#include "DHT22.h"
#include "sensors.h"
#include "mp_sensors_info.h"

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */

bool sistema_verificar_componentes(void) {
    bool estado_ok = true;

    if (!data_logger_init()) {
        uart_print("[ERROR] microSD no pudo montarse\n");
        estado_ok = false;
    } else {
        uart_print("[OK] microSD montada correctamente\n");
    }

    if (!rtc_auto_init()) {
        uart_print("[ERROR] RTC no inicializado correctamente\n");
        estado_ok = false;
    } else {
        uart_print("[OK] RTC detectado y configurado\n");
    }

    // Intentar inicializar sensores SPS30
    for (int i = 0; i < sensores_disponibles; ++i) {
        SPS30 * sensor = &sensores_sps30[i].sensor;
        char serial_tmp[SERIAL_BUFFER_LEN];

        if (sensor->serial_number && sensor->serial_number(sensor, serial_tmp)) {
            uart_print("[OK] Sensor SPS30 ID %d serial: %s\n", sensores_sps30[i].id, serial_tmp);
        } else {
            uart_print("[ERROR] No se pudo leer el numero de serie del sensor SPS30 ID %d\n",
                       sensores_sps30[i].id);
            estado_ok = false; // No bloquea, pero se registra el fallo
        }
    }

    // Verificar sensores DHT22
    if (!DHT22_ReadSimple(&dhtA, NULL, NULL)) {
        uart_print("[ERROR] Sensor DHT22 ambiente no responde\n");
        estado_ok = false;
    } else {
        uart_print("[OK] Sensor DHT22 ambiente funcionando\n");
    }

    if (!DHT22_ReadSimple(&dhtB, NULL, NULL)) {
        uart_print("[ERROR] Sensor DHT22 camara no responde\n");
        estado_ok = false;
    } else {
        uart_print("[OK] Sensor DHT22 camara funcionando\n");
    }

    return estado_ok; // Se puede ignorar si quieres que el sistema continúe de todos modos
}

void sistema_imprimir_datos_iniciales(void) {
    char timestamp[32];
    time_rtc_GetFormattedDateTime(timestamp, sizeof(timestamp));

    uart_print("\n=== INICIO DE MEDICIÓN ===\n");
    uart_print("Fecha de inicio: %s\n", timestamp);
    uart_print("Ubicación: %s\n", LOCATION_COORDS);

    for (int i = 0; i < sensores_disponibles; i++) {
        uart_print("Sensor ID: %d | Serial: %s\n", sensores_sps30[i].id,
                   sensor_metadata[sensores_sps30[i].id - 1].serial_number);
    }

    float temp, hum;
    if (DHT22_ReadSimple(&dht_ambiente, &temp, &hum)) {
        uart_print("Temperatura ambiente inicial: %.1f°C | Humedad: %.1f%%\n", temp, hum);
    }
}

/* === End of documentation ==================================================================== */
