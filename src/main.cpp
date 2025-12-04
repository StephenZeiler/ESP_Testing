#include <Arduino.h>

#define STEP_PIN  10
#define DIR_PIN   11

void setup() {
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  digitalWrite(DIR_PIN, HIGH);  // pick a direction
}

void loop() {
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(500);  // step pulse high
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(500);  // step pulse low
}
