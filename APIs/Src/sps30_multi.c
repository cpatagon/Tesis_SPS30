/**
 * @file sps30_multi.c
 * @brief Inicialización condicional de múltiples sensores SPS30.
 *
 * Este módulo inicializa solo los sensores realmente definidos y conectados.
 * Se basa en los UART que están habilitados en el sistema.
 *
 * @author lgomez
 * @date 04-05-2025
 * @copyright (C) 2023 [Tu nombre o institución]
 * @license GNU GPL v3.0
 */

#include "sps30_multi.h"



#define ID_SENSOR_UNO 1
#define ID_SENSOR_DOS 2
#define ID_SENSOR_TRES 3



/* === UARTs disponibles según usart.c === */
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart7;
extern UART_HandleTypeDef huart1;



/* === Variables globales ===================================================================== */

SensorSPS30 sensores_sps30[NUM_SENSORES_SPS30];
int sensores_disponibles = 0;



/* === Funciones ============================================================================== */

void inicializar_sensores_sps30(void) {
    sensores_disponibles = 0;

#if 1  // UART5 está habilitado
    sensores_sps30[sensores_disponibles].id   =  ID_SENSOR_UNO;
    sensores_sps30[sensores_disponibles].uart = &huart5;
    SPS30_init(&sensores_sps30[sensores_disponibles].sensor, &huart5);
    sensores_disponibles++;
#endif

#if 1  // Habilitar cuando uses UART7
    sensores_sps30[sensores_disponibles].id   =  ID_SENSOR_DOS;
    sensores_sps30[sensores_disponibles].uart = &huart7;
    SPS30_init(&sensores_sps30[sensores_disponibles].sensor, &huart7);
    sensores_disponibles++;
#endif

#if 1  // Habilitar cuando uses UART8
    sensores_sps30[sensores_disponibles].id   =  ID_SENSOR_TRES;
    sensores_sps30[sensores_disponibles].uart = &huart1;
    SPS30_init(&sensores_sps30[sensores_disponibles].sensor, &huart1);
    sensores_disponibles++;
#endif
}
