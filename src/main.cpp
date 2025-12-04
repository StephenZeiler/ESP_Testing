#include <Arduino.h>
#include "driver/rmt.h"

#define STEP_PIN   4
#define DIR_PIN    5

// 200 pulses per rev → 180 degrees = 100 steps
const int STEPS_180 = 100;

// Pulse width controls speed (in microseconds)
const int PULSE_US = 3;

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
    config.clk_div = 8;  // 10MHz resolution -> clk_div=8 on 80MHz base

    rmt_config(&config);
    rmt_driver_install(config.channel, 0, 0);
}

void sendSteps(int steps, int pulseWidth) {
    rmt_item32_t items[1];

    items[0].duration0 = pulseWidth;     // HIGH time
    items[0].level0 = 1;
    items[0].duration1 = pulseWidth;     // LOW time
    items[0].level1 = 0;

    // send N pulses
    rmt_write_items(RMT_CHANNEL_0, items, 1, false);
    rmt_wait_tx_done(RMT_CHANNEL_0, portMAX_DELAY);

    // repeat
    for (int i = 0; i < steps; i++) {
        rmt_write_items(RMT_CHANNEL_0, items, 1, false);
        rmt_wait_tx_done(RMT_CHANNEL_0, portMAX_DELAY);
    }
}

void setup() {
    pinMode(DIR_PIN, OUTPUT);
    setupRMT();
}

void loop() {
    // +180 degrees
    digitalWrite(DIR_PIN, HIGH);
    sendSteps(STEPS_180, PULSE_US);

    delay(200);

    // -180 degrees
    digitalWrite(DIR_PIN, LOW);
    sendSteps(STEPS_180, PULSE_US);

    delay(200);
}
