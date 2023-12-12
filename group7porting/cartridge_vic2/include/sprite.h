/**
 * @file sprite.h
 * 
 * Sprite types and system calls
*/

#include <stdint.h>

/*
* @brief SPRITE_TYPE enum, sprites can be small, medium, or large.
*/
typedef enum SPRITE_TYPE {
    SMALL,
    MEDIUM,
    LARGE
} SPRITE_TYPE;

/*
  Sprite_t data structure, simulates
  the control structure of a sprite
  and includes the type of sprite
*/
typedef struct sprite_t {
    SPRITE_TYPE type;
    int16_t x, y, z;
    uint8_t palette, data_index;
} sprite_t;

//syscall setup:
uint32_t load_sprite_sys(uint32_t sprite_ptr);
uint32_t load_sprite_data_sys(uint32_t type, uint32_t data_ptr, uint32_t index);
uint32_t load_palette_sys(uint32_t type, uint32_t palette_ptr, uint32_t index);
uint32_t display_sprite_sys(uint32_t sprite_id, uint32_t x_off, uint32_t y_off, uint32_t z_off);
uint32_t delete_sprite_sys(uint32_t sprite_id);
uint32_t change_sprite_palette_sys(uint32_t sprite_id, uint32_t palette_index);
uint32_t create_square_sys(uint32_t color);
//functions:

//WRAPPERS FOR SYSTEM CALLS


/** @brief Load Sprite - Loads the control structure and returns the sprite ID related to the sprite.
 * 
 * @param sprite sprite_t structure representing the information for the sprite
 * 
 * @return A 16 bit unsigned value representing the sprite ID, or -1 upon failure to load the sprite
*/
uint16_t load_sprite(sprite_t sprite) {
  return load_sprite_sys((uint32_t) &sprite);
}

/** @brief Load Sprite Data - Loads the data from the buffer into the specified index data section
 * 
 * @param type type of sprite to relate this data to, small, medium, or large.
 * 
 * @param data data buffer for the sprite, assumes the size of the buffer to be an appropriate size for the
 * sprite type - i.e. 1024 values for a medium sprite.
 * 
 * @param index data index to load into, should match the number of data sections for the type of sprite
 * 
 * @return 0 upon success, -1 upon failure
*/
int8_t load_sprite_data(SPRITE_TYPE type, uint8_t * data, uint8_t index) {
  return load_sprite_data_sys((uint32_t)type, (uint32_t)data, (uint32_t)index);
}

/** @brief Load palette - Loads in the data buffer for a palette into the specified index.
 * 
 * @param type type of sprite to relate this data to, small, medium, or large
 * 
 * @param palette data buffer of 256 32 bit values representing the colors of the palette
 * 
 * @param index The palette index to load the new palette into
 * 
 * @return 0 upon success, -1 upon failure.
*/
int8_t load_palette(SPRITE_TYPE type, uint32_t * palette, uint8_t index) {
  return load_palette_sys((uint32_t)type, (uint32_t) palette, (uint32_t) index);
}

/** @brief display sprite - displays the sprite related to the given sprite ID at the given location
 * 
 * @param sprite_id the ID of the sprite to display
 * 
 * @param x_off the x location to display the sprite
 * 
 * @param y_off the y location to display the sprite
 * 
 * @param z_off the z location to display the sprite
 * 
 * @return 0 upon success, -1 upon failure
*/
int16_t display_sprite(uint16_t sprite_id, uint16_t x_off, uint16_t y_off, uint8_t z_off) {
  return display_sprite_sys((uint32_t) sprite_id, (uint32_t) x_off, (uint32_t) y_off, (uint32_t) z_off);
}

/** @brief delete sprite - deletes the control structure related to the given sprite ID, does not affect sprite data or palette
 * 
 * @param sprite_id the ID of the sprite to delete
 * 
 * @return 0 upon success, -1 upon failure
*/
int delete_sprite(int16_t sprite_id) {
  return delete_sprite_sys((uint32_t)sprite_id);
}

/** @brief change sprite palette - changes the palette for the given sprite ID
 * 
 * @param sprite_id the sprite for which to change the palette
 * 
 * @param palette_index the new palette index for the sprite
 * 
 * @return 0 upon success, -1 upon failure.
*/
int change_sprite_palette(int16_t sprite_id, uint8_t palette_index) {
  return change_sprite_palette_sys((uint32_t) sprite_id, (uint32_t) palette_index);
}


// Medium Sprite data 54-63 (10 data sections)
// Medium Sprite Controls 225-255 (30 control structures)
/** @brief create_square - creates a medium square of the given color
 * 
 * @note Use of this function assumes the last medium palette entry is reserved
 * exclusively for this function, as well as the last 10 medium data sections and 30 control structures.
 * This allows for the creation of 30 medium sized squares using 10 unique colors.
 * 
 * @param color the color value for the square
 * 
 * @return the sprite ID of the newly created square, or -1 upon failure.
*/
uint16_t create_square(uint32_t color) {
  return create_square_sys(color);
}
