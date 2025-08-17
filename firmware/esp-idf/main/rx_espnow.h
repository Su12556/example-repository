#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    float roll_deg;     // desired roll angle
    float pitch_deg;    // desired pitch angle
    float yaw_rate_dps; // desired yaw rate
    uint16_t throttle_us; // base throttle in microseconds
    bool arm;
    bool kill;
} setpoint_t;

void rx_init(void);
void rx_get_latest(setpoint_t *out);
uint64_t rx_millis_since_last_packet(void);