#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "cJSON.h"

void start_poolMsg_service(QueueHandle_t poolMsgQueue);