#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    float roll_deg;
    float pitch_deg;
    float yaw_deg;
} euler_t;

bool imu_init(void);
bool imu_read_raw(float *ax_g, float *ay_g, float *az_g, float *gx_dps, float *gy_dps, float *gz_dps);
void imu_update_attitude(float dt_s);
void imu_get_euler(euler_t *out);
bool imu_healthy(void);