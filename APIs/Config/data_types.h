#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include "config_global.h"
/*
#include "rtc_config.h"  // ds3231_time_t

#include "sps30_config.h"
#include "dht22_config.h"
#include "config_mensaje.h"
#include "config_sistema.h"
*/
#include "rtc_ds3231_for_stm32_hal.h"
#include "buffers_config.h"
#include "shdlc.h" // ConcentracionesPM

#ifdef __cplusplus
extern "C" {
#endif

/** Estructura principal de datos adquiridos por sensor */
typedef struct {
    ds3231_time_t timestamp; /**< Timestamp completo con hora y fecha */
    uint8_t bloque_10min;    /**< Índice del bloque de 10 minutos del día */
    uint8_t sensor_id;       /**< ID lógico del sensor */
    float pm1_0;
    float pm2_5;
    float pm4_0;
    float pm10;
    float temp_amb;
    float hum_amb;
    float temp_cam;
    float hum_cam;
} MedicionMP;

/** Estructura para estadísticas de PM2.5 en ventanas sincronizadas */
typedef struct {
    ds3231_time_t timestamp;
    float pm2_5_avg;
    uint16_t sample_count;
    float pm2_5_min;
    float pm2_5_max;
    float pm2_5_std;
} TimeSyncedAverage;

/** Estructura para estadísticas completas del bloque de 10 minutos */
typedef struct {
    uint8_t sensor_id;
    uint16_t year;
    uint8_t month, day, hour, min, sec;
    uint8_t bloque_10min;
    float pm2_5_promedio;
    float pm2_5_min;
    float pm2_5_max;
    float pm2_5_std;
    uint8_t num_validos;
} EstadisticaPM25;

/** Estructura para buffer circular de datos */
typedef struct {
    MedicionMP * datos;
    uint16_t capacidad;
    uint16_t inicio;
    uint16_t cantidad;
} BufferCircular;

typedef struct {
    MedicionMP buffer[BUFFER_10MIN_SIZE];
    uint8_t head;
    uint8_t count;
} BufferCircularSensor;

/** Alias para facilitar compatibilidad */
typedef EstadisticaPM25 PMDataAveraged;

#ifndef MAX_SAMPLES_PER_10MIN
#define MAX_SAMPLES_PER_10MIN 60 // Ajusta si es necesario
#endif

typedef struct {
    ds3231_time_t start_time;
    float samples[MAX_SAMPLES_PER_10MIN];
    uint16_t count;
} TimeWindow;

#ifdef __cplusplus
}
#endif

#endif /* DATA_TYPES_H */
