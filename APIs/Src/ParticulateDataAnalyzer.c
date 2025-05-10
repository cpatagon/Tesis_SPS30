/*
 * Nombre del archivo: ParticulateDataAnalyzer.c
 * Versión: 0.1
 * Descripción:
 *  API para el análisis de datos de material particulado (MP) recogidos por sensores de calidad del
 * aire. Este módulo calcula estadísticas clave como promedio, valor máximo, valor mínimo y
 * desviación estándar, proporcionando herramientas esenciales para la evaluación de la calidad del
 * aire en entornos internos y externos.
 *
 * Autor: Luis Gómez P.
 * Derechos de Autor: (C) 2023 CESE
 * Licencia: GNU General Public License v3.0
 *
 * Este programa es software libre: puedes redistribuirlo y/o modificarlo bajo los términos de la
 * Licencia Pública General GNU publicada por la Free Software Foundation, ya sea la versión 3 de la
 * Licencia, o (a tu elección) cualquier versión posterior.
 *
 * Este programa se distribuye con la esperanza de que sea útil, pero SIN NINGUNA GARANTÍA; sin
 * siquiera la garantía implícita de COMERCIABILIDAD o APTITUD PARA UN PROPÓSITO PARTICULAR. Ver la
 * Licencia Pública General GNU para más detalles.
 *
 * Deberías haber recibido una copia de la Licencia Pública General GNU junto con este programa.
 * Si no es así, visita <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

/**
 * @file ParticulateDataAnalyzer.c
 * @brief API para análisis estadístico de datos de material particulado (MP).
 *
 * Este archivo implementa una API que calcula parámetros estadísticos a
 * partir de datos recogidos por sensores de calidad del aire.
 * Los parámetros incluyen promedio, máximo, mínimo y desviación estándar,
 * necesarios para analizar la concentración de partículas en el aire.
 *
 * Funciones principales:
 * - calculateAverage: Calcula el promedio de valores de MP validados.
 * - findMaxValue: Encuentra el valor máximo de los datos validados de MP.
 * - findMinValue: Encuentra el valor mínimo de los datos validados de MP.
 * - calculateStandardDeviation: Calcula la desviación estándar de los valores de MP.
 *
 * La API es aplicable en sistemas de monitoreo de calidad de aire para análisis
 * en entornos interiores y exteriores.
 */

/* === Headers files inclusions =============================================================== */

#include "ParticulateDataAnalyzer.h"
#include <stddef.h> // Para NULL
#include <stdbool.h>

/* === Macros definitions ====================================================================== */

/**
 * @brief Tolerancia usada en el cálculo de la raíz cuadrada mediante el método de búsqueda binaria.
 */
#define TOLERANCE_SQRT_MET 1e-7

/**
 * @brief Indicador de que no hay datos en el conjunto, utilizado en comprobaciones de tamaño de
 * array.
 */
#define CERODATA 0

/**
 * @brief Índice de inicio estándar para iterar a través de arrays.
 */
#define START_LOCATION 0

/**
 * @brief valor mínimo de datos para considerar valido el divisor
 */
#define MIN_VALID_COUNT 1

/**
 * @brief valor inicial de contador de datos
 */
#define INI_VALID_COUNT 0

/**
 * @brief valor inicial para la suma de cuadrados
 */
#define INI_SUM_OF_SQUARE 0.0

/**
 * @brief valor divisor por 2
 */
#define DIV2 2

/**
 * @brief valor minimo de tolerancia para el cálculo de raiz
 */
#define MIN_VALUE_SQRT_TOLERANCE 0

/**
 * @brief valor inicial suma
 */
#define INI_SUM 0.0

/**
 * @brief valor inicial de resto
 */
#define INI_REST 0

/**
 * @brief La desviación estándar no está definida para n <= 1
 */
#define DS_NOTDEFINI 1

/**
 * @brief La división no esta definida para 0
 */
#define NOT_DIV_NUM 0

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/**
 * @brief Verifica si un dato está dentro del rango válido de concentración de MP.
 *
 * Determina si un valor individual de material particulado (MP) cae dentro de los límites
 * predefinidos establecidos por MP_MIN_VALUE y MP_MAX_VALUE.
 *
 * @param data Valor de MP a verificar.
 * @return Verdadero si el valor está dentro del rango; falso en caso contrario.
 */
bool maskIsDataTrue(float data) {
	 return (data > MP_MIN_VALUE) && (data <= MP_MAX_VALUE);
}

/**
 * @brief Comprueba si un array de datos está vacío o no inicializado.
 *
 * Evalúa si un array dado está vacío basándose en su tamaño o si el puntero es NULL,
 * indicando un array no inicializado o sin datos.
 *
 * @param data Array de flotantes a verificar.
 * @param n_data Número de elementos en el array.
 * @return Verdadero si el array está vacío o no inicializado; falso si contiene elementos.
 */
bool isArrayEmpty(float data[], int n_data) {
    return (n_data == CERODATA || data == NULL);
}

/**
 * @brief Calcula la raíz cuadrada de un número usando el método de búsqueda binaria.
 *
 * Implementa la búsqueda binaria para encontrar la raíz cuadrada de un número, adecuado para
 * sistemas donde las operaciones de punto flotante no son eficientes. Este método es efectivo
 * para números positivos y retorna cero para números no positivos.
 *
 * @param x Número del cual calcular la raíz cuadrada.
 * @return La raíz cuadrada de x, calculada dentro de una tolerancia definida por
 * TOLERANCE_SQRT_MET.
 */
