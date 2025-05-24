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

#include "test_format_csv.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

//#define UART_BUFFER_SIZE 64


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

// Declaración del objeto SPS30
SPS30 sps30;
//extern UART_HandleTypeDef *uart_debug;


//extern void test_format_csv_line(void);


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
  MX_I2C2_Init();
  MX_SPI1_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
 // debug_sd_init();

/*
  HAL_Delay(200);
  //probar_spi_sd();           // <== Esto

  uart_print("Inicializando sistema de almacenamiento de datos 1...\n");
*/

	uart_debug = &huart3;

  // Crea una nueva instancia de MicroSD
      MicroSD *sd = microSD_create(&huart3, "prueba3.txt", "/"); // Inicialización con el directorio raíz
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

  rtc_auto_init();    // Detecta y configura el RTC correcto

  test_format_csv_line();

  RTC_ReceiveTimeFromTerminal(&huart3);

  // rtc_set_test_time(); // <- llamada de prueba

  /*TEST*/



  //char fecha_hora[32];
  //obtener_fecha_hora(fecha_hora);
 //uart_printf("Fecha y hora actual: %s\r\n", fecha_hora);

 /*
  inicializar_sensores_sps30();
*/



  /*Inicializar el objeto SPS30 con el manejador de UART*/
/*
  SPS30_init(&sps30, &huart5);
*/


  /* Initialize RTC */

//  uart_print("Inicializando RTC DS3231...\n");
  //time_rtc_Init(&hi2c2);



  /* Initialization welcome message */
  uart_print("\n\n-----------------------------------------------------------\n");
  uart_print("*** Sistema de Monitoreo de Material Particulado ***\n");
  uart_print("-----------------------------------------------------------\n");





  /* Initialize RTC */
//  uart_print("Inicializando RTC DS1307...\n");
  //time_rtc_Init(&hi2c2);

   /*Despierta al sensor SPS30*/
   sps30.wake_up(&sps30);
   uart_print("WAKE UP :\n");


   /* Initialize data logger */
//     uart_print("Inicializando sistema de almacenamiento de datos...\n");

     if (!data_logger_init()) {
         uart_print("¡Error al inicializar el sistema de almacenamiento!\n");
     }
     else{
    	 ParticulateData test_data = {
    	     .timestamp = "2025-05-22T20:30:00Z",
    	     .sensor_id = 1,
    	     .pm1_0 = 3.2,
    	     .pm2_5 = 5.6,
    	     .pm4_0 = 6.7,
    	     .pm10  = 7.2,
    	     .temp  = 23.4,
    	     .hum   = 42.1
    	 };

         data_logger_write_csv_line(&test_data);
     }

     /* Initialize SPS30 sensors array */

     uart_print("Inicializando sensores SPS30...\n");
     inicializar_sensores_sps30();

   /* Buffer de Mensajes */

   char datetime_buffer[32];
   char msg_buffer[128];
   uint32_t ciclo_contador = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  int contador = 1;
  char buffer[100];
  RTC_ReceiveTimeFromTerminal(&huart3);
    while (1) {

    	/*TEST*/
    	 snprintf(buffer, sizeof(buffer), "%d", contador);  // Convierte el contador a cadena
    	    	  microSD_appendLine(sd, buffer);  // Escribe la cadena en el archivo
    	    	  contador++;  // Incrementa el contador
    	    	  HAL_Delay(5000);




    	  /* Get current date and time */
   	        time_rtc_GetFormattedDateTime(datetime_buffer, sizeof(datetime_buffer));

    	        /* Format header message with timestamp and cycle counter */
    	        snprintf(msg_buffer, sizeof(msg_buffer),
    	                "\n=== Ciclo de medición #%lu: %s ===\n",
    	                ++ciclo_contador, datetime_buffer);
    	        uart_print(msg_buffer);

    	        /* Read all available sensors */
    	        for (int i = 0; i < sensores_disponibles; i++) {
    	            if (proceso_observador(&sensores_sps30[i].sensor, sensores_sps30[i].id)) {
    	                /* Get the last measurement data and store it */
    	                ConcentracionesPM valores = sensores_sps30[i].sensor.get_concentrations(&sensores_sps30[i].sensor);
    	                data_logger_store_measurement(sensores_sps30[i].id, valores, -999.0f, -999.0f);
    	            }
    	        }

    	        /* Print data summary every 10 cycles */
    	        if (ciclo_contador % 10 == 0) {
    	        	data_logger_print_summary();


    	            /* Print average PM2.5 of all sensors */
    	            float pm25_avg = data_logger_get_average_pm25(0, 10);
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
