/*
 * 
 * This file is part of MBL (Motor de Buscador en Línea).
 * Copyright (C) 2017 Mario Gomez.
 * 
 * MBL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Acerca de este archivo: Utiliza este Sketch para probar el correcto
 * funcionamiento del arreglo de sensores del Seguidor de Línea.
 */
// Utilizamos string.h para facilitar el formato de salida de Serie.
#include <string.h>

// Pines del selector de canal de multiplexor análogo. (S0: LSB)
const int S0 = 2;
const int S1 = 3;
const int S2 = 4;
const int S3 = 5;

// Retardo en la lectura de los datos (ms)
const int READ_DELAY = 100;

// Pin utilizado para la lectura análoga
const int ANALOG_IN = A0;

// Buffer de texto para dar formato a los números de salida
char txt_buffer[6];

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  // Usamos 115200bps para obtener una mejor visualización en
  // la respuesta.
  Serial.begin(115200);
}

void loop() {
  int a_data = 0;

  // Según esquemático los sensores están en las posiciones 3-12
  for(int i=3; i<13;i++)  {
    digitalWrite(S0, i&0x01);
    digitalWrite(S1, i&0x02);
    digitalWrite(S2, i&0x04);
    digitalWrite(S3, i&0x08);

    // Adquisición y formato de la salida
    a_data = analogRead(ANALOG_IN);
    sprintf(txt_buffer, "%04d ", a_data);
    Serial.print(txt_buffer);
  }
  Serial.print("\r\n"); // Agregamos un fin de línea a cada iteración
  
  delay(READ_DELAY);
}
