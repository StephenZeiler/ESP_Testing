#include <Arduino.h>

#define STEP_PIN 10
#define DIR_PIN 11

void setup() {
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);
}

void stepOnce() {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(10);
}

void loop() {

    // forward
    digitalWrite(DIR_PIN, HIGH);
    delayMicroseconds(20);  // important
    for (int i = 0; i < 400; i++) stepOnce();
    delay(500);

    // backward
    digitalWrite(DIR_PIN, LOW);
    delayMicroseconds(20);  // important
    for (int i = 0; i < 400; i++) stepOnce();
    delay(500);
}
