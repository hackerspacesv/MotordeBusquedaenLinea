/*
 * 
 * This file is part of I32CTT (Integer 32-bit Control & Telemetry Transport).
 * Copyright (C) 2017 Mario Gomez.
 * 
 * I32CTT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * I32CTT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with I32CTT.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Acerca de este archivo: Este sketch es la implementación de referencia
 * para el protocolo de enteros de 32 bits para control y transporte de
 * telemetría (I32CTT).
 */
// Utilizamos string.h para facilitar el formato de salida de Serie.
#include "I32CTT.h"
#include "I32CTT_NullEndpoint.h"
#include "I32CTT_ArduinoStreamInterface.h"
#include "I32CTT_Arduino802154Interface.h"
#include "MBL_ManualEndpoint.h"
#include <string.h>

I32CTT_Controller controller(8);

//I32CTT_NullInterface myInterface;
I32CTT_ArduinoStreamInterface serialInterface(Serial);
I32CTT_Arduino802154Interface ieee802154;
I32CTT_NullEndpoint idleEndpoint(I32CTT_Endpoint::str2id("NUL"));
MBL_ManualEndpoint manualEndpoint(I32CTT_Endpoint::str2id("MAN"));

void setup() {
  //while(!Serial);
  Serial.begin(9600);
  
  ieee802154.set_pan_id(0xCAFE);
  ieee802154.set_short_addr(0x0100);
  ieee802154.set_channel(C2480);
  ieee802154.set_dst_addr(0x0201);
  controller.set_interface(ieee802154);
  
  controller.add_mode_driver(idleEndpoint);
  controller.add_mode_driver(manualEndpoint);
  manualEndpoint.write(MBL_Registers::IS_ENABLED, HIGH);
  controller.enable_scheduler();
  controller.init();
  Serial.println("Running...");
}

unsigned long t_elapsed = 0;

void loop() {
  // put your main code here, to ru n repeatedly:
  controller.run();

  if((millis()-t_elapsed)>1000) {
    
    Serial.println("I'm alive");
    
    t_elapsed = millis();
  }

}
