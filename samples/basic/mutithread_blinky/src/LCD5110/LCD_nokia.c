#include "LCD_nokia.h"
#include "LCD_nokia_images.h"
#include <string.h>
#include <sys/_stdint.h>

static const uint8_t ASCII[][5] =
{
 {0x00, 0x00, 0x00, 0x00, 0x00} // 20
,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c �
,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j
,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e
,{0x78, 0x46, 0x41, 0x46, 0x78} // 7f
};

// Device tree compatible string
#define DT_DRV_COMPAT nokia_lcd_5110

// Define SPI operation flags (Mode 0)
// #define SPI_OP (SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_MODE_CPHA | SPI_MODE_CPOL)
#define SPI_OP (SPI_WORD_SET(8) | SPI_TRANSFER_MSB)

// SPI device
static const struct spi_dt_spec lcd_spi = SPI_DT_SPEC_INST_GET(0, SPI_OP, 0);
// static const struct spi_dt_spec lcd_spi = SPI_DT_SPEC_INST_GET(0, SPI_OP);

// GPIO for Data/Command (DC) pin
static const struct gpio_dt_spec dc_gpio = GPIO_DT_SPEC_INST_GET(0, dc_gpios);

// GPIO for Reset pin
static const struct gpio_dt_spec reset_gpio = GPIO_DT_SPEC_INST_GET(0, reset_gpios);

// GPIO for Backlight pin
static const struct gpio_dt_spec backlight_gpio = GPIO_DT_SPEC_INST_GET(0, backlight_gpios);

void LCD_nokia_init(void)
{
    int ret;

    printk("Initializing LCD 5110\n");

    // Check if devices are ready
    if (!spi_is_ready_dt(&lcd_spi)) {
        printk("SPI device not ready\n");
        return;
    }
    if (!device_is_ready(dc_gpio.port)) {
        printk("DC GPIO device not ready\n");
        return;
    }
    if (!device_is_ready(reset_gpio.port)) {
        printk("Reset GPIO device not ready\n");
        return;
    }
    if (!device_is_ready(backlight_gpio.port)) {
        printk("Backlight GPIO device not ready\n");
        return;
    }

    // Configure GPIOs
    ret = gpio_pin_configure_dt(&dc_gpio, GPIO_OUTPUT);
    if (ret < 0) {
        printk("Failed to configure DC GPIO\n");
        return;
    }

    ret = gpio_pin_configure_dt(&reset_gpio, GPIO_OUTPUT);
    if (ret < 0) {
        printk("Failed to configure Reset GPIO\n");
        return;
    }

    ret = gpio_pin_configure_dt(&backlight_gpio, GPIO_OUTPUT);
    if (ret < 0) {
        printk("Failed to configure Backlight GPIO\n");
        return;
    }

    // Turn on the backlight
    gpio_pin_set_dt(&backlight_gpio, 1);

    // Reset the LCD
    gpio_pin_set_dt(&reset_gpio, 0);
    k_sleep(K_MSEC(10));
    gpio_pin_set_dt(&reset_gpio, 1);

    // Initialization commands
    LCD_nokia_write_byte(LCD_CMD, 0x21); // Extended instruction set
    LCD_nokia_write_byte(LCD_CMD, 0xB1); // Set LCD Vop (Contrast)
    LCD_nokia_write_byte(LCD_CMD, 0x04); // Set Temperature Coefficient
    LCD_nokia_write_byte(LCD_CMD, 0x14); // LCD Bias Mode 1:48

    LCD_nokia_write_byte(LCD_CMD, 0x20); // Basic instruction set
    LCD_nokia_write_byte(LCD_CMD, 0x0C); // Normal display mode

    printk("LCD 5110 initialization done\n");
}

void LCD_nokia_write_byte(uint8_t data_or_command, uint8_t data)
{
    int ret;
    struct spi_buf tx_buf;
    struct spi_buf_set tx_bufs;

    // Set DC pin
    gpio_pin_set_dt(&dc_gpio, data_or_command == LCD_DATA ? 1 : 0);

    tx_buf.buf = &data;
    tx_buf.len = 1;
    tx_bufs.buffers = &tx_buf;
    tx_bufs.count = 1;

    ret = spi_write_dt(&lcd_spi, &tx_bufs);
    if (ret < 0) {
        printk("Failed to write to SPI device: %d\n", ret);
    }
}

