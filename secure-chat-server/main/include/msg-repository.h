#ifndef MSG_REPOSITORY_H
#define MSG_REPOSITORY_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_HISTORY 50
#define MAX_MSG_LEN 100
#define MAX_USER_LEN 20

typedef struct {
    uint32_t id;               
    char sender[MAX_USER_LEN];  
    char content[MAX_MSG_LEN];  
} ChatMessage;


void msg_repo_init(void);

void msg_repo_save(const char* sender, const char* content);

ChatMessage* msg_repo_get_all(int* return_count);

#endif