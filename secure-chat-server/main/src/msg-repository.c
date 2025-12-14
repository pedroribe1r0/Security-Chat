#include "msg-repository.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"


static ChatMessage history[MAX_HISTORY];
static int head = 0;           
static uint32_t global_id = 1; 
static SemaphoreHandle_t xMsgMutex = NULL;

void msg_repo_init(void) {
    xMsgMutex = xSemaphoreCreateMutex();

    memset(history, 0, sizeof(history));
}

void msg_repo_save(const char* sender, const char* content) {
    if (xMsgMutex == NULL) msg_repo_init();

    if (xSemaphoreTake(xMsgMutex, portMAX_DELAY)) {
        
        history[head].id = global_id++;
        strncpy(history[head].sender, sender, MAX_USER_LEN - 1);
        strncpy(history[head].content, content, MAX_MSG_LEN - 1);
        
        history[head].sender[MAX_USER_LEN - 1] = '\0';
        history[head].content[MAX_MSG_LEN - 1] = '\0';

        head++;
        if (head >= MAX_HISTORY) {
            head = 0;
        }

        xSemaphoreGive(xMsgMutex);
    }
}

ChatMessage* msg_repo_get_all(int* return_count) {
    ChatMessage* buffer = (ChatMessage*)malloc(MAX_HISTORY * sizeof(ChatMessage));
    if (xMsgMutex == NULL) msg_repo_init();
    
    if (buffer == NULL) return NULL; // Falta de RAM

    if (xSemaphoreTake(xMsgMutex, portMAX_DELAY)) {

        int count = (global_id < MAX_HISTORY) ? head : MAX_HISTORY;
        
        *return_count = count;
        int start_index = (global_id < MAX_HISTORY) ? 0 : head;

        for (int i = 0; i < count; i++) {
            int current_idx = (start_index + i) % MAX_HISTORY;
            
            buffer[i] = history[current_idx];
        }

        xSemaphoreGive(xMsgMutex);
    }
    return buffer;
}