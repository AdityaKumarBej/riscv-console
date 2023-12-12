#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sprite.h"
#include "util.h"
#include "controller.h"
#include "background.h"

#define DISPLAY_WIDTH 512
#define DISPLAY_HEIGHT 288

// Timer constants
#define TIMER_INTERVAL 20000
#define MTIME_LOW (*((volatile uint32_t *)0x40000008))

volatile uint32_t *MODE_CTRL_REG = (volatile uint32_t *)(0x500F6780);

void displayBackground(uint32_t myColor, int myIndex);
void toggleBGColor();
int getRand(int high);
int checkCollision(int x1, int y1, int x2, int y2);

volatile int global = 42;
volatile int bg = 1;
volatile uint32_t controller_status = 0;
volatile uint32_t current_status = 0;
volatile uint32_t last_status = 0;
int CMD_INTRR = 0;
int CURRENT_CMD_INTRR;

int snake_speed = 2;
int growSnakeParam = 3;

int main()
{
    int snakeStatus = 1;
    int last_global = 42;
    int m_count = MTIME_LOW;

    displayBackground(0xFF66CCFF, 0); //skyblue
    
    uint32_t pinkPalette[256];
    for (int i = 0; i < 256; ++i) {
        pinkPalette[i] = 0x80FF0000;
    }

    uint16_t small_color_load = load_palette(SMALL, pinkPalette, 2);

    int16_t snake_position, food_position;

    sprite_t food;
    food.type = SMALL;
    food.y = getRand(DISPLAY_HEIGHT);
    food.x = getRand(DISPLAY_WIDTH);
    food.z = 0;
    food.palette = 2;

    uint16_t load_food = load_sprite(food);

    sprite_t snake;
    snake.type = SMALL;
    snake.y = 0;
    snake.x = 0;
    snake.palette = 2;

    uint16_t load_snake = load_sprite(snake);

    while(1) {
        global = GetTicks();
        CURRENT_CMD_INTRR = GetCmd();
        if (CURRENT_CMD_INTRR == 1){
            displayBackground(0xFFFFFFFF, 0);
            clear_cmd();
        }
        if(global != last_global) {
            controller_status = get_controller();
            if(controller_status == NONE){
                current_status = last_status;
            }
            else{
                current_status = controller_status;
            }
            switch (current_status) {
                case a:
                    snake.x = snake.x - snake_speed;
                    break;
                case w:
                    snake.y = snake.y - snake_speed;
                    break;
                case d:
                    snake.x = snake.x + snake_speed;
                    break;
                case x:
                    snake.y = snake.y + snake_speed;
                    break;
                default:
                    break;
            }
            if (snake.x > DISPLAY_WIDTH || snake.y > DISPLAY_HEIGHT || snake.x < 0 || snake.y <0){
                snake.x = 0;
                snake.y = 0;
                snake_speed = 2;
                growSnakeParam = 3;
            }
            snake_position = display_sprite(load_snake, snake.x, snake.y, snake.z);
            if (checkCollision(snake.x, snake.y, food.x, food.y)) {
                snake_speed = snake_speed + 1;
                food.x = getRand(DISPLAY_WIDTH);
                food.y = getRand(DISPLAY_HEIGHT);
                food_position = display_sprite(load_food, food.x, food.y, food.z);
            }
            last_global = global;
            last_status = current_status;
        }
        if (MTIME_LOW >= m_count + TIMER_INTERVAL ){
            toggleBGColor();
            m_count = MTIME_LOW;
        }
    }
    
    /* KEEP THIS HERE */
    //threadExit(); // Calling threadExit anywhere in the main will 'eject' the cartridge
    return 0;
}

void displayBackground(uint32_t myColor, int myIndex){

    uint32_t bluePalette[256];
    for (int i = 0; i < 256; ++i) {
        bluePalette[i] = myColor;
    }

    int8_t paletteLoadResult = load_background_palette(bluePalette, myIndex);

    if (paletteLoadResult == 0) {
        background_t myBackground;
        myBackground.type = PIXEL;
        myBackground.x = 0;
        myBackground.y = 0;
        myBackground.z = 0;
        myBackground.palette = myIndex;
        myBackground.data_index = myIndex;

        uint8_t backgroundID = load_background(myBackground);

    }
}

void toggleBGColor() {
    if(bg==1){
        displayBackground(0x00000000, 0);
        bg=2;
    }
    else if (bg==2){
        displayBackground(0xFF66CCFF, 0);
        bg=1;
    }
}

int getRand(int high){
    srand((unsigned int)MTIME_LOW);
    return rand() % high;
}

int checkCollision(int x1, int y1, int x2, int y2) {
    int size1 = 10;
    int size2 = 10;
    return (x1 < x2 + size2 && x1 + size1 > x2 && y1 < y2 + size2 && y1 + size1 > y2);
}

