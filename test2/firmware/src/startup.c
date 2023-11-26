#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
// #include <stdio.h>

extern uint8_t _erodata[];
extern uint8_t _data[];
extern uint8_t _edata[];
extern uint8_t _sdata[];
extern uint8_t _esdata[];
extern uint8_t _bss[];
extern uint8_t _ebss[];

// Adapted from https://stackoverflow.com/questions/58947716/how-to-interact-with-risc-v-csrs-by-using-gcc-c-code
__attribute__((always_inline)) inline uint32_t csr_mstatus_read(void){
    uint32_t result;
    asm volatile ("csrr %0, mstatus" : "=r"(result));
    return result;
}

__attribute__((always_inline)) inline void csr_mstatus_write(uint32_t val){
    asm volatile ("csrw mstatus, %0" : : "r"(val));
}

__attribute__((always_inline)) inline void csr_write_mie(uint32_t val){
    asm volatile ("csrw mie, %0" : : "r"(val));
}

__attribute__((always_inline)) inline void csr_enable_interrupts(void){
    asm volatile ("csrsi mstatus, 0x8");
}

__attribute__((always_inline)) inline void csr_disable_interrupts(void){
    asm volatile ("csrci mstatus, 0x8");
}

#define MTIME_LOW       (*((volatile uint32_t *)0x40000008))
#define MTIME_HIGH      (*((volatile uint32_t *)0x4000000C))
#define MTIMECMP_LOW    (*((volatile uint32_t *)0x40000010))
#define MTIMECMP_HIGH   (*((volatile uint32_t *)0x40000014))
#define CONTROLLER      (*((volatile uint32_t *)0x40000018))


// MEMORY LOCATION VARIABLES
volatile uint32_t *INTERRUPT_ENABLE = (volatile uint32_t *)(0x40000000);
volatile uint32_t *INTERRUPT_PENDING = (volatile uint32_t *)(0x40000004);
volatile uint32_t *SPRITE_PALLETE = (volatile uint32_t *)(0x500F3000);
volatile uint32_t *PALLETES = (volatile uint32_t *)(0x500F0000);
volatile uint32_t *VIDEO_MODE = (volatile uint32_t *)(0x500F6780);

volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xF4800);

volatile uint32_t *LARGE_SPRITE_CONTROL = (volatile uint32_t *)(0x500F5B00);
volatile uint32_t *LARGE_SPRITE_DATA = (volatile uint32_t *)(0x50090000);

volatile uint32_t *SMALL_SPRITE_CONTROL = (volatile uint32_t *)(0x500F6300);
volatile uint32_t *SMALL_SPRITE_DATA = (volatile uint32_t *)(0x500E0000);

volatile uint32_t *BACKGROUND_CONTROLS = (volatile uint32_t *)(0x500F5A00);
volatile uint32_t *BACKGROUND_DATA = (volatile uint32_t *)(0x50000000);

// API function declarations
uint32_t calcLargeSpriteControl(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t p);
uint32_t writeVidIntCtrToScreen();
uint32_t moveLargeSprite(uint32_t spriteIdx, int32_t x, int32_t y);
uint32_t deleteLargeSprite(uint32_t spriteIdx);
uint32_t writeCharToTextPos(uint32_t pos, char c);
uint32_t setLargeSpriteControl(uint32_t spriteIdx, uint32_t largeSpriteCtrl);
uint32_t setSmallSpriteControl(uint32_t spriteIdx, uint32_t smallSpriteCtrl);
uint32_t moveSmallSprite(uint32_t spriteIdx, int32_t x, int32_t y, int32_t z);
uint32_t deleteSmallSprite(uint32_t spriteIdx);
uint32_t getLargeSpriteControl(uint32_t spriteIdx);
uint32_t getSmallSpriteControl(uint32_t spriteIdx);

uint32_t getNearbySprites(uint32_t spriteIdx);

uint32_t setBackgroundPalette(uint32_t backgroundIdx, uint32_t backgroundControl);

uint32_t setBackgroundControl(uint32_t backgroundIdx, uint32_t backgroundControl);

uint32_t modifySpritePalette(uint32_t spriteIdx, uint32_t paletteIdx);

uint32_t setPalette(uint32_t paletteIdx, uint32_t color);

