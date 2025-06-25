/*
 * Nombre del archivo: buffers_config.h
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
#ifndef CONFIG_BUFFERS_CONFIG_H_
#define CONFIG_BUFFERS_CONFIG_H_
/** @file
 ** @brief
 **/

/* === Headers files inclusions ================================================================ */

/* === Cabecera C++ ============================================================================ */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */

// Tamaños de los buffers circulares
#define BUFFER_HIGH_FREQ_SIZE 100 /**< 60 muestras = 10 minutos con frecuencia 10s */
#define BUFFER_HOURLY_SIZE    24  /**< 24 muestras = 1 por cada 10 minutos en una hora */
#define BUFFER_DAILY_SIZE     30  /**< 30 muestras = 1 cada hora durante 30 horas (aprox. 1 día) */
#define CSV_LINE_BUFFER_SIZE  128 /**< Tamaño máximo para formatear una línea CSV */
#define BUFFER_10MIN_SIZE     100

// Conteo de ciclos para promedios (obsoletos, mantenidos por compatibilidad)
#define CYCLES_AVG_10MIN 60   /**< 60 ciclos equivalen a 10 minutos */
#define CYCLES_AVG_1H    360  /**< 360 ciclos equivalen a 1 hora */
#define CYCLES_AVG_24H   8640 /**< 8640 ciclos equivalen a 24 horas */

/* === Public data type declarations =========================================================== */

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

/* === End of documentation ==================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* CONFIG_BUFFERS_CONFIG_H_ */
