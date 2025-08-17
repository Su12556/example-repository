#include "mixer.h"
#include "config.h"

static float clampf(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

void mixer_quad_x(uint16_t throttle_us, axis_correction_t corr, uint16_t *m1, uint16_t *m2, uint16_t *m3, uint16_t *m4) {
    // Map corrections to microseconds offset (~ scale by 5 us per unit as a placeholder)
    float scale_us = 5.0f;
    float r = corr.roll * scale_us;
    float p = corr.pitch * scale_us;
    float y = corr.yaw * scale_us;

    float m1f = (float)throttle_us + r + p - y; // front-left
    float m2f = (float)throttle_us - r + p + y; // front-right
    float m3f = (float)throttle_us - r - p - y; // rear-right
    float m4f = (float)throttle_us + r - p + y; // rear-left

    if (m1f < MOTOR_MIN_US) m1f = MOTOR_MIN_US; if (m1f > MOTOR_MAX_US) m1f = MOTOR_MAX_US;
    if (m2f < MOTOR_MIN_US) m2f = MOTOR_MIN_US; if (m2f > MOTOR_MAX_US) m2f = MOTOR_MAX_US;
    if (m3f < MOTOR_MIN_US) m3f = MOTOR_MIN_US; if (m3f > MOTOR_MAX_US) m3f = MOTOR_MAX_US;
    if (m4f < MOTOR_MIN_US) m4f = MOTOR_MIN_US; if (m4f > MOTOR_MAX_US) m4f = MOTOR_MAX_US;

    *m1 = (uint16_t)m1f;
    *m2 = (uint16_t)m2f;
    *m3 = (uint16_t)m3f;
    *m4 = (uint16_t)m4f;
}