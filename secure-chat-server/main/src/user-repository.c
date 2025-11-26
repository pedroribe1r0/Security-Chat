#include "user-repository.h"


static User *head = NULL;
static SemaphoreHandle_t xListMutex = NULL;
static const char *TAG = "USER_REPO";

void generate_secure_token(char *buffer, size_t size) {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    size_t charset_len = sizeof(charset) - 1;
    for (size_t i = 0; i < size - 1; i++) {
        buffer[i] = charset[esp_random() % charset_len];
    }
    buffer[size - 1] = '\0';
}

void user_repo_init(void) {
    xListMutex = xSemaphoreCreateMutex();
}

char* user_repo_create_user(const char* username) {
    if (xListMutex == NULL) return NULL;

    if (xSemaphoreTake(xListMutex, pdMS_TO_TICKS(1000)) != pdTRUE) return NULL;

    User *aux = head;
    while(aux != NULL){
        if(strcmp(username, aux->username) == 0){
            ESP_LOGE(TAG, "User already registered");
            xSemaphoreGive(xListMutex);
            return "c"; //conflict
        }
        aux = aux->next;
    }
    
    User *newUser = (User*)malloc(sizeof(User));
    if (newUser == NULL) {
        xSemaphoreGive(xListMutex);
        return NULL;
    }

    strncpy(newUser->username, username, 31);
    newUser->username[31] = '\0';
    
    generate_secure_token(newUser->token, sizeof(newUser->token));
    
    newUser->next = head;
    head = newUser;

    ESP_LOGI(TAG, "DB: User registrado: %s", username);

    xSemaphoreGive(xListMutex);
    return newUser->token; 
}

bool user_repo_auth(const char* username, const char* token) {    
    if (username == NULL || token == NULL) return false;

    if (xListMutex == NULL) return false;

    bool auth = false;
    if (xSemaphoreTake(xListMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
        User *current = head;
        while (current != NULL && strcmp(current->username, username) != 0) {
            current = current->next;
        }
        
        if(current != NULL && strcmp(token, current->token) == 0){
            auth = true;
        }

        xSemaphoreGive(xListMutex);
    }

    return auth;
}