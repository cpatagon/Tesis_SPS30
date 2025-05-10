/*
 * Nombre del archivo: data_logger.c
 * Descripción: [Breve descripción del archivo]
 * Autor: lgomez
 * Creado en: May 10, 2025 
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
#include "time_rtc.h"
#include "uart_printing.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/* === Macros definitions ====================================================================== */


/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Variables estáticas === */
static MedicionMP buffer_alta_frec[BUFFER_HIGH_FREQ_SIZE];
static MedicionMP buffer_horario[BUFFER_HOURLY_SIZE];
static MedicionMP buffer_diario[BUFFER_DAILY_SIZE];

static BufferCircular buffer_alta_frecuencia = {
    .datos = buffer_alta_frec,
    .capacidad = BUFFER_HIGH_FREQ_SIZE,
    .inicio = 0,
    .cantidad = 0
};

static BufferCircular buffer_hora = {
    .datos = buffer_horario,
    .capacidad = BUFFER_HOURLY_SIZE,
    .inicio = 0,
    .cantidad = 0
};

static BufferCircular buffer_dia = {
    .datos = buffer_diario,
    .capacidad = BUFFER_DAILY_SIZE,
    .inicio = 0,
    .cantidad = 0
};

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/**
 * @brief Añade una medición al buffer circular especificado
 *
 * @param buffer Buffer donde almacenar la medición
 * @param medicion Medición a almacenar
 */
static void buffer_circular_agregar(BufferCircular* buffer, const MedicionMP* medicion) {
    uint32_t indice;

    if (buffer->cantidad < buffer->capacidad) {
        // El buffer aún no está lleno
        indice = (buffer->inicio + buffer->cantidad) % buffer->capacidad;
        buffer->cantidad++;
    } else {
        // El buffer está lleno, sobrescribir el elemento más antiguo
        indice = buffer->inicio;
        buffer->inicio = (buffer->inicio + 1) % buffer->capacidad;
    }

    // Copiar la medición al buffer
    memcpy(&buffer->datos[indice], medicion, sizeof(MedicionMP));
}

/* === Public function implementation ========================================================== */

bool data_logger_init(void) {
    // Inicializar buffers
    memset(buffer_alta_frec, 0, sizeof(buffer_alta_frec));
    memset(buffer_horario, 0, sizeof(buffer_horario));
    memset(buffer_diario, 0, sizeof(buffer_diario));

    buffer_alta_frecuencia.inicio = 0;
    buffer_alta_frecuencia.cantidad = 0;

    buffer_hora.inicio = 0;
    buffer_hora.cantidad = 0;

    buffer_dia.inicio = 0;
    buffer_dia.cantidad = 0;

    return true;
}

bool data_logger_store_measurement(uint8_t sensor_id, ConcentracionesPM valores,
                                   float temperatura, float humedad) {
    char timestamp[32];
    MedicionMP nueva_medicion;

    // Obtener timestamp actual
    time_rtc_GetFormattedDateTime(timestamp, sizeof(timestamp));

    // Completar estructura de medición
    strcpy(nueva_medicion.timestamp, timestamp);
    nueva_medicion.sensor_id = sensor_id;
    nueva_medicion.valores = valores;
    nueva_medicion.temperatura = temperatura;
    nueva_medicion.humedad = humedad;

    // Almacenar en buffer de alta frecuencia
    buffer_circular_agregar(&buffer_alta_frecuencia, &nueva_medicion);

    // Actualizar buffers de hora y día según corresponda
    // (implementación pendiente)

    return true;
}

float data_logger_get_average_pm25(uint8_t sensor_id, uint32_t num_mediciones) {
    float suma = 0.0f;
    uint32_t contador = 0;

    // Limitar la cantidad de mediciones a usar
    if (num_mediciones > buffer_alta_frecuencia.cantidad) {
        num_mediciones = buffer_alta_frecuencia.cantidad;
    }

    // Si no hay mediciones, retornar 0
    if (num_mediciones == 0) {
        return 0.0f;
    }

    // Calcular promedio de las últimas 'num_mediciones'
    for (uint32_t i = 0; i < num_mediciones; i++) {
        uint32_t indice = (buffer_alta_frecuencia.inicio +
                         buffer_alta_frecuencia.cantidad - i - 1) %
                         buffer_alta_frecuencia.capacidad;

        // Filtrar por sensor_id si es necesario
        if (sensor_id == 0 || buffer_alta_frecuencia.datos[indice].sensor_id == sensor_id) {
            suma += buffer_alta_frecuencia.datos[indice].valores.pm2_5;
            contador++;
        }
    }

    // Retornar promedio o 0 si no hay datos
    return (contador > 0) ? (suma / contador) : 0.0f;
}

void data_logger_print_summary(UART_Printing* uart) {
    char buffer[256];

    // Imprimir encabezado
    snprintf(buffer, sizeof(buffer),
            "\n--- Resumen de Datos Almacenados ---\n"
            "Buffer alta frecuencia: %lu/%lu muestras\n"
            "Buffer horario: %lu/%lu muestras\n"
            "Buffer diario: %lu/%lu muestras\n",
            buffer_alta_frecuencia.cantidad, buffer_alta_frecuencia.capacidad,
            buffer_hora.cantidad, buffer_hora.capacidad,
            buffer_dia.cantidad, buffer_dia.capacidad);

    uart->print(uart, buffer);

    // Imprimir últimas mediciones si hay datos
    if (buffer_alta_frecuencia.cantidad > 0) {
        uart->print(uart, "\nÚltimas 3 mediciones:\n");

        for (uint32_t i = 0; i < 3 && i < buffer_alta_frecuencia.cantidad; i++) {
            uint32_t indice = (buffer_alta_frecuencia.inicio +
                             buffer_alta_frecuencia.cantidad - i - 1) %
                             buffer_alta_frecuencia.capacidad;

            MedicionMP* medicion = &buffer_alta_frecuencia.datos[indice];

            snprintf(buffer, sizeof(buffer),
                    "[%s] Sensor %d: PM2.5=%.2f ug/m3\n",
                    medicion->timestamp,
                    medicion->sensor_id,
                    medicion->valores.pm2_5);

            uart->print(uart, buffer);
        }
    }
}



/* === End of documentation ==================================================================== */

