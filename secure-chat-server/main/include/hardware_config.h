#include "driver/gpio.h"
#include "driver/i2c.h"

#define BTN_ACEITAR     GPIO_NUM_26
#define BTN_RECUSAR     GPIO_NUM_25
#define GPIO_BIT_MASK   ((1ULL<<BTN_ACEITAR) | (1ULL<<BTN_RECUSAR))

extern void lcd_init(void);
extern void lcd_clear(void);
extern void lcd_set_cursor(int row, int col);
extern void lcd_string(char *str);