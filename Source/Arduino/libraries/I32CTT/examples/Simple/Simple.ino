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
#include "I32CTT.h"
#include "I32CTT_NullEndpoint.h"
#include "I32CTT_NullInterface.h"

I32CTT_Controller controller(8);

I32CTT_NullInterface myInterface;
I32CTT_NullEndpoint idleEndpoint(I32CTT_Endpoint::str2id("NUL"));

void setup() {
  
  controller.set_interface(myInterface);
  
  controller.add_mode_driver(idleEndpoint);
  
  controller.enable_scheduler();
  controller.init();

}

unsigned long t_elapsed = 0;
void loop() {
  // Llamar a Controller.run()
  controller.run();
}
