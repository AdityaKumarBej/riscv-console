/**
 * Returns number of ticks that have elapsed since the system was powered on.
 * @return Ticks
*/
uint32_t getTicks(void);

/**
 * Returns the Multi-Button Controller Status Register.
 * @return controller
*/
uint32_t getStatus(void);

/**
 * (for internal testing)
 * @return Video Toggle variable 
*/
uint32_t getVideoToggle(void);

/**
 * Returns the number of video interrupts since the system was powered on.
 * @return Video Interrupt counter
*/
uint32_t getVidIntCtr(void);

/**
 * Calcs and returns the large sprite control.
 * Note: Maybe change to set and add spriteIdx param
  */
uint32_t calcLargeSpriteControl(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t p);

/**
 * Inserts the parameter "largeSpriteCtrl" into the Large Sprite Control memory 
 * at the specified large sprite index
 * @param spriteIdx index into the memory map of the large sprite controls
 * @param largeSpriteCtrl control data of the -CORRECT- LARGE SPRITE CONTROL format 
  */
uint32_t setLargeSpriteControl(uint32_t spriteIdx, uint32_t largeSpriteCtrl);

/**
 * Inserts the parameter "smallSpriteCtrl" into the Small Sprite Control memory 
 * at the specified small sprite index
 * @param spriteIdx index into the memory map of the small sprite controls
 * @param smallSpriteCtrl control data of the -CORRECT- SMALL SPRITE CONTROL format 
  */
uint32_t setSmallSpriteControl(uint32_t spriteIdx, uint32_t smallSpriteCtrl);

/**
 * Writes the video interrupt counter to the screen starting at text 
 * postition 17 and grows left as the counter increases
*/
uint32_t writeVidIntCtrToScreen();

/**
 * @return pseudo-random integer between 0 and RAND_MAX.
*/
uint32_t rand();

/**
 * Seeds the random number generator
 * @return 1 on success
*/
uint32_t seed_rand(uint32_t seed);

/**
 * Moves the large sprite at a specified large sprite index to a certain (x, y) on the screen
 * @return 1 on success
*/
uint32_t moveLargeSprite(uint32_t spriteIdx, int32_t x, int32_t y);

/**
 * Sets the large sprite control data at the spriteIdx to 0x0
 * @return 1 on success
*/
uint32_t deleteLargeSprite(uint32_t spriteIdx);

/**
 * Sets the small sprite control data at the spriteIdx to 0x0
 * @return 1 on success
*/
uint32_t deleteSmallSprite(uint32_t spriteIdx);

/**
 * Writes some char c to some text position pos
 * @return 1 on success
*/
uint32_t writeCharToTextPos(uint32_t pos, char c);

/**
 * Writes some int num to text position 17 (grows left as it gets bigger)
 * @return 1 on success
*/
uint32_t writeIntToScreen(int num);

/**
 * @return Large Sprite Control data of the specified index
*/
uint32_t* getLargeSpriteControl(uint32_t spriteIdx);

/**
 * @return Small Sprite Control data of the specified index
*/
uint32_t* getSmallSpriteControl(uint32_t spriteIdx);



uint32_t getNearbySprites(uint32_t spriteIdx);

uint32_t checkCollisionLarge(uint32_t spriteIdx1, uint32_t spriteIdx2);

uint32_t checkCollisionSmall(uint32_t spriteIdx1, uint32_t spriteIdx2);

uint32_t checkCollisionLargeSmall(uint32_t largeIdx, uint32_t smallIdx);


// ============================== API FUNCTIONS THAT WERE NEVER NEEDED/IMPLEMENTED ===============================

/**
 * NEVER IMPLEMENTED
 * @param spriteIdx index of the sprite (if small sprite use 63 + small sprite index)
 * @return a list of pointers to the sprites the input parameter is currently in contact with.
*/

/**
 * 
 * @return 1 on success
*/
uint32_t setBackgroundPalette(uint32_t backgroundIdx, uint32_t backgroundControl);

/**
 * 
 * @return 1 on success
*/
uint32_t modifySpritePalette(uint32_t spriteIdx, uint32_t paletteIdx);

/**
 * 
 * @return 1 on success
*/
uint32_t setPalette(uint32_t paletteIdx, uint32_t palette);

void sleep(uint32_t time);

/**
 * @return 1 on success
*/
uint32_t setBackgroundControl(uint32_t backgroundIdx, uint32_t backgroundControl);