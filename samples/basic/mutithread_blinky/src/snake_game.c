/**
 * @file snake_game.c
 * @brief Implementation of snake game functionality.
 */

#include "snake_game.h"
#include "shared_data.h"
#include <zephyr/kernel.h>
#include <LCD_nokia.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/* Game parameters */
#define GAME_AREA_TOP    (2 * CELL_SIZE)            /**< Offset for the top of the game area */
#define GRID_WIDTH       (16)                       /**< Number of cells horizontally */
#define GRID_HEIGHT      (7)                        /**< Number of cells vertically */
#define CELL_SIZE        (5)                        /**< Cell size in pixels */
#define MAX_SNAKE_LENGTH (GRID_WIDTH * GRID_HEIGHT) /**< Maximum snake length */

/* Direction enumeration */
typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} e_Direction;

/* Snake structure */
typedef struct {
    int x[MAX_SNAKE_LENGTH];
    int y[MAX_SNAKE_LENGTH];
    int length;
    e_Direction dir;
} s_Snake;

/* Food structure */
typedef struct {
    int x;
    int y;
} s_Food;

static s_Snake snake;
static s_Food food;
static bool game_over = false;
static int score = 0;

/* Function prototypes */
static void init_game(void);
static void place_food(void);
static void update_direction(void);
static void update_game(void);
static void render_game(void);

/* Snake game main loop */
void snake_game(void)
{
    init_game();

    while (!game_over) {
        update_direction();
        update_game();
        render_game();

        /* Adjust the game speed as needed */
        int sleepTime = 300 - 15 * score;
        if (sleepTime < 100) {
            sleepTime = 100;
        }
        k_msleep(sleepTime);

        if (atomic_get(&mode) != e_VisualizationMode_SnakeGame) {
            /* Exit the game loop if mode changes */
            break;
        }
    }

    LCD_nokia_clear();

    /* Write title */
    LCD_nokia_goto_xy(0, 0);
    LCD_nokia_send_string("Snake");

    /* Game over screen */
    LCD_nokia_goto_xy(0, 2);
    if (game_over) {
        LCD_nokia_send_string("Game Over");
    } else {
        LCD_nokia_send_string("Exit game");
    }
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "Score: %d", score);
    LCD_nokia_goto_xy(0, 3);
    LCD_nokia_send_string(buffer);
    k_msleep(1000);

    /* Wait for mode change to exit */
    while (1) {
        if (atomic_get(&mode) != e_VisualizationMode_SnakeGame) {
            break;
        }
        k_msleep(100);
    }
}

/* Initialize the snake game */
static void init_game(void)
{
    /* Initialize snake in the center */
    snake.length = 3;
    snake.dir = DIR_RIGHT;
    int start_x = GRID_WIDTH / 2;
    int start_y = GRID_HEIGHT / 2;
    for (int i = 0; i < snake.length; i++) {
        snake.x[i] = start_x - i;
        snake.y[i] = start_y;
    }

    /* Place the first food item */
    place_food();

    /* Reset game state */
    game_over = false;
    score = 0;
}

/* Place food at a random position */
static void place_food(void)
{
    bool valid_position = false;
    while (!valid_position) {
        food.x = rand() % GRID_WIDTH;
        food.y = rand() % GRID_HEIGHT;
        valid_position = true;
        /* Ensure food is not placed on the snake */
        for (int i = 0; i < snake.length; i++) {
            if (snake.x[i] == food.x && snake.y[i] == food.y) {
                valid_position = false;
                break;
            }
        }
    }
}

/* Update snake direction based on accelerometer data */
static void update_direction(void)
{
    /* Lock mutex to read accelerometer data */
    k_mutex_lock(&data_mutex, K_FOREVER);
    float acc_x_value = accel_x;
    float acc_y_value = accel_y;
    k_mutex_unlock(&data_mutex);

    /* Define thresholds */
    float threshold = 0.5f;

    /* Determine direction based on tilt */
    if (acc_x_value > threshold && snake.dir != DIR_LEFT) {
        snake.dir = DIR_RIGHT;
    } else if (acc_x_value < -threshold && snake.dir != DIR_RIGHT) {
        snake.dir = DIR_LEFT;
    } else if (acc_y_value > threshold && snake.dir != DIR_DOWN) {
        snake.dir = DIR_UP;
    } else if (acc_y_value < -threshold && snake.dir != DIR_UP) {
        snake.dir = DIR_DOWN;
    }
}

/* Update the game state */
static void update_game(void)
{
    /* Move snake body */
    for (int i = snake.length - 1; i > 0; i--) {
        snake.x[i] = snake.x[i - 1];
        snake.y[i] = snake.y[i - 1];
    }

    /* Move snake head */
    switch (snake.dir) {
    case DIR_UP:
        snake.y[0]--;
        break;
    case DIR_DOWN:
        snake.y[0]++;
        break;
    case DIR_LEFT:
        snake.x[0]--;
        break;
    case DIR_RIGHT:
        snake.x[0]++;
        break;
    }

    /* Wrap around edges */
    if (snake.x[0] < 0) {
        snake.x[0] = GRID_WIDTH - 1;
    }
    if (snake.x[0] >= GRID_WIDTH) {
        snake.x[0] = 0;
    }
    if (snake.y[0] < 0) {
        snake.y[0] = GRID_HEIGHT - 1;
    }
    if (snake.y[0] >= GRID_HEIGHT) {
        snake.y[0] = 0;
    }

    /* Check for collision with self */
    for (int i = 1; i < snake.length; i++) {
        if (snake.x[0] == snake.x[i] && snake.y[0] == snake.y[i]) {
            game_over = true;
            return;
        }
    }

    /* Check for food consumption */
    if (snake.x[0] == food.x && snake.y[0] == food.y) {
        if (snake.length < MAX_SNAKE_LENGTH) {
            snake.length++;
            snake.x[snake.length - 1] = snake.x[snake.length - 2];
            snake.y[snake.length - 1] = snake.y[snake.length - 2];
        }
        score++;
        place_food();
    }
}

/* Render the game on LCD */
static void render_game(void)
{
    /* Prepare the bitmap */
    memset(lcd_bitmap, 0, sizeof(lcd_bitmap));

    /* Draw the rectangle around the play area */
    LCD_nokia_draw_rectangle(lcd_bitmap, 0, GAME_AREA_TOP, GRID_WIDTH * CELL_SIZE,
                 GRID_HEIGHT * CELL_SIZE);

    /* Draw the snake */
    for (int i = 0; i < snake.length; i++) {
        int x_pixel = snake.x[i] * CELL_SIZE;
        int y_pixel = snake.y[i] * CELL_SIZE + GAME_AREA_TOP;
        LCD_nokia_draw_filled_rectangle(lcd_bitmap, x_pixel, y_pixel, CELL_SIZE, CELL_SIZE);
    }

    /* Draw the food */
    int food_x_pixel = food.x * CELL_SIZE;
    int food_y_pixel = food.y * CELL_SIZE + GAME_AREA_TOP;
    LCD_nokia_draw_rectangle(lcd_bitmap, food_x_pixel, food_y_pixel, CELL_SIZE, CELL_SIZE);

    /* Clear the display */
    LCD_nokia_clear();

    /* Update the display */
    LCD_nokia_bitmap(lcd_bitmap);

    /* Display the score */
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "Snake: %d", score);
    LCD_nokia_goto_xy(0, 0);
    LCD_nokia_send_string(buffer);
}
