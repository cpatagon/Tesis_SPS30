/*
 * Nombre del archivo: pm25_buffer.c
 * Descripción: [Breve descripción del archivo]
 * Autor: lgomez
 * Creado en: 22-06-2025
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
/**
 * @file pm25_buffer.c
 * @brief Implementación de buffers circulares para almacenamiento de PM2.5 por sensor.
 *
 * Este módulo permite almacenar lecturas individuales de PM2.5 por sensor,
 * calcular estadísticas (promedio, mínimo, máximo, desviación estándar),
 * y reiniciar el buffer para nuevos ciclos de acumulación.
 */

/* === Headers files inclusions =============================================================== */

#include "pm25_buffer.h"
#include "data_logger.h"
#include "mp_sensors_info.h"
#include <math.h>
#include <string.h> // memset

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */

/**
 * @brief Devuelve la cantidad de muestras disponibles para un sensor en data_logger.
 */
uint8_t pm25_buffer_get_count(uint8_t sensor_id) {
    return data_logger_get_count(sensor_id);
}

/**
 * @brief Obtiene el valor de PM2.5 de una medición específica del sensor.
 */
float pm25_buffer_get_value(uint8_t sensor_id, uint8_t index) {
    const MedicionMP * medicion = data_logger_get_medicion(sensor_id, index);
    if (medicion == NULL)
        return -1.0f;
    return medicion->valores.pm2_5;
}

/**
 * @brief Calcula estadísticas (promedio, min, max, std) del buffer de PM2.5.
 */
void pm25_buffer_calcular_estadisticas(uint8_t sensor_id, EstadisticasPM * stats) {
    uint8_t count = data_logger_get_count(sensor_id);
    if (stats == NULL || count == 0)
        return;

    float sum = 0.0f;
    float min = 0.0f;
    float max = 0.0f;
    float valores[count];

    for (uint8_t i = 0; i < count; ++i) {
        const MedicionMP * m = data_logger_get_medicion(sensor_id, i);
        if (m == NULL)
            continue;
        float val = m->valores.pm2_5;
        valores[i] = val;
        sum += val;
        if (i == 0 || val < min)
            min = val;
        if (i == 0 || val > max)
            max = val;
    }

    float promedio = sum / count;
    float var = 0.0f;
    for (uint8_t i = 0; i < count; ++i) {
        float diff = valores[i] - promedio;
        var += diff * diff;
    }

    stats->pm2_5_promedio = promedio;
    stats->min = min;
    stats->max = max;
    stats->std = sqrtf(var / count);
    stats->n_datos_validos = count;
}

/**
 * @brief Esta función queda como stub. Reset real debe ser realizado en data_logger.
 */
void pm25_buffer_reset(uint8_t sensor_id) {
    // No implementado aquí. Se espera que data_logger tenga función de limpieza.
}

void pm25_rbuffer_limpiar(void) {
    for (uint8_t i = 0; i < NUM_SENSORS_SPS30; ++i) {
        pm25_buffer_reset(i);
    }
}

/* === End of documentation ==================================================================== */
