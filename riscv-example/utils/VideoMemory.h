#ifndef VIDEOMEMORY_H_
#define VIDEOMEMORY_H_

struct VideoMemoryData {
  int numOfLines;
  int memoryLeft;
  int maxCharWidth;
  int maxCharHeight;
  int numOfChars;
  int currentCharPos;
};
struct VideoMemoryData* initializeVideoMemory();
struct VideoMemoryData* getVideoMemoryData();

#endif