#include "Constants.h"
#include <stdint.h>

volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xF4800);
volatile uint32_t *CartridgeStatus = (volatile uint32_t *)(0x4000001C);
volatile uint32_t *MODE_REGISTER = (volatile uint32_t *)(0x500F6780);
volatile uint32_t *MTIME_LOW      = ((volatile uint32_t *)0x40000008);
volatile uint32_t *MTIME_HIGH     = ((volatile uint32_t *)0x4000000C);
volatile uint32_t *MTIMECMP_LOW   = ((volatile uint32_t *)0x40000010);
volatile uint32_t *MTIMECMP_HIGH  = ((volatile uint32_t *)0x40000014);
volatile uint32_t *CONTROLLER     = ((volatile uint32_t *)0x40000018);
volatile uint32_t *ENABLE_INTERRUPT = (volatile uint32_t *)(0x40000000);
volatile uint32_t *PENDING_INTERRUPT = (volatile uint32_t *)(0x40000004);

volatile uint32_t *MEDIUM_PALETTE = (volatile uint32_t *)(0x500F2000);
volatile uint32_t *MEDIUM_CONTROL = (volatile uint32_t *)(0x500F5F00);
volatile uint8_t *MEDIUM_DATA = (volatile uint8_t *)(0x500D0000);
volatile uint32_t *VIDEO_MODE = (volatile uint32_t *)(0x500FF414);