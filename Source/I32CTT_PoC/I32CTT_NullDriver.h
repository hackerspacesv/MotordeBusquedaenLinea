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
#ifndef I32CTT_NullDriver_H
#define I32CTT_NullDriver_H

class I32CTT_NullDriver: public I32CTT_ModeDriver {
  public:
    I32CTT_NullDriver(uint32_t mode_id);
    void init();
    uint32_t read(uint16_t addr);
    uint16_t write(uint16_t addr, uint32_t data);
    void update();
    uint8_t enabled();
    void enable();
    void disable();
};

#endif
