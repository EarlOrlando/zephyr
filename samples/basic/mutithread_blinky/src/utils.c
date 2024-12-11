/**
 * @file utils.c
 * @brief Implementation of utility functions for calculations and drawing.
 */

#include "utils.h"
#include "shared_data.h"
#include <lcd_display.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

/**
 * @brief Calculates the pitch angle.
 *
 * @param acc_x X-axis acceleration
 * @param acc_y Y-axis acceleration
 * @param acc_z Z-axis acceleration
 * @return float Pitch angle in degrees
 */
float calculate_pitch(float acc_x, float acc_y, float acc_z)
{
    return atan2f(acc_y, sqrtf(acc_x * acc_x + acc_z * acc_z)) * (180.0f / M_PI);
}

/**
 * @brief Calculates the roll angle.
 *
 * @param acc_x X-axis acceleration
 * @param acc_y Y-axis acceleration
 * @param acc_z Z-axis acceleration
 * @return float Roll angle in degrees
 */
float calculate_roll(float acc_x, float acc_y, float acc_z)
{
    return atan2f(acc_x, acc_z) * (180.0f / M_PI);
}

/**
 * @brief Calculates the heading.
 *
 * @param mag_x X-axis magnetometer data
 * @param mag_y Y-axis magnetometer data
 * @return float Heading in degrees
 */
float calculate_heading(float mag_x, float mag_y)
{
    float heading = atan2f(mag_y, mag_x) * (180.0f / M_PI);
    if (heading < 0) {
        heading += 360.0f;
    }
    return heading;
}

/**
 * @brief Draws the compass on the LCD.
 *
 * @param heading Heading angle in degrees
 */
void draw_compass(float heading)
{
    /* Prepare the bitmap */
    memset(lcd_bitmap, 0, sizeof(lcd_bitmap));

    /* Draw the compass circle */
    int center_x = SCREENW / 2;
    int center_y = SCREENH / 2 + 3;
    int radius = 10;
    LCD_nokia_draw_circle(lcd_bitmap, center_x, center_y, radius);

    /* Calculate the needle endpoint */
    float radian_angle = heading * (M_PI / 180.0f);
    int needle_x = center_x + (int)(radius * sinf(radian_angle));
    int needle_y = center_y - (int)(radius * cosf(radian_angle));

    /* Draw the needle */
    LCD_nokia_draw_line(lcd_bitmap, center_x, center_y, needle_x, needle_y);

    /* Clear the display */
    LCD_nokia_clear();

    /* Update the display */
    LCD_nokia_bitmap(lcd_bitmap);

    /* Write title */
    LCD_nokia_goto_xy(0, 0);
    LCD_nokia_send_string("Compass");

    /* Mark directions */
    LCD_nokia_goto_xy(center_x - 3, 1);
    LCD_nokia_send_string("N");
    LCD_nokia_goto_xy(center_x + radius + 2, 3);
    LCD_nokia_send_string("E");
    LCD_nokia_goto_xy(center_x - 3, 5);
    LCD_nokia_send_string("S");
    LCD_nokia_goto_xy(center_x - radius - 8, 3);
    LCD_nokia_send_string("W");
}

/**
 * @brief Draws the level bubble on the LCD.
 *
 * @param pitch Pitch angle in degrees
 * @param roll Roll angle in degrees
 */
void draw_level_bubble(float pitch, float roll)
{
    /* Level rectangle dimensions */
    int rect_x = 10;
    int rect_y = 10;
    int rect_width = SCREENW - 20;
    int rect_height = SCREENH - 20;

    memset(lcd_bitmap, 0, sizeof(lcd_bitmap));

    /* Draw rectangle */
    LCD_nokia_draw_rectangle(lcd_bitmap, rect_x, rect_y, rect_width, rect_height);

    /* Calculate bubble position */
    int bubble_x = rect_x + rect_width / 2 - (int)(roll * (rect_width / 2) / 45);
    int bubble_y = rect_y + rect_height / 2 + (int)(pitch * (rect_height / 2) / 45);

    /* Constrain bubble within rectangle */
    if (bubble_x < rect_x) {
        bubble_x = rect_x;
    }
    if (bubble_x > rect_x + rect_width) {
        bubble_x = rect_x + rect_width;
    }
    if (bubble_y < rect_y) {
        bubble_y = rect_y;
    }
    if (bubble_y > rect_y + rect_height) {
        bubble_y = rect_y + rect_height;
    }

    /* Draw bubble */
    LCD_nokia_draw_circle(lcd_bitmap, bubble_x, bubble_y, 3);

    /* Clear the display */
    LCD_nokia_clear();

    /* Update the display */
    LCD_nokia_bitmap(lcd_bitmap);

    /* Write title */
    LCD_nokia_goto_xy(0, 0);
    LCD_nokia_send_string("Level bubble");

}

/**
 * @brief Draws the inclinometer on the LCD.
 *
 * @param pitch Pitch angle in degrees
 * @param roll Roll angle in degrees
 */
