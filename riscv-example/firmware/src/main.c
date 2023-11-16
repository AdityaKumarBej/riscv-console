#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "api.h"
#include "../../utils/Constants.h"
#include "../../utils/PrintUtils.h"
#include "../../utils/VideoMemory.h"

volatile int global = 42;
volatile uint32_t controller_status = 0;
volatile uint32_t command_status = 0;
volatile uint32_t videoToggle = 0;
volatile uint32_t vidIntCtr = 0;
volatile uint32_t machine_timer = 0;
volatile uint32_t cartridge_status = 0;
volatile struct VideoMemoryData* videoMemoryData;


typedef void (*FunctionPtr)(void);

int main() {
  int a = 4;
  int b = 12;
  int last_global = 42;
  int x_pos = 12;
  int prev_machine_timer = 0;
  int prev_command_ctr = 0;

  videoMemoryData = initializeVideoMemory();

  printmnl("OS has started");
  printmnl("Hello from me");

  while (1){
    if (prev_command_ctr != command_status) {
      printmnl("Command button pressed");
      prev_command_ctr = command_status;
    }
    if(*CartridgeStatus & 0x1){
      ((FunctionPtr)((*CartridgeStatus) & 0xFFFFFFFC))();
    }
  }
  return 0;
}

extern char _heap_base[];
extern char _stack[];

char *_sbrk(int numbytes){
  static char *heap_ptr = NULL;
  char *base;

  if (heap_ptr == NULL) {
    heap_ptr = (char *)&_heap_base;
  }

  if((heap_ptr + numbytes) <=_stack) {
    base = heap_ptr;
    heap_ptr += numbytes;
    return (base);
  }
  else {
    //errno = ENOMEM;
    return NULL;
  }
}