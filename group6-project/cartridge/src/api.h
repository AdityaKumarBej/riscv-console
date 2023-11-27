// API List

//**************************TIMER API**************************//
/**
 * Retrieves the current system tick count.
 * @return The number of ticks elapsed since system start.
 */
uint32_t getTicks(void);

//**************************I/O CONTROLLER-API**************************//

/**
 * Retrieves the current status of keyboard inputs.
 * @return The current input status as a 32-bit unsigned integer.
 */
uint32_t getStatus(void);

/**
 * Generates a pseudo-random number within a specified range.
 * @param high The upper bound of the random number range (exclusive).
 * @return A random integer between 0 and high-1.
 */
uint32_t genRandom(int high);

//**************************GRAPHICS-API**************************//

/**
 * Defines a color in a sprite's palette using ARGB values.
 * @param palette_id Identifier for the sprite's palette.
 * @param entry_id Index within the palette.
 * @param rgba The color value in ARGB format.
 */
void setColor(int palette_id, int entry_id, uint32_t rgba);

/**
 * Assigns a background color using ARGB values.
 * @param palette_id Identifier for the background palette.
 * @param entry_id Palette index for the color assignment.
 * @param rgba Color value in ARGB format.
 */
void setBackgroundColor(int palette_id, int entry_id, uint32_t rgba);

/**
 * Computes a control value for small sprites based on their dimensions and position.
 * @param x, y Coordinates for the sprite's upper-left corner.
 * @param w, h Width and height of the sprite.
 * @param p Palette index for the sprite color.
 * @return The calculated control value for the sprite.
 */
uint32_t generateSmallSpriteConfig(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t p);

/**
 * Calculates the control value for a large sprite based on its position and size.
 * @param x, y Position of the sprite's top-left corner.
 * @param w, h Width and height of the sprite.
 * @param p Palette index for the sprite color.
 * @return The control value for configuring the large sprite.
 */
uint32_t generateLargeSpriteConfig(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t p);

/**
 * Determines the control settings for a background based on its position and palette.
 * @param x, y, z Position coordinates of the background.
 * @param p Palette index for the background color.
 * @return The control value for configuring the background.
 */
uint32_t generateBackgroundConfig(uint32_t x, uint32_t y, uint32_t z, uint32_t p);

/**
 * Configures a rectangular small sprite's control register.
 * @param id Identifier for the small sprite.
 * @param addr Configuration value for the sprite control register.
 */
void drawRectangleWithSmallSprite(int id, uint32_t addr);

/**
 * Sets up a rectangular large sprite's control register.
 * @param id Identifier for the large sprite.
 * @param addr Configuration value for the sprite control register.
 */
void drawRectangleWithLargeSprite(int id, uint32_t addr);

/**
 * Configures the control register for a background sprite.
 * @param id Identifier for the background sprite.
 * @param addr Configuration value for the background sprite control register.
 */
void drawRectangleWithBackgroundSpriteControl(int id, uint32_t addr);

/**
 * Updates the position of a small sprite.
 * @param id Identifier for the small sprite.
 * @param x, y New coordinates for the sprite's position.
 */
void moveSmallSprite(int id, uint32_t x, uint32_t y);

/**
 * Moves a large sprite to new coordinates.
 * @param id Identifier for the large sprite.
 * @param x, y New coordinates for the sprite's position.
 */
void moveLargeSprite(int id, uint32_t x, uint32_t y);

/**
 * Updates the position of a small sprite.
 * @param sprite_id Identifier for the small sprite.
 * @param x, y New coordinates for the sprite's position.
 */
uint32_t getSmallSpriteControl(int id);

/**
 * Moves a large sprite to new coordinates.
 * @param sprite_id Identifier for the large sprite.
 * @param x, y New coordinates for the sprite's position.
 */
uint32_t getLargeSpriteControl(int id);

/**
 * Fetches the control value for a background sprite.
 * @param sprite_id Identifier for the background sprite.
 * @return The current control value of the specified background sprite.
 */
uint32_t getBackgroundSpriteControl(int id);

//**************************SWITCH MODES API**************************//

/**
 * Switches the display to a graphics-based mode.
 */
void switchToGraphicsMode(void);

/**
 * Activates text display mode on the screen.
 */
void switchToTextMode(void);

/**
 * Displays a line of text on the screen when in text mode.
 * @param string The text string to be displayed.
 */
void printLine(char* string);

//**************************MULTI-THREADING API**************************//

typedef uint32_t *TContext;
typedef void (*TEntry)(void*);

/**
 * Initializes and creates a new thread context.
 * @param stacktop Pointer to the top of the thread stack.
 * @param entry Entry point function for the thread.
 * @param param Parameter passed to the thread's entry function.
 * @return A new thread context.
 */
TContext InitContext(uint32_t *stacktop, TEntry entry, void *param);

/**
 * Switches execution from one thread context to another.
 * @param old Current thread context.
 * @param new New thread context to switch to.
 */
void SwitchContext(TContext *old, TContext new);

//**************************INTERRUPTS**************************//

/**
 * Gets the count of video interrupts that have occurred.
 * @return Number of video interrupts since system start.
 */
int getVideoInterruptCount();

/**
 * Gets the count of CMD button interrupts that have occurred.
 * @return Number of CMD button interrupts since system start.
 */
int getCMDInterruptCount();