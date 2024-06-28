/*
 * shdlc.h
 *
 * Descripción: Define las estructuras y funciones para el manejo de comunicaciones SHDLC.
 * Autor: Luis Gómez P.
 * Derechos de Autor: (C) 2023 Luis Gómez P. EIRL
 * Licencia: GNU General Public License v3.0
 *
 * Este programa es software libre: puedes redistribuirlo y/o modificarlo
 * bajo los términos de la Licencia Pública General GNU publicada por
 * la Free Software Foundation, ya sea la versión 3 de la Licencia, o
 * (a tu elección) cualquier versión posterior.
 *
 * Este programa se distribuye con la esperanza de que sea útil,
 * pero SIN NINGUNA GARANTÍA; sin siquiera la garantía implícita
 * de COMERCIABILIDAD o APTITUD PARA UN PROPÓSITO PARTICULAR. Ver la
 * Licencia Pública General GNU para más detalles.
 *
 * Deberías haber recibido una copia de la Licencia Pública General GNU
 * junto con este programa. Si no es así, visita <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-only
 *
 */

#ifndef INC_SHDLC_H_
#define INC_SHDLC_H_

/** @file
 ** @brief Definiciones y declaraciones para la interacción con comunicaciones SHDLC.
 **/

/* === Inclusión de archivos de cabecera ======================================================== */
#include <stdint.h>
#include <stddef.h>

/* === Definiciones de macros públicas ========================================================== */
#define FRAME_DATA_VECTOR_SIZE_MOSI 60
#define FRAME_DATA_VECTOR_SIZE_MISO 60

/* === Declaraciones de tipos de datos públicos ================================================= */

// Definición de la estructura para las concentraciones
typedef struct {
    float pm1_0;
    float pm2_5;
    float pm4_0;
    float pm10;
} ConcentracionesPM;

/**
 * @struct Shdlc_FrameMosi
 * @brief Estructura para representar un frame MOSI (Master Out Slave In) en el protocolo SHDLC.
 *
 * Esta estructura almacena los componentes de un frame de salida desde el maestro hacia el esclavo,
 * incluyendo la dirección del dispositivo, el comando a ejecutar, los datos a transmitir, la suma
 * de verificación y la longitud del frame.
 *
 * @param adr Dirección del dispositivo destino. Determina a qué dispositivo esclavo está dirigido
 * el frame.
 * @param cmd Comando a ejecutar por el dispositivo esclavo. Define la operación o la solicitud que
 * el maestro está enviando.
 * @param myVector Arreglo que contiene los datos a transmitir junto con el comando. Su tamaño está
 * definido por FRAME_DATA_VECTOR_SIZE_MOSI.
 * @param chk Suma de verificación del frame para garantizar la integridad de los datos.
 * @param longframe Longitud total del frame, incluyendo todos sus componentes. Este campo puede ser
 * utilizado para verificar la integridad del frame y asegurar que se ha recibido por completo.
 */
typedef struct Shdlc_FrameMosi {
    uint8_t adr;
    uint8_t cmd;
    uint8_t myVector[FRAME_DATA_VECTOR_SIZE_MOSI];
    uint8_t chk;
    uint8_t longframe;
} Shdlc_FrameMosi;

/**
 * @struct Shdlc_FrameMiso
 * @brief Estructura para representar un frame MISO (Master In Slave Out) en el protocolo SHDLC.
 *
 * Esta estructura almacena los componentes de un frame de entrada al maestro procedente de un
 * esclavo, incluyendo la dirección del dispositivo origen, el comando o respuesta ejecutado, los
 * datos recibidos, la suma de verificación y la longitud del frame.
 *
 * @param adr Dirección del dispositivo origen. Indica de qué dispositivo esclavo proviene el frame.
 * @param cmd Comando ejecutado o respuesta proporcionada por el dispositivo esclavo. Puede ser una
 * confirmación de un comando previo del maestro o una solicitud de información.
 * @param lon Longitud de los datos útiles en el frame. Este campo especifica cuántos bytes del
 * arreglo myVector contienen datos significativos.
 * @param myVector Arreglo que contiene los datos recibidos en respuesta al comando del maestro. Su
 * tamaño está definido por FRAME_DATA_VECTOR_SIZE_MISO.
 * @param chk Suma de verificación del frame para verificar la integridad de los datos recibidos.
 * @param longframe Longitud total del frame, incluyendo todos sus componentes. Este campo puede ser
 * utilizado para la validación de la integridad del frame recibido.
 */

typedef struct Shdlc_FrameMiso {
    uint8_t adr;
    uint8_t cmd;
    uint8_t lon;
    uint8_t myVector[FRAME_DATA_VECTOR_SIZE_MISO];
    uint8_t chk;
    uint8_t longframe;
} Shdlc_FrameMiso;

/* === Declaraciones de funciones públicas ====================================================== */

// Función para convertir 4 bytes en un valor float IEEE754
float SHDLC_bytesToFloat(uint8_t * bytes);

// Función para llenar la estructura con los datos de concentración
void SHDLC_llenarConcentraciones(ConcentracionesPM * concentraciones, uint8_t * data);

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

void SHDLC_LoadMyVector(Shdlc_FrameMiso * frame, const uint8_t * DataFrame, size_t DataFrameSize);

/**
 * @brief Obtiene un frame MISO (Master In Slave Out) de ejemplo o de respuesta de una operación
 * SHDLC.
 *
 * Esta función simula la recepción de un frame MISO desde un dispositivo esclavo en respuesta a un
 * comando SHDLC previamente enviado por el maestro. Se utiliza principalmente para fines de prueba
 * y desarrollo, proporcionando un frame de respuesta estático o generado según necesidades
 * específicas.
 *
 * @return Una estructura `Shdlc_FrameMiso` que contiene los datos del frame MISO de ejemplo o de
 * respuesta.
 */

Shdlc_FrameMiso SHDLC_GetFrameMiso(void);

/**
 * @brief Calcula el tamaño de los datos útiles en un frame SHDLC, excluyendo los delimitadores y el
 * checksum.
 *
 * Analiza un arreglo de bytes de entrada correspondiente a un frame SHDLC y calcula la longitud de
 * los datos útiles contenidos en el mismo. Esta función es específica para frames SHDLC, teniendo
 * en cuenta la estructura y delimitadores propios del protocolo.
 *
 * @param input Puntero al arreglo de bytes de entrada que representa el frame SHDLC.
 * @param inputSize Tamaño del arreglo de bytes de entrada.
 * @return El tamaño de los datos útiles contenidos en el frame SHDLC; devuelve -1 si el frame no
 * cumple con las expectativas del protocolo SHDLC.
 */

int SHDLC_CalculateDataSize(const uint8_t * input, size_t inputSize);

/**
 * Revierte el proceso de byte-stuffing para un frame de datos recibido.
 *
 * @param stuffedData Puntero al arreglo de bytes que contiene los datos con byte-stuffing.
 * @param stuffedSize Tamaño del arreglo de bytes con byte-stuffing.
 * @param originalData Puntero al arreglo de bytes donde se almacenarán los datos revertidos.
 * @return El tamaño del arreglo de bytes revertido.
 */
size_t SHDLC_revertByteStuffing(const uint8_t * stuffedData, size_t stuffedSize,
                                uint8_t * originalData);

#endif /* INC_SHDLC_H_ */
