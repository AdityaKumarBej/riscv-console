// API List
uint32_t getTicks(void);
uint32_t getStatus(void);
uint32_t genRandom(int high);
void setColor(int palette_id, int entry_id, uint32_t rgba);
void setBackgroundColor(int palette_id, int entry_id, uint32_t rgba);
uint32_t generateSmallSpriteConfig(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t p);
uint32_t generateLargeSpriteConfig(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t p);
uint32_t generateBackgroundConfig(uint32_t x, uint32_t y, uint32_t z, uint32_t p);
void drawRectangleWithSmallSprite(int id, uint32_t addr);
void drawRectangleWithLargeSprite(int id, uint32_t addr);
void drawRectangleWithBackgroundSpriteControl(int id, uint32_t addr);
void moveSmallSprite(int id, uint32_t x, uint32_t y);
void moveLargeSprite(int id, uint32_t x, uint32_t y);
uint32_t getSmallSpriteControl(int id);
uint32_t getLargeSpriteControl(int id);
uint32_t getBackgroundSpriteControl(int id);
void switchToGraphicsMode(void);
void switchToTextMode(void);
void printText(char* string);
// threads
typedef uint32_t *TContext;
typedef void (*TEntry)(void*);
TContext InitContext(uint32_t *stacktop, TEntry entry, void *param);
void SwitchContext(TContext *old, TContext new);
// Video Interrupt
int getVideoInterruptCount();
int getCMDInterruptCount();