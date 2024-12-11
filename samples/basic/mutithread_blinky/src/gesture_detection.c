/**
 * @file gesture_detection.c
 * @brief Implementation of gesture detection functionality.
 */

#include "gesture_detection.h"
#include "shared_data.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <math.h>

#define SLEEP_TIME_MS (200)

/**
 * @brief Gesture detection thread function.
 *
 * Detects gestures based on accelerometer data.
 * Waits for data availability using a semaphore and uses a mutex to read shared data.
 * Posts events when gestures are detected.
 */
void gesture_detection_thread(void)
{
    while (1) {
        /* Wait for data from sensor sampling thread */
        k_sem_take(&data_ready_sem, K_FOREVER);

        /* Lock mutex to safely read sensor data */
        k_mutex_lock(&data_mutex, K_FOREVER);
        float x = accel_x;
        float y = accel_y;
        float z = accel_z;
        k_mutex_unlock(&data_mutex);

        float magnitude = sqrtf(x * x + y * y + z * z);

        if (magnitude > SHAKE_THRESHOLD) {
            /* Post shake event */
            k_event_post(&gesture_event, SHAKE_EVENT);
        } else if (fabsf(x) > TILT_THRESHOLD || fabsf(y) > TILT_THRESHOLD) {
            /* Post tilt event */
            k_event_post(&gesture_event, TILT_EVENT);
        } else if (z < FLIP_THRESHOLD) {
            /* Post flip event */
            k_event_post(&gesture_event, FLIP_EVENT);
        }

        k_msleep(SLEEP_TIME_MS);
    }
}
