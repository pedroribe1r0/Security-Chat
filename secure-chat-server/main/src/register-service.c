#include "register-service.h"
#include "socket-server.h"
#include "user-repository.h"

#define REGISTER_STACK 4096

static const char *TAG = "REGISTER_SERVICE";

char encryptHashCode[10] = {0};

static void handle_register_queue(void *pvParameters){
    if(encryptHashCode[0] == '\0'){
        ESP_LOGE(TAG, "Encrypt pass is empty");
        abort();
    }

    QueueHandle_t registerQueue = (QueueHandle_t)pvParameters;

    while(true){
        Req *req = NULL;

        cJSON *res = cJSON_CreateObject();

        xQueueReceive(registerQueue, (void*)&req, portMAX_DELAY);

        char* hash = user_repo_create_user(req->username);

        if(hash == NULL){
            cJSON_AddStringToObject(res, "error", "Internal server error, busy");
        }

        else if(strcmp(hash, "c") == 0){
            cJSON_AddStringToObject(res, "error", "User already registered");
        }

        else {
            cJSON_AddStringToObject(res, "hashPass", hash);
            cJSON_AddStringToObject(res, "encryptHashCode", encryptHashCode);
        }

        char *json_string = cJSON_Print(res);
        if (json_string == NULL) { 
            ESP_LOGE(TAG, "failed creating json_string");
            cJSON_Delete(res); 
            close(req->client_sock);
            free(req);

            return;
        }

        size_t len = strlen(json_string);

        ssize_t sent_bytes = send(req->client_sock, json_string, len, 0);

        if(sent_bytes < len){
            ESP_LOGE(TAG, "Error sending response");
        }

        close(req->client_sock);
        free(req);

        free(json_string);
        cJSON_Delete(res);
    }
}


void start_register_service(QueueHandle_t registerQueue)
{
    xTaskCreate(
        handle_register_queue,
        "register_service_task",
        REGISTER_STACK,
        (void*)registerQueue,
        5,
        NULL
    );
}
