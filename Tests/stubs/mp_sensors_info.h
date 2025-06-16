#ifndef MP_SENSORS_INFO_H
#define MP_SENSORS_INFO_H
#define LOCATION_COORDS "0,0"
typedef struct {char serial_number[1]; char location_name[1];} MP_SensorInfo;
static MP_SensorInfo sensor_metadata[1];
#endif
