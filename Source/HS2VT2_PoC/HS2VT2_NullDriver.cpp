#include <stdint.h>
#include "HS2VT2.h"
#include "HS2VT2_NullDriver.h"

HS2VT2_NullDriver::HS2VT2_NullDriver(uint32_t mode_id) : HS2VT2_ModeDriver(mode_id) {
  // Do nothing. Just call superclass parent
}
 
uint32_t HS2VT2_NullDriver::read(uint8_t addr) {
  return 0xAAAAAAAA; // Test Pattern
}

uint8_t HS2VT2_NullDriver::write(uint8_t addr, uint32_t data) {
  return addr; // Null driver always return a valid address
}

void HS2VT2_NullDriver::h_answer_read(uint8_t addr, uint32_t data) {
  // Do nothing.
}

void HS2VT2_NullDriver::h_answer_write(uint8_t addr) {
  // Do nothing.
}

void HS2VT2_NullDriver::update() {
  // Do nothing.  
}

void HS2VT2_NullDriver::on_mode_entry() {
  // Do nothing.
}

void HS2VT2_NullDriver::on_mode_exit() {
  // Do nothing.
}

