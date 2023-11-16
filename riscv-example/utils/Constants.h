#ifndef CONSTANTS_H_
#define CONSTANTS_H_
#include <stdint.h>

extern volatile char *VIDEO_MEMORY;
extern volatile uint32_t *CartridgeStatus;
extern volatile uint32_t *MODE_REGISTER;
extern volatile struct VideoMemoryData* videoMemoryData;

#endif
