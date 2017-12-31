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
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "Arduino.h"
#include "I32CTT.h"
#include "I32CTT_ArduinoStreamInterface.h"

I32CTT_ArduinoStreamInterface::I32CTT_ArduinoStreamInterface(Stream &port) {
  this->rx_buffer = (uint8_t*)malloc(sizeof(uint8_t)*SER_MTU_SIZE);
  memset(this->rx_buffer, 0, sizeof(uint8_t)*SER_MTU_SIZE);
  this->rx_size = 0;
  this->tx_buffer = (uint8_t*)malloc(sizeof(uint8_t)*SER_MTU_SIZE);
  memset(this->tx_buffer, 0, sizeof(uint8_t)*SER_MTU_SIZE);
  this->tx_size = 0;
  this->serial_buffer = (uint8_t*)malloc(sizeof(uint8_t)*SER_BUFF_SIZE);
  memset(this->serial_buffer, 0, sizeof(uint8_t)*SER_BUFF_SIZE);

  this->port = &port;

  this->d_available = 0;
  this->serial_size = 0;
}

I32CTT_ArduinoStreamInterface::~I32CTT_ArduinoStreamInterface() {
  delete this->rx_buffer;
  delete this->rx_buffer;
  delete this->serial_buffer;
}

void I32CTT_ArduinoStreamInterface::init() {
  this->port->println("Arduino Stream Interface initialized...");
}

void I32CTT_ArduinoStreamInterface::update() {

  uint8_t c;
  uint8_t c_available = this->port->available();

  if(c_available) {
    c = this->port->read();
    if(c == '\r' || c == '\n') {

      this->serial_buffer[this->serial_size] = '\0';

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
  return 1; // Siempre retorna 1 para esta interfaz ya que el envio es bloqueante
}

uint8_t I32CTT_ArduinoStreamInterface::data_available() {
  uint8_t result = this->d_available;
  this->d_available = 0;
  return result;
}

void I32CTT_ArduinoStreamInterface::send() {
  
  uint8_t cmd = 0;
  cmd = this->tx_buffer[0];
  uint8_t reg_count = I32CTT_Controller::reg_count(cmd, this->tx_size);
  uint8_t mode = this->tx_buffer[1];
  
  switch(cmd) {
    case CMD_R:
      this->port->print("r");
      this->tx_size = 0;
      break;
    case CMD_W:
      this->port->print("w"); 
      this->tx_size = 0;
      break;
    case CMD_AR:
      this->port->print("ar,");
      this->port->print(mode, DEC);
      for(int i=0;i<reg_count;i++) {
        this->port->print(",");
        this->port->print(I32CTT_Controller::get_reg(this->tx_buffer, cmd, i), HEX);
        this->port->print(",");
        this->port->print(I32CTT_Controller::get_data(this->tx_buffer, cmd, i), HEX);
      }
      this->tx_size = 0;
      break;
    case CMD_AW:
      this->port->print("aw,");
      this->port->print(mode, DEC);
      for(int i=0;i<reg_count;i++) {
        this->port->print(",");
        this->port->print(I32CTT_Controller::get_reg(this->tx_buffer, cmd, i), HEX);
      }
      this->tx_size = 0;
      break;
  }
  
  this->port->print("\r\n");
}

void I32CTT_ArduinoStreamInterface::process_buffer() {
  
  char *string_buffer = (char*)this->serial_buffer;
  char *pch = strtok(string_buffer, ",");
  uint8_t pos = 0;
  uint8_t reg_count = 0;
  uint32_t data = 0;
  uint16_t data16 = 0;
  uint8_t data8 = 0;
  
  //for(int i=0;i<SER_MTU_SIZE;i++) {
  //  this->rx_buffer[i] = 0;
  //}
  this->rx_size = 0;

  while(pch != NULL) {
    if(pos==0) {
      if(strstr(pch,"r")!=NULL) {
        this->rx_buffer[0] = CMD_R;
      }else if(strstr(pch,"w")!=NULL) {
        this->rx_buffer[0] = CMD_W;
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
      if(this->rx_buffer[0] == CMD_R ) {
        data16 = (uint16_t)strtol(pch,NULL, 10);
        I32CTT_Controller::put_reg(this->rx_buffer, data16, CMD_R, pos-2);
        this->rx_size += sizeof(I32CTT_Reg);
      }
      if(this->rx_buffer[0] == CMD_W ) {
        if((pos%2)==0) {
          data16 = (uint16_t)strtol(pch,NULL, 10);
          this->port->print("REG:");
          this->port->println(data16, HEX);
          I32CTT_Controller::put_reg(this->rx_buffer, data16, CMD_W, reg_count);
        } else {
          data = (uint32_t)strtol(pch,NULL, 10);
          this->port->print("DATA:");
          this->port->println(data, HEX);
          I32CTT_Controller::put_data(this->rx_buffer, data, CMD_W, reg_count++);
          this->rx_size += sizeof(I32CTT_RegData);
        }
      }
    }
    pch = strtok(NULL, ","); 
    pos++;
  }
  this->port->println("Packet: ");
  for(int i=0;i<this->rx_size;i++) {
    Serial.print(this->rx_buffer[i], HEX);
    Serial.print(" ");
  }
  this->port->print("\r\n");

  for(int i=0;i<SER_BUFF_SIZE;i++) {
    this->serial_buffer[i] = 0;
  }
  this->serial_size = 0;
  if(pos>1) {
    this->d_available = 1;
  }
  
}

uint16_t I32CTT_ArduinoStreamInterface::get_MTU() {
  return SER_MTU_SIZE;
}