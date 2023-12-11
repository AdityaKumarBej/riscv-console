#include <stdint.h>
#include "video.h"
#include "utils.h"

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

#define MTIME_LOW       (*((volatile uint32_t *)0x40000008))
#define MTIME_HIGH      (*((volatile uint32_t *)0x4000000C))
#define MTIMECMP_LOW    (*((volatile uint32_t *)0x40000010))
#define MTIMECMP_HIGH   (*((volatile uint32_t *)0x40000014))
#define CONTROLLER      (*((volatile uint32_t *)0x40000018))

typedef uint32_t *TContext;
typedef void (*TEntry)(void*);
TContext InitContext(uint32_t *stacktop, TEntry entry, void *param);
void SwitchContext(TContext *old, TContext new);
void ThreadJoin(TContext *first, TContext second);
void ThreadKill(TContext *thread);
void ThreadExit(TContext *thread);

extern volatile int global;
extern volatile uint32_t controller_status;
volatile uint32_t *INT_PEND_REG = (volatile uint32_t *)(0x40000004);
volatile uint32_t *INT_ENABLE_REG = (volatile uint32_t *)(0x40000000);

volatile int VID_INTRR_CNT = 0;    //Count of video interrupts
volatile int CMD_INTRR_CNT = 0;    //Count of CMD interrupts

typedef uint32_t *TContext;
typedef void (*TEntry)(void*);

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
    MTIMECMP_LOW = 1;
    MTIMECMP_HIGH = 0;
}

void c_interrupt_handler(uint32_t mcause){
    uint64_t NewCompare = (((uint64_t)MTIMECMP_HIGH)<<32) | MTIMECMP_LOW;
    NewCompare += 100;
    MTIMECMP_HIGH = NewCompare>>32;
    MTIMECMP_LOW = NewCompare;
    global++;
    controller_status = CONTROLLER;
    initializeSpriteControllers();
    // command button interrupt counter increment
    if (((*INT_PEND_REG) & 0x4) >> 2){
        CMD_INTRR_CNT++;
        (*INT_PEND_REG) |= 0x4;
    }
    // video interrupt counter increment
    if (((*INT_PEND_REG) & 0x2) > 0){
        VID_INTRR_CNT++;
        (*INT_PEND_REG) |= 0x2;
    }
}

uint32_t c_system_call(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t call){
    if(call == 0){
        return global;
    }
    else if(call == 1){
        return CONTROLLER;
    }
    else if (call == 2){
        srand(VID_INTRR_CNT);
        int r = rand(a0);
        return r;
    }
    else if (call == 3){
        switchToGraphicsMode();
    }
    else if (call == 4){
        switchToTextMode();
    }
    else if (call == 5){
        setSmallSpriteColor(a0, a1, a2);
    }
    else if (call == 6){
        uint32_t r = generateSmallSpriteConfig(a0, a1, a2, a3, a4);
        return r;
    }
    else if (call == 7){
        uint32_t r = generateLargeSpriteConfig(a0, a1, a2, a3, a4);
        return r;
    }
    else if (call == 8){
        uint32_t r = generateBackgroundConfig(a0, a1, a2, a3);
        return r;
    }
    else if (call == 9){
        drawRectangleWithSmallSprite(a0, a1);
    }
    else if (call == 10){
        drawRectangleWithLargeSprite(a0, a1);
    }
     else if (call == 11){
        drawRectangleWithBackgroundSpriteControl(a0, a1);
    }
    else if (call == 12){
        moveSmallSprite(a0, a1, a2);
    }
    else if (call == 13){
        moveLargeSprite(a0, a1, a2);
    }
    else if (call == 14){
        return getSmallSpriteControl(a0);
    }
    else if (call == 15){
        return getLargeSpriteControl(a0);
    }
    else if (call == 16){
        return getBackgroundSpriteControl(a0);
    }
    else if (call == 17){
        printText((char*)a0);
    }
    else if (call == 18){
        setBackgroundColor(a0, a1, a2);
    }
    // init thread
    else if (call == 19){
        TContext r = InitContext((TContext) a0, (TEntry) a1, (void*) a2);
        return r;
    }
    // context switch
    else if (call == 20){
        csr_disable_interrupts();
        SwitchContext((TContext*) a0, (TContext) a1);
        csr_enable_interrupts();
    }
    else if (call == 21){
        return VID_INTRR_CNT;
    }
    else if (call == 22){
        return CMD_INTRR_CNT;
    }
    else if (call == 23){
        uint32_t r = generateMediumSpriteConfig(a0, a1, a2, a3, a4);
        return r;
    }
    else if (call == 24){
        drawRectangleWithMediumSprite(a0, a1);
    }
    else if (call == 25){
        moveMediumSprite(a0, a1, a2);
    }
    else if (call == 26){
        return getMediumSpriteControl(a0);
    }
    else if (call == 27){
        setMediumSpriteColor(a0, a1, a2);
    }
    else if (call == 28){
        setLargeSpriteColor(a0, a1, a2);
    }
    else if (call == 29){
        ThreadJoin((TContext*) a0, (TContext) a1);
    }
    else if (call == 30){
        ThreadKill((TContext*) a0);
    }
    else if (call == 31){
        ThreadExit((TContext*) a0);
    }
    return -1;
}
