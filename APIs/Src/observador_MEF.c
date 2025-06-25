/*
 * Nombre del archivo: observador_MEF.c
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
 * @file observador_MEF.c
 * @brief Implementación de la MEF para el sistema de adquisición de datos PM2.5.
 */
/* === Headers files inclusions =============================================================== */

#include "sensor.h"
#include "observador_MEF.h"
#include "data_logger.h"
#include "time_rtc.h"
#include "uart.h"
#include "pm25_buffer.h"

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

static Estado_Observador estado_actual = ESTADO_REPOSO;
static Estado_Observador estado_anterior = ESTADO_REPOSO;

TemporalBuffer buffer_temp;
uint8_t cantidad = 0;

/* === Private variable declarations =========================================================== */

EstadisticaPM25 resultado;

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */

void observador_MEF_init(void) {
    estado_actual = ESTADO_REPOSO;
    estado_anterior = ESTADO_REPOSO;
    uart_print("[MEF] Inicializado en estado REPOSO\r\n");
}

void observador_MEF_cambiar_estado(Estado_Observador nuevo) {
    estado_actual = nuevo;
}

Estado_Observador observador_MEF_estado_actual(void) {
    return estado_actual;
}

void observador_MEF_forzar_reset(void) {
    pm25_rbuffer_limpiar();
    observador_MEF_init();
    uart_print("[MEF] Reinicio forzado del sistema de adquisición\r\n");
}

void observador_MEF_debug_estado(void) {
    const char * nombres[] = {"REPOSO", "LECTURA", "ALMACENAMIENTO", "CALCULO", "ERROR"};
    uart_printf("[MEF] Estado actual: %s\r\n", nombres[estado_actual]);
}

void observador_MEF_actualizar(void) {
    if (estado_actual != estado_anterior) {
        uart_printf("[MEF] Transición: %d -> %d\r\n", estado_anterior, estado_actual);
        estado_anterior = estado_actual;
    }

    switch (estado_actual) {

    case ESTADO_REPOSO:
        if (rtc_esta_activo()) {
            observador_MEF_cambiar_estado(ESTADO_LECTURA);
        }
        break;

    case ESTADO_LECTURA:
        if (sensor_leer_datos(&buffer_temp) == SENSOR_OK) {
            observador_MEF_cambiar_estado(ESTADO_ALMACENAMIENTO);
        } else {
            observador_MEF_cambiar_estado(ESTADO_ERROR);
        }
        break;

    case ESTADO_ALMACENAMIENTO:
        if (data_logger_store_sensor_data(&buffer_temp, &buffers_10min)) {
            if (time_rtc_hay_cambio_bloque()) {
                observador_MEF_cambiar_estado(ESTADO_CALCULO);
            } else {
                observador_MEF_cambiar_estado(ESTADO_LECTURA);
            }
        } else {
            observador_MEF_cambiar_estado(ESTADO_ERROR);
        }
        break;

    case ESTADO_CALCULO: {
        if (data_logger_estadistica_10min_pm25(&buffers_10min, &resultado)) {
            observador_MEF_cambiar_estado(ESTADO_GUARDADO);
        } else {
            uart_print("[ERROR] No se pudieron calcular estadísticas de PM2.5\r\n");
            observador_MEF_cambiar_estado(ESTADO_ERROR);
        }
        break;
    }
    case ESTADO_GUARDADO: {
        data_logger_store_avg10_csv(&resultado); // o resultado_global si es global
        observador_MEF_cambiar_estado(ESTADO_LIMPIESA);
        break;
    }
    case ESTADO_LIMPIESA: {
        data_logger_buffer_limpiar_todos(&buffers_10min);
        data_logger_buffer_limpiar_todos(&buffer_temp);
        observador_MEF_cambiar_estado(ESTADO_REPOSO);
        break;
    }

    case ESTADO_ERROR:
        uart_print("[ERROR] Se detectó un problema en el sistema de adquisición\r\n");
        observador_MEF_cambiar_estado(ESTADO_REPOSO);
        break;
    }
}

/* === End of documentation ==================================================================== */
