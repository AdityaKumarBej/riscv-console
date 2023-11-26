#include <stdint.h>
// #include <stdio.h>
#include "api.h"

volatile int global = 42;
volatile uint32_t videoToggle = 0;
volatile uint32_t controller_status = 0;
volatile uint32_t vidIntCtr = 0;

// FOR SNAKE
int dirs[128];

volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xFE800);

volatile uint32_t *SPRITE_PALLETE = (volatile uint32_t *)(0x500F3000);
volatile uint32_t *SPRITE_PALLETE2 = (volatile uint32_t *)(0x500F2000);
volatile uint32_t *LARGE_SPRITE_CONTROL = (volatile uint32_t *)(0x500F5B00);
volatile uint32_t *VIDEO_MODE = (volatile uint32_t *)(0x500F6780);
volatile uint32_t *LARGE_SPRITE_DATA = (volatile uint32_t *)(0x50090000);
volatile uint32_t *SMALL_SPRITE_CONTROL = (volatile uint32_t *)(0x500F6300);

volatile uint32_t *INTERRUPT_ENABLE = (volatile uint32_t *)(0x40000000);
volatile uint32_t *INTERRUPT_PENDING = (volatile uint32_t *)(0x40000004);

// Private vars
int SMALL_SIZE = 16;

uint32_t calcSmallSpriteControl(int32_t x, int32_t y, int32_t z, uint32_t w, uint32_t h, uint32_t p);
uint32_t addToTail(uint32_t tailIdx);
uint32_t spawnFood();
uint32_t moveSnake(uint32_t x, uint32_t y, uint32_t tailIdx);
uint32_t moveSegmentForwardSmall(uint32_t spriteIdx);
uint32_t startGame();
uint32_t clearSprites(uint32_t tailIdx);
uint32_t resetGame(uint32_t tailIdx);


int main() {
    VIDEO_MEMORY[80] = 'G';
    VIDEO_MEMORY[81] = 'A';
    VIDEO_MEMORY[82] = 'M';
    VIDEO_MEMORY[83] = 'E';
    VIDEO_MEMORY[84] = ' ';
    VIDEO_MEMORY[85] = 'O';
    VIDEO_MEMORY[86] = 'V';
    VIDEO_MEMORY[87] = 'E';
    VIDEO_MEMORY[88] = 'R';
    int last_global = 42;
    int videoToggle = 0;
    int tailIdx = 0; 

    // For testing move sprite
    int x = 64; int y = 64;

    // setSmallSpriteControl(2, calcSmallSpriteControl(220,220,4,16,16,0));
    
    *VIDEO_MODE = 1; // Turns on video mode
    startGame();
    
    
    // 0 left, 1 right, 2 up, 3 down
    int prevDir = 1;

    while (1) {
        global = getTicks();

        // *VIDEO_MODE = getVideoToggle() % 2;
        if(global != last_global){
            controller_status = getStatus();
            if(controller_status){
                if(controller_status & 0x1){
                    if (x > 0 && prevDir != 1) {
                        // x--;
                        prevDir = 0;
                    }
                }
                if(controller_status & 0x2){
                    if (y > 0 && prevDir != 3) {
                        // y--;
                        prevDir = 2;
                    }
                }
                if(controller_status & 0x4){
                    writeIntToScreen(rand());
                    
                    if (y < 224 && prevDir != 2) {
                        // y++;
                        prevDir = 3;
                    }
                }
                if(controller_status & 0x8){
                    if (x < 448 && prevDir != 0) {
                        // x++;
                        prevDir = 1;
                    }
                }
                
            }

            // check for dirs to move
            if (prevDir == 0) {
                // left
                x-=8;
                // moveLargeSprite(0, x, y);
                

            } else if (prevDir == 1) {
                //right
                x+=8;
                // moveLargeSprite(0, x, y);
                
            } else if (prevDir == 2) {
                //up
                y-=8;
                // y--;
                // moveLargeSprite(0, x, y);
                
            } else if (prevDir == 3) {
                //down
                y+=8;
                // moveLargeSprite(0, x, y);
            }
            // update head dir
            dirs[0] = prevDir;
            moveSnake(x, y, tailIdx);
            last_global = global;

            if (checkCollisionSmall(0, 127) == 1) {
                spawnFood();
                addToTail(tailIdx);
                tailIdx++;
            }
        }
                if (x <= 0 || x+16 >= 512) {
            // reset
            // resetGame(tailIdx);
            // int x = 64; int y = 64;
            // prevDir = 1;
            // tailIdx = 0;
            // *VIDEO_MODE = 0;
            clearSprites(tailIdx);
            tailIdx=0;
            break;
        }
        if (y <= 0 || y+16 >= 288) {
            // reset.. clear and start
            // resetGame(tailIdx);
            // int x = 64; int y = 64;
            // prevDir = 1;
            // tailIdx = 0;
            // *VIDEO_MODE = 0;
            clearSprites(tailIdx);
            tailIdx=0;
            break;
        }
    }
    return 0;
}

