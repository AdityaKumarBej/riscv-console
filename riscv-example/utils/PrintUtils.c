#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "PrintUtils.h"
#include "VideoMemory.h"
#include "Constants.h"

void* growStringMemory(char* result, int current_size) {
  int new_size = current_size * 2;
  char* new_mem = (char*)malloc(new_size);
  for (int pos = 0; pos < current_size; pos++) {
    new_mem[pos] = result[pos];
  }
  return new_mem;
}

char* intToStr(int val) {
  int current_size = 1;
  char * result = malloc(current_size);
  int pos = 0;
  while (val > 0) {
    if (current_size < pos) {
      result = growStringMemory(result, current_size);
      current_size *= 2;
    }
    result[pos] = val%10;
    val /= 10;
    pos++;
  } 
  return result;
}


void changeModeToWrite() {
  *MODE_REGISTER = 0;
}

void changeModeToDraw() {
  *MODE_REGISTER = 1;
}

void printm(char* message) {
  writeCharToVideoMemory(message, 0);
}

void clearVideoMemory() {
  for (int i = 0; i < 36; i++) {
    for (int j = 0; j < 64; j++) {
      VIDEO_MEMORY[i*64+j] = 0;
    }
  }
  videoMemoryData->numOfLines = -1;
  videoMemoryData->numOfChars = 0;
  videoMemoryData->currentCharPos = 0;
}

void printmnl(char* message) {
  writeCharToVideoMemory(message, 1);
}

void writeCharToVideoMemory(char* message, int newLine) {
  changeModeToWrite();
  int videoPos = 0;
  int messagePos = 0;
  int startingLine = 0;
  int messageLength =  strlen(message);
  if (newLine == 1) {
    startingLine = (videoMemoryData->numOfLines + 1) * 64;
    videoMemoryData->numOfLines++;
  }
  videoPos = startingLine;
  VIDEO_MEMORY[videoPos] = 'h';

  // while (messagePos < messageLength) {
  //   VIDEO_MEMORY[videoPos] = message[messagePos];
  //   videoPos++;
  //   messagePos++;
  //   videoMemoryData->currentCharPos = videoPos;
  // }
}

