/*
 * Nombre del archivo: sensor.c
 * @author Luis Gómez
 * @date 15-06-2025
 * @copyright (C) 2023 Luis Gómez - CESE FIUBA
 * @license GNU GPL v3.0 <http://www.gnu.org/licenses/>
 * SPDX-License-Identifier: GPL-3.0-only
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
 * @file sensor.c
 * @brief Implementación de lectura de sensores ambientales (DHT22) y de partículas (SPS30).
 *
 * Este módulo inicializa y gestiona la lectura de sensores conectados al sistema.
 * Se integran sensores DHT22 (temperatura y humedad) y SPS30 (material particulado PM2.5).
 * Los datos leídos se almacenan y procesan a través del sistema observador.
 *
 **/

/* === Headers files inclusions =============================================================== */

#include "config_global.h"
#include "sensor.h"
#include "sps30_multi.h"
#include "time_rtc.h"
#include "dht22_config.h"
#include "uart.h"
#include "DHT22.h" // si usas DHT22 u otro sensor ambiental
#include "usart.h"
#include "uart.h"
#include "time_rtc.h"
#include "DHT22.h"
#include "proceso_observador.h"
#include "sps30_comm.h"
#include "shdlc.h"
#include "rtc_ds3231_for_stm32_hal.h" // para ds3231_get_datetime()
#include <string.h>

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

// Sensores SPS30 individuales (si deseas usarlos además del arreglo general)
SPS30 sps30_A;
SPS30 sps30_B;
SPS30 sps30_C;

// Variables globales para almacenamiento temporal de datos DHT22
DHT22_HandleTypeDef dhtA;
DHT22_HandleTypeDef dhtB;
// DHT22_HandleTypeDef dhtC;

// Variables globales para almacenamiento temporal de datos DHT22
float temp_amb = 0.0f, hum_amb = 0.0f;
float temp_cam = 0.0f, hum_cam = 0.0f;

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */

/* === Función de inicialización ============================================================ */

void sensors_init_all(void) {
    // Inicializar sensores SPS30 (definidos externamente en sps30_multi.c)
    inicializar_sensores_sps30();

    // Inicialización de sensores DHT22 con los GPIO correspondientes
    DHT22_Init(&dhtA, GPIOB, GPIO_PIN_11); // Ambiente
    DHT22_Init(&dhtB, GPIOB, GPIO_PIN_12); // Cámara
    // DHT22_Init(&dhtC, ...); // Sensor adicional si se usa
}

/* === Función de lectura principal ========================================================== */
/*
SensorStatus sensor_leer_datos(void) {
    DHT22_Data sensorData;
    bool dht_ok = true;

    // Lectura DHT22 ambiente
    if (DHT22_Read(&dhtA, &sensorData) == DHT22_OK) {
        temp_amb = sensorData.temperatura;
        hum_amb = sensorData.humedad;
        uart_print("Ambiente: Temp: %.1f °C, Hum: %.1f %%\r\n", temp_amb, hum_amb);
    } else {
        uart_print("Error leyendo DHT22 ambiente\r\n");
        dht_ok = false;
    }

    // Lectura DHT22 cámara
    if (DHT22_Read(&dhtB, &sensorData) == DHT22_OK) {
        temp_cam = sensorData.temperatura;
        hum_cam = sensorData.humedad;
        uart_print("Cámara: Temp: %.1f °C, Hum: %.1f %%\r\n", temp_cam, hum_cam);
    } else {
        uart_print("Error leyendo DHT22 cámara\r\n");
        dht_ok = false;
    }

    // Obtener la fecha y hora actual
    char datetime_buffer[32];
    obtener_fecha_hora(datetime_buffer);

    // Ejecutar proceso observador con cada sensor SPS30
    for (int i = 0; i < sensores_disponibles; ++i) {
        proceso_observador_3PM_2TH(&sensores_sps30[i].sensor, sensores_sps30[i].id, datetime_buffer,
                                   temp_amb, hum_amb, temp_cam, hum_cam);
    }

    return dht_ok ? SENSOR_OK : SENSOR_ERROR;
}
*/

// #define NUM_SENSORES_SPS30 3

