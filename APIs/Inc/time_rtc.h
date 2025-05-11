/*
 * Nombre del archivo: time_rtc.h
 * Descripción: Este archivo proporciona funciones para inicializar el RTC, configurar la hora inicial,
 * y obtener la fecha y hora formateada. Además, se incluye una función para imprimir
 * la fecha y hora en formato ISO 8601.
 *
 * Funciones incluidas:
 * - getFormattedDateTime: Obtiene la fecha y hora formateadas como cadena.
 * - printFormattedDateTime: Imprime la fecha y hora formateadas.
 * - DS1307_SetInitialTime: Configura la hora inicial del RTC.
 *
 * Futuras ampliaciones pueden incluir:
 * - Funciones para ajustar la alarma del RTC.
 * - Funciones para leer y escribir en la SRAM del RTC.
 * - Integración con otros módulos de tiempo o sincronización.
 * Autor: lgomez
 * Creado en: 08-06-2024 
 * Derechos de Autor: (C) 2023 [Tu nombre o el de tu organización]
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



#ifndef TIME_RTC_H
#define TIME_RTC_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    RTC_SOURCE_INTERNAL,
    RTC_SOURCE_EXTERNAL
} RTC_Source;

extern RTC_Source active_rtc;

void rtc_auto_init(void);
void rtc_get_time(char *buffer, size_t len);
void obtener_fecha_hora(char *fecha_hora_str);  // compatibilidad

void time_rtc_Init(void);
void time_rtc_GetFormattedDateTime(char *buffer, size_t len);

#endif /* TIME_RTC_H */
