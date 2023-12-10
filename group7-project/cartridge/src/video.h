#include <stdint.h>

uint32_t generateSmallSpriteConfig(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t p);

uint32_t generateLargeSpriteConfig(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t p);

uint32_t generateBackgroundConfig(uint32_t x, uint32_t y, uint32_t z, uint32_t p);

void drawRectangleWithSmallSprite(int sprite_id, uint32_t addr);

void drawRectangleWithLargeSprite(int sprite_id, uint32_t addr);

void drawRectangleWithBackgroundSpriteControl(int sprite_id, uint32_t addr);

void moveSmallSprite(int sprite_id, uint32_t x, uint32_t y);

void moveLargeSprite(int sprite_id, uint32_t x, uint32_t y);

void switchToGraphicsMode(void);

void switchToTextMode(void);

void setColor(int palette_id, int entry_id, uint32_t rgba);

void setBackgroundColor(int palette_id, int entry_id, uint32_t rgba);

void initializeSpriteControllers();

uint32_t getSmallSpriteControl(int sprite_id);

uint32_t getLargeSpriteControl(int sprite_id);

uint32_t getBackgroundSpriteControl(int sprite_id);

void printText(char* string);