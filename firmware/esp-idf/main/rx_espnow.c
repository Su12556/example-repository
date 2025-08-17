#include "rx_espnow.h"
#include "config.h"

#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <string.h>

static const char *TAG_RX = "RX";

#pragma pack(push, 1)
typedef struct {
    uint16_t magic; // 0xA1D1
    int16_t roll_tenths_deg;    // -2500..2500 => -250.0..250.0 deg
    int16_t pitch_tenths_deg;
    int16_t yaw_rate_tenths_dps; // -3600..3600 => -360.0..360.0 dps
    uint16_t throttle_us;       // 1000..2000
    uint8_t arm;                // 0/1
    uint8_t kill;               // 0/1
    uint32_t seq;
} rc_packet_t;
#pragma pack(pop)

static setpoint_t s_sp = {0};
static volatile uint64_t s_last_rx_us = 0;

static void wifi_init(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Fix channel for ESP-NOW
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
    ESP_ERROR_CHECK(esp_wifi_set_channel(ESPNOW_WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE));
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(false));
}

static void on_recv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
    if (len < (int)sizeof(rc_packet_t)) return;
    rc_packet_t pkt; memcpy(&pkt, data, sizeof(pkt));
    if (pkt.magic != 0xA1D1) return;

    s_sp.roll_deg = pkt.roll_tenths_deg / 10.0f;
    s_sp.pitch_deg = pkt.pitch_tenths_deg / 10.0f;
    s_sp.yaw_rate_dps = pkt.yaw_rate_tenths_dps / 10.0f;
    if (s_sp.yaw_rate_dps > YAW_RATE_MAX_DPS) s_sp.yaw_rate_dps = YAW_RATE_MAX_DPS;
    if (s_sp.yaw_rate_dps < -YAW_RATE_MAX_DPS) s_sp.yaw_rate_dps = -YAW_RATE_MAX_DPS;

    if (pkt.throttle_us < MOTOR_MIN_US) pkt.throttle_us = MOTOR_MIN_US;
    if (pkt.throttle_us > MOTOR_MAX_US) pkt.throttle_us = MOTOR_MAX_US;
    s_sp.throttle_us = pkt.throttle_us;
    s_sp.arm = pkt.arm != 0;
    s_sp.kill = pkt.kill != 0;

    s_last_rx_us = esp_timer_get_time();
}

void rx_init(void) {
    wifi_init();
    ESP_ERROR_CHECK(esp_now_init());
    // Leave open to any sender; for encryption add peers here.
    ESP_ERROR_CHECK(esp_now_register_recv_cb(on_recv));
    ESP_LOGI(TAG_RX, "ESP-NOW RX ready on channel %d", ESPNOW_WIFI_CHANNEL);
}

void rx_get_latest(setpoint_t *out) {
    *out = s_sp;
}

uint64_t rx_millis_since_last_packet(void) {
    uint64_t now = esp_timer_get_time();
    if (s_last_rx_us == 0) return UINT64_MAX;
    return (now - s_last_rx_us) / 1000ULL;
}