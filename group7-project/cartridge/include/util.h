/**
 * @file util.h
 * 
 * utility functions for basic timing and cmd
*/

#include <stdint.h>


/** @brief get ticks - gets the number of timer interrupts (ticks)
 * 
 * @return a uint32_t value representing the number of ticks
*/
uint32_t GetTicks(void);

/** @brief get cmd - gets whether the cmd button has been pressed
 * 
 * @return 0 if the cmd button has not been pressed 1 if it has
*/
uint32_t GetCmd(void);

/** @brief clear cmd - clears the cmd variable to allow for another cmd button press
 * 
 * @return 0, is essentially a void
*/
uint32_t clear_cmd(void);


// int rand(int high);
// uint32_t srand(uint32_t new_seed);
uint32_t genRandom(int high);