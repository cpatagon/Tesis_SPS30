# Sistema de Medición de Material Particulado (MP2.5)

> Proyecto de Tesis de Magíster en Sistemas Embebidos – UBA  
> Autor: Luis Gómez  
> Año: 2025

---

## 📌 Resumen

Este proyecto desarrolla un sistema embebido de bajo consumo para medir concentraciones de material particulado fino (MP2.5) en ambientes urbanos y rurales. Utiliza múltiples sensores SPS30 controlados por un microcontrolador STM32F429ZI, con almacenamiento en microSD, transmisión por WiFi y un sistema robusto de adquisición, análisis y almacenamiento de datos.

---

## ⚙️ Arquitectura del Sistema

![Arquitectura del Sistema](https://via.placeholder.com/800x400?text=Diagrama+de+Arquitectura)

### Hardware

- **Microcontrolador:** STM32F429ZI (ARM Cortex-M4, 180 MHz)
- **Sensores de Material Particulado:** 3 × SPS30 vía UART (115200 baudios)
- **Sensor ambiental:** DHT22 (Temperatura y Humedad)
- **Reloj RTC:** DS3231 vía I²C
- **Almacenamiento:** microSD con FAT32 (SPI a 42 MHz)
- **Comunicación:** ESP8266 vía UART (115200 baudios)
- **Alimentación:** Sistema con conmutación automática a batería

### Software

La arquitectura implementa una estructura de tres capas:

1. **Capa de aplicación:** Procesos principales y lógica de negocio
2. **Capa de drivers:** Controladores específicos para cada periférico
3. **Capa HAL:** Interfaces estandarizadas para acceso al hardware STM32

---

## ✅ Estado Actual del Proyecto

| Módulo                     | Estado       | Detalles                                                       |
|---------------------------|--------------|----------------------------------------------------------------|
| Inicialización STM32      | ✅ Completo   | Configuración de reloj, GPIO, UARTs, I2C, SPI                  |
| Drivers SPS30             | ✅ Completo   | Comunicación SHDLC, funciones `wake_up`, `read_data`, etc.     |
| Comunicación multi-sensor | ✅ Operativo  | Tres sensores operando por UART1, UART5 y UART7               |
| Proceso Observador        | ⚠️ Parcial    | Implementada la lectura periódica con reintentos               |
| Análisis estadístico      | ⚠️ Parcial    | Implementadas funciones base para cálculos estadísticos        |
| Almacenamiento microSD    | ⏳ Pendiente  | Estructura de archivos CSV, sincronización, recuperación       |
| Comunicación WiFi/ESP8266 | ⏳ Pendiente  | Envío por HTTP y manejo de reconexiones                        |
| Sistema de alarmas        | ⏳ Pendiente  | LED y notificaciones de error                                  |
| Documentación Doxygen     | ⚠️ En progreso| Comentarios iniciales agregados en todos los archivos          |

### Módulos Implementados

- **Comunicación UART para sensores SPS30**: Implementación robusta del protocolo SHDLC.
- **Gestión de múltiples sensores**: Sistema para operar tres sensores SPS30 en paralelo.
- **Proceso Observador**: Captura datos de los sensores con manejo de errores y reintentos.
- **Análisis de datos**: Funciones estadísticas para validación y corrección de mediciones.

---

## 🎯 Objetivos Próximos

1. **Finalizar el Proceso Observador**
   - Integrar los buffers circulares para almacenamiento temporal.
   - Mejorar el manejo de errores y estrategia de reintentos.

2. **Implementar el Almacenamiento en SD**
   - Desarrollar sistema de archivos para datos CSV con timestamp.
   - Implementar mecanismos de recuperación ante corrupción.

3. **Completar el Proceso de Análisis**
   - Validar coherencia entre sensores con límite máximo de 15% de desviación.
   - Aplicar corrección estadística con coeficientes configurables.

4. **Desarrollar Comunicación vía WiFi**
   - Sistema de envío por HTTP con formato JSON.
   - Implementar buffer para datos no enviados con reintento exponencial.

5. **Sistema de Alarmas**
   - Indicadores LED con diferentes frecuencias según estado.
   - Almacenamiento de códigos de error en memoria no volátil.

6. **Optimización**
   - Reducción de consumo energético para operación con batería.
   - Optimización de memoria RAM (<128KB) y flash (<512KB).

7. **Documentación y Pruebas**
   - Completar documentación Doxygen para todas las funciones.
   - Desarrollar pruebas unitarias para cada módulo.

---

## 📂 Estructura del Repositorio

```
Tesis_SPS30/
├── Core/
│   ├── Inc/
│   │   ├── sps30_comm.h        # Comunicación con sensores SPS30
│   │   ├── sps30_multi.h       # Gestión de múltiples sensores
│   │   ├── shdlc.h             # Implementación protocolo SHDLC  
│   │   ├── uart_printing.h     # Utilidades para debug por UART
│   │   ├── proceso_observador.h # Proceso de adquisición
│   │   └── ParticulateDataAnalyzer.h # Análisis estadístico
│   └── Src/
│       ├── sps30_comm.c
│       ├── sps30_multi.c
│       ├── shdlc.c
│       ├── uart_printing.c
│       ├── proceso_observador.c
│       └── ParticulateDataAnalyzer.c
├── Drivers/           # Drivers HAL de STM32
├── Middlewares/       # Middlewares (FatFS, etc.)
├── Tesis_SPS30.ioc    # Configuración CubeMX
└── README.md
```

---

## 📊 Resultados Preliminares

El sistema ha sido probado con sensores reales en condiciones controladas. Los resultados muestran:

- Tiempo de inicio del sensor: 8-30 segundos dependiendo de la concentración
- Precisión de medición: ±10% comparado con sensores de referencia
- Consumo de energía: 55mA en modo medición continua, 3.5mA con ciclo de trabajo optimizado

### Estrategia de Bajo Consumo

Siguiendo las recomendaciones del fabricante, se ha implementado un esquema de bajo consumo que:

1. Activa el sensor solo durante los períodos de medición
2. Pone el sensor en modo sleep entre mediciones
3. Optimiza el ciclo de trabajo del ventilador

Con esta estrategia, se logra una reducción del consumo de hasta 15 veces comparado con el modo continuo.

---

## 🔍 Notas Técnicas

### Protocolo SHDLC

El SPS30 utiliza el protocolo SHDLC (Sensirion HDLC) para la comunicación UART, con las siguientes características:

- Velocidad: 115200 baudios
- Formato: 8 bits de datos, sin paridad, 1 bit de parada
- Formato de trama: `0x7E [ADDR] [CMD] [LEN] [DATA] [CHECKSUM] 0x7E`

### Modos de Operación del SPS30

El sensor tiene tres modos principales:
- **Medición**: Fan y láser activos, consumo 45-65mA
- **Idle**: Electrónica activa pero fan y láser apagados
- **Sleep**: Modo de bajo consumo (~38μA)

---

## 📜 Referencias

- Datasheets del sensor SPS30
- Documentación STM32F4xx
- Artículos científicos sobre medición de material particulado

---

## 📝 Licencia

Este proyecto se distribuye bajo la Licencia GNU GPL v3.

---
