/*
 * Nombre del archivo: sensors.h
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
#ifndef INC_SENSORS_H_
#define INC_SENSORS_H_
/** @file
 ** @brief
 **/

/* === Headers files inclusions ================================================================ */

#include "stm32f4xx_hal.h"
#include "sps30_multi.h"
#include "DHT22.h"
#include "sps30_multi.h"

/* === Cabecera C++ ============================================================================ */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */

/* === Public data type declarations =========================================================== */

/* === Public variable declarations ============================================================ */

/* === Sensores de Material Particulado (SPS30) ============================================= */

extern DHT22_HandleTypeDef dht_ambiente;

extern SensorSPS30 sensores_sps30[NUM_SENSORES_SPS30];
extern int sensores_disponibles;

extern SPS30 sps30_A;
extern SPS30 sps30_B;
extern SPS30 sps30_C;

/* === Sensores de Temperatura y Humedad (DHT22) ============================================ */
extern DHT22_HandleTypeDef dhtA;
extern DHT22_HandleTypeDef dhtB;

/* === Public function declarations ============================================================ */

/**
 * @brief Inicializa todos los sensores SPS30 y DHT22 con sus GPIOs correspondientes.
 */
void sensors_init_all(void);

/* === End of documentation ==================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* INC_SENSORS_H_ */
