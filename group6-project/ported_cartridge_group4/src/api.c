#include "api.h"

// something reference https://github.com/fangqyi/riscv-console
// mem map for background controls 0x100(256B)
volatile uint32_t *BACKGROUND_CONTROL = (volatile uint32_t *)(0x500F5A00);

// mem map for large sprite control 0x400 (1KiB)
volatile uint32_t *LARGE_SPRITE_CONTROL = (volatile uint32_t *)(0x500F5B00);

// mem map for medium sprite control 0x400 (1KiB)
volatile uint32_t *MEDIUM_SPRITE_CONTROL = (volatile uint32_t *)(0x500F5F00);

// mem map for small sprite control 0x400 (1KiB)
volatile uint32_t *SMALL_SPRITE_CONTROL = (volatile uint32_t *)(0x500F6300);
#define MODE_CONTROL_REG (*((volatile uint32_t *)0x500F6780))
// void setVideoModel(int cmd){
//     (*MODE_CONTROL_REG)|=(cmd&0x1);
// }

volatile int global = 42;
volatile uint32_t controller_status = 0;
volatile uint32_t *MODE_REGISTER = (volatile uint32_t *)(0x500F6780);

volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xF4800);
volatile int pos1 = 0x40 * 1;
volatile int pos2 = 0x40 * 2;
volatile int count1 = 0;
volatile int count2 = 0;

void kmemcpy(uint8_t *dst, uint8_t *src, size_t num) {
  for (size_t i = 0; i < num; i++) {
    dst[i] = src[i];
  }
};

int setBackGround(uint8_t idx, char *addr) {
  char *offset = BACKGROUND_DATA_BASE + idx * BACKGROUND_DATA_SIZE;

  kmemcpy(offset, addr, BACKGROUND_DATA_SIZE);
  // for(uint32_t i=0;i<BACKGROUND_DATA_SIZE;i++) ((uint8_t*)offset)[i]=addr[i];
  return 0;
}

int setSprite(uint8_t idx, uint8_t *addr, Sprite sprites) {
  char *offset;
  size_t size = 0;
  switch (sprites) {
  case Large:
    size = LARGE_SPRITE_DATA_SIZE;
    offset = LARGE_SPRITE_DATA_BASE + idx * size;
    break;
  case Medium:
    size = MEDIUM_SPRITE_DATA_SIZE;
    offset = ((char *)MEDIUM_SPRITE_DATA_BASE) + idx * size;
    break;
  case Small:
    size = SMALL_SPRITE_DATA_SIZE;
    offset = SMALL_SPRITE_DATA_BASE + idx * size;
    break;
  default:
    break;
  }
  kmemcpy(offset, addr, size);
  return 0;
}

int initBackGroundPalettes(uint8_t idx, uint8_t *addr) {
  char *offset = BACKGROUND_PALETTE_BASE + idx * BACKGROUND_PALETTE_SIZE;
  kmemcpy(offset, addr, LARGE_SPRITE_PALETTE_SIZE);
  return 0;
}

int initSpritesPalettes(uint8_t idx, uint32_t *addr, Sprite sprites) {
  char *offset;
  switch (sprites) {
  case Large:
    /* code */
    offset = LARGE_SPRITE_PALETTE_BASE + idx * LARGE_SPRITE_CONTROL_SIZE;
    break;
  case Medium:
    offset =
        ((char *)MEDIUM_SPRITE_PALETTE_BASE) + idx * MEDIUM_SPRITE_PALETTE_SIZE;
    break;
  case Small:
    offset = SMALL_SPRITE_PALETTE_BASE + idx * SMALL_SPRITE_PALETTE_SIZE;
    break;
  default:
    break;
  }
  kmemcpy(offset, addr, 0x400);
  return 0;
}

void setBackGroundControl(uint8_t ctrl_idx, uint8_t data_idx, uint16_t x,
                          uint16_t y, uint8_t z, uint8_t palette) {

  y = (y + 288) & 0x3FF;
  x = (x + 512) & 0x3FF;
  z = (z + 0) & 0x7;
  palette = palette & 0x3;
  uint32_t value =
      (data_idx << 29) | (z << 22) | (y << 12) | (x << 2) | palette;
  BACKGROUND_CONTROL[ctrl_idx] = value;
}

void setTileBackGroundControl(uint8_t ctrl_x, uint8_t tile_idx, uint8_t sub_idx,
                              uint16_t x, uint16_t y, uint8_t z,
                              uint8_t palette) {

  y = (y + 288) & 0x3FF;
  x = (x + 512) & 0x3FF;
  z = (z + 0) & 0x7;
  palette = palette & 0x3;
  BACKGROUND_CONTROL[ctrl_x] = (1 << 31) | (tile_idx << 28) | (sub_idx << 25) |
                               (z << 22) | ((y + 288) << 12) |
                               ((x + 512) << 2) | palette;
}

