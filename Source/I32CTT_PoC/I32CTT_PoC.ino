#include "I32CTT.h"
#include "I32CTT_NullDriver.h"
#include "I32CTT_ArduinoStreamInterface.h"

I32CTT_Controller controller;

//I32CTT_NullInterface myInterface;
I32CTT_ArduinoStreamInterface serialInterface(Serial);
I32CTT_NullDriver myDriver(0x99999999);

void setup() {
  while(!Serial);
  Serial.begin(9600);
  controller.set_interface(serialInterface);
  controller.add_mode_driver(myDriver);
  controller.init();
  Serial.println("Running...");
}

void loop() {
  // put your main code here, to run repeatedly:
  controller.run();
}
