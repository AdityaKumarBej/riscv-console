/**
 * @file background.h
 * 
 * background types and system calls
*/
#include <stdint.h>


typedef enum BACKGROUND_TYPE {
    PIXEL,
    TILE
} BACKGROUND_TYPE;

typedef struct background_t {
    BACKGROUND_TYPE type;
    int16_t x, y, z;
    uint8_t palette, data_index;
} background_t;


uint32_t load_background_palette_sys(uint32_t data_ptr, uint32_t index);
uint32_t load_background_data_pixel_sys(uint32_t data_ptr, uint32_t index);
uint32_t load_background_sys(uint32_t background_ptr);
uint32_t delete_background_sys(uint32_t background_id);

//SYSCALL WRAPPER FUNCTIONS:

/** @brief load background palette
 * 
 * @param palette The palette data buffer of 256 values to load into the palette
 * 
 * @param index the index to load the palette into
 * 
 * @return 0 upon success, -1 upon failure.
*/
int8_t load_background_palette(uint32_t* palette, uint8_t index) {
    return load_background_palette_sys((uint32_t) palette, (uint32_t) index);
}

/** @brief Load background data pixel - loads the background data buffer in pixel mode
 * 
 * @param data the data buffer of pixel values to load in (assumes a full buffer)
 * 
 * @param index the index to load the data into
 * 
 * @return 0 upon success, -1 upon failure.
*/
int8_t load_background_data_pixel(uint8_t* data, uint8_t index) {
    return load_background_data_pixel_sys((uint32_t) data, (uint32_t) index);
}

/** @note THIS IS AN UNIMPLEMENTED FUNCTION AND SHOULD NOT BE USED
 *  
 * @brief load background data tile - loads the background data in tile mode
 * 
 * @param data the data buffer of pixel values to load in (assumes a full buffer)
 * 
 * @param index the index to load the data into
 * 
 * @return 0 upon success, -1 upon failure.
*/
int8_t load_background_data_tile(uint32_t* data, uint8_t index);

/** @brief load background - loads the background control structure from the background_t type
 * 
 * @param background the background data type to create the control structure from
 * 
 * 
 * @return returns the background ID upon success, -1 upon failure.
*/
uint8_t load_background(background_t background) {
    return load_background_sys((uint32_t)&background);
}

/** @brief delete background - deletes the background associated with the given ID
 * 
 * @param background_id the background ID to delete.
 * 
 * @return 0 upon success, -1 upon failure.
*/
uint8_t delete_background(uint8_t background_id) {
    return delete_background_sys((uint32_t)background_id);
}
