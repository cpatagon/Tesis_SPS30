/*
 * shdlc.c
 *
 *  Created on: 16-03-2024
 *      Author: lgomez
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "shdlc.h"

// Shdlc_FrameMiso SHDLC_GetFrameMiso(const uint8_t* DataFrame, size_t DataLength) {
//
////	SHDLC_LoadMyVector(Shdlc_FrameMiso* frame, const uint8_t* DataFrame, size_t DataFrameSize);
//
//    Shdlc_FrameMiso Newframe = {
//        .adr = DataFrame[1],
//        .cmd = DataFrame[2],
//        .lon = DataFrame[3], // Supongamos que solo hay 3 bytes significativos de datos
//        .myVector = {0xAA, 0xBB, 0xCC}, // Datos de ejemplo
//        .chk = 0xFF // Checksum de ejemplo
//    };
//    return frame;
//}

/**
 * @brief Carga el vector de datos myVector de una estructura Shdlc_FrameMiso basado
 * 		  en los datos contenidos en un arreglo DataFrame.
 *
 * @param frame Un puntero a una estructura Shdlc_FrameMiso donde se cargará myVector.
 * @param DataFrame Un arreglo de bytes que contiene los datos a copiar en myVector.
 * @param DataFrameSize El tamaño total del arreglo DataFrame para evitar
 *        desbordamientos de búfer.
 * @return void
 */

void SHDLC_LoadMyVector(Shdlc_FrameMiso * frame, const uint8_t * DataFrame, size_t DataFrameSize) {
    if (frame == NULL || DataFrame == NULL || DataFrameSize < 5) {
        // Validación básica para asegurarnos de que los parámetros son válidos y DataFrame tiene el
        // tamaño mínimo esperado.
        printf("Datos no válidos para cargar en myVector.\n");
        return;
    }

    uint8_t dataLength = DataFrame[4]; // Longitud de los datos útiles.

    if (dataLength > FRAME_DATA_VECTOR_SIZE_MISO || (4 + dataLength) > DataFrameSize) {
        // Asegúrate de que la longitud de los datos no excede el tamaño de myVector ni los límites
        // de DataFrame.
        printf("Longitud de los datos excede el tamaño permitido de myVector o los límites de "
               "DataFrame.\n");
        return;
    }

    // Copiar los datos útiles desde DataFrame a myVector en la estructura Shdlc_FrameMiso.
    memcpy(frame->myVector, &DataFrame[5], dataLength + 1);

    // Actualizar el campo 'lon' con la longitud de los datos copiados.
    frame->lon = dataLength;

    // Asume que podrías querer también inicializar otros campos aquí.
}

/**
 * @brief Calcula el tamaño de los datos contenidos entre dos
 *        delimitadores en un arreglo de bytes.
 *
 * Esta función busca los delimitadores de inicio y fin (0x7E) en
 * el arreglo de entrada y calcula la longitud de los datos entre ellos.
 *
 * @param input Puntero al arreglo de bytes de entrada.
 * @param inputSize Tamaño del arreglo de entrada.
 * @return int La longitud de los datos contenidos entre los dos delimitadores,
 *         o -1 si los delimitadores no se encuentran en la secuencia correcta.
 */

int SHDLC_CalculateDataSize(const uint8_t * input, size_t inputSize) {
    int start = -1;
    int end = -1;

    // Buscar el primer delimitador 0xE7.
    for (size_t i = 0; i < inputSize; ++i) {
        if (input[i] == 0x7E) {
            start = i + 1; // Comenzar después del primer 0xE7
            break;
        }
    }

    // Buscar el segundo delimitador 0xE7.
    for (size_t i = start; i < inputSize; ++i) {
        if (input[i] == 0x7E) {
            end = i;
            break;
        }
    }

    // Verificar si se encontraron ambos delimitadores.
    if (start == -1 || end == -1 || start >= end) {
        return -1; // No se encontró la secuencia válida.
    }

    // Calcular la longitud de los datos a copiar y asegurarse de que no
    // exceda el buffer de salida.
    size_t dataLength = end - start;
    return dataLength;
}

/**
 * Revierte el proceso de byte-stuffing para un frame de datos recibido.
 *
 * @param stuffedData Puntero al arreglo de bytes que contiene los datos con byte-stuffing.
 * @param stuffedSize Tamaño del arreglo de bytes con byte-stuffing.
 * @param originalData Puntero al arreglo de bytes donde se almacenarán los datos revertidos.
 * @return El tamaño del arreglo de bytes revertido.
 */
size_t SHDLC_revertByteStuffing(const uint8_t * stuffedData, size_t stuffedSize,
                                uint8_t * originalData) {
    size_t originalIndex = 0;

    for (size_t i = 0; i < stuffedSize; ++i) {
        if (stuffedData[i] == 0x7D) { // Detecta el indicador de byte-stuffing
            i++;                      // Avanza al siguiente byte para determinar el valor real
            if (i < stuffedSize) {
                switch (stuffedData[i]) {
                case 0x5E: // Valor original era 0x7E
                    originalData[originalIndex++] = 0x7E;
                    break;
                case 0x5D: // Valor original era 0x7D
                    originalData[originalIndex++] = 0x7D;
                    break;
                case 0x31: // Valor original era 0x11
                    originalData[originalIndex++] = 0x11;
                    break;
                case 0x33: // Valor original era 0x13
                    originalData[originalIndex++] = 0x13;
                    break;
                default:
                    // Trata el caso en que el byte siguiente a 0x7D no es uno esperado
                    // Esto podría indicar un error o una situación inesperada.
                    // Se podría manejar de manera especial si es necesario.
                    break;
                }
            }
        } else {
            // Si no es un byte-stuffing, simplemente copia el byte
            originalData[originalIndex++] = stuffedData[i];
        }
    }

    return originalIndex; // Retorna el tamaño del arreglo revertido
}

// Función para convertir 4 bytes en big-endian a un valor float IEEE754
float SHDLC_bytesToFloat(uint8_t * bytes) {
    float value;
    uint8_t reversedBytes[sizeof(value)];

    // Revertir el orden de los bytes para big-endian
    for (int i = 0; i < sizeof(value); i++) {
        reversedBytes[i] = bytes[sizeof(value) - i - 1];
    }

    // Copiar los bytes revertidos en la variable float
    memcpy(&value, reversedBytes, sizeof(value));

    return value;
}

// Función para llenar la estructura con los datos de concentración
void SHDLC_llenarConcentraciones(ConcentracionesPM * concentraciones, uint8_t * data) {
    concentraciones->pm1_0 = SHDLC_bytesToFloat(&data[0]);
    concentraciones->pm2_5 = SHDLC_bytesToFloat(&data[4]);
    concentraciones->pm4_0 = SHDLC_bytesToFloat(&data[8]);
    concentraciones->pm10 = SHDLC_bytesToFloat(&data[12]);
}
