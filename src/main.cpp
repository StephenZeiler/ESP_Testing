#include <Arduino.h>

#define STEP_PIN  10
#define DIR_PIN   11

void setup() {
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  digitalWrite(DIR_PIN, HIGH);  // pick a direction
}

void loop() {
  // 1 revolution = 200 steps (full-step) on your driver setting
  for (int i = 0; i < 200; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(1000);    // 1 ms high
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(1000);    // 1 ms low
  }

  delay(1000);  // wait 1 second

  // Reverse direction and do 1 rev back
  digitalWrite(DIR_PIN, LOW);
  for (int i = 0; i < 200; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(1000);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(1000);
  }

  delay(2000);  // pause, then repeat
}
