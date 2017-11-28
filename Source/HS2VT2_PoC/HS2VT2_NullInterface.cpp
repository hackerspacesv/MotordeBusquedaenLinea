#include <stdint.h>
#include "HS2VT2.h"
#include "HS2VT2_NullInterface.h"

void HS2VT2_NullInterface::init() {
  // Do nothing
}

void HS2VT2_NullInterface::update() {
  // Do nothing
}

uint8_t HS2VT2_NullInterface::available() {
  // Do nothing
  return 0;
}

void HS2VT2_NullInterface::send() {
  // Do nothing
}

