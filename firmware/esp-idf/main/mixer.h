#pragma once
#include <stdint.h>

typedef struct {
    float roll;
    float pitch;
    float yaw;
} axis_correction_t;

void mixer_quad_x(uint16_t throttle_us, axis_correction_t corr, uint16_t *m1, uint16_t *m2, uint16_t *m3, uint16_t *m4);