#ifndef CONSTANTS_H_
#define CONSTANTS_H_
#include <stdint.h>

extern volatile char *VIDEO_MEMORY;
extern volatile uint32_t *CartridgeStatus;
extern volatile uint32_t *MODE_REGISTER;
extern volatile uint32_t *MTIME_LOW;
extern volatile uint32_t *MTIME_HIGH;
extern volatile uint32_t *MTIMECMP_LOW;
extern volatile uint32_t *MTIMECMP_HIGH;
extern volatile uint32_t *CONTROLLER;
extern volatile uint32_t *ENABLE_INTERRUPT;
extern volatile uint32_t *PENDING_INTERRUPT;

extern volatile uint32_t *MEDIUM_PALETTE;
extern volatile uint32_t *MEDIUM_CONTROL;
extern volatile uint8_t *MEDIUM_DATA;
extern volatile uint32_t *VIDEO_MODE;
#endif
