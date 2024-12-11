# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(multithread_blinky)

target_sources(app PRIVATE src/main.c)
zephyr_include_directories(src/LCD5110)
zephyr_include_directories(src)
zephyr_library_sources(src/button.c)
zephyr_library_sources(src/gesture_detection.c)
zephyr_library_sources(src/lcd_display.c)
zephyr_library_sources(src/led_control.c)
zephyr_library_sources(src/sensor_sampling.c)
zephyr_library_sources(src/shared_data.c)
zephyr_library_sources(src/snake_game.c)
zephyr_library_sources(src/terminal_logging.c)
zephyr_library_sources(src/utils.c)
zephyr_library_sources(src/LCD5110/LCD_nokia.c)
zephyr_library_sources(src/LCD5110/LCD_nokia_images.c)

