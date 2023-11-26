#include <stdint.h>

volatile int global = 42;
volatile uint32_t controller_status = 0;
volatile uint32_t *CARTRIDGE = (volatile uint32_t *)(0x4000001C);
typedef void (*FunPtr)(void);

int main() {
    while(1){
        if((*CARTRIDGE) & 0x1){
            ((FunPtr)((*CARTRIDGE) & 0xFFFFFFFC))();
        }
    }
    return 0;
}
