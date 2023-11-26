#include "game.h"
#include "api.h"
#include "display.h"

volatile uint8_t *small_sprite_data = (volatile uint8_t *)(0x500E0000);

int checkAlive(int cur_x, int cur_y, int budget){
    int alive = 1;
    int x, y;
    uint32_t sprite_control;
    if (cur_x != 0){
        for (int i = 1; i < budget; i++){
            sprite_control = getSmallSpriteControl(i);
            x = ((sprite_control >> 2) & 0x3FF) - SMALL_SPRITE_OFFSET;
            y = ((sprite_control >> 12) & 0x1FF) - SMALL_SPRITE_OFFSET;
            if (x == cur_x & y == cur_y){
                alive = 0;
                break;
            }
        }
    }
    return alive;
}

int checkGetPellet(int cur_x, int cur_y, int center_x, int center_y, int budget){
    return (cur_x < center_x + 10) & (cur_y < center_y + 10) & (cur_x > center_x - 10) & (cur_y > center_y - 10) & (budget <= 129);
}

void drawPellet(){
    for(int y = 0; y < 16; y++){
        for(int x = 0; x < 16; x++){
            small_sprite_data[(y<<4) + x] = ((x >= 3) & (x <= 5) & (y >= 0) & (y <= 8)) | ((x >= 2) & (x <= 6) & (y >= 3) & (y <= 5)) ? 1 : 2;
        }
    }
}

void gameOver(){
    switchToTextMode();
    printText("GAME OVER!");
}