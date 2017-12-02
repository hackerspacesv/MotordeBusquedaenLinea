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
#include <stdint.h>
#include "I32CTT.h"
#include "I32CTT_NullDriver.h"

I32CTT_NullDriver::I32CTT_NullDriver(uint32_t mode_id) : I32CTT_ModeDriver(mode_id) {
  // Do nothing. Just call superclass parent
}
 
uint32_t I32CTT_NullDriver::read(uint16_t addr) {
  uint32_t result;
  switch(addr) {
    case 0x0000:
      result = this->driver_name;
      break;
    default:
      result = 0xAAAAAAAA;
  }
  return result; // Test Pattern
}

uint16_t I32CTT_NullDriver::write(uint16_t addr, uint32_t data) {
  return addr; // Null driver always return a valid address
}

void I32CTT_NullDriver::init() {
  // Do nothing.  
}

void I32CTT_NullDriver::update() {
  // Do nothing.  
}

uint8_t I32CTT_NullDriver::enabled() {
  return 1; // Driver is always enabled
}

void I32CTT_NullDriver::enable() {
  // Do nothing.  
}

void I32CTT_NullDriver::disable() {
  // Do nothing.  
}