void setSpriteControl(uint8_t ctrl_idx, uint8_t data_idx, uint16_t x,
                      uint16_t y, uint16_t z, uint8_t palette, Sprite sprites) {
  volatile uint32_t *base = 0;
  uint8_t size = 0;
  switch (sprites) {
  case Large:
    size = LARGE_SPRITES_SIZE;
    base = LARGE_SPRITE_CONTROL;
    break;
  case Medium:
    size = MEDIUM_SPRITES_SIZE;
    base = MEDIUM_SPRITE_CONTROL;
    break;
  case Small:
    size = SMALL_SPRITES_SIZE;
    base = SMALL_SPRITE_CONTROL;
    break;
  default:
    break;
  }
  base[ctrl_idx] = (data_idx << 24) | (z << 21) | ((y + size) << 12) |
                   ((x + size) << 2) | palette;
}

// Function to set background
void setBackground(uint8_t backgroundIndex, char *pixelData,
                   uint8_t controlIndex, uint8_t pixelIndex, uint8_t tileIndex,
                   uint8_t subIndex, uint16_t posX, uint16_t posY,
                   uint8_t zIndex, uint8_t paletteIndex,
                   uint32_t *paletteData) {
  setBackGround(backgroundIndex, pixelData);
  setBackGroundControl(controlIndex, pixelIndex, posX, posY, zIndex,
                       paletteIndex);
  setTileBackGroundControl(controlIndex, tileIndex, subIndex, posX, posY,
                           zIndex, paletteIndex);
  initBackGroundPalettes(paletteIndex, paletteData);
}

// Function to set a large sprite
void setLargeSprite(uint8_t spriteIndex, uint8_t *spriteData,
                    uint8_t controlIndex, uint8_t spriteDataIndex,
                    uint16_t posX, uint16_t posY, uint16_t zIndex,
                    uint8_t paletteIndex, uint32_t *paletteData) {
  setSprite(spriteIndex, spriteData, Large);
  setSpriteControl(controlIndex, spriteDataIndex, posX, posY, zIndex,
                   paletteIndex, Large);
  initSpritesPalettes(paletteIndex, paletteData, Large);
}

// Function to set a medium sprite
void setMediumSprite(uint8_t spriteIndex, uint8_t *spriteData,
                     uint8_t controlIndex, uint8_t spriteDataIndex,
                     uint16_t posX, uint16_t posY, uint16_t zIndex,
                     uint8_t paletteIndex, uint32_t *paletteData) {
  setSprite(spriteIndex, spriteData, Medium);
  setSpriteControl(controlIndex, spriteDataIndex, posX, posY, zIndex,
                   paletteIndex, Medium);
  initSpritesPalettes(paletteIndex, paletteData, Medium);
}

// Function to set a small sprite
void setSmallSprite(uint8_t spriteIndex, uint8_t *spriteData,
                    uint8_t controlIndex, uint8_t spriteDataIndex,
                    uint16_t posX, uint16_t posY, uint16_t zIndex,
                    uint8_t paletteIndex, uint32_t *paletteData) {
  setSprite(spriteIndex, spriteData, Small);
  setSpriteControl(controlIndex, spriteDataIndex, posX, posY, zIndex,
                   paletteIndex, Small);
  initSpritesPalettes(paletteIndex, paletteData, Small);
}

void simple_medium_sprite_green(int16_t x, int16_t y, int16_t z) {
  MODE_CONTROL_REG = 0x01;

  uint8_t sprite_data[0x400];
  uint32_t palette_data[0x100];

  // Fill palette data and sprite data
  for (int i = 0; i < 0x20; i++) {
    for (int j = 0; j < 0x20; j++) {
      palette_data[(i * 0x20 + j) % 0x100] = 0;
      sprite_data[i * 0x20 + j] = i < 0x10 ? 0 : 1;
    }
  }

  // Set specific colors in the palette
  palette_data[0] = GREEN;
  palette_data[1] = GREEN;

  // Use your API functions to set the medium sprite palette, data, and control
  initSpritesPalettes(2, palette_data, Medium);
  setSprite(10, sprite_data, Medium);
  setSpriteControl(5, 10, x, y, z, 2, Medium);
}
void simple_medium_sprite_red(int16_t x, int16_t y, int16_t z) {
  MODE_CONTROL_REG = 0x01;

  uint8_t sprite_data[0x400];
  uint32_t palette_data[0x100];

  // Fill palette data and sprite data
  for (int i = 0; i < 0x20; i++) {
    for (int j = 0; j < 0x20; j++) {
      palette_data[(i * 0x20 + j) % 0x100] = 0;
      sprite_data[i * 0x20 + j] = i < 0x10 ? 0 : 1;
    }
  }

  // Set specific colors in the palette
  palette_data[0] = RED;
  palette_data[1] = RED;

  // Use your API functions to set the medium sprite palette, data, and control
  initSpritesPalettes(2, palette_data, Medium);
  setSprite(10, sprite_data, Medium);
  setSpriteControl(5, 10, x, y, z, 2, Medium);
}

ThreadID thread_create_gp(TContextEntry entry, void *param, uint32_t memsize,
                          ThreadPriority prio) {
  uint32_t thread_gp;
  asm volatile("mv %0,gp" : "=r"(thread_gp));
  thread_create(entry, param, memsize, prio, thread_gp);
}