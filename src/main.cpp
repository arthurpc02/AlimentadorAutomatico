#include <Arduino.h>

#define PIN_BLINK LED_BUILTIN

void setup() {
  pinMode(PIN_BLINK, OUTPUT);
}

void loop() {
  digitalWrite(PIN_BLINK,!digitalRead(PIN_BLINK));
  delay(200);
}