/**
 * @file shared_data.c
 * @brief Implementation of shared data and synchronization primitives.
 */

#include "shared_data.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>

/* Synchronization primitives */
struct k_sem data_ready_sem;
struct k_mutex data_mutex;
struct k_msgq sensor_msgq;
struct k_event gesture_event;

/* Sensor data */
volatile float accel_x = 0, accel_y = 0, accel_z = 0;
volatile float magn_x = 0, magn_y = 0, magn_z = 0;
volatile float temperature = 0.0;

/* Data buffers for plotting */
float accel_x_data[SCREENW] = {0};
float accel_y_data[SCREENW] = {0};
float accel_z_data[SCREENW] = {0};
float magn_x_data[SCREENW] = {0};
float magn_y_data[SCREENW] = {0};
float magn_z_data[SCREENW] = {0};

/* LCD bitmap buffer */
uint8_t lcd_bitmap[SCREENW * SCREENH / 8] = {0};

/* Atomic variable for visualization mode */
atomic_t mode;

/* Buffer for message queue */
static float sensor_msgq_buffer[10 * sizeof(float[6])];

/**
 * @brief Initialize shared data and synchronization primitives.
 */
void shared_data_init(void)
{
    k_sem_init(&data_ready_sem, 0, 1);
    k_mutex_init(&data_mutex);
    k_msgq_init(&sensor_msgq, (char *)sensor_msgq_buffer, sizeof(float[6]), 10);
    k_event_init(&gesture_event);
    atomic_set(&mode, e_VisualizationMode_SplashScreen);
}
