#include "driver/i2c.h"
#include "esp_log.h"
#include <unistd.h> 

// --- Configurações do I2C e LCD ---
#define I2C_NUM           I2C_NUM_0
#define I2C_SCL_IO        GPIO_NUM_27  
#define I2C_SDA_IO        GPIO_NUM_14  
#define I2C_FREQ_HZ       50000        // <--- MUDANÇA 1: Baixei para 50kHz (Mais estável)
#define LCD_ADDR          0x27         
#define TAG_LCD           "LCD"


void lcd_send_cmd (uint8_t cmd) {
  uint8_t data_u, data_l;
  uint8_t data_t[4];
  data_u = (cmd & 0xf0);
  data_l = ((cmd << 4) & 0xf0);
  data_t[0] = data_u | 0x0C;  //en=1, rs=0
  data_t[1] = data_u | 0x08;  //en=0, rs=0
  data_t[2] = data_l | 0x0C;  //en=1, rs=0
  data_t[3] = data_l | 0x08;  //en=0, rs=0
  i2c_master_write_to_device(I2C_NUM, LCD_ADDR, data_t, 4, 1000 / portTICK_PERIOD_MS);
}

void lcd_send_data (uint8_t data) {
  uint8_t data_u, data_l;
  uint8_t data_t[4];
  data_u = (data & 0xf0);
  data_l = ((data << 4) & 0xf0);
  data_t[0] = data_u | 0x0D;  //en=1, rs=1
  data_t[1] = data_u | 0x09;  //en=0, rs=1
  data_t[2] = data_l | 0x0D;  //en=1, rs=1
  data_t[3] = data_l | 0x09;  //en=0, rs=1
  i2c_master_write_to_device(I2C_NUM, LCD_ADDR, data_t, 4, 1000 / portTICK_PERIOD_MS);
}

void lcd_clear (void) {
    lcd_send_cmd(0x01);
    vTaskDelay(pdMS_TO_TICKS(10)); 
}

void lcd_init (void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_IO,
        .scl_io_num = I2C_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_FREQ_HZ,
    };
    i2c_param_config(I2C_NUM, &conf);
    i2c_driver_install(I2C_NUM, conf.mode, 0, 0, 0);

    // Sequência de Inicialização "Hitachi" Estrita
    vTaskDelay(pdMS_TO_TICKS(100)); // Espera a tensão estabilizar
    lcd_send_cmd (0x30);
    vTaskDelay(pdMS_TO_TICKS(10));
    lcd_send_cmd (0x30);
    vTaskDelay(pdMS_TO_TICKS(10));
    lcd_send_cmd (0x30);
    vTaskDelay(pdMS_TO_TICKS(10));
    lcd_send_cmd (0x20);  
    vTaskDelay(pdMS_TO_TICKS(10));

    lcd_send_cmd (0x28); 
    vTaskDelay(pdMS_TO_TICKS(1));
    lcd_send_cmd (0x08); 
    vTaskDelay(pdMS_TO_TICKS(1));
    lcd_send_cmd (0x01); // Clear
    vTaskDelay(pdMS_TO_TICKS(20)); // Delay longo aqui
    lcd_send_cmd (0x06); 
    vTaskDelay(pdMS_TO_TICKS(1));
    lcd_send_cmd (0x0C); 
    vTaskDelay(pdMS_TO_TICKS(1));
}

void lcd_string (char *str) {
    while (*str) lcd_send_data ((uint8_t)(*str++));
}

void lcd_set_cursor (int row, int col) {
    if (row == 0) lcd_send_cmd(0x80 + col);
    else if (row == 1) lcd_send_cmd(0xC0 + col);
}