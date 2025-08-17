#pragma once

// MCU pins (ESP32-S3 default mapping; adjust as needed)
#define MOTOR_GPIO_1 2
#define MOTOR_GPIO_2 3
#define MOTOR_GPIO_3 4
#define MOTOR_GPIO_4 5

#define I2C_SDA_GPIO 8
#define I2C_SCL_GPIO 9
#define I2C_FREQ_HZ 400000

// Motor control (PWM mode for ESCs)
#define MOTOR_PWM_FREQ_HZ 400
#define MOTOR_PWM_RES_BITS 16
#define MOTOR_MIN_US 1000
#define MOTOR_MAX_US 2000
#define MOTOR_ARM_US MOTOR_MIN_US

// Control loop
#define CONTROL_LOOP_HZ 500
#define CONTROL_STATUS_HZ 5

// PID gains (angle mode for roll/pitch; yaw uses rate)
#define ROLL_KP 4.0f
#define ROLL_KI 0.0f
#define ROLL_KD 0.25f

#define PITCH_KP 4.0f
#define PITCH_KI 0.0f
#define PITCH_KD 0.25f

#define YAW_RATE_KP 0.8f
#define YAW_RATE_KI 0.0f
#define YAW_RATE_KD 0.02f

#define ANGLE_MAX_DEG 25.0f
#define YAW_RATE_MAX_DPS 180.0f

// Receiver / failsafe
#define FAILSAFE_TIMEOUT_MS 500
#define ESPNOW_WIFI_CHANNEL 6

// Logging
#define LOG_TAG "DRONE"