void draw_inclinometer(float pitch, float roll)
{
    int center_x = SCREENW / 2;
    int center_y = SCREENH / 2;

    memset(lcd_bitmap, 0, sizeof(lcd_bitmap));

    /* Draw reference lines */
    LCD_nokia_draw_line(lcd_bitmap, 0, center_y, SCREENW - 1, center_y);
    LCD_nokia_draw_line(lcd_bitmap, center_x, 0, center_x, SCREENH - 1);

    /* Draw indicator */
    int indicator_x = center_x + (int)(roll * SCREENW / 90);
    int indicator_y = center_y - (int)(pitch * SCREENH / 90);

    LCD_nokia_draw_circle(lcd_bitmap, indicator_x, indicator_y, 3);

    /* Clear the display */
    LCD_nokia_clear();

    /* Update the display */
    LCD_nokia_bitmap(lcd_bitmap);


    /* Write title */
    LCD_nokia_goto_xy(0, 0);
    LCD_nokia_send_string("Inclinometer");

    /* Display pitch and roll values */
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "P:%.1f", (double)pitch);
    LCD_nokia_goto_xy(0, 4);
    LCD_nokia_send_string(buffer);

    snprintf(buffer, sizeof(buffer), "R:%.1f", (double)roll);
    LCD_nokia_goto_xy(0, 5);
    LCD_nokia_send_string(buffer);
}

/**
 * @brief Displays numeric sensor data on the LCD.
 *
 * @param x X-axis data
 * @param y Y-axis data
 * @param z Z-axis data
 * @param temp Temperature data
 */
void display_numeric_data(float x, float y, float z, float temp)
{
    /* Clear the display */
    LCD_nokia_clear();

    char buffer[16];

    LCD_nokia_goto_xy(0, 1);
    snprintf(buffer, sizeof(buffer), "X:%.2f", (double)x);
    LCD_nokia_send_string(buffer);

    LCD_nokia_goto_xy(0, 2);
    snprintf(buffer, sizeof(buffer), "Y:%.2f", (double)y);
    LCD_nokia_send_string(buffer);

    LCD_nokia_goto_xy(0, 3);
    snprintf(buffer, sizeof(buffer), "Z:%.2f", (double)z);
    LCD_nokia_send_string(buffer);

    LCD_nokia_goto_xy(0, 5);
    snprintf(buffer, sizeof(buffer), "Temp:%.2fC", (double)temp);
    LCD_nokia_send_string(buffer);
}

/**
 * @brief Displays graphical sensor data on the LCD.
 *
 * @param data_x X-axis data array
 * @param data_y Y-axis data array
 * @param data_z Z-axis data array
 * @param min_value Minimum value for scaling
 * @param max_value Maximum value for scaling
 */
void display_graphical_data(float *data_x, float *data_y, float *data_z, float min_value,
                float max_value)
{
    /* Prepare the bitmap */
    memset(lcd_bitmap, 0, sizeof(lcd_bitmap));

    /* Draw axes */
    LCD_nokia_draw_line(lcd_bitmap, 7, 3 * SCREENH / 12, 9,
                3 * SCREENH / 12); /* Horizontal axis */
    LCD_nokia_draw_line(lcd_bitmap, 7, 7 * SCREENH / 12, 9,
                7 * SCREENH / 12); /* Horizontal axis */
    LCD_nokia_draw_line(lcd_bitmap, 7, 11 * SCREENH / 12, 9,
                11 * SCREENH / 12);                /* Horizontal axis */
    LCD_nokia_draw_line(lcd_bitmap, 8, 0, 8, SCREENH - 1); /* Vertical axis */

    /* Plot data */
    int prev_pixel_x = 11;
    int prev_pixel_y_x =
        LCD_nokia_map_value_to_pixel(data_x[0], min_value, max_value) - 3 * SCREENH / 12;
    int prev_pixel_y_y =
        LCD_nokia_map_value_to_pixel(data_y[0], min_value, max_value) + 1 * SCREENH / 12;
    int prev_pixel_y_z =
        LCD_nokia_map_value_to_pixel(data_z[0], min_value, max_value) + 5 * SCREENH / 12;

    for (int x_pos = 11; x_pos < SCREENW; x_pos++) {
        int pixel_x = x_pos;

        int pixel_y_x = LCD_nokia_map_value_to_pixel(data_x[x_pos], min_value, max_value) -
                3 * SCREENH / 12;
        int pixel_y_y = LCD_nokia_map_value_to_pixel(data_y[x_pos], min_value, max_value) +
                1 * SCREENH / 12;
        int pixel_y_z = LCD_nokia_map_value_to_pixel(data_z[x_pos], min_value, max_value) +
                5 * SCREENH / 12;

        /* Draw lines between previous and current points */
        LCD_nokia_draw_line(lcd_bitmap, prev_pixel_x, prev_pixel_y_x, pixel_x, pixel_y_x);
        LCD_nokia_draw_line(lcd_bitmap, prev_pixel_x, prev_pixel_y_y, pixel_x, pixel_y_y);
        LCD_nokia_draw_line(lcd_bitmap, prev_pixel_x, prev_pixel_y_z, pixel_x, pixel_y_z);

        prev_pixel_x = pixel_x;
        prev_pixel_y_x = pixel_y_x;
        prev_pixel_y_y = pixel_y_y;
        prev_pixel_y_z = pixel_y_z;
    }

    /* Clear the display */
    LCD_nokia_clear();

    /* Update the display */
    LCD_nokia_bitmap(lcd_bitmap);

    LCD_nokia_goto_xy(0, 1);
    LCD_nokia_send_char('X');
    LCD_nokia_goto_xy(0, 3);
    LCD_nokia_send_char('Y');
    LCD_nokia_goto_xy(0, 5);
    LCD_nokia_send_char('Z');
}
