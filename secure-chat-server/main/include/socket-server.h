#pragma once

#include <string.h>
#include <errno.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"

#include "lwip/sockets.h"
#include "lwip/netdb.h"

#include "cJSON.h"

typedef struct Req{
    char header[16];
    char username[32];
    char hashCode[64];
    char msg[256];
    int client_sock;
} Req;

void socket_server_start(QueueHandle_t registerQueue);
