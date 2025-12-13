#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "cJSON.h"

void start_sendMsg_service(QueueHandle_t sendMsgQueue);

extern char encryptHashCode[10];