uint32_t checkCollisionLarge(uint32_t spriteIdx1, uint32_t spriteIdx2);

uint32_t checkCollisionSmall(uint32_t spriteIdx1, uint32_t spriteIdx2);

uint32_t checkCollisionLargeSmall(uint32_t largeIdx, uint32_t smallIdx);



// Helpers
uint32_t writeIntToScreen(int num);

void init(void){
    uint8_t *Source = _erodata;
    uint8_t *Base = _data < _sdata ? _data : _sdata;
    uint8_t *End = _edata > _esdata ? _edata : _esdata;

    while(Base < End){
        *Base++ = *Source++;
    }
    Base = _bss;
    End = _ebss;
    while(Base < End){
        *Base++ = 0;
    }

    csr_write_mie(0x888);       // Enable all interrupt soruces
    csr_enable_interrupts();    // Global interrupt enable
    *INTERRUPT_ENABLE = 6;
    MTIMECMP_LOW = 1;
    MTIMECMP_HIGH = 0;
}

extern volatile int global;
extern volatile uint32_t videoToggle;
extern volatile uint32_t controller_status;
extern volatile uint32_t vidIntCtr;

void c_interrupt_handler(uint32_t mcause){
    uint64_t NewCompare = (((uint64_t)MTIMECMP_HIGH)<<32) | MTIMECMP_LOW;
    NewCompare += 100;
    MTIMECMP_HIGH = NewCompare>>32;
    MTIMECMP_LOW = NewCompare;
    global++;
    controller_status = CONTROLLER;

    if ((mcause == 0x8000000B)) {
        // For machine external interrupts
        // Command interrupt
        if (((*INTERRUPT_ENABLE >> 2) & 1) && ((*INTERRUPT_PENDING >> 2) & 1)) {
            videoToggle++;
            *INTERRUPT_PENDING = 4;
        }
        // Video Interrupt
        if (((*INTERRUPT_ENABLE >> 1) & 1) && ((*INTERRUPT_PENDING >> 1) & 1)) {
            vidIntCtr++;
            *INTERRUPT_PENDING = 2;
        }
    }

}

uint32_t c_system_call(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t call){
    if(call == 0){
        return global;
    }
    else if(call == 1){
        return CONTROLLER;
    }
    else if(call == 2){
        return videoToggle;
    }
    else if(call == 3){
        return vidIntCtr;
    }
    else if(call == 4){
        uint32_t largeSpriteCtrl = calcLargeSpriteControl(a0, a1, a2, a3, a4);
        return largeSpriteCtrl;
    }
    else if(call == 5){
        return writeVidIntCtrToScreen();
    }
    else if(call == 6){
        // For sleep
        for (int i =0; i < 10; i++) {
            // do nothing
        }
        return 1;
    }
    else if(call == 7){
        // Returns a pseudo-random integer between 0 and RAND_MAX.
        return rand();
    }
    else if(call == 8){
        srand(a0);
        return 1;
    }
    else if(call == 9){
        return moveLargeSprite(a0, a1, a2);
    }
    else if(call == 10){
        return deleteLargeSprite(a0);
    }
    else if(call == 11){
        return writeCharToTextPos(a0, a1);
    }
    else if(call == 12){
        return writeIntToScreen(a0);
    }
    else if(call==13){
        return setLargeSpriteControl(a0, a1);
    }
    else if(call==14){
        return setSmallSpriteControl(a0, a1);
    }
    else if(call==15){
        return deleteSmallSprite(a0);
    }
    else if(call==16){
        return getLargeSpriteControl(a0);
    }
    else if(call==17){
        return getSmallSpriteControl(a0);
    }
    else if(call==18){
        //return getNearbySprites(a0);
        return 0;
    }
    else if(call==19){
        // return setBackgroundPalette(a0, a1);
        return 0;
    }
    else if(call==20){
        return setBackgroundControl(a0, a1);
    }
    else if(call==21){
        // return modifySpritePalette(a0, a1);
        return 0;
    }
    else if(call==22){
        // return setPalette(a0, a1);
        return 0;
    }
    else if(call==23){
        return checkCollisionLarge(a0, a1);
    }
    else if(call==24){
        return checkCollisionSmall(a0, a1);
    }else if(call==25){
        return checkCollisionLargeSmall(a0, a1);
    }
    return 0;
}

