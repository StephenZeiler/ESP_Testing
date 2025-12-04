#include <Arduino.h>
#include "driver/rmt.h"

#define STEP_PIN   10
#define DIR_PIN    11
#define EN_PIN     12

// 200 pulses per rev → 180 degrees = 100 steps
const int STEPS_180 = 100;

// ---- Motion tuning ----
const int START_PULSE_US = 200;     // slow start (200µs high/low ≈ 2.5 kHz)
const int MIN_PULSE_US   = 6;       // top speed (6µs high/low ≈ 85 kHz)
const int ACCEL_STEPS    = 30;      // how many steps to accelerate
const int DECEL_STEPS    = 30;      // how many to decelerate

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
    config.clk_div = 8;  // 10MHz resolution

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

    // ---- 1. ACCELERATION ----
    for (int i = 0; i < ACCEL_STEPS && i < totalSteps; i++) {
        // Linearly decrease pulse width from START_PULSE_US → MIN_PULSE_US
        int pulse = START_PULSE_US - 
                    ( (START_PULSE_US - MIN_PULSE_US) * i / ACCEL_STEPS );
        firePulse(pulse);
    }

    // ---- 2. CRUISE ----
    int cruiseSteps = totalSteps - ACCEL_STEPS - DECEL_STEPS;
    if (cruiseSteps < 0) cruiseSteps = 0;

    for (int i = 0; i < cruiseSteps; i++) {
        firePulse(MIN_PULSE_US);
    }

    // ---- 3. DECELERATION ----
    for (int i = DECEL_STEPS - 1; i >= 0 && (ACCEL_STEPS + cruiseSteps + (DECEL_STEPS - i - 1)) < totalSteps; i--) {
        int pulse = START_PULSE_US - 
                    ( (START_PULSE_US - MIN_PULSE_US) * i / DECEL_STEPS );
        firePulse(pulse);
    }
}

void setup() {
    pinMode(DIR_PIN, OUTPUT);
    setupRMT();
}

void loop() {
    // Move +180
    digitalWrite(DIR_PIN, HIGH);
    sendStepsAccel(STEPS_180);
    delay(200);

    // Move -180
    digitalWrite(DIR_PIN, LOW);
    sendStepsAccel(STEPS_180);
    delay(200);
}
