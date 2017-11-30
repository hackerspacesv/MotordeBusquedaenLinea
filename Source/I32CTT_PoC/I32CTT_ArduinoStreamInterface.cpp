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
  for(int i=0;i<this->tx_size;i++) {
    this->port->print(this->tx_buffer[i], DEC);  
  }
  this->port->print("\r\n");
}

void I32CTT_ArduinoStreamInterface::process_buffer() {
  char *string_buffer = (char*)this->serial_buffer;
  char *pch = strtok(string_buffer, ",");
  uint8_t pos = 0;
  uint32_t data = 0;
  while(pch != NULL) {
    this->port->println(pch);

    if(pos==0) {
      if(strstr(pch,"r")!=NULL) {
        this->rx_buffer[0] = CMD_R<<1;
      }else if(strstr(pch,"w")!=NULL) {
        this->rx_buffer[0] = CMD_W<<1;
      } else {
        this->rx_size = 0;
        this->port->println("Command rejected");
        break;
      }
      this->rx_size = sizeof(uint8_t);
    } else {
      data = strtol(pch,NULL, 10);
      this->rx_size += sizeof(data);
    }
    pch = strtok(NULL, ","); 
    pos++;
  }
  if(pos>1) {
    this->data_available = 1;
  }
}