/* ========================= API & HELPER FUNCTIONS IMPLEMENTATION BELOW ========================= */

uint32_t calcLargeSpriteControl(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t p) {
    return ((h-33)<<26) | ((w-33)<<21) | ((y+64)<<12) | ((x+64)<<2) | p;
}

uint32_t writeVidIntCtrToScreen() {
    int pos = 17;
    VIDEO_MEMORY[pos] = vidIntCtr;
    int ctr = vidIntCtr;
    while(ctr) {
        VIDEO_MEMORY[pos] = (ctr%10) + '0';
        ctr /= 10;
        pos--;
    }
    return 1;
}

// large always on z plane 4
uint32_t moveLargeSprite(uint32_t spriteIdx, int32_t x, int32_t y) {
    // need to get height, width and palette
    uint32_t start = LARGE_SPRITE_CONTROL[spriteIdx] >> 22;
    uint32_t p = LARGE_SPRITE_CONTROL[spriteIdx] & 3;
    LARGE_SPRITE_CONTROL[spriteIdx] = (start<<22) | ((y+64)<<12) | ((x+64)<<2) | p;
    return 1;
}

// We can handle how we delete however we want... could just move off screen
uint32_t deleteLargeSprite(uint32_t spriteIdx) {
    LARGE_SPRITE_CONTROL[spriteIdx] = 0x0;
    return 1;
}

uint32_t writeCharToTextPos(uint32_t pos, char c) {
    VIDEO_MEMORY[pos] = c;
    return 1;
}

uint32_t writeIntToScreen(int num) {
    int pos = 17;
    VIDEO_MEMORY[pos] = num;
    int ctr = num;
    while(ctr) {
        VIDEO_MEMORY[pos] = (ctr%10) + '0';
        ctr /= 10;
        pos--;
    }
    return 1;
}

uint32_t setLargeSpriteControl(uint32_t spriteIdx, uint32_t largeSpriteCtrl) {
    LARGE_SPRITE_CONTROL[spriteIdx] = largeSpriteCtrl;
    return 1;
}

uint32_t setSmallSpriteControl(uint32_t spriteIdx, uint32_t smallSpriteCtrl) {
    SMALL_SPRITE_CONTROL[spriteIdx] = smallSpriteCtrl;
    return 1;
}

uint32_t deleteSmallSprite(uint32_t spriteIdx) {
    SMALL_SPRITE_CONTROL[spriteIdx] = 0x0;
    return 1;
}

// Helper function
uint32_t calcSmallSpriteControl(int32_t x, int32_t y, int32_t z, uint32_t w, uint32_t h, uint32_t p) {
    return ((z) | (h-1)<<25 | ((w-1)<<21) | ((y+16)<<12) | ((x+16)<<2) | p);
}

uint32_t getLargeSpriteControl(uint32_t spriteIdx) {
    return LARGE_SPRITE_CONTROL[spriteIdx];
}

uint32_t getSmallSpriteControl(uint32_t spriteIdx) {
    return SMALL_SPRITE_CONTROL[spriteIdx];
}

uint32_t getNearbySprites(uint32_t spriteIdx) {
    // returns list of spriteIdx of sprites that are currently in contact with the input
    // input is sprite index... if >63 its a small sprite
    return 0;

}

uint32_t checkCollisionLarge(uint32_t spriteIdx1, uint32_t spriteIdx2) { 
    int x1 = ((LARGE_SPRITE_CONTROL[spriteIdx1] >> 2) & 0b1111111111) - 64;
    int y1 = ((LARGE_SPRITE_CONTROL[spriteIdx1] >> 12) & 0b111111111) - 64;
    int h1 = ((LARGE_SPRITE_CONTROL[spriteIdx1] >> 26) & 0b11111) + 33;
    int w1 = ((LARGE_SPRITE_CONTROL[spriteIdx1] >> 21) & 0b11111) + 33;

    int x2 = ((LARGE_SPRITE_CONTROL[spriteIdx2] >> 2) & 0b1111111111) - 64;
    int y2 = ((LARGE_SPRITE_CONTROL[spriteIdx2] >> 12) & 0b111111111) - 64;
    int h2 = ((LARGE_SPRITE_CONTROL[spriteIdx2] >> 26) & 0b11111) + 33;
    int w2 = ((LARGE_SPRITE_CONTROL[spriteIdx2] >> 21) & 0b11111) + 33;


    // if between (X and X+W) && (Y and Y+H)
    if ( ( (x1 >= x2) && (x1 <= x2+w2) ) || ((x1+w1 >= x2) && (x1+w1 <= x2+w2) ) ) {
        // check Y
        if ( ( (y1 >= y2) && (y1 <= y2+h2) ) || ((y1+h1 >= y2) && (y1+h1 <= y2+h2) ) ) {
            return 1;
        }
    } 
    return 0;

}

