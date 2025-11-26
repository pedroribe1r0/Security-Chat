#pragma once

#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

void wifi_init_sta(void);