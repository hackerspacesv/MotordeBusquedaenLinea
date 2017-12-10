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
#include "I32CTT_NullDriver.h"
#include "I32CTT_ArduinoStreamInterface.h"
#include "MBL_ManualDriver.h"

I32CTT_Controller controller(8);

//I32CTT_NullInterface myInterface;
I32CTT_ArduinoStreamInterface serialInterface(Serial);
I32CTT_NullDriver idleDriver(I32CTT_ModeDriver::str2name("NUL"));
MBL_ManualDriver manualDriver(I32CTT_ModeDriver::str2name("MAN"));

void setup() {
  while(!Serial);
  Serial.begin(9600);
  controller.set_interface(serialInterface);
  controller.add_mode_driver(idleDriver);
  controller.add_mode_driver(manualDriver);
  manualDriver.enable();
  controller.enable_scheduler();
  controller.init();
  Serial.println("Running...");
}

unsigned long t_elapsed = 0;
void loop() {
  // put your main code here, to run repeatedly:
  controller.run();

  if((millis()-t_elapsed)>1000) {
    
    controller.master.set_mode(1);
    controller.master.read_record({1});
    controller.master.read_record({2});
    controller.master.read_record({3});
    controller.master.try_send();

    controller.master.set_mode(0);
    controller.master.write_record({1,0x1000000});
    controller.master.write_record({2,0x1000000});
    controller.master.try_send();
    
    t_elapsed = millis();
  }
}
