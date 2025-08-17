#pragma once
#include <stdint.h>

typedef struct {
    float kp;
    float ki;
    float kd;
    float integral;
    float previous_error;
    float output_min;
    float output_max;
} pid_t;

void pid_init(pid_t *pid, float kp, float ki, float kd, float out_min, float out_max);
void pid_reset(pid_t *pid);
float pid_update(pid_t *pid, float error, float dt_s);