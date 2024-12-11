/**
 * @file utils.h
 * @brief Header file for utility functions.
 */

#ifndef UTILS_H
#define UTILS_H

float calculate_pitch(float acc_x, float acc_y, float acc_z);
float calculate_roll(float acc_x, float acc_y, float acc_z);
float calculate_heading(float mag_x, float mag_y);
void draw_compass(float heading);
void draw_level_bubble(float pitch, float roll);
void draw_inclinometer(float pitch, float roll);
void display_numeric_data(float x, float y, float z, float temp);
void display_graphical_data(float *data_x, float *data_y, float *data_z, float min_value,
                float max_value);

#define M_PI ((float)3.14159265358979323846)

#endif /* UTILS_H */
