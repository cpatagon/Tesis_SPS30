# 📄 observador_MEF.c

> Implementación de la **Máquina de Estados Finitos (MEF)** para el sistema de observación de material particulado PM2.5.

---

## 🧾 Información del archivo

- **Autor**: lgomez
- **Fecha de creación**: 22-06-2025
- **Licencia**: GNU GPL v3.0
- **SPDX-License-Identifier**: GPL-3.0-only

---

## 🧠 Propósito

Este archivo coordina el ciclo de adquisición de datos en un sistema embebido STM32. A través de una MEF se gestionan sensores, almacenamiento y análisis, controlando la transición entre distintos estados.

---

## 📊 Estados del Observador

| Estado             | Descripción                                                                 |
|--------------------|-----------------------------------------------------------------------------|
| `ESTADO_REPOSO`     | Espera activa hasta que el RTC indique que debe comenzar adquisición.       |
| `ESTADO_LECTURA`    | Lectura de datos de sensores SPS30 y DHT22.                                 |
| `ESTADO_ALMACENAMIENTO` | Guarda las mediciones en el buffer circular de 10 minutos.                 |
| `ESTADO_CALCULO`     | Calcula estadísticas (prom, min, max, std) si hay cambio de bloque horario. |
| `ESTADO_GUARDADO`     | Escribe los promedios en la microSD en formato CSV.                         |
| `ESTADO_LIMPIESA`     | Limpia buffers y vuelve a estado inicial.                                   |
| `ESTADO_ERROR`        | Error detectado en adquisición, vuelve al estado de reposo.                 |

---

## 🔧 Funciones públicas

| Función                          | Descripción                                                                 |
|----------------------------------|-----------------------------------------------------------------------------|
| `observador_MEF_init()`          | Inicializa la MEF en estado `REPOSO`.                                      |
| `observador_MEF_cambiar_estado()`| Cambia el estado actual de la MEF.                                         |
| `observador_MEF_estado_actual()` | Retorna el estado actual.                                                  |
| `observador_MEF_forzar_reset()`  | Fuerza reinicio del sistema y limpia buffers.                              |
| `observador_MEF_debug_estado()`  | Imprime por UART el estado actual en formato legible.                      |
| `observador_MEF_actualizar()`    | Ciclo principal de la MEF, gestiona transiciones y lógica de adquisición.  |

---

## 🧱 Variables internas

| Variable           | Tipo                  | Descripción                                               |
|--------------------|------------------------|-----------------------------------------------------------|
| `estado_actual`     | `Estado_Observador`    | Estado actual de la máquina.                              |
| `estado_anterior`   | `Estado_Observador`    | Último estado para comparación y trazabilidad.            |
| `buffer_temp`       | `TemporalBuffer`       | Buffer temporal con las últimas lecturas de sensores.     |
| `resultado`         | `EstadisticaPM25`      | Resultado de las estadísticas de PM2.5 promediadas.       |

---

## 🔄 Diagrama de estados (FSM)

```stateDiagram-v2
    [*] --> ESTADO_REPOSO

    ESTADO_REPOSO --> ESTADO_LECTURA : rtc_esta_activo() == true

    ESTADO_LECTURA --> ESTADO_ALMACENAMIENTO : sensor_leer_datos() == SENSOR_OK
    ESTADO_LECTURA --> ESTADO_ERROR : sensor_leer_datos() != SENSOR_OK

    ESTADO_ALMACENAMIENTO --> ESTADO_CALCULO : data_logger_store_sensor_data() == true && time_rtc_hay_cambio_bloque() == true
    ESTADO_ALMACENAMIENTO --> ESTADO_LECTURA : data_logger_store_sensor_data() == true && time_rtc_hay_cambio_bloque() == false
    ESTADO_ALMACENAMIENTO --> ESTADO_ERROR : data_logger_store_sensor_data() == false

    ESTADO_CALCULO --> ESTADO_GUARDADO : data_logger_estadistica_10min_pm25() == true
    ESTADO_CALCULO --> ESTADO_ERROR : data_logger_estadistica_10min_pm25() == false

    ESTADO_GUARDADO --> ESTADO_LIMPIESA : data_logger_store_avg10_csv() ejecutado

    ESTADO_LIMPIESA --> ESTADO_REPOSO : data_logger_buffer_limpiar_todos() completado

    ESTADO_ERROR --> ESTADO_REPOSO : siempre

    note right of ESTADO_REPOSO
        - Espera a que RTC esté activo
        - Estado de bajo consumo
        - Punto de reinicio después de errores
    end note

    note right of ESTADO_LECTURA
        - Lee datos de sensores PM2.5
        - Almacena en buffer_temp.muestras
        - Actualiza buffer_temp.cantidad
    end note

    note right of ESTADO_ALMACENAMIENTO
        - Guarda datos en buffers_10min
        - Verifica cambio de bloque temporal
        - Bucle de lectura si no hay cambio
    end note

    note right of ESTADO_CALCULO
        - Calcula estadísticas de PM2.5
        - Procesa datos de 10 minutos
        - Genera resultado estadístico
    end note

    note right of ESTADO_GUARDADO
        - Escribe archivo CSV con promedios
        - Almacenamiento permanente
        - Datos para análisis posterior
    end note

    note right of ESTADO_LIMPIESA
        - Limpia buffers_10min
        - Resetea buffer_temp.cantidad
        - Prepara para nuevo ciclo
    end note

    note right of ESTADO_ERROR
        - Imprime mensaje de error por UART
        - Manejo simple: retorno a REPOSO
        - No hay reintentos automáticos
    end note
```



## Posibles mejoras

    Implementar temporización con tick para que el sistema sea no bloqueante al 100%.

    Integrar control de errores más robusto con reintentos por estado.

    Generar logs persistentes de cada transición (útil para depuración post-mortem).

## Archivos relacionados

    sensor.h — Funciones para adquisición desde SPS30 y DHT22

    data_logger.h — Gestión de almacenamiento en buffers y microSD

    pm25_buffer.h — Buffers circulares y estadísticas

    uart.h — Comunicación serial

    time_rtc.h — Control y sincronización de RTC


Licencia

Este proyecto está licenciado bajo GNU GPL v3.0.
