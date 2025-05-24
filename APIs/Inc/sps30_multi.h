/**
 * @file sps30_multi.h
 * @brief Módulo para manejar múltiples sensores SPS30 en paralelo.
 *
 * Este archivo define la estructura de control y funciones asociadas para
 * inicializar y operar múltiples sensores SPS30 utilizando UARTs independientes.
 *
 * @author lgomez
 * @date 04-05-2025
 * @copyright (C) 2023 [Tu nombre o institución]
 * @license GNU General Public License v3.0
 *
 * Este programa es software libre: puedes redistribuirlo y/o modificarlo
 * bajo los términos de la Licencia Pública General GNU publicada por
 * la Free Software Foundation, ya sea la versión 3 de la Licencia, o
 * (a tu elección) cualquier versión posterior.
 *
 * Este programa se distribuye con la esperanza de que sea útil,
 * pero SIN NINGUNA GARANTÍA; sin siquiera la garantía implícita
 * de COMERCIABILIDAD o APTITUD PARA UN PROPÓSITO PARTICULAR.
 * Ver la Licencia Pública General GNU para más detalles.
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef INC_SPS30_MULTI_H_
#define INC_SPS30_MULTI_H_

#ifdef __cplusplus
extern "C" {
#endif

/* === Inclusión de archivos de cabecera ====================================================== */
#include "sps30_comm.h"
#include "usart.h"

/* === Definiciones públicas de macros ======================================================== */
#define NUM_SENSORES_SPS30 3 /**< Número de sensores SPS30 a gestionar */

/* === Declaraciones públicas de tipos de datos ============================================== */

/**
 * @brief Estructura que representa un sensor SPS30 con su configuración asociada.
 */
typedef struct {
    SPS30 sensor;              /**< Objeto de comunicación SPS30 */
    uint8_t id;                /**< ID único del sensor */
    UART_HandleTypeDef * uart; /**< UART asociada al sensor */
} SensorSPS30;

/* === Declaraciones públicas de variables ==================================================== */

extern SensorSPS30 sensores_sps30[NUM_SENSORES_SPS30];
extern int sensores_disponibles;

/* === Declaraciones públicas de funciones ==================================================== */

/**
 * @brief Inicializa las estructuras de los sensores SPS30.
 *
 * Esta función debe llamarse una vez al inicio del sistema para configurar
 * las UART y registrar cada sensor con su ID y canal correspondiente.
 */
void inicializar_sensores_sps30(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_SPS30_MULTI_H_ */
