# Documentación del Sistema de Medición de Material Particulado (MP2.5)

Este repositorio contiene la documentación técnica detallada de los módulos principales del sistema embebido para monitoreo de calidad del aire basado en el microcontrolador STM32F429ZI.

## 📂 Contenido de la Carpeta `docs/`

| Archivo                                            | Descripción                                                                   |
| -------------------------------------------------- | ----------------------------------------------------------------------------- |
| [`data_logger_API.md`](./data_logger_API.md)       | Documentación del módulo de almacenamiento en microSD y análisis estadístico. |
| [`proceso_observador.md`](./proceso_observador.md) | Detalles del ciclo de adquisición y validación de datos desde los sensores.   |

---

## 📓 Resumen de Archivos

### [`data_logger_API.md`](./data_logger_API.md)

Documenta el funcionamiento interno del sistema de registro en microSD:

* Escritura de datos crudos y promedios en CSV.
* Buffers circulares por resolución temporal (10min, 1h, 24h).
* Estructuras y funciones de cálculo estadístico.
* Diagramas `mermaid` de flujo modular.

**Estructura de Archivos:**

```
/AAAA/MM/DD/
├── RAW.csv
├── avg10.csv
├── avg60.csv
└── avg24.csv
```

### [`proceso_observador.md`](./proceso_observador.md)

Documenta el módulo de adquisición, validación y coordinación del ciclo de lectura:

* Lógica de inicio/parada de medición SPS30.
* Integración con RTC (DS3231) y sensores DHT22.
* Enlace con el sistema de promedios.
* Tabla con funciones, entradas y salidas.

---

## ✍️ Autoría

* **Autor principal:** Luis Gómez
* **Institución:** CESE - FIUBA
* **Licencia:** GNU General Public License v3.0
* **Fecha:** Junio 2025

---
