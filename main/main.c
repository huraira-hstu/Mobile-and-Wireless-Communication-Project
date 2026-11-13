
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_err.h"

#include "dht11_service.h"
#include "utils/c_utils.h"

static const char *TAG = "main";


void app_main(void)
{
    ESP_LOGI(TAG, "Starting application");

    ESP_ERROR_CHECK(dht11_service_init(action((float temperature, float humidity), {
        ESP_LOGI(TAG, "Temperature: %.1f C", temperature);
        ESP_LOGI(TAG, "Humidity: %.1f %%", humidity);
    })));

 
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
    