#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "cJSON.h"

void start_register_service(QueueHandle_t registerQueue);

extern char encryptHashCode[10];