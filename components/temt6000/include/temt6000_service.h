
#ifndef TEMT6000_SERVICE_H
#define TEMT6000_SERVICE_H

#include <stdint.h>
#include "esp_err.h"

typedef void (*temt6000_data_callback_t)(
    float illuminance
);

esp_err_t temt6000_service_init(
    temt6000_data_callback_t callback
);

#endif
