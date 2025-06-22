Perfecto, a continuación te presento la **documentación en formato Markdown** para el módulo `proceso_observador`, siguiendo el mismo estilo que el ejemplo de `data_logger`:

---

# Módulo `proceso_observador` - Ciclo de Adquisición y Registro SPS30 (PM2.5)

## 📌 Descripción General

El módulo `proceso_observador` implementa la lógica de adquisición, validación y registro de mediciones de material particulado (PM1.0 a PM10) usando sensores ópticos **SPS30**, junto con sensores de **temperatura y humedad DHT22**. Este módulo se integra con el sistema de almacenamiento en microSD y los buffers de análisis estadístico.

Forma parte del sistema embebido para monitoreo ambiental urbano en el contexto del proyecto de tesis en FIUBA.

---

## 🧭 Funcionalidades Principales

* Inicia y detiene mediciones de los sensores SPS30.
* Valida los datos leídos (rango físico plausible).
* Obtiene `timestamp` desde RTC (DS3231).
* Registra datos crudos en microSD (formato CSV).
* Imprime mediciones por UART para depuración.
* Integra datos ambientales (T y HR) mediante DHT22.
* Alimenta el sistema de promedios temporales: 10min, 1h, 24h.

---

## 🧩 Macros relevantes

| Macro                        | Descripción                                                    |
| ---------------------------- | -------------------------------------------------------------- |
| `NUM_REINT`                  | Reintentos de lectura por sensor en caso de error              |
| `CONC_MIN_PM`, `CONC_MAX_PM` | Rango aceptable para las concentraciones de partículas (µg/m³) |
| `DELAY_MS_SPS30_LECTURA`     | Tiempo de espera tras iniciar la medición (en ms)              |
| `MSG_PM_FORMAT_WITH_TIME`    | Formato de impresión para datos UART                           |

---

## 🔁 Flujo General del Módulo

```mermaid
graph TD
    S1["SPS30 start_measurement()"]
    D1["Delay 5s"]
    S2["SPS30 get_concentrations()"]
    S3["Validar datos"]
    RTC["Leer timestamp DS3231"]
    SD1["Guardar datos crudos"]
    B1["registrar_lectura_pm25()"]
    A1["Acumular en buffers"]
    A2["Calcular promedio si hay 3 sensores"]
    SD2["Guardar promedio si corresponde"]
    U1["Imprimir por UART"]

    S1 --> D1 --> S2 --> S3
    S3 -->|válido| RTC --> SD1 --> B1 --> A1 --> A2 --> SD2
    S3 -->|válido| U1


---

## 📦 Estructura de Funciones

| Función                        | Descripción                                                       |
| ------------------------------ | ----------------------------------------------------------------- |
| `proceso_observador`           | Ejecuta un ciclo de medición con timestamp interno del RTC        |
| `proceso_observador_with_time` | Usa un timestamp externo y mide T/HR de la cámara                 |
| `proceso_observador_3PM_2TH`   | Versión explícita con T/HR ambiente y de cámara                   |
| `registrar_lectura_pm25`       | Almacena una medición individual de PM2.5 y asocia T/HR vía DHT22 |

---

## 🧪 Validación

* Una medición solo se acepta si al menos una concentración de PM (1.0, 2.5, 4.0, 10.0) está entre `CONC_MIN_PM` y `CONC_MAX_PM`.
* Si falla la lectura del RTC, se notifica vía UART y se aborta el registro.
* Si no hay respuesta del sensor tras `NUM_REINT` intentos, se descarta.

---

## 📝 Ejemplo de Salida UART

```
[2025-06-21T14:10:30] SPS30 ID:2 | PM1.0: 12.33 | PM2.5: 15.22 | PM4.0: 16.55 | PM10: 19.88 | ug/m3
```

---

## ✍️ Autoría

* **Autor:** Luis Gómez
* **Fecha de creación:** 04 de mayo de 2025
* **Licencia:** GNU GPLv3
* **Institución:** CESE - FIUBA

---

¿Te gustaría que lo guarde como `docs/proceso_observador.md` o lo integro directamente en tu repositorio? También puedo generar automáticamente un índice para todos los módulos.
