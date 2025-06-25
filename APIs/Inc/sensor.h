/*
 * Nombre del archivo: sensors.h
 * Descripción: Declaraciones para sensores ambientales y de partículas
 * Autor: lgomez
 * Creado en: 15-06-2025
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
 *
 */
#ifndef INC_SENSOR_H_
#define INC_SENSOR_H_
/**
 * @file sensor.h
 * @brief Declaraciones para sensores ambientales (DHT22) y de material particulado (SPS30).
 *
 * Proporciona funciones de inicialización y adquisición de datos ambientales y de partículas.
 * Integra sensores múltiples (SPS30 y DHT22) en una interfaz unificada para su uso en sistemas
 *embebidos.
 *
 **/

/* === Headers files inclusions
 * ================================================================ */

#include "DHT22.h"
#include "sps30_multi.h"
#include "stm32f4xx_hal.h"

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

/* === Public variable declarations
 * ============================================================ */

typedef enum { SENSOR_OK = 0, SENSOR_ERROR = -1 } SensorStatus;

typedef struct {
    // 🕓 Timestamp primero
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;

    uint8_t bloque_10min; ///< 0 a 5 (bloques de 10 min)

    // 🔢 ID del sensor SPS30
    uint8_t sensor_id;

    // 🌫️ Concentraciones PM
    float pm1_0;
    float pm2_5;
    float pm4_0;
    float pm10;

    // 🌡️ Variables ambientales
    float temp_amb;
    float hum_amb;
    float temp_cam;
    float hum_cam;
} SensorData;

typedef struct {
    SensorData datos[NUM_SENSORES_SPS30];
    uint8_t cantidad;
} SensorBufferTemp;

/* === Sensores de Material Particulado (SPS30)
 * ============================================= */

/* SPS30 individuales (opcional si usas arreglo sensores_sps30[]) */
extern SPS30 sps30_A;
extern SPS30 sps30_B;
extern SPS30 sps30_C;

/* Estructuras de sensores DHT22 */
extern DHT22_HandleTypeDef dhtA;
extern DHT22_HandleTypeDef dhtB;

/* Arreglo de sensores SPS30 y contador */
extern SensorSPS30 sensores_sps30[NUM_SENSORES_SPS30];
extern int sensores_disponibles;

/* === Public function declarations
 * ============================================================ */

/**
 * @brief Inicializa todos los sensores SPS30 y DHT22 con sus pines definidos.
 */
void sensors_init_all(void);

/**
 * @brief Lee datos de los sensores (DHT22 y SPS30) y los registra en el sistema.
 * @return SENSOR_OK si la lectura fue exitosa; SENSOR_ERROR en caso de fallo.
 */

/**
 * @brief Obtiene los datos actuales de todos los sensores SPS30 + ambientales.
 * @param out_array Arreglo de estructuras donde se guardarán los datos.
 * @param max_len Tamaño máximo del arreglo.
 * @return Número de datos válidos obtenidos (0 si fallaron todas).
 */
SensorStatus sensor_leer_datos(SensorBufferTemp * buffer);

/**
 * @brief Guarda los datos de un sensor en el buffer circular de 10 minutos.
 *
 * Esta función recibe una estructura `SensorData` completa, que incluye
 * identificador del sensor, datos de PM, temperatura, humedad y timestamp,
 * y lo inserta en el buffer correspondiente.
 *
 * @param data Estructura con los datos del sensor.
 * @return true si se guardaron correctamente, false si ocurrió un error (e.g. overflow).
 */

/* === End of documentation
 * ==================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* INC_SENSOR_H_ */
