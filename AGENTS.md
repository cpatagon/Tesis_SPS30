# `AGENTS.md` – Sistema Embebido para Medición de MP2.5

## 🧭 Información General del Proyecto

- **Proyecto**: Tesis FIUBA – Sistema embebido de monitoreo de material particulado fino PM2.5
- **Plataforma**: STM32F429ZI (ARM Cortex-M4, 180 MHz, FPU)
- **IDE**: STM32CubeIDE + CubeMX (versión 1.27.1)
- **Lenguaje**: C (compatible C99/C11)
- **RTOS**: No utilizado (baremetal)
- **Estilo**: Modular, no bloqueante, con buffers circulares y Doxygen

## 📁 Estructura del Proyecto

```bash
Tesis_SPS30/
├── APIs/
│   ├── Inc/   # Headers de módulos de aplicación y drivers
│   └── Src/   # Implementación de cada módulo funcional
├── Core/
│   ├── Inc/   # HAL + configuración general del sistema
│   └── Src/   # main.c, inicialización de periféricos, handlers
├── Middlewares/
│   └── FatFs/ # Sistema de archivos FAT32 para microSD
└── Startup/
    └── startup_stm32f429xx.s
```

## 🔌 Hardware y Periféricos

| Componente      | Función                            | Interfaz     | Pines STM32                  |
|----------------|-------------------------------------|--------------|------------------------------|
| SPS30 (x3)     | Sensor PM UART                     | UART1/5/7    | PA9/PA10, PC12/PD2, PE8/PE7  |
| DHT22 (x2)     | Temperatura y humedad              | GPIO/1-Wire  | PB11, PB12                   |
| RTC DS3231     | Tiempo real preciso                | I²C2         | PB8 (SCL), PB9 (SDA)         |
| microSD        | Almacenamiento FAT32               | SPI1         | PA5, PA6, PA7, PB5           |
| ESP8266        | Comunicación WiFi (en progreso)    | UART6        | PC6, PC7                     |
| UART Debug     | Consola serial para logging        | UART3        | (115200 baudios)             |

## 🧱 Arquitectura del Software

### 🔸 Capa de Aplicación
- `proceso_observador.c`: Muestreo periódico y control de adquisición
- `data_logger.c`: Registro en CSV, estructura `/YYYY/MM/DD/`
- `ParticulateDataAnalyzer.c`: Estadísticas e IQR para validación
- `microSD_utils.c`: Formateo de strings y rutas
- `sistema_alarmas.c`: (pendiente) Estados LED y fallos

### 🔹 Capa de Drivers
- `sps30_multi.c`: Comunicación SHDLC para múltiples SPS30
- `rtc_ds3231_for_stm32_hal.c`: Acceso I²C + fallback automático
- `fatfs_sd.c`: Inicialización y manejo del filesystem FAT
- `DHT22.c`: Sensor con timings estrictos
- `uart.c`: Abstracción de impresión por UART

### 🔸 Capa HAL
- `main.c`, clocks, interrupciones, GPIO, DMA (via CubeMX)

## 📐 Estilo y Reglas de Programación

### ⛔ Reglas de No Bloqueo
- Todas las funciones deben evitar llamadas bloqueantes como `HAL_Delay()`.
- Utilizar temporizadores, flags y estado de máquina para flujos asincrónicos.
- Las operaciones con sensores o periféricos deben tener timeouts definidos.
- Se debe preferir diseño basado en eventos, polling no bloqueante o interrupciones.


### 🧾 Convenciones
- `snake_case` en funciones y variables
- `UPPER_SNAKE_CASE` en constantes
- `PascalCase_t` en `struct`
- Prefijos por módulo: `sps30_`, `data_logger_`, `rtc_`, etc.

### 📚 Requisitos
- Indentación: 4 espacios
- Máximo: 100 caracteres por línea
- Comentarios Doxygen obligatorios en funciones públicas
- Evitar `malloc()` / `free()`, usar buffers estáticos
- Todas las funciones públicas deben retornar estado (`enum`)

## 📊 Parámetros Críticos

```c
#define NUM_REINTENTOS_SPS30      3
#define CONC_MIN_PM               0.5f     // μg/m³
#define CONC_MAX_PM               1000.0f
#define DELAY_MEDICION_MS         5000     // Delay entre lecturas SPS30
#define FSYNC_EVERY_N_LINES       10
#define FSYNC_MAX_INTERVAL_MS     300000   // 5 minutos
```

### Buffers Circulares
```c
#define BUFFER_10MIN_SIZE         60       // Muestras cada 10s o 1min
#define BUFFER_1H_SIZE            24
#define BUFFER_24H_SIZE           30
```

## 🔄 Flujo del Sistema

1. Lectura cíclica de sensores (cada 10 s o 1 min)
2. Almacenamiento de datos crudos en buffer
3. Cada 10 minutos:
   - Calcular promedio, min, max, stddev, cantidad válida
   - Almacenar CSV (AVG10), limpiar buffer
   - Enviar por UART para monitoreo
4. Cada hora / día:
   - Generar AVG60 y AVG24
5. Fsync automático y estructura de carpetas `/AAAA/MM/DD/`

## ⚙️ Ejemplo de Función Correcta

```c
/**
 * @brief Lee y valida datos SPS30
 * @param sensor_id ID lógico del sensor (1 a 3)
 * @param data Puntero a estructura de salida
 * @param timeout_ms Tiempo máximo de espera
 * @return STATUS_OK si éxito, error si falla
 */
System_Status_t sps30_read_validated_data(uint8_t sensor_id,
                                          MP_SensorData_t* data,
                                          uint32_t timeout_ms);
```

## 📤 Logs de Diagnóstico (UART3)

```c
[23:16:02:142] [INFO] Sistema iniciado correctamente
[23:16:03:010] [ERROR] No se pudo iniciar medición SPS30 ID=1
[23:16:10:120] [OK] Datos promediados guardados: 00:10
```

## 🧪 Verificación y Testing

1. **Build Debug**: STM32CubeIDE (opción `-Og`)
2. **Build Release**: Optimización `-O2`
3. **Pruebas en hardware real**: Con 3 SPS30 y RTC activo
4. **Verificación de memoria**: `.map` file
5. **Logs UART**: Validación visual + parsing automático futuro
6. **Pruebas de regresión**: Verificar integridad tras cambios

## 📌 Consideraciones para Codex u otros Modelos

### Al agregar funciones:
- Documentar con Doxygen
- Usar `System_Status_t`
- No usar malloc()
- Usar `__WFI()` si corresponde a espera

### Al reportar errores:
- Logs UART completos
- Estado del hardware conectado
- Versión de firmware usada
- Pasos de reproducción exactos

## 🛠️ Archivos de Configuración Relevantes

### STM32CubeMX `.ioc`
- Frecuencia sistema: 180 MHz
- FPU: Habilitada
- SPI1: 42 MHz (MicroSD)
- UART: 6 habilitados
- I2C2: DS3231
- DMA: UARTx y SPI1
- RTC interno habilitado

### Configuración FatFS
- Formato FAT32 obligatorio
- fsync cada 10 líneas o 5 minutos
- Recuperación de filesystem al montar

### Configuración RTC
- DS3231 preferido
- Fallback a RTC interno si falla
- Timestamp en formato ISO 8601
- Sincronización en boot o por comando
