#include "sps30_comm.h"
#include "uart.h"
#include "shdlc.h"
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE                    7
#define BUFFER_SIZE_READ_DATA          60
#define BUFFER_SIZE_SERIAL_NUMBER      30
#define BUFFER_SIZE_NUMBER             30
#define BUFFER_SIZE_STOP_MEASUREMENT   8
#define BUFFER_SIZE_SLEEP              8
#define BUFFER_SIZE_WAKEUP             8
#define BUFFER_SIZE_RESPONSE           50
#define BUFFER_PRINT_CONCENTRATION     100

#define MENSAJE_SIZE_RESPUESTA         "\nLongitud de respuesta:\n %d <--"
#define MSN_SERIAL_NUMBER              "\nSerial Number: %s\n"

#define MSG_INICIO_MEDICION            "\n Inicio_medicion:\n"
#define MSG_RESPUESTA_INICIO_MEDICION  "\n Respuesta inicio:\n"
#define MSG_SOLICITAR                  "\n Solicitar:\n"
#define MSG_RESPUESTA                  "\n Respuesta:\n"
#define MSG_DATOS_RESPUESTA            "\n Respuesta:\n"
#define MSG_RESPUESTA_CON_BYTESTUFFING "\n Respuesta con ByteStuffing:\n"
#define MSG_LONGITUD_RESPUESTA         "\nLongitud de la respuesta:\n %d bytes\n"

#define FORMATO_CONCENTRACION_PM1_0    "\nConcentracion de PM1.0:\n %f ug/m3\n"
#define FORMATO_CONCENTRACION_PM2_5    "\nConcentracion de PM2.5:\n %f ug/m3\n"
#define FORMATO_CONCENTRACION_PM4_0    "\nConcentracion de PM4.0:\n %f ug/m3\n"
#define FORMATO_CONCENTRACION_PM10     "\nConcentracion de PM10:\n %f ug/m3\n"

#define DELAY_WAKEUP                   50
#define DELAY_START_MEASUREMENT        2
#define DELAY_STOP_MEASUREMENT         0

#define CLEAR_VAR                      0

// Implementación de las funciones del objeto SPS30
void sps30_send_command(SPS30 * self, const uint8_t * command, uint16_t commandSize) {
    HAL_UART_Transmit(self->huart, command, commandSize, 100);
}

void sps30_receive_async(SPS30 * self, uint8_t * dataBuffer, uint16_t bufferSize) {
    HAL_UART_Receive(self->huart, dataBuffer, bufferSize, 100);
}

void sps30_send_receive(SPS30 * self, const uint8_t * command, uint16_t commandSize,
                        uint8_t * dataBuffer, uint16_t bufferSize) {
    HAL_UART_Transmit(self->huart, command, commandSize, 100);
    HAL_UART_Receive(self->huart, dataBuffer, bufferSize, 100);
}

void sps30_start_measurement(SPS30 * self) {
    uint8_t startCmd[] = SPS30_FRAME_START_MEASUREMENT; // Comando para iniciar la medición
    uint8_t dataBuf[BUFFER_SIZE] = {0};      // Buffer para almacenar la respuesta del sensor
    char respuestaStr[BUFFER_SIZE_RESPONSE]; // Buffer para el mensaje de longitud de respuesta

    // Envío del comando de inicio de medición
    // uart_print(MSG_INICIO_MEDICION);               // Notifica por UART el inicio de la operación
    // uart_vector_print(sizeof(startCmd), startCmd); // Muestra el comando enviado
    self->send_command(self, startCmd, sizeof(startCmd)); // Envía el comando al sensor SPS30
    HAL_Delay(DELAY_START_MEASUREMENT); // Espera para el procesamiento del comando

    // Recepción y procesamiento de la respuesta
    self->receive_async(self, dataBuf, sizeof(dataBuf));
    // uart_print(MSG_RESPUESTA_INICIO_MEDICION); // Notifica la recepción de la respuesta
    // uart_vector_print(sizeof(dataBuf), dataBuf);   // Muestra la respuesta recibida

    // Cálculo y visualización de la longitud de la respuesta
    int longRespuesta = SHDLC_CalculateDataSize(
        dataBuf, sizeof(dataBuf)); // Calcula la longitud de los datos útiles
    // snprintf(respuestaStr, sizeof(respuestaStr), MENSAJE_SIZE_RESPUESTA, longRespuesta);  //
    // Formatea el mensaje de longitud uart_print(respuestaStr); // Imprime la longitud de la
    // respuesta
}

