#include <stdint.h>

//This file contains game-specific functions.

int getSnakeStatus(int currXPos, int currYPos, int growSnakeParam);
int getFoodStatus(int currXPos, int currYPos, int centerXPos, int centerYPos, int growSnakeParam);
void generateFoodBlock(void);
void endGame(void);
void printText(char* string);