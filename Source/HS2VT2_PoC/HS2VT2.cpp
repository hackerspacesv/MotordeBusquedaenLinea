#include <stdint.h>
#include "HS2VT2.h"

HS2VT2_ModeDriver::HS2VT2_ModeDriver(uint32_t mode_id) {
  this->mode_id = mode_id;
}

HS2VT2_Controller::HS2VT2_Controller() {
  this->interface = 0;
  this->mode_counter = 0;
  this->current_mode = 0;
}

uint8_t HS2VT2_Controller::set_interface(HS2VT2_Interface &iface) {
  this->interface = &iface;
  return 0;
}

uint8_t HS2VT2_Controller::add_mode_driver(HS2VT2_ModeDriver &drv) {
  uint8_t result = 0;
  if(this->mode_counter<64) {
    this->drivers[this->mode_counter++] = &drv;
    result = this->mode_counter;
  }
  return result;
}

void HS2VT2_Controller::init() {
  if(this->interface) {
    this->interface->init();
  }
}

void HS2VT2_Controller::run() {
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

void HS2VT2_Controller::parse(uint8_t *buffer, uint8_t buffsize) {
  // TODO:
  // 1. Chequear primer bit 1: Escritura / 2: lectura
  // 2. Chequear si la trama tiene sentido
  // 3. Llamar a los registros paquete por paquete para llenar el buffer de salida.
  // 3.1 Si es un paquete escritura en 0. Llamar a las funciones de cambio de estado
  // 4. Enviar la respuesta antes de continuar.
}

