idf_component_register(
    SRCS
        "app_main.c"
        "control.c"
        "motor_pwm.c"
        "imu.c"
        "pid.c"
        "mixer.c"
        "rx_espnow.c"
    INCLUDE_DIRS "."
    REQUIRES driver esp_timer esp_wifi esp_event nvs_flash esp_netif
)