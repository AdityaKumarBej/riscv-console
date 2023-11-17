#include <stdint.h>
#include "../../utils/Constants.h"

extern uint8_t _erodata[];
extern uint8_t _data[];
extern uint8_t _edata[];
extern uint8_t _sdata[];
extern uint8_t _esdata[];
extern uint8_t _bss[];
extern uint8_t _ebss[];

// Adapted from https://stackoverflow.com/questions/58947716/how-to-interact-with-risc-v-csrs-by-using-gcc-c-code
__attribute__((always_inline)) inline uint32_t csr_mstatus_read(void){
    uint32_t result;
    asm volatile ("csrr %0, mstatus" : "=r"(result));
    return result;
}

__attribute__((always_inline)) inline void csr_mstatus_write(uint32_t val){
    asm volatile ("csrw mstatus, %0" : : "r"(val));
}

__attribute__((always_inline)) inline void csr_write_mie(uint32_t val){
    asm volatile ("csrw mie, %0" : : "r"(val));
}

__attribute__((always_inline)) inline void csr_enable_interrupts(void){
    asm volatile ("csrsi mstatus, 0x8");
}

__attribute__((always_inline)) inline void csr_disable_interrupts(void){
    asm volatile ("csrci mstatus, 0x8");
}

extern volatile uint32_t *MTIME_LOW;
extern volatile uint32_t *MTIME_HIGH;
extern volatile uint32_t *MTIMECMP_LOW;
extern volatile uint32_t *MTIMECMP_HIGH;
extern volatile uint32_t *CONTROLLER;

void init(void){
    uint8_t *Source = _erodata;
    uint8_t *Base = _data < _sdata ? _data : _sdata;
    uint8_t *End = _edata > _esdata ? _edata : _esdata;

    while(Base < End){
        *Base++ = *Source++;
    }
    Base = _bss;
    End = _ebss;
    while(Base < End){
        *Base++ = 0;
    }

    csr_write_mie(0x888);       // Enable all interrupt soruces
    csr_enable_interrupts();    // Global interrupt enable
    *MTIMECMP_LOW = 1;
    *MTIMECMP_HIGH = 0;
}

extern volatile int global;
extern volatile uint32_t controllerStatus;
extern volatile uint32_t videoToggle;
extern volatile uint32_t vidIntCtr;
extern volatile uint32_t commandStatus;
extern volatile uint32_t machineTimer;
extern volatile uint32_t cartridgeStatus;

uint32_t c_system_call(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t call){
    if(call == 0){
        return global;
    }
    else if(call == 1){
        return *CONTROLLER;
    }
    else if(call == 2){
        return videoToggle;
    }
    else if(call == 13){
        return vidIntCtr;
    }
    else if (call == 4){
        return commandStatus;
    }
}