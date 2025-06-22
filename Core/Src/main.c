/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>  // Añade esto para sprintf
#include <string.h> // Añade esto para strlen
#include "sps30_multi.h"
#include "sps30_comm.h"
#include "uart.h"
#include "proceso_observador.h"
#include "data_logger.h"
#include "time_rtc.h"
#include "rtc_ds3231_for_stm32_hal.h"
#include "fatfs_sd.h"
#include "microSD.h"
#include "mp_sensors_info.h"
#include "DHT22.h"
#include "sensors.h"

#include "sistema_init.h"

#include "test_format_csv.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

// #define UART_BUFFER_SIZE 64

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

// Declaración del objeto SPS30

// extern UART_HandleTypeDef *uart_debug;

// extern void test_format_csv_line(void);

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    sensors_init_all();

    // SPS30_Init(&huart5);

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_RTC_Init();
    MX_UART5_Init();
    MX_USART3_UART_Init();
    MX_UART7_Init();
    MX_USART6_UART_Init();
    MX_USART1_UART_Init();
    MX_I2C2_Init();
    MX_SPI1_Init();
    MX_FATFS_Init();
    /* USER CODE BEGIN 2 */

    uart_debug = &huart3;

    /* Initialization welcome message */
    uart_print("\n\n-------------------------------------------------------------------\n");
    uart_print("|   Sistema de Monitoreo de Material Particulado  |\n");
    uart_print("---------------------------------------------------------------------\n");

    bool sistema_ok = sistema_verificar_componentes();

    if (!sistema_ok) {
        uart_print("[WARN] Error en la verificación del sistema. Algunos componentes no están "
                   "operativos.\r\n");
        // Aquí podrías registrar el error, encender un LED de advertencia o guardar en un log.
    } else {
        uart_print("[INFO] Todos los componentes verificados correctamente.\r\n");
    }

    // Crea una nueva instancia de MicroSD
    MicroSD * sd =
        microSD_create(&huart3, "initlog.txt", "/"); // Inicialización con el directorio raíz
    uart_print("Inicializando sistema de almacenamiento de datos 1A ...\n");

    if (sd == NULL) {
        // Manejar error de creación
        Error_Handler();
    }

    uart_print("fin Inicializando sistema de almacenamiento de datos 4...\n");
    microSD_setDirectory(sd, "/"); // Cambia el directorio según sea necesario

    uart_print("Inicializando sistema de almacenamiento de datos 3 ...\n");
    if (!data_logger_init()) {
        uart_print("¡Error al inicializar el sistema de almacenamiento!\n");
    }

    HAL_Delay(200);

    /* Inicializar sensores SPS30 disponibles*/

    rtc_auto_init(); // Detecta y configura el RTC correcto

    //    test_format_csv_line();

    RTC_ReceiveTimeFromTerminal(&huart3);

    /* Initialize SPS30 sensors array */

    uart_print("Inicializando sensores SPS30...\n");
    inicializar_sensores_sps30();

    mp_sensors_info_init(); // ← Aquí obtienes y guardas los seriales

    /* Buffer de Mensajes */

    uint32_t ciclo_contador = 0;
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    while (1) {

        /* === Medición de sensores DHT22 (ambiente y cámara) ========================== */

        DHT22_Data sensorData;
        float temp_amb = -99.9f;
        float hum_amb = -99.9f;
        float temp_cam = -99.9f;
        float hum_cam = -99.9f;

        if (DHT22_Read(&dhtA, &sensorData) == DHT22_OK) {
            temp_amb = sensorData.temperatura;
            hum_amb = sensorData.humedad;
            uart_print("Ambiente: Temp: %.1f C, Hum: %.1f%%\n", temp_amb, hum_amb);
        } else {
            uart_print("Error leyendo DHT22 ambiente\n");
        }

        if (DHT22_Read(&dhtB, &sensorData) == DHT22_OK) {
            temp_cam = sensorData.temperatura;
            hum_cam = sensorData.humedad;
            uart_print("Camara: Temp: %.1f C, Hum: %.1f%%\n", temp_cam, hum_cam);
        } else {
            uart_print("Error leyendo DHT22 cámara\n");
        }

        /* =========Pruebas de Buffer ============*/

        uart_print("Imprime Sumario\n");
        data_logger_print_summary(); /* para borrar*/

        uart_print("Imprime resumen\n");
        data_logger_print_value(); /* para borrar*/

        /* === Timestamp y encabezado de ciclo ========================================= */
        char datetime_buffer[32];
        char msg_buffer[128];
        time_rtc_GetFormattedDateTime(datetime_buffer, sizeof(datetime_buffer));
        time_rtc_ActualizarEstadoPorTiempo(); // <--- ACTUALIZA LA MÁQUINA DE ESTADOS

        snprintf(msg_buffer, sizeof(msg_buffer), "\n=== Ciclo de medicion #%lu: %s ===\n",
                 ++ciclo_contador, datetime_buffer);
        uart_print(msg_buffer);

        /* === Ciclo de medición SPS30 ================================================= */
        for (int i = 0; i < sensores_disponibles; i++) {
            proceso_observador_3PM_2TH(&sensores_sps30[i].sensor, sensores_sps30[i].id,
                                       datetime_buffer, temp_amb, hum_amb, temp_cam, hum_cam);
        }

        /* === Reporte de resumen cada 10 ciclos ======================================= */
        if (ciclo_contador % 10U == 0U) {
            data_logger_print_summary();

            float pm25_avg = data_logger_get_average_pm25_id(0U, 10U);
            snprintf(msg_buffer, sizeof(msg_buffer),
                     "Promedio PM2.5 (ultimas 10 mediciones): %.2f ug/m3\n", pm25_avg);
            uart_print(msg_buffer);
        }

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */

        HAL_Delay(10000); // Espera 10 segundos antes de la próxima lectura
    }

    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t * file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