uint32_t startGame() {
    volatile uint32_t snakeColor = 0xFF77FFC8;
    volatile uint32_t foodColor = 0xFFFF0000;
    SPRITE_PALLETE[0] = snakeColor;
    SPRITE_PALLETE2[0] = foodColor;

    // For testing move sprite
    // seed_rand(getTicks());
    int x = 64; int y = 64;
    uint32_t headControl = calcSmallSpriteControl(x,y,4,16,16,0);
    setSmallSpriteControl(0, headControl);
    dirs[0] = 1;
    spawnFood();

    return 1;
}

uint32_t clearSprites(uint32_t tailIdx) {
    for (int i = 0; i <= tailIdx; i++) {
        setSmallSpriteControl(i, calcSmallSpriteControl(0,0,0,0,0,3));
    }
    return 1;
}

uint32_t resetGame(uint32_t tailIdx) {
    clearSprites(tailIdx);
    startGame();
    return 1;
}

uint32_t moveSmallSprite(uint32_t spriteIdx, uint32_t x, uint32_t y, uint32_t z, uint32_t pIdx) {
    uint32_t smallSpriteCtrl = calcSmallSpriteControl(x, y, z, SMALL_SIZE, SMALL_SIZE, pIdx);
    setSmallSpriteControl(0, smallSpriteCtrl);
}

uint32_t calcSmallSpriteControl(int32_t x, int32_t y, int32_t z, uint32_t w, uint32_t h, uint32_t p) {
    return ((z) | (h-1)<<25) | ((w-1)<<21) | ((y+16)<<12) | ((x+16)<<2) | p;
}

uint32_t addToTail(uint32_t tailIdx) {
    
    int xTail = ((SMALL_SPRITE_CONTROL[tailIdx] >> 2) & 0b1111111111);
    int yTail = ((SMALL_SPRITE_CONTROL[tailIdx] >> 12) & 0b111111111);

    if (dirs[tailIdx] == 0) {
        // left... add to the right
        xTail += 128;
        // SMALL_SPRITE_CONTROL[tailIdx + 1] = calcSmallSpriteControl(xTail+32,yTail, 4, 16, 16, 0);
    }
    if (dirs[tailIdx] == 1) {
        // right... add to the left
        xTail -= 128;
        // SMALL_SPRITE_CONTROL[tailIdx + 1] = calcSmallSpriteControl(xTail-16,yTail, 4, 16, 16, 0);
    }
    if (dirs[tailIdx] == 2) {
        // up... add below
        yTail += 128;
        // SMALL_SPRITE_CONTROL[tailIdx + 1] = calcSmallSpriteControl(xTail,yTail+32, 4, 16, 16, 0);
    } 
    if (dirs[tailIdx] == 3) {
        // down... ad above
        yTail -= 128;
        // SMALL_SPRITE_CONTROL[tailIdx + 1] = calcSmallSpriteControl(xTail,yTail-16, 4, 16, 16, 0);
    }
    SMALL_SPRITE_CONTROL[tailIdx + 1] = calcSmallSpriteControl(xTail,yTail, 4, 16, 16, 0);
    dirs[tailIdx+1] = dirs[tailIdx];
    return 1;
}

uint32_t spawnFood() {
    setSmallSpriteControl(127, calcSmallSpriteControl(rand()%496,rand()%272,5,16,16,1));
}

// make previous segments move by getting next segs x,y and replacing it with theirs... only headmoves

uint32_t moveSnake(uint32_t x, uint32_t y, uint32_t tailIdx) {
    for (int i = tailIdx; i > 0; i--) {
        moveSegmentForwardSmall(i);
    }

    SMALL_SPRITE_CONTROL[0] = calcSmallSpriteControl(x, y, 4, 16, 16, 0);
}

uint32_t moveSegmentForwardSmall(uint32_t spriteIdx) {
    int x = ((SMALL_SPRITE_CONTROL[spriteIdx] >> 2) & 0b1111111111) - 16;
    int y = ((SMALL_SPRITE_CONTROL[spriteIdx] >> 12) & 0b111111111) - 16;

    int xNext = ((SMALL_SPRITE_CONTROL[spriteIdx - 1] >> 2) & 0b1111111111) - 16;
    int yNext = ((SMALL_SPRITE_CONTROL[spriteIdx - 1] >> 12) & 0b111111111) - 16;

    if (x < xNext) {
        // next seg is to the right
        // direction is right
        dirs[spriteIdx] = 1; // right
        // x += 1;
    }
    if (x > xNext) {
        dirs[spriteIdx] = 0; // left
        // x -= 1;
    }
    if (y < yNext) {
        dirs[spriteIdx] = 3; // down
        // y +=1;

    } 
    if (y > yNext) {
        dirs[spriteIdx] = 2; // up
        // y-=1;

    }

    SMALL_SPRITE_CONTROL[spriteIdx] = calcSmallSpriteControl(xNext, yNext, 4, 16, 16, 0);

    return 1;
}