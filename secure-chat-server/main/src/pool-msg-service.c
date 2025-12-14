#include "send-msg-service.h"
#include "user-repository.h"
#include "socket-server.h"
#include "msg-repository.h"
#include "sdkconfig.h"
#include "esp_log.h"


#define POOLMSG_STACK 4096

static const char *TAG = "POOL_MSG_TASK";

void handle_poolMsg_queue(void* pvParameters){

    QueueHandle_t poolMsgQueue = (QueueHandle_t)pvParameters;

    while(true) {
        Req *req;
        cJSON *res;

        xQueueReceive(poolMsgQueue, (void*)&req, portMAX_DELAY);

        if(strcmp(req->username, "") == 0 || strcmp(req->hashCode, "") == 0){
            ESP_LOGI(TAG, "Invalid payload");
            res = cJSON_CreateObject();
            cJSON_AddStringToObject(res, "error", "Invalid payload");   
        }

        else if(user_repo_auth(req->username, req->hashCode)){
            int qnt_msgs = 0;
            ChatMessage* buffer = msg_repo_get_all(&qnt_msgs);

            res = cJSON_CreateArray();
            
            if(buffer != NULL){
                
                for (int i = 0; i < qnt_msgs; i++) {
                    cJSON *item = cJSON_CreateObject();
                    
                    cJSON_AddNumberToObject(item, "id", buffer[i].id);
                    cJSON_AddStringToObject(item, "user", buffer[i].sender);
                    cJSON_AddStringToObject(item, "msg", buffer[i].content);
                    
                    cJSON_AddItemToArray(res, item);
                }
            }

            ESP_LOGI(TAG, "Data pool successfull");
            free(buffer);
        }
        else{
            ESP_LOGI(TAG, "Invalid token");
            res = cJSON_CreateObject();
            cJSON_AddStringToObject(res, "error", "Invalid HashCode");
        }

        char* json_string = cJSON_Print(res);

        if(json_string){
            send(req->client_sock, json_string, strlen(json_string), 0);
            free(json_string);
        }

        close(req->client_sock);

        free(req);
        cJSON_Delete(res);
    }
}

void start_poolMsg_service(QueueHandle_t poolMsgQueue) {
    xTaskCreate(
        handle_poolMsg_queue,
        "chat_pooling_task",
        POOLMSG_STACK,
        (void*)poolMsgQueue,
        5,
        NULL
    );
}