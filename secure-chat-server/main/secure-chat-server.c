#include <stdio.h>
#include "socket-server.h"
#include "wifi-connect.h"
#include "user-repository.h"
#include "register-service.h"

static const char* TAG = "MAIN";

void app_main(void){
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    QueueHandle_t registerQueue = xQueueCreate(10, sizeof(Req*)); 

    ESP_LOGI(TAG, "Connecting to WiFi...");
    wifi_init_sta();

    generate_secure_token(encryptHashCode, 10);

    user_repo_init();

    ESP_LOGI(TAG, "Starting Socket Server...");
    socket_server_start(registerQueue);

    ESP_LOGI(TAG, "Starting Register Service");
    start_register_service(registerQueue);
    
}