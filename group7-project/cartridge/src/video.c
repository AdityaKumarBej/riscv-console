#include "video.h"

volatile uint32_t *SMALL_SPRITE_CONTROLS[128];
volatile uint32_t *LARGE_SPRITE_CONTROLS[64];
volatile uint32_t *BACKGROUND_SPRITE_CONTROLS[5];
volatile uint32_t *MODE_CTRL_REG = (volatile uint32_t *)(0x500F6780);
volatile char *VIDEO_MEMORY = (volatile char *)(0x500F4800);


uint32_t generateSmallSpriteConfig(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t palette){
    return ((h-1)<<25) | ((w-1)<<21) | ((y+16)<<12) | ((x+16)<<2) | palette;
}

uint32_t generateLargeSpriteConfig(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t palette){
    return ((h-33)<<26) | ((w-33)<<21) | ((y+64)<<12) | ((x+64)<<2) | palette;
}

uint32_t generateBackgroundConfig(uint32_t x, uint32_t y, uint32_t z, uint32_t p){
    return ((z<<22) | ((y+288)<<12) | ((x+512)<<2)) | p;
}

void drawRectangleWithSmallSprite(int sprite_id, uint32_t addr){
    *SMALL_SPRITE_CONTROLS[sprite_id] = addr;
}

void drawRectangleWithLargeSprite(int sprite_id, uint32_t addr){
    *LARGE_SPRITE_CONTROLS[sprite_id] = addr;
}

void drawRectangleWithBackgroundSpriteControl(int sprite_id, uint32_t addr){
    *BACKGROUND_SPRITE_CONTROLS[sprite_id] = addr;
}

void moveSmallSprite(int sprite_id, uint32_t x, uint32_t y){
    *SMALL_SPRITE_CONTROLS[sprite_id] &= 0xFFE00003;
    *SMALL_SPRITE_CONTROLS[sprite_id] |= (((y+16)<<12) | ((x+16)<<2));
}

void moveLargeSprite(int sprite_id, uint32_t x, uint32_t y){
    *LARGE_SPRITE_CONTROLS[sprite_id] &= 0xFFE00003;
    *LARGE_SPRITE_CONTROLS[sprite_id] |= (((y+64)<<12) | ((x+64)<<2));
}

void switchToGraphicsMode(){
    *MODE_CTRL_REG |= 0x1;
}

void switchToTextMode(){
    *MODE_CTRL_REG &= 0x0;
}

void setColor(int palette_id, int entry_id, uint32_t rgba){
    volatile uint32_t *SPRITE_PALETTE = (volatile uint32_t *)(0x500F3000 + 1024 * palette_id);
    SPRITE_PALETTE[entry_id] = rgba;
}

void setBackgroundColor(int palette_id, int entry_id, uint32_t rgba){
    volatile uint32_t *SPRITE_PALETTE = (volatile uint32_t *)(0x500F0000 + 1024 * palette_id);
    SPRITE_PALETTE[entry_id] = rgba;
}

void initializeSpriteControllers(){
    for (int i = 0; i < 128; i++){
        SMALL_SPRITE_CONTROLS[i] = (volatile uint32_t *)(0x500F6300 + i * 4);
    }
    for (int i = 0; i < 64; i++){
        LARGE_SPRITE_CONTROLS[i] = (volatile uint32_t *)(0x500F5B00 + i * 4);
    }
    for (int i = 0; i < 5; i++){
        BACKGROUND_SPRITE_CONTROLS[i] = (volatile uint32_t *)(0x500F5A00 + i * 4);
    }
}

uint32_t getSmallSpriteControl(int sprite_id){
    return *SMALL_SPRITE_CONTROLS[sprite_id];
}

uint32_t getLargeSpriteControl(int sprite_id){
    return *LARGE_SPRITE_CONTROLS[sprite_id];
}

uint32_t getBackgroundSpriteControl(int sprite_id){
    return *BACKGROUND_SPRITE_CONTROLS[sprite_id];
}

void printText(char* string){
    int i;

    for( i = 0; string[i] != '\0'; i++){
        VIDEO_MEMORY[i] = string[i];
    }
    for(int j = i ; j < 2304 ; j++){
        VIDEO_MEMORY[j] = ' ';
    }
}