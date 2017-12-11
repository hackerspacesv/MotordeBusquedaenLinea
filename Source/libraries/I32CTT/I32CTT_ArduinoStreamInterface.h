/*
 * 
 * This file is part of I32CTT (Integer 32-bit Control & Telemetry Transport).
 * Copyright (C) 2017 Mario Gomez / Hackerspace San Salvador.
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
#ifndef I32CTT_ArduinoStreamInterface_H
#define I32CTT_ArduinoStreamInterface_H

#ifdef ARDUINO
#define SER_BUFF_SIZE 126
#define SER_MTU_SIZE 100

class I32CTT_ArduinoStreamInterface: public I32CTT_Interface {
  public:
    I32CTT_ArduinoStreamInterface(Stream &port);
    ~I32CTT_ArduinoStreamInterface();
    void init();
    void update();
    uint8_t available();
    uint8_t data_available();
    void send();
    uint16_t get_MTU();
  private:
    Stream *port;
    void process_buffer();
    uint8_t d_available = 0;
    uint8_t *serial_buffer;
    uint16_t serial_size = 0;
};

#endif

#endif
