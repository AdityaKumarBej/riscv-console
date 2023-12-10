#include "game.h"
#include "api.h"
#include "display.h"

volatile uint8_t *SMALL_SPRITE = (volatile uint8_t *)(0x500E0000);
// volatile char *VIDEO_MEMORY = (volatile char *)(0x500F4800);


int getSnakeStatus(int currXPos, int currYPos, int growSnakeParam){
    int alive = 1;
    int x, y;
    uint32_t sprite_control;
    if (currXPos != 0){
        for (int i = 1; i < growSnakeParam; i++){
            sprite_control = getSmallSpriteControl(i);
            x = ((sprite_control >> 2) & 0x3FF) - SMALL_SPRITE_OFFSET;
            y = ((sprite_control >> 12) & 0x1FF) - SMALL_SPRITE_OFFSET;
            if (x == currXPos & y == currYPos){
                alive = 0;
                break;
            }
        }
    }
    return alive;
}

int getFoodStatus(int currXPos, int currYPos, int centerXPos, int centerYPos, int growSnakeParam){
    return (currXPos < centerXPos + 10) & (currYPos < centerYPos + 10) & (currXPos > centerXPos - 10) & (currYPos > centerYPos - 10) & (growSnakeParam <= 100);
}

void generateFoodBlock(){
    for(int y = 0; y < 16; y++){
        for(int x = 0; x < 16; x++){
            SMALL_SPRITE[(y<<4) + x] = ((x >= 3) & (x <= 5) & (y >= 0) & (y <= 8)) | ((x >= 2) & (x <= 6) & (y >= 3) & (y <= 5)) ? 1 : 2;
        }
    }
}

void endGame(){
    switchToTextMode();
    printText("GAME OVER! HIT RST TO RESTART");
}