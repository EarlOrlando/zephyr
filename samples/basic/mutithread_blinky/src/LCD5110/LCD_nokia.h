#ifndef LCD_NOKIA_H_
#define LCD_NOKIA_H_

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <stdint.h>

#define SCREENW (84)
#define SCREENH (48)

#define LCD_X    (SCREENW)
#define LCD_Y    (SCREENH)
#define LCD_DATA (1)
#define LCD_CMD  (0)

void LCD_nokia_init(void);
void LCD_nokia_write_byte(uint8_t data_or_command, uint8_t data);
void LCD_nokia_clear(void);
void LCD_nokia_goto_xy(uint8_t x, uint8_t y);
void LCD_nokia_bitmap(const uint8_t bitmap[]);
void LCD_nokia_send_char(uint8_t character);
void LCD_nokia_send_string(const char *string);
void LCD_nokia_draw_line(uint8_t *bitmap, int x0, int y0, int x1, int y1);
void LCD_nokia_draw_rectangle(uint8_t *bitmap, int x, int y, int width, int height);
void LCD_nokia_draw_filled_rectangle(uint8_t *bitmap, int x, int y, int width, int height);
void LCD_nokia_draw_circle(uint8_t *bitmap, int x0, int y0, int radius);
int LCD_nokia_map_value_to_pixel(float value, int min_value, int max_value);
void LCD_nokia_set_pixel_in_bitmap(uint8_t *bitmap, int x, int y);

#endif /* LCD_NOKIA_H_ */