uint32_t checkCollisionSmall(uint32_t spriteIdx1, uint32_t spriteIdx2) { 
    int x1 = ((SMALL_SPRITE_CONTROL[spriteIdx1] >> 2) & 0b1111111111) - 16;
    int y1 = ((SMALL_SPRITE_CONTROL[spriteIdx1] >> 12) & 0b111111111) - 16;
    int h1 = ((SMALL_SPRITE_CONTROL[spriteIdx1] >> 25) & 0b1111) + 1;
    int w1 = ((SMALL_SPRITE_CONTROL[spriteIdx1] >> 21) & 0b1111) + 1;

    int x2 = ((SMALL_SPRITE_CONTROL[spriteIdx2] >> 2) & 0b1111111111) - 16;
    int y2 = ((SMALL_SPRITE_CONTROL[spriteIdx2] >> 12) & 0b111111111) - 16;
    int h2 = ((SMALL_SPRITE_CONTROL[spriteIdx2] >> 25) & 0b1111) + 1;
    int w2 = ((SMALL_SPRITE_CONTROL[spriteIdx2] >> 21) & 0b1111) + 1;


    // if between (X and X+W) && (Y and Y+H)
    if ( ( (x1 >= x2) && (x1 <= x2+w2) ) || ((x1+w1 >= x2) && (x1+w1 <= x2+w2) ) ) {
        // check Y
        if ( ( (y1 >= y2) && (y1 <= y2+h2) ) || ((y1+h1 >= y2) && (y1+h1 <= y2+h2) ) ) {
            return 1;
        }
    } 
    return 0;

}

uint32_t checkCollisionLargeSmall(uint32_t largeIdx, uint32_t smallIdx) { 
    int x1 = ((LARGE_SPRITE_CONTROL[largeIdx] >> 2) & 0b1111111111) - 64;
    int y1 = ((LARGE_SPRITE_CONTROL[largeIdx] >> 12) & 0b111111111) - 64;
    int h1 = ((LARGE_SPRITE_CONTROL[largeIdx] >> 26) & 0b11111) + 33;
    int w1 = ((LARGE_SPRITE_CONTROL[largeIdx] >> 21) & 0b11111) + 33;

    int x2 = ((SMALL_SPRITE_CONTROL[smallIdx] >> 2) & 0b1111111111) - 16;
    int y2 = ((SMALL_SPRITE_CONTROL[smallIdx] >> 12) & 0b111111111) - 16;
    int h2 = ((SMALL_SPRITE_CONTROL[smallIdx] >> 25) & 0b1111) + 1;
    int w2 = ((SMALL_SPRITE_CONTROL[smallIdx] >> 21) & 0b1111) + 1;


    // if between (X and X+W) && (Y and Y+H)
    if ( ( (x1 >= x2) && (x1 <= x2+w2) ) || ((x1+w1 >= x2) && (x1+w1 <= x2+w2) ) ) {
        // check Y
        if ( ( (y1 >= y2) && (y1 <= y2+h2) ) || ((y1+h1 >= y2) && (y1+h1 <= y2+h2) ) ) {
            return 1;
        }
    } 
    return 0;

}

uint32_t setBackgroundPalette(uint32_t backgroundIdx, uint32_t backgroundPalette) {
    
    return 1;
}

uint32_t setBackgroundControl(uint32_t backgroundIdx, uint32_t backgroundControl) {
    BACKGROUND_CONTROLS[backgroundIdx] = backgroundControl;
    return 1;
}

uint32_t modifySpritePalette(uint32_t spriteIdx, uint32_t palette) {
    // change the palette of a sprite (use 63 + small index for small sprites)

    return 1;
}

uint32_t setPalette(uint32_t paletteIdx, uint32_t palette) {
    // set the palette at the specified palette 
    return 1;
}