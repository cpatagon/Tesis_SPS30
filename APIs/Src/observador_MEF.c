/*
 * Nombre del archivo: observador_MEF.c
 * Descripción: Implementación de la máquina de estados para el proceso de observación de PM2.5.
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
 * @brief Implementación de la máquina de estados para el proceso de observación de PM2.5.
 *
 * Coordina el flujo del sistema entre adquisición, almacenamiento y cálculo estadístico.
 */
/* === Headers files inclusions =============================================================== */

#include "sensor.h"
#include "observador_MEF.h"
#include "data_logger.h"
#include "time_rtc.h"
#include "uart.h"
#include "pm25_buffer.h"
#include "data_types.h"

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

static Estado_Observador estado_actual = ESTADO_REPOSO;
static Estado_Observador estado_anterior = ESTADO_REPOSO;

/* === Buffers temporales y resultados ========================================================= */

static TemporalBuffer buffer_temp = {0};
static EstadisticaPM25 resultado;

static uint32_t tiempo_inicio_reposo = 0;
static bool reposo_esperando = false;

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */

/**
 * @brief Inicializa la máquina de estados del observador en estado REPOSO.
 *
 * Reinicia los estados interno y anterior a REPOSO y muestra mensaje por UART.
 */
void observador_MEF_init(void) {
    estado_actual = ESTADO_REPOSO;
    estado_anterior = ESTADO_REPOSO;
    uart_print("[MEF] Inicializado en estado REPOSO\r\n");
}

/**
 * @brief Cambia el estado actual de la máquina de estados del observador.
 *
 * @param nuevo Nuevo estado a asignar (enum Estado_Observador).
 */
void observador_MEF_cambiar_estado(Estado_Observador nuevo) {
    estado_actual = nuevo;
}

/**
 * @brief Obtiene el estado actual de la máquina de estados.
 *
 * @return Estado_Observador Estado actual del observador.
 */
Estado_Observador observador_MEF_estado_actual(void) {
    return estado_actual;
}

/**
 * @brief Fuerza un reinicio de la máquina de estados y limpia los buffers de PM2.5.
 *
 * Utiliza `pm25_rbuffer_limpiar()` y vuelve al estado REPOSO.
 */
void observador_MEF_forzar_reset(void) {
    pm25_rbuffer_limpiar();
    observador_MEF_init();
    uart_print("[MEF] Reinicio forzado del sistema de adquisición\r\n");
}

/**
 * @brief Imprime por UART el estado actual en formato legible.
 *
 * Utiliza un arreglo de strings con nombres legibles para depuración.
 */
void observador_MEF_debug_estado(void) {
    const char * nombres[] = {"REPOSO", "LECTURA", "ALMACENAMIENTO", "CALCULO", "ERROR"};
    uart_printf("[MEF] Estado actual: %s\r\n", nombres[estado_actual]);
}

/**
 * @brief Actualiza el estado de la máquina según el flujo lógico del sistema.
 *
 * Este es el núcleo de la MEF. Evalúa transiciones entre estados según condiciones:
 * - `ESTADO_REPOSO`: espera cambio de tiempo.
 * - `ESTADO_LECTURA`: adquiere datos de sensores.
 * - `ESTADO_ALMACENAMIENTO`: guarda datos en buffer circular.
 * - `ESTADO_CALCULO`: calcula estadísticas si hubo cambio de bloque de tiempo.
 * - `ESTADO_GUARDADO`: guarda estadísticas en microSD.
 * - `ESTADO_LIMPIESA`: limpia buffers y vuelve a reposo.
 * - `ESTADO_ERROR`: muestra mensaje y reinicia.
 */

void observador_MEF_actualizar(void) {

    if (estado_actual != estado_anterior) {
        uart_printf("[MEF] Transicion: %d -> %d\r\n", estado_anterior, estado_actual);
        estado_anterior = estado_actual;
    }

    switch (estado_actual) {

    case ESTADO_REPOSO:
        if (!reposo_esperando) {
            tiempo_inicio_reposo = HAL_GetTick(); // Marca tiempo de entrada
            reposo_esperando = true;
        }

        // Verificamos si pasó el tiempo deseado
        if (HAL_GetTick() - tiempo_inicio_reposo >= DURACION_REPOSO_MS) {
            reposo_esperando = false;

            if (rtc_esta_activo()) {
                observador_MEF_cambiar_estado(ESTADO_LECTURA);
            }
        }
        break;

    case ESTADO_LECTURA: {
        SensorStatus status = sensor_leer_datos(buffer_temp.muestras); // ✅ Correcto

        if (status == SENSOR_OK) {
            buffer_temp.cantidad =
                sensores_disponibles; // o el valor real que devuelvas si modificas la función
            observador_MEF_cambiar_estado(ESTADO_ALMACENAMIENTO);
        } else {
            observador_MEF_cambiar_estado(ESTADO_ERROR);
        }
        break;
    }
    case ESTADO_ALMACENAMIENTO:
        if (data_logger_store_sensor_data(buffer_temp.muestras, buffer_temp.cantidad,
                                          buffers_10min)) {
            if (time_rtc_hay_cambio_bloque()) {
                observador_MEF_cambiar_estado(ESTADO_CALCULO);
            } else {
                observador_MEF_cambiar_estado(ESTADO_LECTURA);
            }
        } else {
            observador_MEF_cambiar_estado(ESTADO_ERROR);
        }
        break;

    case ESTADO_CALCULO:
        if (data_logger_estadistica_10min_pm25(buffers_10min, &resultado)) {
            observador_MEF_cambiar_estado(ESTADO_GUARDADO);
        } else {
            uart_print("[ERROR] No se pudieron calcular estadísticas de PM2.5\r\n");
            observador_MEF_cambiar_estado(ESTADO_ERROR);
        }
        break;
    case ESTADO_GUARDADO: {
        data_logger_store_avg10_csv(&resultado); // o resultado_global si es global
        observador_MEF_cambiar_estado(ESTADO_LIMPIESA);
        break;
    }
    case ESTADO_LIMPIESA: {
        data_logger_buffer_limpiar_todos(buffers_10min);
        buffer_temp.cantidad = 0; // limpiar buffer temporal
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
