#include "imu.h"
#include "config.h"

#include "driver/i2c.h"
#include "esp_log.h"
#include <math.h>

#define I2C_PORT I2C_NUM_0
#define I2C_TIMEOUT_MS 20

static const char *TAG_IMU = "IMU";

static bool s_imu_ok = false;
static euler_t s_euler = {0};

// MPU-6050 minimal
#define MPU6050_ADDR 0x68
#define MPU6050_REG_WHOAMI 0x75
#define MPU6050_REG_PWR_MGMT_1 0x6B
#define MPU6050_REG_ACCEL_XOUT_H 0x3B

static bool i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t val) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, val, true);
    i2c_master_stop(cmd);
    esp_err_t err = i2c_master_cmd_begin(I2C_PORT, cmd, I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return err == ESP_OK;
}

static bool i2c_read_bytes(uint8_t addr, uint8_t reg, uint8_t *buf, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_READ, true);
    if (len > 1) {
        i2c_master_read(cmd, buf, len - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, buf + len - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    esp_err_t err = i2c_master_cmd_begin(I2C_PORT, cmd, I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return err == ESP_OK;
}

bool imu_init(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_GPIO,
        .scl_io_num = I2C_SCL_GPIO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
#if ESP_IDF_VERSION_MAJOR >= 5
        .master.clk_speed = I2C_FREQ_HZ,
#else
        .clk_stretch_tick = 0,
#endif
    };
#if ESP_IDF_VERSION_MAJOR >= 5
    ESP_ERROR_CHECK(i2c_param_config(I2C_PORT, &conf));
#else
    ESP_ERROR_CHECK(i2c_param_config(I2C_PORT, &conf));
#endif
    ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0));

    // Wake MPU-6050 if present
    uint8_t who = 0;
    if (i2c_read_bytes(MPU6050_ADDR, MPU6050_REG_WHOAMI, &who, 1) && who == MPU6050_ADDR) {
        if (i2c_write_reg(MPU6050_ADDR, MPU6050_REG_PWR_MGMT_1, 0x00)) {
            s_imu_ok = true;
            ESP_LOGI(TAG_IMU, "MPU-6050 detected");
        }
    } else {
        ESP_LOGW(TAG_IMU, "No IMU detected on I2C; running without attitude");
        s_imu_ok = false;
    }

    return s_imu_ok;
}

bool imu_read_raw(float *ax_g, float *ay_g, float *az_g, float *gx_dps, float *gy_dps, float *gz_dps) {
    if (!s_imu_ok) {
        *ax_g = *ay_g = 0.0f; *az_g = 1.0f; *gx_dps = *gy_dps = *gz_dps = 0.0f;
        return false;
    }
    uint8_t buf[14] = {0};
    if (!i2c_read_bytes(MPU6050_ADDR, MPU6050_REG_ACCEL_XOUT_H, buf, sizeof(buf))) {
        return false;
    }
    int16_t ax = (buf[0] << 8) | buf[1];
    int16_t ay = (buf[2] << 8) | buf[3];
    int16_t az = (buf[4] << 8) | buf[5];
    int16_t gx = (buf[8] << 8) | buf[9];
    int16_t gy = (buf[10] << 8) | buf[11];
    int16_t gz = (buf[12] << 8) | buf[13];

    // Default sensitivities: AFS=2g, GFS=250 dps
    *ax_g = (float)ax / 16384.0f;
    *ay_g = (float)ay / 16384.0f;
    *az_g = (float)az / 16384.0f;
    *gx_dps = (float)gx / 131.0f;
    *gy_dps = (float)gy / 131.0f;
    *gz_dps = (float)gz / 131.0f;
    return true;
}

void imu_update_attitude(float dt_s) {
    float ax, ay, az, gx, gy, gz;
    bool ok = imu_read_raw(&ax, &ay, &az, &gx, &gy, &gz);

    // Integrate gyro
    s_euler.roll_deg += gx * dt_s;
    s_euler.pitch_deg += gy * dt_s;
    s_euler.yaw_deg += gz * dt_s;

    // Complementary with accel tilt
    if (ok) {
        float roll_acc = atan2f(ay, az) * 57.29578f; // rad->deg
        float pitch_acc = atan2f(-ax, sqrtf(ay * ay + az * az)) * 57.29578f;
        const float alpha = 0.98f;
        s_euler.roll_deg = alpha * s_euler.roll_deg + (1.0f - alpha) * roll_acc;
        s_euler.pitch_deg = alpha * s_euler.pitch_deg + (1.0f - alpha) * pitch_acc;
        // yaw left uncorrected without mag
    }
}

void imu_get_euler(euler_t *out) {
    *out = s_euler;
}

bool imu_healthy(void) {
    return s_imu_ok;
}