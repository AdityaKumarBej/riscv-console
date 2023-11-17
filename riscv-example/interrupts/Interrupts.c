#include <stdint.h>
#include "../utils/Constants.h"

extern volatile int global;
extern volatile uint32_t controllerStatus;
extern volatile uint32_t videoToggle;
extern volatile uint32_t vidIntCtr;
extern volatile uint32_t commandStatus;
extern volatile uint32_t machineTimer;
extern volatile uint32_t cartridgeStatus;
extern volatile uint32_t *MTIME_LOW;
extern volatile uint32_t *MTIME_HIGH;
extern volatile uint32_t *MTIMECMP_LOW;
extern volatile uint32_t *MTIMECMP_HIGH;
extern volatile uint32_t *CONTROLLER;
extern volatile uint32_t *ENABLE_INTERRUPT;
extern volatile uint32_t *PENDING_INTERRUPT;

void c_interrupt_handler(uint32_t mcause) {
    uint64_t NewCompare = (((uint64_t)*MTIMECMP_HIGH)<<32) | *MTIMECMP_LOW;
    NewCompare += 100;
    *MTIMECMP_HIGH = NewCompare>>32;
    *MTIMECMP_LOW = NewCompare;
    global++;
    controllerStatus = *CONTROLLER;

    // MACHINE TIMER INTERRUPT
    if (mcause == 0x80000007) {
        machineTimer++;
    }

    // if (mcause == 0x8000000B) {
        // CARTRIDGE INTERRUPT
        if ((*PENDING_INTERRUPT) & 1) {
            cartridgeStatus++;
            *PENDING_INTERRUPT = *PENDING_INTERRUPT | 0x1;
        }
        //VIDEO INTERRUPT
        if ((*PENDING_INTERRUPT >> 1) & 1) {
            vidIntCtr++;
            *PENDING_INTERRUPT = *PENDING_INTERRUPT | 0x1 << 1;
        }
        //COMMAND INTERRUPT
        if ((*PENDING_INTERRUPT >> 2) & 1) {
            commandStatus = commandStatus ^ 0x1;
            *PENDING_INTERRUPT = *PENDING_INTERRUPT | 0x1 << 2;
        }
    // }
}