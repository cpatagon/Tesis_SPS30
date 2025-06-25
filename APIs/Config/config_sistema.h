/*
 * Nombre del archivo: config_sistema.h
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
#ifndef CONFIG_CONFIG_SISTEMA_H_
#define CONFIG_CONFIG_SISTEMA_H_
/** @file
 ** @brief
 **/

/* === Headers files inclusions ================================================================ */

/* === Cabecera C++ ============================================================================ */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */

/** Unidades estándar de las mediciones */
#define UNIT_PM_CONCENTRATION "ug/m3"   ///< Unidades de concentración de partículas
#define UNIT_TEMPERATURE      "Celsius" ///< Temperatura en grados Celsius
#define UNIT_HUMIDITY         "%%RH"    ///< Humedad relativa en porcentaje

/** Información de ubicación del sistema */
#define LOCATION_NAME      "Cerrillos, Santiago, Chile"
#define LOCATION_LATITUDE  -33.4940f
#define LOCATION_LONGITUDE -70.7260f
#define LOCATION_COORDS    "-33.495, -70.720"

/** Cantidad de sensores de MP instalados */
#define NUM_SENSORS_SPS30  3
#define MAX_SENSORES_SPS30 3

/** Longitud máxima para nombres y seriales */
#define SENSOR_NAME_MAX_LEN   32
#define SENSOR_SERIAL_MAX_LEN 32

// Nuevas constantes basadas en tiempo real
#define MAX_SAMPLES_PER_10MIN       60 /**< Muestras de 10 s en 10 minutos */
#define AVG10_PER_HOUR              6  /**< Cantidad de ventanas de 10 min en 1 hora */
#define AVG1H_PER_DAY               24 /**< Cantidad de promedios horarios en 24 h */

#define BUFFER_SIZE_MSG_PM_FORMAT   256
#define BUFFER_SIZE_MSG_ERROR_FALLO 96

#define NUM_REINT                   3
#define CONC_MIN_PM                 0.0f
#define CONC_MAX_PM                 1000.0f
#define DELAY_MS_SPS30_LECTURA      5000

/* === Public data type declarations =========================================================== */

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

/* === End of documentation ==================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* CONFIG_CONFIG_SISTEMA_H_ */