void LCD_nokia_send_char(uint8_t character)
{
    uint16_t index = 0;

    LCD_nokia_write_byte(LCD_DATA, 0x00); // Blank vertical line padding

    for (index = 0; index < 5; index++) {
        LCD_nokia_write_byte(LCD_DATA, ASCII[character - 0x20][index]);
    }

    LCD_nokia_write_byte(LCD_DATA, 0x00); // Blank vertical line padding
}

void LCD_nokia_send_string(const char *characters)
{
    while (*characters) {
        LCD_nokia_send_char(*characters++);
    }
}

void LCD_nokia_draw_line(uint8_t* bitmap, int x0, int y0, int x1, int y1)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        LCD_nokia_set_pixel_in_bitmap(bitmap, x0, y0);
        if (x0 == x1 && y0 == y1)
            break;
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void LCD_nokia_draw_rectangle(uint8_t* bitmap, int x, int y, int width, int height)
{
    LCD_nokia_draw_line(bitmap, x, y, x + width, y);
    LCD_nokia_draw_line(bitmap, x + width, y, x + width, y + height);
    LCD_nokia_draw_line(bitmap, x + width, y + height, x, y + height);
    LCD_nokia_draw_line(bitmap, x, y + height, x, y);
}

void LCD_nokia_draw_filled_rectangle(uint8_t* bitmap, int x, int y, int width, int height)
{
    for (int i = x; i < x + width; i++) {
        for (int j = y; j < y + height; j++) {
            LCD_nokia_set_pixel_in_bitmap(bitmap, i, j);
        }
    }
}

void LCD_nokia_draw_circle(uint8_t* bitmap, int x0, int y0, int radius)
{
    int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

    LCD_nokia_set_pixel_in_bitmap(bitmap, x0, y0 + radius);
    LCD_nokia_set_pixel_in_bitmap(bitmap, x0, y0 - radius);
    LCD_nokia_set_pixel_in_bitmap(bitmap, x0 + radius, y0);
    LCD_nokia_set_pixel_in_bitmap(bitmap, x0 - radius, y0);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x + 1;

        LCD_nokia_set_pixel_in_bitmap(bitmap, x0 + x, y0 + y);
        LCD_nokia_set_pixel_in_bitmap(bitmap, x0 - x, y0 + y);
        LCD_nokia_set_pixel_in_bitmap(bitmap, x0 + x, y0 - y);
        LCD_nokia_set_pixel_in_bitmap(bitmap, x0 - x, y0 - y);
        LCD_nokia_set_pixel_in_bitmap(bitmap, x0 + y, y0 + x);
        LCD_nokia_set_pixel_in_bitmap(bitmap, x0 - y, y0 + x);
        LCD_nokia_set_pixel_in_bitmap(bitmap, x0 + y, y0 - x);
        LCD_nokia_set_pixel_in_bitmap(bitmap, x0 - y, y0 - x);
    }
}

void LCD_nokia_clear(void)
{
    uint16_t index = 0;
    for (index = 0; index < (LCD_X * LCD_Y / 8); index++) {
        LCD_nokia_write_byte(LCD_DATA, 0x00);
    }
    LCD_nokia_goto_xy(0, 0); // After clearing, return to home position
}

void LCD_nokia_goto_xy(uint8_t x, uint8_t y)
{
    LCD_nokia_write_byte(LCD_CMD, 0x80 | x); // Column
    LCD_nokia_write_byte(LCD_CMD, 0x40 | y); // Row
}

void LCD_nokia_bitmap(const uint8_t bitmap[])
{
    uint16_t index = 0;
    for (index = 0; index < (LCD_X * LCD_Y / 8); index++) {
        LCD_nokia_write_byte(LCD_DATA, bitmap[index]);
    }
}

int LCD_nokia_map_value_to_pixel(float value, int min_value, int max_value)
{
    if (min_value == max_value) return SCREENH / 2;

    // Clamp the value within the min and max
    if (value < min_value) value = min_value;
    if (value > max_value) value = max_value;

    // Map the value to 0 - (SCREENH - 1)
    int pixel = (value - min_value) * (SCREENH - 1) / (max_value - min_value);
    // Invert the pixel value because Y=0 is at the top of the LCD
    return SCREENH - 1 - pixel;
}

void LCD_nokia_set_pixel_in_bitmap(uint8_t* bitmap, int x, int y)
{
    if (x < 0 || x >= SCREENW || y < 0 || y >= SCREENH)
        return;

    // Calculate the index in the bitmap array
    int byte_index = x + (y / 8) * SCREENW;
    int bit_position = y % 8;

    bitmap[byte_index] |= (1 << bit_position);
}
