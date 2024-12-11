/**
 * @file terminal_logging.c
 * @brief Implementation of terminal logging functionality.
 */

#include "terminal_logging.h"
#include "shared_data.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define SLEEP_TIME_MS (200)

/**
 * @brief Terminal logging thread function.
 *
 * Outputs sensor data and other information to the terminal.
 * Consumes data from the message queue.
 */
void terminal_logging_thread(void)
{
    float sensor_data[6];

    while (1) {
        /* Wait for data from the message queue */
        if (k_msgq_get(&sensor_msgq, &sensor_data, K_FOREVER) == 0) {
            printk("Accel: X=%.2f, Y=%.2f, Z=%.2f | Mag: X=%.2f, Y=%.2f, Z=%.2f\n",
                   (double)sensor_data[0], (double)sensor_data[1],
                   (double)sensor_data[2], (double)sensor_data[3],
                   (double)sensor_data[4], (double)sensor_data[5]);
        }
        k_msleep(SLEEP_TIME_MS);
    }
}
