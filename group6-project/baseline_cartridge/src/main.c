#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "api.h"
#include "display.h"
#define SMALL_SPRITE_CTRL_OFFSET 16

volatile int global = 42;
volatile uint32_t controller_status = 0;
volatile char *VIDEO_MEMORY = (volatile char *)(0x500F4800);
volatile uint32_t *SMALL_SPRITE_CONTROL = (volatile uint32_t *)(0x500E0000);

//thread implementation declaration
typedef uint32_t *TContext;
typedef void (*TEntry)(void*);
TContext InitContext(uint32_t *stacktop, TEntry entry,void *param);
void SwitchContext(TContext *oldcontext, TContext newcontext);
uint32_t ThreadStack[128];
TContext Mainthread;
TContext Otherthread;

int main() {
    int last_global = 42;
    int x_pos = 0;
    int y_pos = 0;
    uint32_t current_status = 0;
    uint32_t last_status = 0;
    switchToGraphicsMode();                 //api call to switch between graphic and text mode - refer documentation
    setColor(0, 0, 0x80FF0000);             //api call to change sprite color - refer documentation
    setBackgroundColor(0, 0, 0x00000000);  //api call to change background color - refer documentation

    drawRectangleWithBackgroundSpriteControl(0, generateBackgroundConfig(0,0,0,0)); //api call to draw a sprite

    int CMD_INTRR = 0;
    int CURRENT_CMD_INTRR;

    while(1){

        global = getTicks();
        CURRENT_CMD_INTRR = getCMDInterruptCount();
        if (CURRENT_CMD_INTRR != CMD_INTRR){           //keep holding the "d" key to break and switch to text mode
            break;
        }

        if(global != last_global){
            controller_status = getStatus();
            if (controller_status == 0x0){
                current_status = last_status;
            }
            else {
                current_status = controller_status;
            }

            if(controller_status & 0x1){        //move to the top left corner
                if(x_pos & 0x3F){
                    x_pos--;
                }
            }
            if(controller_status & 0x2){        //move to the top right corner
                if(x_pos >= 0x40){
                    x_pos -= 0x40;
                }
            }
            if(controller_status & 0x4){        //move to the bottom right corner
                if(x_pos < 0x8C0){
                    x_pos += 0x40;
                }
            }
            if(controller_status & 0x8){        //move to the bottom left corner
                if((x_pos & 0x3F) != 0x3F){
                    x_pos++;
                }
            }
            moveSmallSprite(0, (x_pos & 0x3F)<<3, (x_pos>>6)<<3);
        }

    }
    switchToTextMode();

    printText("MODE SWITCHED!! Hit RST to go back to graphic mode");
    return 0;
}