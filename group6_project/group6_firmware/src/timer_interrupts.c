#include <stdint.h>
#include "thread_scheduler.h"

#define CONTROLLER         (*((volatile uint32_t *)0x40000018))

extern volatile int global;
extern volatile uint32_t controller_status;

uint32_t old_controller_value = 0;
uint32_t new_controller_value = 0;

/*
On a timer interrupts:
- increment global counter
- Capture the value of the controller
- Update new and old controller values
   - These will be used for checking if any buttons were pressed or released
- Switch to the next thread and return
*/
void timer_interrupt_handler() {
  global++;
  controller_status = CONTROLLER;
  old_controller_value = new_controller_value;
  new_controller_value = CONTROLLER;
  resume_next_thread();
}
