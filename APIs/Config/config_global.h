/*
 * Nombre del archivo: config_global.h
 * Descripción: [Breve descripción del archivo]
 * Autor: lgomez
 * Creado en: 25-06-2025
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
#ifndef CONFIG_CONFIG_GLOBAL_H_
#define CONFIG_CONFIG_GLOBAL_H_
/** @file
 ** @brief
 **/

/* === Headers files inclusions ================================================================ */

// === Configuración de estructura de datos principal ===
#include "data_types.h" // SensorData, EstadisticaPM25, etc.

// === Configuraciones de sensores ===
#include "sps30_config.h" // Parámetros SPS30
#include "dht22_config.h" // Parámetros DHT22

// === Configuración de tiempo y RTC ===
#include "rtc_config.h" // RTC DS3231 y tipos de tiempo

// === Configuración de buffers ===
#include "buffers_config.h" // Tamaños y definiciones de buffers

// === Configuración del sistema ===
#include "config_sistema.h" // Pines LED, modo debug, etc.

// === Configuración de mensajes ===
#include "config_mensaje.h" // Mensajes UART, encabezados CSV/JSON

/* === Cabecera C++ ============================================================================ */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */

/* === Public data type declarations =========================================================== */

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

/* === End of documentation ==================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* CONFIG_CONFIG_GLOBAL_H_ */
