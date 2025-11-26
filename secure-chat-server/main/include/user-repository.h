#pragma once
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_log.h"
#include <string.h>
#include <stdlib.h>
#include "esp_random.h"

typedef struct User {
    char username[32];
    char token[10];
    struct User *next; 
} User;

void user_repo_init(void);

char* user_repo_create_user(const char* username);

bool user_repo_auth(const char* username, const char* token);

void generate_secure_token(char *buffer, size_t size);