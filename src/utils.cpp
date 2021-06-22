#include "utils.h"

void PrintAscii(char *buffer, bool show_placeholder) {
  for (uint8_t i = 0; i < 16; i++) {
    if (i == 8) {
      Serial.print(" ");
    }

    if (buffer[i] > 31 and buffer[i] < 127) {
      Serial.print(buffer[i]);
    } else {
      if (show_placeholder) {
        Serial.print(".");
      }
    }
  }
}

void ThrowError(char* scope, char* message, bool new_line) {
  Serial.print(F("["));
  if (scope != NULL) {
    Serial.print(scope);
    Serial.print(F(":"));
  }
  Serial.print(F("Error] "));
  Serial.print(message);

  if (new_line) {
    Serial.println("");
  }
}