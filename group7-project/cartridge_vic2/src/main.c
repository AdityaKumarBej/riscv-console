#include <stdint.h>
#include <sprite.h>
#include <stdlib.h>
#include <background.h>
#include <threading/threads.h>
#include <controller.h>
#include <util.h>



int main()
{


    create_square(0xFFFF0000);
    while (1) {}

    /* KEEP THIS HERE */
    threadExit(); // Calling threadExit anywhere in the main will 'eject' the cartridge
    return 0;
}


