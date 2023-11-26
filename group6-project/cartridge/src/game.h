#include <stdint.h>

//This file contains game-specific functions.

int checkAlive(int cur_x, int cur_y, int budget);
int checkGetPellet(int cur_x, int cur_y, int center_x, int center_y, int budget);
void drawPellet(void);
void gameOver(void);
