#include "control.h"
#include "config.h"
#include "imu.h"
#include "pid.h"
#include "mixer.h"
#include "motor_pwm.h"
#include "rx_espnow.h"

#include "esp_timer.h"
#include "esp_log.h"
#include <math.h>

static const char *TAG_CTRL = "CTRL";

static pid_t pid_roll;
static pid_t pid_pitch;
static pid_t pid_yaw_rate;

static bool s_armed = false;
static uint64_t s_last_step_us = 0;

static float clampf(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

void control_init(void) {
    pid_init(&pid_roll, ROLL_KP, ROLL_KI, ROLL_KD, -200.0f, 200.0f);
    pid_init(&pid_pitch, PITCH_KP, PITCH_KI, PITCH_KD, -200.0f, 200.0f);
    pid_init(&pid_yaw_rate, YAW_RATE_KP, YAW_RATE_KI, YAW_RATE_KD, -100.0f, 100.0f);

    s_last_step_us = esp_timer_get_time();
}

void control_update(void) {
    uint64_t now_us = esp_timer_get_time();
    float dt = (now_us - s_last_step_us) / 1000000.0f;
    if (dt <= 0.0f || dt > 0.1f) dt = 1.0f / CONTROL_LOOP_HZ;
    s_last_step_us = now_us;

    // Update attitude
    imu_update_attitude(dt);

    // Get RC setpoint
    setpoint_t sp; rx_get_latest(&sp);

    // Failsafe logic
    bool link_ok = rx_millis_since_last_packet() < FAILSAFE_TIMEOUT_MS;
    if (!link_ok || sp.kill) {
        s_armed = false;
    } else if (sp.arm) {
        s_armed = true;
    }

    euler_t e; imu_get_euler(&e);

    // Angle clamp on user input
    float target_roll = clampf(sp.roll_deg, -ANGLE_MAX_DEG, ANGLE_MAX_DEG);
    float target_pitch = clampf(sp.pitch_deg, -ANGLE_MAX_DEG, ANGLE_MAX_DEG);

    // Errors
    float err_roll = target_roll - e.roll_deg;
    float err_pitch = target_pitch - e.pitch_deg;
    // Yaw: rate control, we do not have a yaw estimator without mag
    float err_yaw_rate = sp.yaw_rate_dps; // target minus measured ~ 0 for now

    // PID
    float corr_roll = pid_update(&pid_roll, err_roll, dt);
    float corr_pitch = pid_update(&pid_pitch, err_pitch, dt);
    float corr_yaw = pid_update(&pid_yaw_rate, err_yaw_rate, dt);

    axis_correction_t corr = {
        .roll = corr_roll,
        .pitch = corr_pitch,
        .yaw = corr_yaw,
    };

    uint16_t m1, m2, m3, m4;

    if (!s_armed) {
        m1 = m2 = m3 = m4 = MOTOR_ARM_US;
    } else {
        mixer_quad_x(sp.throttle_us, corr, &m1, &m2, &m3, &m4);
    }

    motor_pwm_set_all_us(m1, m2, m3, m4);

    static uint32_t cnt = 0;
    if ((cnt++ % (CONTROL_LOOP_HZ / CONTROL_STATUS_HZ)) == 0) {
        ESP_LOGI(TAG_CTRL, "armed:%d roll=%.1f/%.1f pitch=%.1f/%.1f yaw=%.1f thr=%u m:%u,%u,%u,%u", (int)s_armed,
                 e.roll_deg, target_roll, e.pitch_deg, target_pitch, sp.yaw_rate_dps, sp.throttle_us, m1, m2, m3, m4);
    }
}