double sqrt_binary_search(double x) {
    if (x <= 0)
        return 0;

    double low = MIN_VALUE_SQRT_TOLERANCE, high = x, mid, guess;
    double tolerance = TOLERANCE_SQRT_MET;

    while (high - low > tolerance) {
        mid = (low + high) / DIV2;
        guess = mid * mid;

        if (guess > x)
            high = mid;
        else
            low = mid;
    }

    return (low + high) / DIV2;
}

/* === Public function implementation ========================================================== */

/**
 * @brief Calcula el promedio de un conjunto de datos, excluyendo valores fuera de rango.
 *
 * Esta función evalúa cada elemento del array para determinar si cae dentro de los límites
 * aceptables definidos por MP_MIN_VALUE y MP_MAX_VALUE. Solo se consideran los valores válidos
 * para el cálculo del promedio.
 *
 * @param data Array de valores flotantes.
 * @param n_data Número de elementos en el array.
 * @return El promedio de los valores válidos o MSN_VOID_ARRAY_VALUE si el array está vacío
 *         o todos los elementos son inválidos.
 */

float calculateAverage(float data[], int n_data) {
    if (isArrayEmpty(data, n_data))
        return MSN_VOID_ARRAY_VALUE; // Manejo de array vacío
    float sum = INI_SUM;
    int rest = INI_REST; // numero de valor descontado del n total cuando
    for (int i = 0; i < n_data; i++) {
        if (maskIsDataTrue(data[i])) { // salta valores negativos o iguales a 0
            sum += data[i];
        } else {
            rest++; // valores no validados
        }
    }
    n_data = n_data - rest; // recalcula el valor de n en función de valores no validados

    if (n_data > NOT_DIV_NUM) { // verifica no estar dividiendo por un número negativo o 0
        return sum / (n_data);  // retorna promedio
    } else {
        return MSN_VOID_ARRAY_VALUE; // retorna un mensaje de error por array nulo
    }
}

/**
 * @brief Encuentra el valor máximo en un conjunto de datos validados.
 *
 * Itera a través del array para encontrar el máximo de los valores que cumplen con la validación
 * establecida por maskIsDataTrue. Ignora valores inválidos.
 *
 * @param data Array de valores flotantes.
 * @param n_data Número de elementos en el array.
 * @return El valor máximo encontrado o MSN_VOID_ARRAY_VALUE si el array está vacío o todos
 *         los elementos son inválidos.
 */

float findMaxValue(float data[], int n_data) {
    if (isArrayEmpty(data, n_data))
        return MSN_VOID_ARRAY_VALUE; // Manejo de array vacío
    float value = data[START_LOCATION];
    //int location = START_LOCATION;
    for (int i = 1; i < n_data; i++) {
        if (maskIsDataTrue(data[i])) { // salta valores negativos o iguales a 0
            if (value < data[i]) {
                value = data[i];
      //          location = i;
            }
        }
    }
    return value;
};

/**
 * @brief Encuentra el valor mínimo en un conjunto de datos validados.
 *
 * Similar a findMaxValue, esta función recorre el array para identificar el mínimo de los valores
 * que pasan la validación de maskIsDataTrue.
 *
 * @param data Array de valores flotantes.
 * @param n_data Número de elementos en el array.
 * @return El valor mínimo encontrado o MSN_VOID_ARRAY_VALUE si el array está vacío o todos
 *         los elementos son inválidos.
 */

float findMinValue(float data[], int n_data) {
    if (isArrayEmpty(data, n_data))
        return MSN_VOID_ARRAY_VALUE; // Manejo de array vacío
    float value = data[START_LOCATION];
   // int location = START_LOCATION;
    for (int i = 1; i < n_data; i++) {
        if (maskIsDataTrue(data[i])) { // salta valores negativos o iguales a 0
            if (value > data[i]) {
                value = data[i];
             //   location = i;
            }
        }
    }
    return value;
};

/**
 * @brief Calcula la desviación estándar de un conjunto de datos validados.
 *
 * Determina la desviación estándar de los valores en el array que son validados por maskIsDataTrue.
 * Utiliza la media calculada de los datos y la suma de los cuadrados de las diferencias para
 * obtener la desviación estándar.
 *
 * @param data Array de valores flotantes.
 * @param n Número de elementos en el array.
 * @return La desviación estándar de los valores válidos o 0 si hay menos de dos elementos válidos
 *         o el array está vacío.
 */

float calculateStandardDeviation(float data[], int n) {
    if (isArrayEmpty(data, n))
        return MSN_VOID_ARRAY_VALUE; // Manejo de array vacío

    if (n <= DS_NOTDEFINI)
        return MSN_DS_NOTDEFINI; // La desviación estándar no está definida para n <= 1

    int validCount = INI_VALID_COUNT; // Contador para el número de valores validados
    float mean = calculateAverage(data, n);
    float sumOfSquares = INI_SUM_OF_SQUARE;

    for (int i = 0; i < n; i++) {
        if (maskIsDataTrue(data[i])) {
            sumOfSquares += (data[i] - mean) * (data[i] - mean);
            validCount++; // Incrementa solo para datos validados
        }
    }

    // Calcula la desviación estándar solo si hay suficientes datos validados
    if (validCount > MIN_VALID_COUNT) {
        return sqrt_binary_search(sumOfSquares / (validCount - 1));
    } else {
        return MSN_NOT_DATA; // Retorna -777 si no hay suficientes datos para calcular la desviación
                             // estándar
    }
}

/* === End of documentation ==================================================================== */
