// SPDX-FileCopyrightText: 2024 Sidings Media <contact@sidingsmedia.com>
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"

#include "esp_system.h"
#include "nvs_flash.h"
#include "driver/uart.h"

#include "config/uart.hpp"
#include "config/config.hpp"

void UART::Reset() {
    esp_restart();
}

uart_err_t UART::ResetWiFiConf() {
    esp_err_t err = config_.EraseWiFiConfig();
    if (err != ESP_OK) {
        ESP_LOGE(TAG_, "Failed to reset WiFi config");
        return UART_ERR_FAIL;
    }
    return UART_ERR_OK;
}

UART::UART(int baud) {
    uart_config_t conf = {
        .baud_rate = baud,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &conf));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0, 0, NULL, 0));
}

void UART::Listen() {
    uint8_t cmd;
    while (1) {
        // Poll for a command
        int len = uart_read_bytes(UART_NUM_0, &cmd, 1, 20 / portTICK_RATE_MS);
        if (len == 0) {
            continue;
        }

        uart_err_t status[1] = { UART_ERR_OK };

        switch (cmd) {
        case UART_CMD_RESET:
            Reset();
            break;

        case UART_CMD_CONFIG_SET_WIFI_SSID:
            status[0] = UART_ERR_NOT_IMPLEMENTED;
            break;

        case UART_CMD_CONFIG_GET_WIFI_SSID:
            status[0] = UART_ERR_NOT_IMPLEMENTED;
            break;

        case UART_CMD_CONFIG_SET_WIFI_AUTH:
            status[0] = UART_ERR_NOT_IMPLEMENTED;
            break;

        case UART_CMD_CONFIG_CLEAR_WIFI:
            status[0] = ResetWiFiConf();
            break;

        default:
            status[0] = UART_ERR_INVALID_CMD;
            break;
        }

        uart_write_bytes(UART_NUM_0, (const char*)status, 1);
        cmd = 0;
    }
}
