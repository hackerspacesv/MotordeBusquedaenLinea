#include <stdint.h>
#include "I32CTT.h"
#include "I32CTT_NullInterface.h"

void I32CTT_NullInterface::init() {
  // Do nothing
}

void I32CTT_NullInterface::update() {
  // Do nothing
}

uint8_t I32CTT_NullInterface::available() {
  // Do nothing
  return 0;
}

void I32CTT_NullInterface::send() {
  // Do nothing
}

