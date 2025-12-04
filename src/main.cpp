#include <Arduino.h>
#include "driver/rmt.h"

#define STEP_PIN   10
#define DIR_PIN    11
#define EN_PIN     12

const int STEPS_180 = 100;

// ---- Much faster tuning ----
const int START_PULSE_US = 80;   // much faster start
const int MIN_PULSE_US   = 3;    // higher top speed
const int ACCEL_STEPS    = 10;   // quicker ramp
const int DECEL_STEPS    = 10;

void setupRMT() {
    rmt_config_t config;
    config.rmt_mode = RMT_MODE_TX;
    config.channel = RMT_CHANNEL_0;
    config.gpio_num = (gpio_num_t)STEP_PIN;
    config.mem_block_num = 1;
    config.tx_config.loop_en = false;
    config.tx_config.idle_output_en = true;
    config.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
    config.tx_config.carrier_en = false;
    config.clk_div = 8;

    rmt_config(&config);
    rmt_driver_install(config.channel, 0, 0);
}

void firePulse(int pulseWidth) {
    rmt_item32_t item;
    item.level0 = 1;
    item.duration0 = pulseWidth;
    item.level1 = 0;
    item.duration1 = pulseWidth;
    rmt_write_items(RMT_CHANNEL_0, &item, 1, false);
    rmt_wait_tx_done(RMT_CHANNEL_0, portMAX_DELAY);
}

void sendStepsAccel(int totalSteps) {

    for (int i = 0; i < ACCEL_STEPS && i < totalSteps; i++) {
        int pulse = START_PULSE_US -
                    ((START_PULSE_US - MIN_PULSE_US) * i / ACCEL_STEPS);
        firePulse(pulse);
    }

    int cruiseSteps = totalSteps - ACCEL_STEPS - DECEL_STEPS;
    if (cruiseSteps < 0) cruiseSteps = 0;

    for (int i = 0; i < cruiseSteps; i++) {
        firePulse(MIN_PULSE_US);
    }

    for (int i = DECEL_STEPS - 1; i >= 0 && 
         (ACCEL_STEPS + cruiseSteps + (DECEL_STEPS - i - 1) < totalSteps);
         i--) 
    {
        int pulse = START_PULSE_US -
                    ((START_PULSE_US - MIN_PULSE_US) * i / DECEL_STEPS);
        firePulse(pulse);
    }
}

void setup() {
    pinMode(DIR_PIN, OUTPUT);
    setupRMT();
}

void loop() {
    digitalWrite(DIR_PIN, HIGH);
    sendStepsAccel(STEPS_180);
    delay(100);

    digitalWrite(DIR_PIN, LOW);
