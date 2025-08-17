#include "config.h"
#include "motor_pwm.h"
#include "imu.h"
#include "rx_espnow.h"
#include "control.h"

#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = LOG_TAG;

static void control_timer_cb(void *arg) {
    (void)arg;
    control_update();
}

void app_main(void) {
    ESP_LOGI(TAG, "ESP32 AI Drone starting");

    // Motors first at safe min
    motor_pwm_init();

    // IMU
    imu_init();

    // RX
    rx_init();

    // Control
    control_init();

    // Periodic timer
    const esp_timer_create_args_t targs = {
        .callback = &control_timer_cb,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "ctrl"
    };
    esp_timer_handle_t timer;
    ESP_ERROR_CHECK(esp_timer_create(&targs, &timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer, 1000000ULL / CONTROL_LOOP_HZ));

    ESP_LOGI(TAG, "Control loop running at %d Hz", CONTROL_LOOP_HZ);
}