/*
 * Nombre del archivo: dht22_config.h
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
#ifndef CONFIG_DHT22_CONFIG_H_
#define CONFIG_DHT22_CONFIG_H_
/** @file
 ** @brief
 **/

/* === Headers files inclusions ================================================================ */

/* === Cabecera C++ ============================================================================ */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */
// Definir constantes de estado
#define DHT22_OK             0     // Indica que la operación fue exitosa
#define DHT22_ERROR          1     // Indica que ocurrió un error general
#define DHT22_ERROR_PIN_HIGH 99901 // Error: No hay respuesta del sensor (pin en estado alto)
#define DHT22_ERROR_PIN_LOW  99902 // Error: No hay respuesta del sensor (pin en estado bajo)
#define DHT22_ERROR_CHECKSUM 99903 // Error: Checksum incorrecto
#define DHT22_ERROR_RESPONSE 99904 // Error: Respuesta incorrecta del sensor
/* === Public data type declarations =========================================================== */

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

/* === End of documentation ==================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* CONFIG_DHT22_CONFIG_H_ */
