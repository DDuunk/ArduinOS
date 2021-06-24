#include "Arduino.h"
#include "cli.h"

static Cli cli;

void setup() {
  Serial.begin(9600);

//  cli.Start();
  cli.Start(true);
}

void loop() {
  cli.Service();
  cli.ServiceProcess();
}