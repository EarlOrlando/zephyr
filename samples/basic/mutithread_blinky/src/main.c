/**
 * @file main.c
 * @brief Main entry point for the multi-threaded sensor application.
 */

#include <zephyr/kernel.h>

#include "sensor_sampling.h"
#include "gesture_detection.h"
#include "led_control.h"
#include "lcd_display.h"
#include "terminal_logging.h"
#include "button.h"
#include "shared_data.h"

/* Stack size and thread priorities */
#define STACKSIZE                  (1024)
#define PRIORITY_SENSOR_SAMPLING   (4)
#define PRIORITY_GESTURE_DETECTION (5)
#define PRIORITY_LED_CONTROL       (6)
#define PRIORITY_TERMINAL_LOGGING  (7)
#define PRIORITY_LCD_DISPLAY       (8)
#define PRIORITY_BUTTON            (9)

/* Define threads */
K_THREAD_DEFINE(sensor_sampling_id, STACKSIZE, sensor_sampling_thread, NULL, NULL, NULL,
        PRIORITY_SENSOR_SAMPLING, 0, 0);
K_THREAD_DEFINE(gesture_detection_id, STACKSIZE, gesture_detection_thread, NULL, NULL, NULL,
        PRIORITY_GESTURE_DETECTION, 0, 0);
K_THREAD_DEFINE(led_control_id, STACKSIZE, led_control_thread, NULL, NULL, NULL,
        PRIORITY_LED_CONTROL, 0, 0);
K_THREAD_DEFINE(lcd_display_id, STACKSIZE, lcd_display_thread, NULL, NULL, NULL,
        PRIORITY_LCD_DISPLAY, 0, 0);
K_THREAD_DEFINE(terminal_logging_id, STACKSIZE, terminal_logging_thread, NULL, NULL, NULL,
        PRIORITY_TERMINAL_LOGGING, 0, 0);
K_THREAD_DEFINE(button_id, STACKSIZE, button_thread, NULL, NULL, NULL, PRIORITY_BUTTON, 0, 0);

/**
 * @brief Main function.
 *
 * Initializes shared data and starts the threads.
 *
 * @return int Always returns 0.
 */
int main(void)
{
    /* Initialize shared data and synchronization primitives */
    shared_data_init();

    return 0;
}
