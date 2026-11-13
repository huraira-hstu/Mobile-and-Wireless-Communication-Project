#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_rom_sys.h"
#include "esp_log.h"

#include "dht11_service.h"

#define DHT11_PIN           GPIO_NUM_13
#define DHT11_TIMEOUT_US    100
#define DATA_FRAME_SIZE     40


static const char *TAG = "DHT11";
static bool s_initialized = false;

static dht11_data_callback_t s_data_callback = NULL;




static bool dht11_wait_for_level(int8_t level, int64_t timeout_us)
{
    int64_t start = esp_timer_get_time();

    while (gpio_get_level(DHT11_PIN) != level)
    {
        if (esp_timer_get_time() - start >= timeout_us)
        {
            return false;
        }
    }

    return true;
}



static bool dht11_read(float *temperature, float *humidity)
{
    uint8_t data[5] = {0};

    gpio_set_direction(DHT11_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(DHT11_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(20));

    gpio_set_level(DHT11_PIN, 1);
    esp_rom_delay_us(30);

    gpio_set_direction(DHT11_PIN, GPIO_MODE_INPUT);
 

    if (!dht11_wait_for_level(0, DHT11_TIMEOUT_US))
        return false;

    if (!dht11_wait_for_level(1, DHT11_TIMEOUT_US))
        return false;

    if (!dht11_wait_for_level(0, DHT11_TIMEOUT_US))
        return false;
    

    for (int i = 0; i < DATA_FRAME_SIZE; i++)
    { 
        if (!dht11_wait_for_level(1, DHT11_TIMEOUT_US))
            return false;

        int64_t start = esp_timer_get_time();
 
        if (!dht11_wait_for_level(0, DHT11_TIMEOUT_US))
            return false;

        int64_t duration = esp_timer_get_time() - start;

        data[i / 8] <<= 1;
 
        if (duration > 40)
        {
            data[i / 8] |= 1;
        }
    }
 
    if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) != data[4])
    {
        return false;
    }


    *humidity = data[0] + data[1] * 0.1f;
    
    *temperature = data[2] + data[3] * 0.1f;

    return true;
}

 
static void dht11_task(void *pvParameters)
{
    float temperature;
    float humidity;

    while (1)
    {
        if (dht11_read(&temperature, &humidity))
        {
            if (s_data_callback != NULL)
            {
                s_data_callback(temperature, humidity);
            }
        }
        else
        {
            ESP_LOGE(TAG, "Failed to read DHT11");
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
 
esp_err_t dht11_service_init(dht11_data_callback_t callback)
{
    s_data_callback = callback;

    if (s_initialized)
    {
        return ESP_OK;
    }

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << DHT11_PIN),
        .mode = GPIO_MODE_INPUT_OUTPUT_OD,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    esp_err_t err = gpio_config(&io_conf);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "GPIO configuration failed: %s", esp_err_to_name(err));
        return err;
    }

    BaseType_t task_result = xTaskCreate(
        dht11_task,
        "dht11_task",
        4096,
        NULL,
        5,
        NULL
    );

    if (task_result != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create DHT11 task");
        return ESP_FAIL;
    }

    s_initialized = true;

    ESP_LOGI(TAG, "DHT11 service initialized on GPIO %d", DHT11_PIN);

    return ESP_OK;
}



 