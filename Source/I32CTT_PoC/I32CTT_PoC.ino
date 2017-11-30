#include "I32CTT.h"
#include "I32CTT_NullDriver.h"
#include "I32CTT_NullInterface.h"

I32CTT_Controller controller;

I32CTT_NullInterface myInterface;
I32CTT_NullDriver myDriver(0x99999999);

void setup() {
  controller.set_interface(myInterface);
  controller.add_mode_driver(myDriver);
  controller.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  controller.run();
}
