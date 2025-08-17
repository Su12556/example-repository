#include "motor_pwm.h"
#include "config.h"

#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"

static ledc_channel_config_t channels[4];
static uint32_t ledc_timer_period_us = 0;
static uint32_t ledc_timer_max_duty = 0;

static uint32_t clamp_u32(uint32_t v, uint32_t lo, uint32_t hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

void motor_pwm_init(void) {
    ledc_timer_config_t timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = MOTOR_PWM_RES_BITS,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = MOTOR_PWM_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer));

    // Compute helper values
    ledc_timer_period_us = 1000000UL / MOTOR_PWM_FREQ_HZ;
    ledc_timer_max_duty = (1UL << MOTOR_PWM_RES_BITS) - 1UL;

    const int motor_gpios[4] = {MOTOR_GPIO_1, MOTOR_GPIO_2, MOTOR_GPIO_3, MOTOR_GPIO_4};

    for (int i = 0; i < 4; i++) {
        channels[i] = (ledc_channel_config_t){
            .gpio_num = motor_gpios[i],
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = (ledc_channel_t)(LEDC_CHANNEL_0 + i),
            .intr_type = LEDC_INTR_DISABLE,
            .timer_sel = LEDC_TIMER_0,
            .duty = 0,
            .hpoint = 0
        };
        ESP_ERROR_CHECK(ledc_channel_config(&channels[i]));
    }

    // Initialize motors at minimum pulse
    motor_pwm_set_all_us(MOTOR_ARM_US, MOTOR_ARM_US, MOTOR_ARM_US, MOTOR_ARM_US);
}

static void set_channel_us(int index, uint16_t pulse_width_us) {
    uint32_t clamped = clamp_u32(pulse_width_us, MOTOR_MIN_US, MOTOR_MAX_US);
    uint32_t duty = (clamped * ledc_timer_max_duty) / ledc_timer_period_us;
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, channels[index].channel, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, channels[index].channel));
}

void motor_pwm_set_us(int motor_index, uint16_t pulse_width_us) {
    if (motor_index < 0 || motor_index > 3) return;
    set_channel_us(motor_index, pulse_width_us);
}

void motor_pwm_set_all_us(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4) {
    set_channel_us(0, m1);
    set_channel_us(1, m2);
    set_channel_us(2, m3);
    set_channel_us(3, m4);
}