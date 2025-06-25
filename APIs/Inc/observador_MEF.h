/*
 * Nombre del archivo: observador_MEF.h
 * Descripción: Máquina de Estados Finitos para el proceso de adquisición de datos.
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
#ifndef INC_OBSERVADOR_MEF_H_
#define INC_OBSERVADOR_MEF_H_
/**
 * @file observador_MEF.h
 * @brief Máquina de Estados Finitos para el proceso de adquisición de datos.
 *
 * Este módulo define los estados del sistema de adquisición y proporciona funciones
 * para inicialización, actualización del estado, cambio manual, trazabilidad y reinicio.
 *
 * @author
 * @date 2025
 */

/* === Headers files inclusions ================================================================ */

#include <stdint.h>
#include <stdbool.h>

/* === Cabecera C++ ============================================================================ */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */

/* === Public data type declarations =========================================================== */

/**
 * @enum Estado_Observador
 * @brief Estados posibles del proceso de adquisición de datos.
 */
typedef enum {
    ESTADO_REPOSO = 0,     /**< Sistema en espera. */
    ESTADO_LECTURA,        /**< Lectura del sensor. */
    ESTADO_ALMACENAMIENTO, /**< Almacenamiento en buffer o microSD. */
    ESTADO_CALCULO,        /**< Cálculo de estadísticas. */
    ESTADO_ERROR           /**< Estado de error del sistema. */
} Estado_Observador;

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */
/**
 * @brief Inicializa la máquina de estados al estado base.
 */
void observador_MEF_init(void);

/**
 * @brief Ejecuta la lógica principal de la MEF.
 */
void observador_MEF_actualizar(void);

/**
 * @brief Cambia el estado actual del sistema.
 * @param nuevo El nuevo estado deseado.
 */
void observador_MEF_cambiar_estado(Estado_Observador nuevo);

/**
 * @brief Retorna el estado actual del sistema.
 * @return Estado actual.
 */
Estado_Observador observador_MEF_estado_actual(void);

/**
 * @brief Reinicia la MEF y limpia buffers.
 */
void observador_MEF_forzar_reset(void);

/**
 * @brief Imprime por UART el estado actual (para debug).
 */
void observador_MEF_debug_estado(void);
/* === End of documentation ==================================================================== */

#ifdef __cplusplus
}
#endif
#endif /* INC_OBSERVADOR_MEF_H_ */
