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
#include "I32CTT_NullInterface.h"

void I32CTT_NullInterface::init() {
  // Do nothing
}

void I32CTT_NullInterface::update() {
  // Do nothing
}

uint8_t I32CTT_NullInterface::available() {
  // Do nothing
  return 0;
}

void I32CTT_NullInterface::send() {
  // Do nothing
}

