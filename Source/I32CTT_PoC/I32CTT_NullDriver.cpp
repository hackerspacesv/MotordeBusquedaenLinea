#include <stdint.h>
#include "I32CTT.h"
#include "I32CTT_NullDriver.h"

I32CTT_NullDriver::I32CTT_NullDriver(uint32_t mode_id) : I32CTT_ModeDriver(mode_id) {
  // Do nothing. Just call superclass parent
}
 
uint32_t I32CTT_NullDriver::read(uint8_t addr) {
  return 0xAAAAAAAA; // Test Pattern
}

uint8_t I32CTT_NullDriver::write(uint8_t addr, uint32_t data) {
  return addr; // Null driver always return a valid address
}

void I32CTT_NullDriver::h_answer_read(uint8_t addr, uint32_t data) {
  // Do nothing.
}

void I32CTT_NullDriver::h_answer_write(uint8_t addr) {
  // Do nothing.
}

void I32CTT_NullDriver::update() {
  // Do nothing.  
}

void I32CTT_NullDriver::on_mode_entry() {
  // Do nothing.
}

void I32CTT_NullDriver::on_mode_exit() {
  // Do nothing.
}

