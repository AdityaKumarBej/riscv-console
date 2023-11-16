#include "Constants.h"
#include <stdint.h>

volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xF4800);
volatile uint32_t *CartridgeStatus = (volatile uint32_t *)(0x4000001C);
volatile uint32_t *MODE_REGISTER = (volatile uint32_t *)(0x500F6780);
