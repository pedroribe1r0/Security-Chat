#include "socket-server.h"


#define CHAT_SERVER_PORT      3333
#define CHAT_SERVER_STACK     4096
#define CHAT_CLIENT_STACK     4096

static const char *TAG = "SOCKET_SERVER";

static Req *parse_json(char *buffer){
    Req* input = (Req*)calloc(1, sizeof(Req));
    
    if(input == NULL){
        ESP_LOGE(TAG, "Error creating Req object");
        return NULL;
    }

    cJSON *data = cJSON_Parse(buffer);
    if(!data){
        ESP_LOGE(TAG, "Invalid payload");
        return input;
    }

    cJSON *header = cJSON_GetObjectItem(data, "header");
    cJSON *hashCode = cJSON_GetObjectItem(data, "hashCode");
    cJSON *msg = cJSON_GetObjectItem(data, "msg");
    cJSON *username = cJSON_GetObjectItem(data, "username");

    if (header && cJSON_IsString(header)) {
        strncpy(input->header, header->valuestring, sizeof(input->header) - 1);
    }
    if (hashCode && cJSON_IsString(hashCode)) {
        strncpy(input->hashCode, hashCode->valuestring, sizeof(input->hashCode) - 1);
    }
    if (msg && cJSON_IsString(msg)) {
        strncpy(input->msg, msg->valuestring, sizeof(input->msg) - 1);
    }
    if (username && cJSON_IsString(username)) {
        strncpy(input->username, username->valuestring, sizeof(input->username) - 1);
    }
    
    cJSON_Delete(data);

    return input;
}

static void socket_server_task(void *pvParameters){
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    Queues* queues = (Queues*)pvParameters;

    if(sock < 0){
        ESP_LOGE(TAG, "Error starting socket server");
        abort();
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(CHAT_SERVER_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        ESP_LOGE(TAG, "bind failed: errno=%d", errno);
        close(sock);
        abort();
    }

    if (listen(sock, 10) < 0) {
        ESP_LOGE(TAG, "listen failed: errno=%d", errno);
        close(sock);
        abort();
    }
    
    while(true){
        ESP_LOGE(TAG, "Socket listening");
        int client_sock = accept(sock, NULL, NULL);

        if (client_sock < 0) {
            ESP_LOGE(TAG, "accept failed: errno=%d", errno);
            continue;
        }

        char buffer[512];
        int len = recv(client_sock, buffer, sizeof(buffer) - 1, 0);

        if(len <= 0){
            ESP_LOGE(TAG, "Client Error or closed connection");
            close(client_sock);
            continue;
        }

        buffer[len] = '\0';
        ESP_LOGE(TAG, "Payload recived %s", buffer);

        Req *input = parse_json(buffer);

        if(input == NULL || input->header[0] == '\0'){
            ESP_LOGE(TAG, "Invalid payload");
            if(input) free(input);
            close(client_sock);
            continue;
        }

        input->client_sock = client_sock;

        if(strcmp(input->header, "register") == 0){
            ESP_LOGE(TAG, "Sending to register queue");

            BaseType_t err = xQueueSend(queues->registerQueue, (void*)&input, 0);
            if(err != pdTRUE){
                ESP_LOGE(TAG, "Queue full, closing connection");
                free(input);
                close(client_sock);
            }
        }
        else if(strcmp(input->header, "send") == 0){
            ESP_LOGE(TAG, "Sending to sendMSGQueue");

            BaseType_t err = xQueueSend(queues->sendMsgQueue, (void*)&input, 0);
            if(err != pdTRUE){
                ESP_LOGE(TAG, "Queue full, closing connection");
                free(input);
                close(client_sock);
            }
        }
        else{
            ESP_LOGE(TAG, "Invalid header");
            cJSON* res;
            cJSON_AddStringToObject(res, "error", "invalid header");
            const char* json_string = cJSON_Print(res);
            send(input->client_sock, json_string, strlen(json_string), 0);

            free(input);
            close(client_sock);
        }
    }
}

void socket_server_start(Queues* queues)
{
    xTaskCreate(
        socket_server_task,
        "chat_server_task",
        CHAT_SERVER_STACK,
        (void*)queues,
        5,
        NULL
    );
}

