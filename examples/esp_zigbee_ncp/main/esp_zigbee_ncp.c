/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "nvs_flash.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_zb_ncp.h"

#include "driver/gpio.h"
#include "esp_zigbee_core.h"
#include "esp_zigbee_secur.h"

void app_main(void)
{
    /* ========================================================== */
    /* 1. XIAO ESP32-C6 外部アンテナの強制有効化                  */
    /*    GPIO3: LOW, GPIO14: HIGH (Seeed Studio XIAO ESP32-C6)   */
    /* ========================================================== */
    gpio_set_direction(GPIO_NUM_3, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_3, 0);
    gpio_set_direction(GPIO_NUM_14, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_14, 1);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_ncp_init(NCP_HOST_CONNECTION_MODE_UART));
    ESP_ERROR_CHECK(esp_ncp_start());

    /* ========================================================== */
    /* NCPタスク内のZigbeeスタック初期化完了を待機                */
    /* ========================================================== */
    vTaskDelay(pdMS_TO_TICKS(3000)); // 3秒待機

    /* ========================================================== */
    /* 2. 送信出力（TX Power）の最大化                            */
    /*    最大20dBmに設定（ハードウェア上限に自動クリップされます）*/
    /* ========================================================== */
    esp_zb_set_tx_power(20);

    /* ========================================================== */
    /* 3. 「電波の弱い子機」の足切り解除                          */
    /*    ネットワーク参加時の最小LQIを0に設定し、足切りを無効化  */
    /* ========================================================== */
    esp_zb_secur_network_min_join_lqi_set(0);
}
