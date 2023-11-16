#include "VideoMemory.h"
#include <stdlib.h>

struct VideoMemoryData* initializeVideoMemory() {
  struct VideoMemoryData* defaultData = malloc(sizeof(struct VideoMemoryData)); 
  defaultData->maxCharWidth = 64;
  defaultData->maxCharHeight = 36;
  defaultData->memoryLeft = 0;
  defaultData->numOfLines = -1;
  defaultData->numOfChars = 0;
  defaultData->currentCharPos = 0;
  
  return defaultData;
}