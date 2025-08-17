#include "pid.h"

static float clampf(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

void pid_init(pid_t *pid, float kp, float ki, float kd, float out_min, float out_max) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->integral = 0.0f;
    pid->previous_error = 0.0f;
    pid->output_min = out_min;
    pid->output_max = out_max;
}

void pid_reset(pid_t *pid) {
    pid->integral = 0.0f;
    pid->previous_error = 0.0f;
}

float pid_update(pid_t *pid, float error, float dt_s) {
    float derivative = (error - pid->previous_error) / (dt_s > 1e-6f ? dt_s : 1e-6f);
    pid->integral += error * dt_s;

    float output = pid->kp * error + pid->ki * pid->integral + pid->kd * derivative;
    output = clampf(output, pid->output_min, pid->output_max);

    // anti-windup: clamp integral to keep output within bounds if ki!=0
    if (pid->ki != 0.0f) {
        float max_int = (pid->output_max) / (pid->ki + 1e-6f);
        float min_int = (pid->output_min) / (pid->ki + 1e-6f);
        if (pid->integral > max_int) pid->integral = max_int;
        if (pid->integral < min_int) pid->integral = min_int;
    }

    pid->previous_error = error;
    return output;
}