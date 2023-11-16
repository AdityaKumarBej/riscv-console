#ifndef PRINTUTILS_H_
#define PRINTUTILS_H_

void changeModeToWrite();

void changeModeToDraw();

void writeCharToVideoMemory(char* message, int newLine);

void printm(char* message);

void printmnl(char* message);

void clearVideoMemory();

char* intToStr(int val);

void* growStringMemory(char* result, int current_size);

#endif