SensorStatus sensor_leer_datos(MedicionMP * datos_array) {
    if (datos_array == NULL)
        return SENSOR_ERROR;

    DHT22_Data sensorData;
    bool dht_ok = true;

    float temp_amb = -100.0f, hum_amb = -1.0f;
    float temp_cam = -100.0f, hum_cam = -1.0f;

    // Leer DHT ambiente
    if (DHT22_Read(&dhtA, &sensorData) == DHT22_OK) {
        temp_amb = sensorData.temperatura;
        hum_amb = sensorData.humedad;
    } else {
        dht_ok = false;
    }

    // Leer DHT cámara
    if (DHT22_Read(&dhtB, &sensorData) == DHT22_OK) {
        temp_cam = sensorData.temperatura;
        hum_cam = sensorData.humedad;
    } else {
        dht_ok = false;
    }

    // Obtener fecha y hora
    ds3231_time_t dt;
    if (!ds3231_get_datetime(&dt)) {
        memset(&dt, 0, sizeof(dt));
        uart_print("[WARN] RTC no respondió, se colocaron ceros en fecha/hora.\r\n");
    }

    char datetime_buffer[32];
    snprintf(datetime_buffer, sizeof(datetime_buffer), "%04u-%02u-%02u %02u:%02u:%02u", dt.year,
             dt.month, dt.day, dt.hour, dt.min, dt.sec);

    uint8_t count = 0;

    for (uint8_t i = 0; i < sensores_disponibles && count < NUM_SENSORES_SPS30; ++i) {
        ConcentracionesPM pm =
            sensores_sps30[i].sensor.get_concentrations(&sensores_sps30[i].sensor);

        if (pm.pm2_5 < 0.0f || pm.pm2_5 > 1000.0f)
            continue;

        MedicionMP * m = &datos_array[count++];
        m->timestamp = dt;
        m->bloque_10min = dt.min / 10;
        m->sensor_id = sensores_sps30[i].id;
        m->pm1_0 = pm.pm1_0;
        m->pm2_5 = pm.pm2_5;
        m->pm4_0 = pm.pm4_0;
        m->pm10 = pm.pm10;
        m->temp_amb = temp_amb;
        m->hum_amb = hum_amb;
        m->temp_cam = temp_cam;
        m->hum_cam = hum_cam;

        proceso_observador_3PM_2TH(&sensores_sps30[i].sensor, sensores_sps30[i].id, datetime_buffer,
                                   temp_amb, hum_amb, temp_cam, hum_cam);
    }

    return (count > 0 && dht_ok) ? SENSOR_OK : SENSOR_ERROR;
}

/**
 * @brief Obtiene todos los datos actuales de sensores SPS30 y variables ambientales.
 *
 * Esta función recorre todos los sensores SPS30 disponibles y almacena sus concentraciones
 * junto con la hora actual y datos de temperatura y humedad (ambiente y cámara) en el arreglo de
 * salida.
 *
 * @param[out] out_array Arreglo de estructuras SensorData a llenar.
 * @param[in]  max_len   Cantidad máxima de elementos a llenar en out_array.
 * @return Número de sensores leídos correctamente.
 */
uint8_t sensor_get_all(SensorData * out_array, uint8_t max_len) {
    if (!out_array || max_len == 0)
        return 0;

    const char * rtc_error_msg = "[WARN] RTC no respondió, se colocarán ceros en fecha/hora.\r\n";

    ds3231_time_t dt;
    bool rtc_ok = ds3231_get_datetime(&dt);
    if (!rtc_ok) {
        uart_print(rtc_error_msg);
        // Se colocan ceros explícitamente
        memset(&dt, 0, sizeof(ds3231_time_t));
    }
    /*
        float temp_amb = 0.0f, hum_amb = 0.0f;
        float temp_cam = 0.0f, hum_cam = 0.0f;
      */
    DHT22_Data sensorData;
    float temp_amb = -99.9f;
    float hum_amb = -99.9f;
    float temp_cam = -99.9f;
    float hum_cam = -99.9f;

    // Lectura sensores ambientales si están disponibles
    if (!DHT22_Read(&dhtA, &sensorData) == DHT22_OK) {
        temp_amb = sensorData.temperatura;
        hum_amb = sensorData.humedad;
        uart_print("Ambiente: Temp: %.1f C, Hum: %.1f%%\n", temp_amb, hum_amb);
    }

    // Opcional: sensores internos para cámara
    if (DHT22_Read(&dhtB, &sensorData) == DHT22_OK) {
        temp_cam = sensorData.temperatura;
        hum_cam = sensorData.humedad;
        uart_print("Camara: Temp: %.1f C, Hum: %.1f%%\n", temp_cam, hum_cam);
    }

    uint8_t count = 0;
    for (uint8_t i = 0; i < sensores_disponibles && count < max_len; ++i) {
        ConcentracionesPM pm =
            sensores_sps30[i].sensor.get_concentrations(&sensores_sps30[i].sensor);

        SensorData d = {
            .year = dt.year,
            .month = dt.month,
            .day = dt.day,
            .hour = dt.hour,
            .min = dt.min,
            .sec = dt.sec,
            .bloque_10min = dt.min / 10,

            .sensor_id = sensores_sps30[i].id,
            .pm1_0 = pm.pm1_0,
            .pm2_5 = pm.pm2_5,
            .pm4_0 = pm.pm4_0,
            .pm10 = pm.pm10,

            .temp_amb = temp_amb,
            .hum_amb = hum_amb,
            .temp_cam = temp_cam,
            .hum_cam = hum_cam,
        };

        out_array[count++] = d;
    }

    return count;
}
/*
bool buffer_guardar(SensorBufferTemp *buffer) {
    if (!buffer || buffer->cantidad == 0) return false;

    for (uint8_t i = 0; i < buffer->cantidad; ++i) {
     //   buffer_circular_insertar(&buffers_10min[buffer->array[i].sensor_id], buffer->array[i]);
    }

    buffer->cantidad = 0;  // Limpiar después de guardar

    return true;
}*/

/* === End of documentation ==================================================================== */
