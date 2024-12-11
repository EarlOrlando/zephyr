/**
 * @file sensor_sampling.c
 * @brief Implementation of sensor sampling functionality.
 */

#include "sensor_sampling.h"
#include "shared_data.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>
#include <string.h>

#define SLEEP_TIME_MS (50)

/**
 * @brief Sensor sampling thread function.
 *
 * Fetches data from sensors and updates shared variables.
 * Uses a semaphore to signal data availability and a mutex to protect shared data.
 */
void sensor_sampling_thread(void)
{
    struct sensor_value accel[3];
    struct sensor_value magn[3];
    struct sensor_value temp;
    const struct device *const dev = DEVICE_DT_GET_ONE(nxp_fxos8700);

    if (!device_is_ready(dev)) {
        printk("Device %s is not ready\n", dev->name);
        return;
    }

    while (1) {
        if (sensor_sample_fetch(dev)) {
            printk("Failed to fetch sensor data\n");
            return;
        }

        sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, accel);
        sensor_channel_get(dev, SENSOR_CHAN_MAGN_XYZ, magn);
#ifdef CONFIG_FXOS8700_TEMP
        sensor_channel_get(dev, SENSOR_CHAN_DIE_TEMP, &temp);
#endif

        /* Lock mutex to safely update shared sensor data */
        k_mutex_lock(&data_mutex, K_FOREVER);
        accel_x = sensor_value_to_double(&accel[0]);
        accel_y = sensor_value_to_double(&accel[1]);
        accel_z = sensor_value_to_double(&accel[2]);
        magn_x = sensor_value_to_double(&magn[0]);
        magn_y = sensor_value_to_double(&magn[1]);
        magn_z = sensor_value_to_double(&magn[2]);
#ifdef CONFIG_FXOS8700_TEMP
        temperature = sensor_value_to_double(&temp);
#endif

        /* Update data buffers for plotting */
        /* Shift data to the left */
        memmove(&accel_x_data[0], &accel_x_data[1], (SCREENW - 1) * sizeof(float));
        memmove(&accel_y_data[0], &accel_y_data[1], (SCREENW - 1) * sizeof(float));
        memmove(&accel_z_data[0], &accel_z_data[1], (SCREENW - 1) * sizeof(float));
        memmove(&magn_x_data[0], &magn_x_data[1], (SCREENW - 1) * sizeof(float));
        memmove(&magn_y_data[0], &magn_y_data[1], (SCREENW - 1) * sizeof(float));
        memmove(&magn_z_data[0], &magn_z_data[1], (SCREENW - 1) * sizeof(float));

        /* Add new data at the end */
        accel_x_data[SCREENW - 1] = accel_x;
        accel_y_data[SCREENW - 1] = accel_y;
        accel_z_data[SCREENW - 1] = accel_z;
        magn_x_data[SCREENW - 1] = magn_x;
        magn_y_data[SCREENW - 1] = magn_y;
        magn_z_data[SCREENW - 1] = magn_z;

        k_mutex_unlock(&data_mutex);

        /* Signal data readiness using a semaphore */
        k_sem_give(&data_ready_sem);

        /* Enqueue sensor data in the message queue */
        float sensor_data[] = {accel_x, accel_y, accel_z, magn_x, magn_y, magn_z};
        k_msgq_put(&sensor_msgq, &sensor_data, K_NO_WAIT);

        k_msleep(SLEEP_TIME_MS);
    }
}
