#pragma once
#include <stdint.h>
#include <stdbool.h>

void motor_pwm_init(void);
void motor_pwm_set_us(int motor_index, uint16_t pulse_width_us);
void motor_pwm_set_all_us(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4);