/**
 * @file button.c
 * @brief Implementation of button handling functionality.
 */

#include "button.h"
#include "shared_data.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/printk.h>

#define SW0_NODE DT_ALIAS(sw0) /**< SW0 is the mode toggle button */

#define SLEEP_TIME_MS (50)

/* Button configuration */
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});
static struct gpio_callback button_cb_data;

/* Function prototype */
static void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
static void button_debounced(struct k_work *work);

/* Workload delay to debounce button */
static K_WORK_DELAYABLE_DEFINE(button_rescheduled, button_debounced);

/**
 * @brief Button debounce callback.
 *
 * The actions to do after the button is debounced.
 *
 * @param work Pointer to the work structure
 */
static void button_debounced(struct k_work *work)
{
    ARG_UNUSED(work);

    /* If the button is pressed */
    if(gpio_pin_get_dt(&button)) {
        /* Read and update mode atomically */
        int current_mode = atomic_get(&mode);
        current_mode = (current_mode + 1) % e_VisualizationMode_MAX;
        atomic_set(&mode, current_mode);

        printk("Button pressed, mode changed to %d\n", current_mode);
    }
}

/**
 * @brief Button interrupt handler.
 *
 * Changes the visualization mode when the button is pressed.
 *
 * @param dev Pointer to the device structure
 * @param cb Pointer to the callback structure
 * @param pins Pins that triggered the interrupt
 */
static void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    k_work_reschedule(&button_rescheduled, K_MSEC(15));
}

/**
 * @brief Button configuration thread.
 *
 * Configures the button in a separate thread.
 */
void button_thread(void)
{
    int ret = 0;

    /* Configure the button */
    if (!device_is_ready(button.port)) {
        printk("Error: Button device %s is not ready\n", button.port->name);
        return;
    }

    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret != 0) {
        printk("Error %d: Failed to configure button port %s, pin %d\n", ret,
               button.port->name, button.pin);
        return;
    }

    ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        printk("Error %d: Failed to configure button interrupt on pin %d\n", ret,
               button.pin);
        return;
    }

    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);
    printk("Configured button at port %s, pin %d\n", button.port->name, button.pin);

    /* Keep the thread alive */
    while (1) {
        k_msleep(SLEEP_TIME_MS);
    }
}
