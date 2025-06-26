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

```mermaid
stateDiagram-v2
    [*] --> REPOSO

    REPOSO --> LECTURA     : rtc_esta_activo()
    LECTURA --> ALMACENAMIENTO : sensor_leer_datos() == OK
    LECTURA --> ERROR      : sensor_leer_datos() == ERROR

    ALMACENAMIENTO --> CALCULO     : time_rtc_hay_cambio_bloque()
    ALMACENAMIENTO --> LECTURA     : si no hay cambio de bloque
    ALMACENAMIENTO --> ERROR       : fallo en data_logger_store_sensor_data()

    CALCULO --> GUARDADO   : estadísticas exitosas
    CALCULO --> ERROR      : fallo en estadística

    GUARDADO --> LIMPIESA
    LIMPIESA --> REPOSO

    ERROR --> REPOSO
\🧪 Posibles mejoras

    Implementar temporización con tick para que el sistema sea no bloqueante al 100%.

    Integrar control de errores más robusto con reintentos por estado.

    Generar logs persistentes de cada transición (útil para depuración post-mortem).

\📦 Archivos relacionados

    sensor.h — Funciones para adquisición desde SPS30 y DHT22

    data_logger.h — Gestión de almacenamiento en buffers y microSD

    pm25_buffer.h — Buffers circulares y estadísticas

    uart.h — Comunicación serial

    time_rtc.h — Control y sincronización de RTC

🪪 Licencia

Este proyecto está licenciado bajo GNU GPL v3.0.
