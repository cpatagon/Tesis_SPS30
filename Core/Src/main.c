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
#include "rtc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>  // Añade esto para sprintf
#include <string.h> // Añade esto para strlen
//#include "sps30.h"
#include "sps30_multi.h"
#include "sps30_comm.h"
#include "uart_printing.h"
#include "proceso_observador.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

// Declaración del objeto SPS30
SPS30 sps30;
UART_Printing uart;

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
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

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
  /* USER CODE BEGIN 2 */

  // Inicializar sensores SPS30 disponibles
  inicializar_sensores_sps30();

/*

  */

    // Inicializar el objeto SPS30 con el manejador de UART
        SPS30_init(&sps30, &huart5);
        UART_Printing_init(&uart, &huart3);



        static uint8_t * message =
            (uint8_t *)"\n\n"
                       "-----------------------------------------------------------\n"
                       "*** UART port initialization successful !!! ***\n"
                       "-----------------------------------------------------------\n";

        HAL_UART_Transmit(&huart3, (uint8_t *)message, strlen((char *)message), HAL_MAX_DELAY);

        uart.print(&uart, "\n*********************************************\n");
        uart.print(&uart, "WAKE UP :\n");
        sps30.wake_up(&sps30);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1) {

    	  for (int i = 0; i < sensores_disponibles; i++) {
    	      proceso_observador(&sensores_sps30[i].sensor, &uart, sensores_sps30[i].id);
    	  }




    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    	/*
    	  uart.print(&uart, "\n*********************************************\n");
    	        uart.print(&uart, "START MEASUREMENT:\n");
    	        sps30.start_measurement(&sps30);

    	        HAL_Delay(2500);

    	        uart.print(&uart, "\n*********************************************\n");
    	        uart.print(&uart, "READ DATA 1:\n");
    	        //sps30.read_data(&sps30);


    	        ConcentracionesPM datos = sps30.get_concentrations(&sps30);

    	        char buffer[128];
    	        snprintf(buffer, sizeof(buffer),
    	                 "PM1.0 : %.2f ug/m3\n"
    	                 "PM2.5 : %.2f ug/m3\n"
    	                 "PM4.0 : %.2f ug/m3\n"
    	                 "PM10  : %.2f ug/m3\n",
    	                 datos.pm1_0,
    	                 datos.pm2_5,
    	                 datos.pm4_0,
    	                 datos.pm10);

    	        uart.print(&uart, buffer);

    	        HAL_Delay(1000);

    	        uart.print(&uart, "\n*********************************************\n");
    	        uart.print(&uart, "SERIAL NUMBER:\n");
    	        sps30.serial_number(&sps30);

    	        HAL_Delay(100);

    	        uart.print(&uart, "\n*********************************************\n");
    	        uart.print(&uart, "STOP MEASUREMENT:\n");
    	        sps30.stop_measurement(&sps30);

    	        HAL_Delay(100);

    	        uart.print(&uart, "\n*********************************************\n");
    	        uart.print(&uart, "SLEEP :\n");
    	        sps30.sleep(&sps30);

    	        HAL_Delay(4000); // Espera 10 segundos antes de la próxima lectura

    	        uart.print(&uart, "\n*********************************************\n");
    	        uart.print(&uart, "WAKE UP :\n");
    	        sps30.wake_up(&sps30);
  */

        HAL_Delay(10000); // Espera 10 segundos antes de la próxima lectura
    }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
