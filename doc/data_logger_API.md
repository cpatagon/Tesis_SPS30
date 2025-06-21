# Módulo `data_logger` - Sistema de Registro para Medición de Material Particulado (PM2.5)

## 📌 Descripción General

El módulo `data_logger` tiene como objetivo central registrar de forma estructurada y eficiente
los datos de concentración de material particulado (PM1.0, PM2.5, PM4.0 y PM10), temperatura y humedad
en una tarjeta microSD. Está diseñado para operar sobre un microcontrolador STM32F429ZI en un sistema bare-metal
sin RTOS, utilizando FATFS y sincronización temporal vía RTC DS3231.

---

## 🧭 Funcionalidades Principales

- Inicialización y montaje de sistema FAT32 en microSD.
- Almacenamiento de datos crudos y promedios temporales.
- Gestión de buffers circulares por frecuencia:
  - Alta frecuencia (cada 10s o 1min)
  - Promedios horarios
  - Promedios diarios
- Cálculo estadístico: media, máximo, mínimo, desviación estándar.
- Escritura automática en archivos `/AVG10/`, `/AVG60/`, `/AVG24/`.
- Formateo de líneas CSV con timestamp ISO8601.
- Visualización en UART para depuración y monitoreo.

---

## 📦 Estructura de Archivos

```
/YYYY/MM/DD/
├── RAW.csv                  # Datos crudos
├── avg10.csv                # Promedios cada 10 minutos
├── avg60.csv                # Promedios cada 1 hora
└── avg24.csv                # Promedios cada 24 horas
```

---

## 🧩 Estructuras de Datos

| Estructura             | Descripción                                         |
|------------------------|-----------------------------------------------------|
| `MedicionMP`           | Datos crudos con timestamp, ID sensor, PM, T, H     |
| `PMDataAveraged`       | Promedios estadísticos de PM2.5                     |
| `TimeWindow`           | Ventana activa de 10 minutos para acumular muestras |
| `TimeSyncedAverage`    | Resultado con estadísticas y timestamp              |
| `BufferCircular`       | FIFO para crudos y promedios                        |

---

## 🔁 Flujo General del Sistema

1. Se obtiene una medición de PM2.5 desde el sensor.
2. `proceso_analisis_periodico()` almacena el dato en una ventana de 10 minutos.
3. Si han pasado 10 min (según RTC), se:
   - Calcula el promedio
   - Se imprime por UART
   - Se guarda en `/AVG10/avg10.csv`
4. Cada 6 promedios → se calcula un promedio horario → `/AVG60/`
5. Cada 24 promedios horarios → se calcula un promedio diario → `/AVG24/`

---

## 📜 Diagrama de Flujo Modular

```mermaid
graph TD
    subgraph Adquisición
        A1[SPS30 / DHT22]
        A2[RTC_DS3231]
    end

    subgraph Procesamiento
        P1[proceso_analisis_periodico]
        P2[accumulate_sample_in_current_window]
        P3[finalize_temporal_window]
    end

    subgraph Estadísticas
        E1[calculateAverage]
        E2[findMinValue]
        E3[findMaxValue]
        E4[calculateStandardDeviation]
    end

    subgraph Almacenamiento
        S1[log_avg10_data]
        S2[log_avg1h_data]
        S3[log_avg24h_data]
        S4[save_temporal_average_to_csv]
        S5[data_logger_store_raw]
        S6[log_data_to_sd]
        S7[guardar_promedio_csv]
    end

    subgraph Infraestructura
        F1[f_mount]
        F2[f_open]
        F3[f_write]
        F4[f_mkdir]
        F5[uart_print]
    end

    A1 --> P1
    A2 --> P1
    P1 --> P2
    P2 --> P3
    P3 --> E1
    P3 --> E2
    P3 --> E3
    P3 --> E4
    E1 --> S1
    E1 --> S2
    E1 --> S3
    S1 --> S4
    S2 --> S4
    S3 --> S4
    P1 --> S5
    P1 --> S6
    S6 --> S7
    S4 --> F2
    S5 --> F4
    S6 --> F3
    S7 --> F3
    S1 --> F5
    S2 --> F5
    S3 --> F5
```

---

## ✍️ Autoría

- **Autor:** Luis Gómez
- **Fecha de creación:** 10 de mayo de 2025
- **Licencia:** GNU GPLv3
