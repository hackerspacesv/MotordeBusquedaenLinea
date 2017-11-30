#include <stdint.h>
#include <string.h>
#include "I32CTT.h"

I32CTT_ModeDriver::I32CTT_ModeDriver(uint32_t mode_id) {
  this->mode_id = mode_id;
}

I32CTT_Controller::I32CTT_Controller() {
  this->interface = 0;
  this->mode_counter = 0;
  this->current_mode = 0;
}

uint8_t I32CTT_Controller::set_interface(I32CTT_Interface &iface) {
  this->interface = &iface;
  return 0;
}

uint8_t I32CTT_Controller::add_mode_driver(I32CTT_ModeDriver &drv) {
  uint8_t result = 0;
  if(this->mode_counter<MAX_MODE_COUNT) {
    this->drivers[this->mode_counter++] = &drv;
    result = this->mode_counter;
  }
  return result;
}

void I32CTT_Controller::init() {
  if(this->interface) {
    this->interface->init();
  }
}

void I32CTT_Controller::run() {
  // Look for network events
  if(this->interface != 0) {
    this->interface->update();
    if(this->interface->available()) {
      this->parse(this->interface->tx_buffer, this->interface->tx_size);
    }
  }

  if(this->drivers[this->current_mode] != 0) {
    this->drivers[this->current_mode]->update();
  }
}

uint8_t I32CTT_Controller::valid_size(uint8_t cmd_type, uint8_t buffsize) {
  uint8_t result = 0;
  
  if(cmd_type == CMD_R || cmd_type == CMD_AW) {
     // Check for header (1-byte) + [Addr (4-byte) ...]
     result = (buffsize>4) && ((buffsize-1)%4 == 0);
  } else if (cmd_type == CMD_W || cmd_type == CMD_AR) {
     // Check for header (1-byte) + [(Addr (4-byte) + Data (4-byte) ...]
     result = (buffsize>8) && ((buffsize-1)%8 == 0);
  }

  return result;
}

uint8_t I32CTT_Controller::reg_count(uint8_t cmd_type, uint8_t buffsize) {
  uint8_t result = 0;
  
  if(cmd_type == CMD_R || cmd_type == CMD_AW) {
     // Check for header (1-byte) + [Addr (4-byte) ...]
     result = (buffsize-1)%4;
  } else if (cmd_type == CMD_W || cmd_type == CMD_AR) {
     // Check for header (1-byte) + [(Addr (4-byte) + Data (4-byte) ...]
     result = (buffsize-1)%8;
  }

  return result;
}

uint32_t I32CTT_Controller::get_reg(uint8_t *buffer, uint8_t cmd_type, uint8_t pos) {
  uint32_t result = 0;
  uint8_t offset = 0;
  
  if(cmd_type == CMD_R || cmd_type == CMD_AW) {
     offset = 1+pos*4;
  } else if (cmd_type == CMD_W || cmd_type == CMD_AR) {
     offset = 1+pos*8;
  } else {
    return result;
  }

  memcpy(&result, buffer+offset, sizeof(uint32_t));

  return result;
}

uint32_t I32CTT_Controller::get_data(uint8_t *buffer, uint8_t cmd_type, uint8_t pos) {
  uint32_t result = 0;
  uint8_t offset = 0;
  
  if (cmd_type == CMD_W || cmd_type == CMD_AR) {
     offset = 5+pos*8;
  } else {
    return result;
  }

  memcpy(&result, buffer+offset, sizeof(uint32_t));

  return result;
}

void I32CTT_Controller::put_reg(uint8_t *buffer, uint32_t reg, uint8_t cmd_type, uint8_t pos) {
  uint8_t offset = 0;
  
  if(cmd_type == CMD_R || cmd_type == CMD_AW) {
     offset = 1+pos*4;
  } else if (cmd_type == CMD_W || cmd_type == CMD_AR) {
     offset = 1+pos*8;
  } else {
    return;
  }

  memcpy(buffer+offset, &reg, sizeof(uint32_t));
}

void I32CTT_Controller::put_data(uint8_t *buffer, uint32_t data, uint8_t cmd_type, uint8_t pos) {
  uint8_t offset = 0;
  
  if (cmd_type == CMD_W || cmd_type == CMD_AR) {
     offset = 5+pos*8;
  } else {
    return;
  }

  memcpy(buffer+offset, &data, sizeof(uint32_t));
}

void I32CTT_Controller::parse(uint8_t *buffer, uint8_t buffsize) {
  if(buffsize==0) // Should never happend. But here just in case.
    return;

  uint8_t cmd = buffer[0]>>1;
  uint8_t ping = buffer[0]&0x01;

  if(!valid_size(cmd, buffsize)) // return if size invalid
    return;

  uint8_t records = reg_count(cmd, buffsize);

  if((this->interface != 0) && (this->drivers[this->current_mode] !=  0)) {
    I32CTT_ModeDriver *driver = this->drivers[this->current_mode];
    switch(cmd) {
      case CMD_R:
        this->interface->tx_size = buffsize+records*4;
        this->interface->tx_buffer[0] = CMD_AR<<1 | ping;
        
        for(int i=0;i<records;i++) {
          uint32_t reg = get_reg(buffer, cmd, i);
          put_reg(this->interface->tx_buffer, reg, cmd, i);
          put_data(this->interface->tx_buffer, driver->read(reg) , cmd, i);
        }

        this->interface->send();
        break;
      case CMD_AR:
        // Forward to response handler
        break;
      case CMD_W:
        this->interface->tx_size = buffsize-records*4;
        this->interface->tx_buffer[0] = CMD_AW<<1 | ping;

        // Handle the special machine-state change:
        if(get_reg(buffer, cmd, 0)==0) {
          uint32_t new_mode = get_data(buffer, cmd, 0);
          if(new_mode<MAX_MODE_COUNT && this->drivers[new_mode] != 0) {
            driver->on_mode_exit();
            this->current_mode = new_mode;
            driver = this->drivers[this->current_mode];
            driver->on_mode_entry();
          }
        }
        
        for(int i=0;i<records;i++) {
          uint32_t reg = get_reg(buffer, cmd, i);
          uint32_t data = get_data(buffer, cmd, i);
          driver->write(reg, data);
          put_reg(this->interface->tx_buffer, reg, cmd, i);
        }

        this->interface->send();
        break;
      case CMD_AW:
        // Forward to response handler
        break;
      default:
        break;
    }
  }
}

