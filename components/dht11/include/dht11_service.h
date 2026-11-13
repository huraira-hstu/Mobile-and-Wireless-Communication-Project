
#pragma once

#include "esp_err.h"


typedef void (*dht11_data_callback_t)(float temperature, float humidity);



esp_err_t dht11_service_init(dht11_data_callback_t callback);



 