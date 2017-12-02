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
#include <string.h>
#include <stdlib.h>
#include "Arduino.h"
#include "I32CTT.h"
#include "I32CTT_ArduinoStreamInterface.h"

I32CTT_ArduinoStreamInterface::I32CTT_ArduinoStreamInterface(Stream &port) {
  this->port = &port;
}

void I32CTT_ArduinoStreamInterface::init() {
  this->port->println("Arduino Stream Interface initialized...");
}

void I32CTT_ArduinoStreamInterface::update() {
  uint8_t c;
  if(this->port->available()) {
    c = this->port->read();
    if(c == '\r' || c == '\n') {
      this->port->println((char*)serial_buffer);
      
      this->process_buffer();
      
      this->serial_buffer[0] = '\0';
      this->serial_size = 0;
    } else {
      if(this->serial_size<(SER_BUFF_SIZE-1)) {
        this->serial_buffer[this->serial_size++] = c;
      } else {
        this->serial_buffer[0] = '\0';
        this->serial_size = 0;
      }
    }
  }
}

uint8_t I32CTT_ArduinoStreamInterface::available() {
  uint8_t result = this->data_available;
  this->data_available = 0;
  return result;
}

void I32CTT_ArduinoStreamInterface::send() {
  
  uint8_t cmd = 0;
  cmd = this->tx_buffer[0]>>1;
  uint8_t reg_count = I32CTT_Controller::reg_count(cmd, this->tx_size);
  
  switch(cmd) {
    case CMD_R:
      this->port->print("r");
      break;
    case CMD_W:
      this->port->print("w"); 
      break;
    case CMD_AR:
      this->port->print("ar");
      for(int i=0;i<reg_count;i++) {
        this->port->print(",");
        this->port->print(I32CTT_Controller::get_reg(this->tx_buffer, cmd, i), HEX);
        this->port->print(",");
        this->port->print(I32CTT_Controller::get_data(this->tx_buffer, cmd, i), HEX);
      }
      break;
    case CMD_AW:
      this->port->print("aw");
      for(int i=0;i<reg_count;i++) {
        this->port->print(",");
        this->port->print(I32CTT_Controller::get_reg(this->tx_buffer, cmd, i), HEX);
      }
      break;
  }
  
  this->port->print("\r\n");
}

void I32CTT_ArduinoStreamInterface::process_buffer() {
  char *string_buffer = (char*)this->serial_buffer;
  char *pch = strtok(string_buffer, ",");
  uint8_t pos = 0;
  uint32_t data = 0;
  uint16_t data16 = 0;
  uint8_t data8 = 0;

  while(pch != NULL) {
    if(pos==0) {
      if(strstr(pch,"r")!=NULL) {
        this->rx_buffer[0] = CMD_R<<1;
      }else if(strstr(pch,"w")!=NULL) {
        this->rx_buffer[0] = CMD_W<<1;
      } else {
        this->rx_size = 0;
        break;
      }
      this->rx_size = sizeof(uint8_t);
    } else if(pos==1) {
      data8 = (uint8_t)strtol(pch,NULL, 10);
      memcpy(this->rx_buffer+this->rx_size, &data8, sizeof(uint8_t));
      this->rx_size += sizeof(uint8_t);
    } else {
      if(this->rx_buffer[0] == CMD_R<<1 ) {
        data16 = (uint16_t)strtol(pch,NULL, 10);
        memcpy(this->rx_buffer+this->rx_size, &data16, sizeof(uint16_t));
        this->rx_size += sizeof(uint16_t);
      }
      if(this->rx_buffer[0] == CMD_W<<1 ) {
        if((this->rx_size-sizeof(I32CTT_Header))%sizeof(I32CTT_RegData) == 0) {
          data16 = (uint16_t)strtol(pch,NULL, 10);
          memcpy(this->rx_buffer+this->rx_size, &data16, sizeof(uint16_t));
          this->rx_size += sizeof(uint16_t);
        } else {
          data = (uint32_t)strtol(pch,NULL, 10);
          memcpy(this->rx_buffer+this->rx_size, &data, sizeof(uint32_t));
          this->rx_size += sizeof(uint32_t);
        }
      }
    }
    pch = strtok(NULL, ","); 
    pos++;
  }
  if(pos>1) {
    this->data_available = 1;
  }
}
