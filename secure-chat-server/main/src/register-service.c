#include "register-service.h"
#include "socket-server.h"
#include "user-repository.h"
#include "hardware_config.h"
#include "freertos/task.h"

#define REGISTER_STACK 4096
static const char *TAG = "REGISTER_SERVICE";

char encryptHashCode[10] = {0};
static TaskHandle_t xRegisterTaskHandle = NULL; 

// --- 1. INTERRUPÇÃO (ISR) ---
static void IRAM_ATTR gpio_isr_handler(void* arg) {
    static uint32_t ultimo_tempo = 0;
    uint32_t tempo_atual = xTaskGetTickCountFromISR();
    
    // Debounce de 200ms
    if ((tempo_atual - ultimo_tempo) > pdMS_TO_TICKS(200)) {
        ultimo_tempo = tempo_atual;
        
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        
        if (xRegisterTaskHandle != NULL) {
            uint32_t acao = ((uint32_t)arg == BTN_ACEITAR) ? 1 : 2;

            xTaskNotifyFromISR(xRegisterTaskHandle, acao, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
        }
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

// --- 2. CONFIGURAÇÃO DOS PINOS ---
void setup_hardware_register() {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.pin_bit_mask = GPIO_BIT_MASK;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN_ACEITAR, gpio_isr_handler, (void*) BTN_ACEITAR);
    gpio_isr_handler_add(BTN_RECUSAR, gpio_isr_handler, (void*) BTN_RECUSAR);

    lcd_init();
    lcd_clear();
    lcd_string("Server Ready...");
}


static void handle_register_queue(void *pvParameters){
    if(encryptHashCode[0] == '\0'){
        ESP_LOGE(TAG, "Encrypt pass is empty");
        abort();
    }

    QueueHandle_t registerQueue = (QueueHandle_t)pvParameters;

    while(true){
        
        while(gpio_get_level(BTN_ACEITAR) == 0 || gpio_get_level(BTN_RECUSAR) == 0) {
            vTaskDelay(pdMS_TO_TICKS(50));
        }

        Req *req = NULL;
        
        if (xQueueReceive(registerQueue, (void*)&req, portMAX_DELAY)) {
            
            ESP_LOGI(TAG, "New Request Recived: %s", req->username);

            cJSON *res = cJSON_CreateObject();

            if(strcmp(req->username, "") != 0){
                lcd_clear();
                lcd_set_cursor(0, 0);
                lcd_string(req->username);
                lcd_set_cursor(1, 0);
                lcd_string("Accept?? (Y/N)");

                xTaskNotifyStateClear(NULL);

                uint32_t decisao = 0;
                
                xTaskNotifyWait(0, 0, &decisao, portMAX_DELAY);

                bool aprovado = false;
                
                if (decisao == 1) { 
                    aprovado = true;
                    lcd_clear();
                    lcd_string("Approved!");
                } else if (decisao == 2) { 
                    aprovado = false;
                    lcd_clear();
                    lcd_string("Refused!");
                } else {
                    aprovado = false;
                    lcd_string("Error");
                }
                
                vTaskDelay(pdMS_TO_TICKS(1000));
                lcd_clear();
                lcd_string("Waiting...");

                if (aprovado) {
                    char* hash = user_repo_create_user(req->username);

                    if(hash && strcmp(hash, "c") != 0) {
                        cJSON_AddStringToObject(res, "hashPass", hash);
                        cJSON_AddStringToObject(res, "encryptHashCode", encryptHashCode);
                    } 
                    else {
                        cJSON_AddStringToObject(res, "error", "Error creating user");
                    }
                } 
                else {
                    cJSON_AddStringToObject(res, "error", "Access Denied by Admin");
                }
            }

            else{
                cJSON_AddStringToObject(res, "error", "Invalid username");
            }

            char *json_string = cJSON_Print(res);
            if (json_string) {
                send(req->client_sock, json_string, strlen(json_string), 0);
                free(json_string);
            }
            cJSON_Delete(res); 
            close(req->client_sock);
            free(req);
        }
    }
}

void start_register_service(QueueHandle_t registerQueue)
{
    setup_hardware_register();

    xTaskCreate(
        handle_register_queue,
        "register_service_task",
        REGISTER_STACK,
        (void*)registerQueue,
        5,
        &xRegisterTaskHandle
    );
}