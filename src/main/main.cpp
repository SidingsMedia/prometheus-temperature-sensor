// SPDX-FileCopyrightText: 2023-2024 Sidings Media <contact@sidingsmedia.com>
// SPDX-License-Identifier: MIT

#include "esp_log.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "sensor/aht10.hpp"

void show_startup_info()
{
    const char tag[] = "STARTUP";
    // Firmware info
    ESP_LOGI(tag, "%s %s", PROJECT_NAME, PROJECT_VERSION);
    ESP_LOGI(tag, "Compiled %s %s", __DATE__, __TIME__);
    ESP_LOGI(tag, "Repository %s\n", PROJECT_REPO);

    // IC Info
    esp_chip_info_t chip;
    esp_chip_info(&chip);
    ESP_LOGI(
        tag, "Model: %s",
        (chip.model == CHIP_ESP8266) ? "ESP8266" : "ESP32");
    ESP_LOGI(tag, "Silicon revision: %d", chip.revision);
    ESP_LOGI(tag, "Cores: %d", chip.cores);
    ESP_LOGI(tag, "Crystal: %dMHz\n", CRYSTAL_USED);

    // Chip features
    ESP_LOGI(tag, "Features:");
    ESP_LOGI(
        tag, "Embedded flash: %s",
        (chip.features & CHIP_FEATURE_EMB_FLASH) ? "Yes" : "No");
    ESP_LOGI(
        tag, "2.4GHz WiFi: %s",
        (chip.features & CHIP_FEATURE_WIFI_BGN) ? "Yes" : "No");
    ESP_LOGI(
        tag, "Bluetooth LE: %s",
        (chip.features & CHIP_FEATURE_BLE) ? "Yes" : "No");
    ESP_LOGI(
        tag, "Bluetooth Classic: %s\n",
        (chip.features & CHIP_FEATURE_BT) ? "Yes" : "No");

    // MAC addresses
    unsigned char mac[8];
    esp_efuse_mac_get_default(mac);
    ESP_LOGI(
        tag, "MAC Address: %02x:%02x:%02x:%02x:%02x:%02x",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    // Flash Info
    ESP_LOGI(
        tag, "%dMB %s flash\n",
        spi_flash_get_chip_size() / (1024 * 1024),
        (chip.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
}

extern "C" void app_main()
{
    show_startup_info();
    AHT10 sensor = AHT10(GPIO_NUM_0, GPIO_NUM_2, I2C_NUM_0, 0x38);

    while (1)
    {
        aht10_measurement_t res = {};
        sensor.Measure(&res);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}