void sps30_stop_measurement(SPS30 * self) {
    uint8_t stopCmd[] = SPS30_FRAME_STOP_MEASUREMENT;
    uint8_t dataBuf[BUFFER_SIZE_STOP_MEASUREMENT] = {0};
    char respuestaStr[BUFFER_SIZE_RESPONSE];

    // uart_print(MSG_SOLICITAR);
    // uart_vector_print(sizeof(stopCmd), stopCmd);
    self->send_command(self, stopCmd, sizeof(stopCmd));
    HAL_Delay(DELAY_STOP_MEASUREMENT);

    self->receive_async(self, dataBuf, sizeof(dataBuf));
    // uart_print(MSG_RESPUESTA);
    // uart_vector_print(sizeof(dataBuf), dataBuf);

    int longRespuesta = SHDLC_CalculateDataSize(dataBuf, sizeof(dataBuf));
    // snprintf(respuestaStr, sizeof(respuestaStr), MENSAJE_SIZE_RESPUESTA, longRespuesta);
    // uart_print(respuestaStr);
}

void sps30_sleep(SPS30 * self) {
    uint8_t sleepCmd[] = SPS30_FRAME_SLEEP;
    uint8_t dataBuf[BUFFER_SIZE_SLEEP] = {0};
    char respuestaStr[BUFFER_SIZE_RESPONSE];

    // uart_print(MSG_SOLICITAR);
    // uart_vector_print(sizeof(sleepCmd), sleepCmd);
    self->send_receive(self, sleepCmd, sizeof(sleepCmd), dataBuf, sizeof(dataBuf));
    // uart_print(MSG_RESPUESTA);
    // uart_vector_print(sizeof(dataBuf), dataBuf);

    int longRespuesta = SHDLC_CalculateDataSize(dataBuf, sizeof(dataBuf));
    // snprintf(respuestaStr, sizeof(respuestaStr), MENSAJE_SIZE_RESPUESTA, longRespuesta);
    // uart_print(respuestaStr);
}

void sps30_read_data(SPS30 * self) {
    uint8_t readCmd[] = SPS30_FRAME_READ_MEASUREMENT;
    uint8_t dataBuf[BUFFER_SIZE_READ_DATA] = {0};
    uint8_t originalData[BUFFER_SIZE_READ_DATA] = {0};
    ConcentracionesPM concentraciones;
    char respuestaStr[BUFFER_SIZE_RESPONSE];

    // uart_print(MSG_SOLICITAR);
    // uart_vector_print(sizeof(readCmd), readCmd);
    self->send_receive(self, readCmd, sizeof(readCmd), dataBuf, sizeof(dataBuf));
    // uart_print(MSG_RESPUESTA);
    // uart_vector_print(sizeof(dataBuf), dataBuf);

    SHDLC_revertByteStuffing(dataBuf, sizeof(dataBuf), originalData);
    // uart_print(MSG_RESPUESTA_CON_BYTESTUFFING);
    // uart_vector_print(sizeof(originalData), originalData);

    int longRespuesta = SHDLC_CalculateDataSize(originalData, sizeof(originalData));
    // snprintf(respuestaStr, sizeof(respuestaStr), MSG_LONGITUD_RESPUESTA, longRespuesta);
    // uart_print(respuestaStr);

    Shdlc_FrameMiso Newframe = {};
    SHDLC_LoadMyVector(&Newframe, originalData, longRespuesta);
    // uart_print(MSG_DATOS_RESPUESTA);
    // uart_vector_print(Newframe.lon, Newframe.myVector);

    SHDLC_llenarConcentraciones(&concentraciones, Newframe.myVector);

    char buffer[BUFFER_PRINT_CONCENTRATION];
    // sprintf(buffer, FORMATO_CONCENTRACION_PM1_0, concentraciones.pm1_0);
    // uart_print(buffer);
    // sprintf(buffer, FORMATO_CONCENTRACION_PM2_5, concentraciones.pm2_5);
    // uart_print(buffer);
    // sprintf(buffer, FORMATO_CONCENTRACION_PM4_0, concentraciones.pm4_0);
    // uart_print(buffer);
    // sprintf(buffer, FORMATO_CONCENTRACION_PM10, concentraciones.pm10);
    // uart_print(buffer);
}

