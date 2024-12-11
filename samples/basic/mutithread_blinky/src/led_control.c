/**
 * @file led_control.c
 * @brief Implementation of RGB LED control functionality.
 */

#include "led_control.h"
#include "shared_data.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/sys/printk.h>
#include <stdlib.h>

#define SLEEP_TIME_MS (200)
#define MAX_PERCENT   (100)

/* PWM specifications for RGB LED */
static const struct pwm_dt_spec red_pwm_led = PWM_DT_SPEC_GET(DT_ALIAS(red_pwm_led));
static const struct pwm_dt_spec green_pwm_led = PWM_DT_SPEC_GET(DT_ALIAS(green_pwm_led));
static const struct pwm_dt_spec blue_pwm_led = PWM_DT_SPEC_GET(DT_ALIAS(blue_pwm_led));

/* Function prototypes */
static int set_rgb_led(uint8_t red_pct, uint8_t green_pct, uint8_t blue_pct,
               uint8_t brightness_pct);

/**
 * @brief LED control thread function.
 *
 * Updates RGB LED based on gestures.
 * Waits for gesture events using an event and controls the RGB LED accordingly.
 */
void led_control_thread(void)
{
    printk("Initializing PWM-based RGB LED control\n");

    if (!pwm_is_ready_dt(&red_pwm_led) || !pwm_is_ready_dt(&green_pwm_led) ||
        !pwm_is_ready_dt(&blue_pwm_led)) {
        printk("Error: one or more PWM devices not ready\n");
        return;
    }

    uint32_t red = 100, green = 100, blue = 100, brightness = 50;

    while (1) {
        static bool led_on = true;
        /* Wait for gesture events */
        uint32_t events = k_event_wait(
            &gesture_event, SHAKE_EVENT | TILT_EVENT | FLIP_EVENT, false, K_FOREVER);

        if (events & SHAKE_EVENT) {
            printk("Shake detected!\n");
            red = rand() % 100;
            green = rand() % 100;
            blue = rand() % 100;
        }

        if (events & TILT_EVENT) {
            printk("Tilt detected!\n");
            brightness = (brightness + 10) % 100;
        }

        if (events & FLIP_EVENT) {
            printk("Flip detected!\n");
            led_on = !led_on;
        }

        /* Set the RGB LED */
        if (led_on) {
            set_rgb_led(red, green, blue, brightness);
        } else {
            set_rgb_led(0, 0, 0, 0); /* Turn off LED */
        }

        /* Clear the event flags */
        k_event_set(&gesture_event, 0);

        k_msleep(SLEEP_TIME_MS);
    }
}

/**
 * @brief Sets RGB LED colors and brightness.
 *
 * @param red_pct Red intensity percentage (0-100)
 * @param green_pct Green intensity percentage (0-100)
 * @param blue_pct Blue intensity percentage (0-100)
 * @param brightness_pct Overall brightness percentage (0-100)
 * @return int 0 on success, negative error code on failure
 */
static int set_rgb_led(uint8_t red_pct, uint8_t green_pct, uint8_t blue_pct, uint8_t brightness_pct)
{
    int ret;

    /* Ensure the percentage values are within 0-100% */
    if (red_pct > MAX_PERCENT || green_pct > MAX_PERCENT || blue_pct > MAX_PERCENT ||
        brightness_pct > MAX_PERCENT) {
        printk("Error: percentage values must be between 0 and 100\n");
        return -EINVAL;
    }

    /* Use 64-bit integers to avoid overflow */
    uint64_t pulse_red, pulse_green, pulse_blue;

    /* Compute pulse widths */
    pulse_red = ((uint64_t)red_pwm_led.period * red_pct * brightness_pct) /
            (MAX_PERCENT * MAX_PERCENT);
    pulse_green = ((uint64_t)green_pwm_led.period * green_pct * brightness_pct) /
              (MAX_PERCENT * MAX_PERCENT);
    pulse_blue = ((uint64_t)blue_pwm_led.period * blue_pct * brightness_pct) /
             (MAX_PERCENT * MAX_PERCENT);

    /* Set pulse widths */
    ret = pwm_set_pulse_dt(&red_pwm_led, (uint32_t)pulse_red);
    if (ret != 0) {
        printk("Error %d: red PWM write failed\n", ret);
        return ret;
    }

    ret = pwm_set_pulse_dt(&green_pwm_led, (uint32_t)pulse_green);
    if (ret != 0) {
        printk("Error %d: green PWM write failed\n", ret);
        return ret;
    }

    ret = pwm_set_pulse_dt(&blue_pwm_led, (uint32_t)pulse_blue);
    if (ret != 0) {
        printk("Error %d: blue PWM write failed\n", ret);
        return ret;
    }

    return 0;
}
