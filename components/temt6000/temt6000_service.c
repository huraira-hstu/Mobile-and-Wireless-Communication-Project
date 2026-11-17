
#include <stdint.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_err.h"

#include "esp_adc/adc_oneshot.h"

#include "temt6000_service.h"


#define TEMT6000_ADC_UNIT       ADC_UNIT_1
#define TEMT6000_ADC_CHANNEL    ADC_CHANNEL_6
#define TEMT6000_ADC_WIDTH      ADC_BITWIDTH_12
#define TEMT6000_ADC_ATTEN      ADC_ATTEN_DB_12

#define TEMT6000_SAMPLE_TIME    1000


static const char *TAG = "TEMT6000";

static bool s_initialized = false;

static adc_oneshot_unit_handle_t s_adc_handle = NULL;

static temt6000_data_callback_t s_data_callback = NULL;

 

static bool temt6000_read(float *illuminance)
{
    
    if (illuminance == NULL)
    {
        return false;
    }

    int adc_value = 0;

    esp_err_t err = adc_oneshot_read(
        s_adc_handle,
        TEMT6000_ADC_CHANNEL,
        &adc_value
    );

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "ADC read failed: %s", esp_err_to_name(err));
        return false;
    }
 
    *illuminance = adc_value * 0.040293f;

    return true;
}


 
static void temt6000_task(void *pvParameters)
{
    float illuminance;

    while (1)
    {
        if (temt6000_read(&illuminance))
        {

            if (s_data_callback != NULL)
            {
                s_data_callback(illuminance);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(TEMT6000_SAMPLE_TIME));
    }
}

 
esp_err_t temt6000_service_init(
    temt6000_data_callback_t callback
)
{
    if (s_initialized)
    {
        s_data_callback = callback;
        return ESP_OK;
    }

    s_data_callback = callback;


    adc_oneshot_unit_init_cfg_t unit_config = {
        .unit_id = TEMT6000_ADC_UNIT,
        .ulp_mode = ADC_ULP_MODE_DISABLE
    };

    esp_err_t err = adc_oneshot_new_unit(
        &unit_config,
        &s_adc_handle
    );

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "ADC initialization failed: %s", esp_err_to_name(err));
        return err;
    }

    adc_oneshot_chan_cfg_t channel_config = {
        .bitwidth = TEMT6000_ADC_WIDTH,
        .atten = TEMT6000_ADC_ATTEN
    };

    err = adc_oneshot_config_channel(
        s_adc_handle,
        TEMT6000_ADC_CHANNEL,
        &channel_config
    );

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "ADC channel configuration failed: %s", esp_err_to_name(err));
        adc_oneshot_del_unit(s_adc_handle);
        s_adc_handle = NULL;

        return err;
    }


    BaseType_t task_result = xTaskCreate(
        temt6000_task,
        "temt6000_task",
        4096,
        NULL,
        5,
        NULL
    );

    if (task_result != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create TEMT6000 task");

        adc_oneshot_del_unit(s_adc_handle);
        s_adc_handle = NULL;

        return ESP_FAIL;
    }


    s_initialized = true;

    ESP_LOGI(TAG, "TEMT6000 initialized on GPIO34 (ADC1_CH6)");

    return ESP_OK;
}
