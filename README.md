# Sistema de Medición de Material Particulado (MP2.5)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![STM32F4](https://img.shields.io/badge/STM32-F429ZI-blue)](https://www.st.com/en/microcontrollers-microprocessors/stm32f429zi.html)
[![Sensor: SPS30](https://img.shields.io/badge/Sensor-SPS30-green)](https://sensirion.com/products/catalog/SPS30/)

## 📋 Descripción

Este sistema embebido permite la medición precisa y confiable de material particulado fino (MP2.5) en ambientes urbanos y rurales. Diseñado para formar parte de redes de monitoreo de calidad del aire urbanas, contribuye a la gestión ambiental y la salud pública mediante la recolección, procesamiento y transmisión de datos sobre concentraciones de partículas en el aire.

El sistema integra múltiples sensores SPS30 para ofrecer mediciones redundantes, mejorando la precisión y confiabilidad. Incorpora capacidades de análisis estadístico en tiempo real, almacenamiento local y transmisión inalámbrica de datos.

## ⚙️ Características principales

- **Medición redundante**: Tres sensores SPS30 operando en paralelo para mayor precisión y confiabilidad
- **Registro temporal**: Integración con RTC DS3231 para sincronización temporal precisa (±2ppm)
- **Almacenamiento local**: Sistema de archivos FAT32 en tarjeta microSD
- **Transmisión inalámbrica**: Módulo ESP8266 para envío de datos a servidores remotos
- **Bajo consumo**: Estrategias de optimización energética para operación con batería
- **Procesamiento estadístico**: Análisis en tiempo real para validación y corrección de mediciones
- **Monitoreo ambiental**: Sensor DHT22 para variables de temperatura y humedad
- **Diseño robusto**: Sistema de respaldo de alimentación y recuperación ante fallos

## 🏗️ Arquitectura del sistema

### Hardware

| Componente | Modelo | Características | Interfaz |
|------------|--------|-----------------|----------|
| Microcontrolador | STM32F429ZI | ARM Cortex-M4, 180 MHz, FPU | - |
| Sensores MP | 3 × SPS30 | Rango: 0-1000 μg/m³, 4 fracciones de tamaño | UART (115200 baudios) |
| Sensor ambiental | DHT22 | Temperatura: -40 a 80°C, Humedad: 0-100% | Digital (1-Wire) |
| Reloj RTC | DS3231 | Precisión: ±2ppm | I²C |
| Almacenamiento | Módulo microSD | Sistema FAT32 | SPI (42 MHz) |
| Comunicación | ESP8266 | WiFi 802.11 b/g/n | UART (115200 baudios) |
| Alimentación | S-25-5 | Entrada: 220V AC, Salida: 5V DC, 5A | - |

### Software

La arquitectura de software implementa una estructura de tres capas:

1. **Capa de aplicación**
   - Proceso observador (adquisición de datos)
   - Proceso de análisis (validación estadística)
   - Proceso de almacenamiento
   - Proceso de comunicación
   - Sistema de alarmas

2. **Capa de drivers**
   - Controladores específicos para cada periférico
   - Implementación de protocolos (SHDLC, FAT32)

3. **Capa HAL**
   - Interfaces estandarizadas para acceso al hardware STM32
   - Abstracciones de periféricos (GPIO, UART, I2C, SPI)

## 🔄 Protocolos implementados

| Protocolo | Aplicación | Características |
|-----------|------------|-----------------|
| SHDLC | Sensores SPS30 | Protocolo propietario Sensirion, 115200 baudios |
| I²C | RTC DS3231 | 100 kHz, dirección 0x68 |
| SPI | Tarjeta microSD | 42 MHz, FAT32 |
| UART | Módulo ESP8266 | 115200 baudios, control de flujo hardware |
| HTTP | Transmisión de datos | Formato JSON, solicitudes POST |

## 📊 Flujo de datos

El sistema implementa el siguiente flujo de procesamiento:

1. **Adquisición**: Muestreo programable desde sensores (intervalo mínimo: 1 segundo)
2. **Validación**: Detección de valores atípicos y errores de medición
3. **Procesamiento**: Cálculo de promedios móviles y estadísticas
4. **Corrección**: Ajuste por variables ambientales (temperatura, humedad)
5. **Almacenamiento**: Registro en microSD con estructura jerárquica temporal
6. **Transmisión**: Envío a servidor remoto mediante protocolo HTTP

## 📁 Estructura del repositorio

```
Tesis_SPS30/
├── APIs
│   ├── Config
│   │   └── rtc_config.h
│   ├── Inc
│   │   ├── data_logger.h
│   │   ├── fatfs_sd.h
│   │   ├── microSD.h
│   │   ├── ParticulateDataAnalyzer.h
│   │   ├── proceso_observador.h
│   │   ├── rtc_buildtime.h
│   │   ├── rtc_ds1307_for_stm32_hal.h
│   │   ├── rtc_ds3231_for_stm32_hal.h
│   │   ├── shdlc.h
│   │   ├── sps30_comm.h
│   │   ├── sps30_multi.h
│   │   ├── time_rtc.h
│   │   └── uart.h
│   └── Src
│       ├── data_logger.c
│       ├── fatfs_sd.c
│       ├── microSD.c
│       ├── ParticulateDataAnalyzer.c
│       ├── proceso_oservador.c
│       ├── rtc_buildtime.c
│       ├── rtc_ds1307_for_stm32_hal.c
│       ├── rtc_ds3231_for_stm32_hal.c
│       ├── shdlc.c
│       ├── sps30_comm.c
│       ├── sps30_multi.c
│       ├── time_rtc.c
│       └── uart.c
├── Core
│   ├── Inc
│   │   ├── gpio.h
│   │   ├── i2c.h
│   │   ├── main.h
│   │   ├── rtc.h
│   │   ├── spi.h
│   │   ├── stm32f4xx_hal_conf.h
│   │   ├── stm32f4xx_it.h
│   │   └── usart.h
│   ├── Src
│   │   ├── gpio.c
│   │   ├── i2c.c
│   │   ├── main.c
│   │   ├── rtc.c
│   │   ├── spi.c
│   │   ├── stm32f4xx_hal_msp.c
│   │   ├── stm32f4xx_it.c
│   │   ├── syscalls.c
│   │   ├── sysmem.c
│   │   ├── system_stm32f4xx.c
│   │   └── usart.c
│   └── Startup
│       └── startup_stm32f429zitx.s
```

## ✅ Estado actual del proyecto

| Módulo | Estado | Detalles |
|--------|--------|----------|
| Inicialización STM32 | ✅ Completo | Configuración de reloj, GPIO, UARTs, I2C, SPI |
| Drivers SPS30 | ✅ Completo | Comunicación SHDLC, funciones wake_up, read_data, etc. |
| Comunicación multi-sensor | ✅ Operativo | Tres sensores operando por UART1, UART5 y UART7 |
| Proceso Observador | ✅ Completo | Adquisición c/1min y cálculo promedio c/10min almacenado en SD |
| Análisis estadístico | ⚠️ Parcial | Implementadas funciones base para cálculos estadísticos |
| Almacenamiento microSD | ✅ Operativo | Archivos CSV por timestamp, con sincronización |
| Comunicación WiFi/ESP8266 | ⏳ Pendiente | Envío por HTTP y manejo de reconexiones |
| Sistema de alarmas | ⏳ Pendiente | LED y notificaciones de error |
| Documentación Doxygen | ⚠️ En progreso | Comentarios iniciales agregados en todos los archivos |

## 🎯 Objetivos próximos

- Finalizar el Proceso de Análisis
  - Validar coherencia entre sensores con límite máximo de 15% de desviación
  - Aplicar corrección estadística con coeficientes configurables

- Desarrollar Comunicación vía WiFi
  - Sistema de envío por HTTP con formato JSON
  - Implementar buffer para datos no enviados con reintento exponencial

- Sistema de Alarmas
  - Indicadores LED con diferentes frecuencias según estado
  - Almacenamiento de códigos de error en memoria no volátil

- Optimización
  - Reducción de consumo energético para operación con batería
  - Optimización de memoria RAM (<128KB) y flash (<512KB)

- Documentación y Pruebas
  - Completar documentación Doxygen para todas las funciones
  - Desarrollar pruebas unitarias para cada módulo

## 🛠️ Compilación e instalación

### Prerrequisitos

- STM32CubeIDE 1.13.0 o superior
- STM32CubeMX 6.9.0 o superior
- Compilador ARM GCC
- Herramientas de programación STM32 (ST-Link)

### Pasos para compilar

1. Clonar el repositorio:
   ```bash
   git clone https://github.com/username/Tesis_SPS30.git
   cd Tesis_SPS30
   ```

2. Abrir el proyecto en STM32CubeIDE:
   - Abrir STM32CubeIDE
   - Seleccionar File > Open Projects from File System
   - Seleccionar el directorio del proyecto

3. Compilar el proyecto:
   - Hacer clic derecho en el proyecto > Build Project

4. Programar el microcontrolador:
   - Conectar el programador ST-Link al puerto SWD
   - Hacer clic derecho en el proyecto > Run As > STM32 C/C++ Application

## 🔌 Conexiones

### Pines del microcontrolador STM32F429ZI

| Periférico | Pin | Función | Notas |
|------------|-----|---------|-------|
| Sensor SPS30 #1 | PA9/PA10 | UART1 TX/RX | 115200 baudios |
| Sensor SPS30 #2 | PC12/PD2 | UART5 TX/RX | 115200 baudios |
| Sensor SPS30 #3 | PE8/PE7 | UART7 TX/RX | 115200 baudios |
| RTC DS3231 | PB8/PB9 | I2C1 SCL/SDA | 100 kHz |
| MicroSD | PA5/PA6/PA7/PB5 | SPI1 SCK/MISO/MOSI/CS | 42 MHz |
| ESP8266 | PC6/PC7 | UART6 TX/RX | 115200 baudios |
| DHT22 | PD0 | GPIO | Digital 1-Wire |
| LED Estado | PB0 | GPIO | PWM |
| Sensor Batería | PA0 | ADC | Divisor de tensión |

## 📊 Resultados preliminares

El sistema ha sido evaluado en condiciones de laboratorio y campo, con los siguientes resultados:

- **Tiempo de inicio del sensor**: 8-30 segundos dependiendo de la concentración
- **Precisión de medición**: ±10% comparado con sensores de referencia
- **Consumo de energía**: 55mA en modo medición continua, 3.5mA con ciclo de trabajo optimizado
- **Completitud de datos**: 90.54% en entorno operativo real
- **Correlación entre sensores**: r > 0.90 para todas las parejas de sensores

### Estrategia de bajo consumo

Se ha implementado un esquema de bajo consumo que:
- Activa el sensor solo durante los períodos de medición
- Pone el sensor en modo sleep entre mediciones
- Optimiza el ciclo de trabajo del ventilador

Con esta estrategia, se logra una reducción del consumo de hasta 15 veces comparado con el modo continuo.

## 📚 Referencias

- [Datasheet SPS30](https://sensirion.com/products/catalog/SPS30/)
- [Manual de referencia STM32F429ZI](https://www.st.com/resource/en/reference_manual/dm00031020-stm32f405-415-stm32f407-417-stm32f427-437-and-stm32f429-439-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf)
- [FAT File System Module for STM32](http://elm-chan.org/fsw/ff/00index_e.html)
- Kuula, J., Kuuluvainen, H., Rönkkö, T. et al. (2020). Applicability of optical and diffusion charging-based particulate matter sensors to urban air quality measurements. Aerosol Air Qual. Res., 20, 1-16.
- Nasar, Z., Selleck, P., Abed, E. A., & Shukla, N. (2024). Low-cost sensors for air quality monitoring: A comprehensive review. Sensors and Actuators B: Chemical, 394, 134481.

## 📄 Licencia

Este proyecto se distribuye bajo la [Licencia GNU GPL v3](https://www.gnu.org/licenses/gpl-3.0.html).

## 👤 Autor

**Luis Gómez** - *Tesis de Magíster en Sistemas Embebidos* - Universidad de Buenos Aires

## 🙏 Agradecimientos

- Universidad de Buenos Aires, Facultad de Ingeniería
- Carrera de Especialización en Sistemas Embebidos
- Laboratorio de Sistemas Embebidos
- Dr. [Nombre del Director] - Director de tesis
