/**
 * @file lcd_display.c
 * @brief Implementation of LCD display functionality.
 */

#include "lcd_display.h"
#include "shared_data.h"
#include "snake_game.h"
#include "utils.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>
#include <LCD_nokia.h>

#define SLEEP_TIME_MS (100)

/* External bitmap for splash screen */
extern const uint8_t ITESO[];

/**
 * @brief LCD display thread function.
 *
 * Updates the LCD display based on the current visualization mode.
 * Uses a mutex to access shared data and an atomic variable to read the current mode.
 */
void lcd_display_thread(void)
{
    /* Initialize the LCD */
    LCD_nokia_init();
    LCD_nokia_clear();

    while (1) {
        /* Read the current mode */
        int current_mode = atomic_get(&mode);

        switch (current_mode) {
        case e_VisualizationMode_SplashScreen: {
            LCD_nokia_clear();
            LCD_nokia_bitmap(ITESO);
            while (atomic_get(&mode) == e_VisualizationMode_SplashScreen) {
                k_msleep(50);
            }
            break;
        }
        case e_VisualizationMode_AccData: {
            /* Lock mutex to read sensor data */
            k_mutex_lock(&data_mutex, K_FOREVER);
            float x = accel_x;
            float y = accel_y;
            float z = accel_z;
            float temp = temperature;
            k_mutex_unlock(&data_mutex);
            display_numeric_data(x, y, z, temp);

            /* Display labels */
            LCD_nokia_goto_xy(0, 0);
            LCD_nokia_send_string("Acceleration");
            break;
        }

        case e_VisualizationMode_MagData: {
            /* Lock mutex to read sensor data */
            k_mutex_lock(&data_mutex, K_FOREVER);
            float x = magn_x;
            float y = magn_y;
            float z = magn_z;
            float temp = temperature;
            k_mutex_unlock(&data_mutex);
            display_numeric_data(x, y, z, temp);

            /* Display labels */
            LCD_nokia_goto_xy(0, 0);
            LCD_nokia_send_string("Magnetometer");
            break;
        }

        case e_VisualizationMode_AccPlot: {
            /* Lock mutex to read data buffers */
            k_mutex_lock(&data_mutex, K_FOREVER);
            float *data_x = accel_x_data;
            float *data_y = accel_y_data;
            float *data_z = accel_z_data;
            k_mutex_unlock(&data_mutex);
            display_graphical_data(data_x, data_y, data_z, -40.0f, 40.0f);

            /* Display labels */
            LCD_nokia_goto_xy(0, 0);
            LCD_nokia_send_string("Acceleration");
            break;
        }

        case e_VisualizationMode_MagPlot: {
            /* Lock mutex to read data buffers */
            k_mutex_lock(&data_mutex, K_FOREVER);
            float *data_x = magn_x_data;
            float *data_y = magn_y_data;
            float *data_z = magn_z_data;
            k_mutex_unlock(&data_mutex);
            display_graphical_data(data_x, data_y, data_z, -5.0f, 5.0f);

            /* Display labels */
            LCD_nokia_goto_xy(0, 0);
            LCD_nokia_send_string("Magnetometer");
            break;
        }

        case e_VisualizationMode_Compass: {
            /* Lock mutex to read sensor data */
            k_mutex_lock(&data_mutex, K_FOREVER);
            float mag_x_value = magn_x;
            float mag_y_value = magn_y;
            k_mutex_unlock(&data_mutex);
            float heading = calculate_heading(mag_x_value, mag_y_value);
            draw_compass(heading);
            break;
        }

        case e_VisualizationMode_LevelBubble: {
            /* Lock mutex to read accelerometer data */
            k_mutex_lock(&data_mutex, K_FOREVER);
            float acc_x_value = accel_x;
            float acc_y_value = accel_y;
            float acc_z_value = accel_z;
            k_mutex_unlock(&data_mutex);
            float pitch = calculate_pitch(acc_x_value, acc_y_value, acc_z_value);
            float roll = calculate_roll(acc_x_value, acc_y_value, acc_z_value);
            draw_level_bubble(pitch, roll);
            break;
        }

        case e_VisualizationMode_TiltIndicator: {
            /* Lock mutex to read accelerometer data */
            k_mutex_lock(&data_mutex, K_FOREVER);
            float acc_x_value = accel_x;
            float acc_y_value = accel_y;
            float acc_z_value = accel_z;
            k_mutex_unlock(&data_mutex);
            float pitch = calculate_pitch(acc_x_value, acc_y_value, acc_z_value);
            float roll = calculate_roll(acc_x_value, acc_y_value, acc_z_value);
            draw_inclinometer(pitch, roll);
            break;
        }

        case e_VisualizationMode_SnakeGame:
            snake_game();
            break;

        default:
            LCD_nokia_clear();
            LCD_nokia_goto_xy(0, 0);
            LCD_nokia_send_string("Invalid Mode");
            break;
        }

        k_msleep(SLEEP_TIME_MS);
    }
}