ConcentracionesPM sps30_get_concentrations(SPS30 * self) {
    uint8_t readCmd[] = SPS30_FRAME_READ_MEASUREMENT;
    uint8_t dataBuf[BUFFER_SIZE_READ_DATA] = {0};
    uint8_t originalData[BUFFER_SIZE_READ_DATA] = {0};
    ConcentracionesPM concentraciones;

    self->send_receive(self, readCmd, sizeof(readCmd), dataBuf, sizeof(dataBuf));
    SHDLC_revertByteStuffing(dataBuf, sizeof(dataBuf), originalData);

    Shdlc_FrameMiso Newframe = {};
    SHDLC_LoadMyVector(&Newframe, originalData,
                       SHDLC_CalculateDataSize(originalData, sizeof(originalData)));
    SHDLC_llenarConcentraciones(&concentraciones, Newframe.myVector);

    return concentraciones;
}

void sps30_serial_number(SPS30 * self) {
    static const uint8_t cmd_serial[] = SPS30_FRAME_SERIAL_NUMBER;
    uint8_t respuesta[48] = {0}; // El número de serie está en los primeros 32 bytes

    self->send_receive(self, cmd_serial, sizeof(cmd_serial), respuesta, sizeof(respuesta));

    // El número de serie comienza en el byte 4
    strncpy(self->serial_buf, (char *)&respuesta[4], SERIAL_BUFFER_LEN - 1);
    self->serial_buf[SERIAL_BUFFER_LEN - 1] = '\0'; // Seguridad por si no hay \0 en respuesta

    uart_print("Sensor UART: %p -> Serial: %s\n", self->huart, self->serial_buf);
}

void sps30_wake_up(SPS30 * self) {
    uint8_t Pulse = SPS30_PULSE_WAKE_UP;
    uint8_t readCmd[] = SPS30_FRAME_WAKE_UP;
    uint8_t dataBuf[BUFFER_SIZE_WAKEUP] = {0};
    char respuestaStr[BUFFER_SIZE_RESPONSE];

    // uart_print(MSG_SOLICITAR);
    // uart_vector_print(sizeof(readCmd), readCmd);
    self->send_command(self, &Pulse, sizeof(Pulse));
    HAL_Delay(DELAY_WAKEUP);
    self->send_receive(self, readCmd, sizeof(readCmd), dataBuf, sizeof(dataBuf));
    // uart_print(MSG_RESPUESTA);
    // uart_vector_print(sizeof(dataBuf), dataBuf);

    int longRespuesta = SHDLC_CalculateDataSize(dataBuf, sizeof(dataBuf));
    // snprintf(respuestaStr, sizeof(respuestaStr), MENSAJE_SIZE_RESPUESTA, longRespuesta);
    // uart_print(respuestaStr);
}

void SPS30_init(SPS30 * self, UART_HandleTypeDef * huart) {
    self->huart = huart;
    self->send_command = sps30_send_command;
    self->receive_async = sps30_receive_async;
    self->send_receive = sps30_send_receive;
    self->start_measurement = sps30_start_measurement;
    self->stop_measurement = sps30_stop_measurement;
    self->sleep = sps30_sleep;
    self->read_data = sps30_read_data;
    self->serial_number = sps30_serial_number;
    self->wake_up = sps30_wake_up;
    self->get_concentrations = sps30_get_concentrations;
}
