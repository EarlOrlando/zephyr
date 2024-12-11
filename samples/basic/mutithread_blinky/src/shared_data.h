/**
 * @file shared_data.h
 * @brief Shared data and synchronization primitives used across modules.
 */

#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>
#include "LCD_nokia.h"

/* Synchronization primitives */
extern struct k_sem data_ready_sem;
extern struct k_mutex data_mutex;
extern struct k_msgq sensor_msgq;
extern struct k_event gesture_event;

/* Sensor data */
extern volatile float accel_x, accel_y, accel_z;
extern volatile float magn_x, magn_y, magn_z;
extern volatile float temperature;

/* Data buffers for plotting */
extern float accel_x_data[];
extern float accel_y_data[];
extern float accel_z_data[];
extern float magn_x_data[];
extern float magn_y_data[];
extern float magn_z_data[];

/* LCD bitmap buffer */
extern uint8_t lcd_bitmap[SCREENW * SCREENH / 8];

/* Visualization modes */
typedef enum {
    e_VisualizationMode_SplashScreen = 0,
    e_VisualizationMode_AccData,
    e_VisualizationMode_AccPlot,
    e_VisualizationMode_MagData,
    e_VisualizationMode_MagPlot,
    e_VisualizationMode_Compass,
    e_VisualizationMode_LevelBubble,
    e_VisualizationMode_TiltIndicator,
    e_VisualizationMode_SnakeGame,
    e_VisualizationMode_MAX
} e_VisualizationMode;

/** @brief Atomic variable to store the current visualization mode. */
extern atomic_t mode;

/* Gesture thresholds */
#define SHAKE_THRESHOLD (12.0f)
#define TILT_THRESHOLD  (2.0f)
#define FLIP_THRESHOLD  (-9.0f)

/* Gesture event bits */
#define SHAKE_EVENT BIT(0)
#define TILT_EVENT  BIT(1)
#define FLIP_EVENT  BIT(2)

/* Function to initialize shared data */
void shared_data_init(void);

#endif /* SHARED_DATA_H */
