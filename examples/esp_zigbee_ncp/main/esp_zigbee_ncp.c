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
    // 初期化前にピンをリセット（ESP-IDFの推奨作法）
    gpio_reset_pin(GPIO_NUM_3);
    gpio_reset_pin(GPIO_NUM_14);

    // RFスイッチ回路の電源をON
    gpio_set_direction(GPIO_NUM_3, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_3, 0);

    // 【重要】ハードウェア安定化のための待機（Seeed公式仕様）
    vTaskDelay(pdMS_TO_TICKS(100)); 

    // 外部アンテナを選択
    gpio_set_direction(GPIO_NUM_14, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_14, 1);

    ESP_ERROR_CHECK(nvs_flash_init());
    
    // NCPの初期化（内部でZigbeeスタックの構造が作られます）
    ESP_ERROR_CHECK(esp_ncp_init(NCP_HOST_CONNECTION_MODE_UART));

    /* ========================================================== */
    /* 2. 送信出力（TX Power）の最大化                            */
    /* 3. 「電波の弱い子機」の足切り解除                          */
    /* ========================================================== */
    esp_zb_set_tx_power(20);
    esp_zb_secur_network_min_join_lqi_set(0);

    // NCPタスクを開始（Hostからのコマンド受付開始）
    ESP_ERROR_CHECK(esp_ncp_start());
}