#include "Arduino.h"
#include "cli.h"

//#include "avr8-stub.h"
//#include "app_api.h"

Cli* cli;

void setup() {
//  debug_init();
   Serial.begin(9600);

   cli = new Cli();
}

void loop() {
  cli->Service();
  cli->ServiceProcess();
}