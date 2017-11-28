#include "HS2VT2.h"
#include "HS2VT2_NullDriver.h"
#include "HS2VT2_NullInterface.h"

HS2VT2_Controller controller;

HS2VT2_NullInterface myInterface;
HS2VT2_NullDriver myDriver(0x99999999);

void setup() {
  controller.set_interface(myInterface);
  controller.add_mode_driver(myDriver);
  controller.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  controller.run();
}
