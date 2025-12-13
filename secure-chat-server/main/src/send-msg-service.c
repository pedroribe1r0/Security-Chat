#include "send-msg-service.h"
#include "user-repository.h"
#include "socket-server.h"
#include "msg-repository.h"
#include "sdkconfig.h"
#include "esp_log.h"


#define SENDMSG_STACK 4096

static const char *TAG = "SEND_MSG_TASK";

void handle_sendMsg_queue(void* pvParameters){

    QueueHandle_t sendMsgQueue = (QueueHandle_t)pvParameters;

    while(true) {
        Req *req;
        cJSON *res = cJSON_CreateObject();

        xQueueReceive(sendMsgQueue, (void*)&req, portMAX_DELAY);

        if(req->username == "" || req->hashCode == "" || req->msg == ""){
            ESP_LOGI(TAG, "Invalid payload");
            cJSON_AddStringToObject(res, "error", "Invalid payload");
        }

        else if(!user_repo_auth(req->username, req->hashCode)){
            msg_repo_save(req->username, req->msg);
            ESP_LOGI(TAG, "Message saved: %s", req->msg);

            cJSON_AddStringToObject(res, "status", "Message successfully saved");
            cJSON_AddStringToObject(res, "msg", req->msg);
        }
        else{
            ESP_LOGI(TAG, "Invalid token");
            cJSON_AddStringToObject(res, "error", "Invalid HashCode");
        }

        char* json_string = cJSON_Print(res);

        if(json_string){
            send(req->client_sock, json_string, strlen(json_string), 0);
            free(json_string);
        }

        free(req);
        cJSON_Delete(res);
    }
    
}

void start_sendMsg_service(QueueHandle_t sendMsgQueue) {
    xTaskCreate(
        handle_sendMsg_queue,
        "chat_recive_task",
        SENDMSG_STACK,
        (void*)sendMsgQueue,
        5,
        NULL
    );
}