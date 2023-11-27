#include <stdint.h>
#include "utils.h"

static unsigned long int next = 1;

uint32_t srand(uint32_t new_seed)
{
    next = (unsigned)new_seed & 0x7fffffffU;
}

int rand(int high)
{
    next = (next * 1103515245U + 12345U) & 0x7fffffffU;
    return (uint32_t)next % high;
}