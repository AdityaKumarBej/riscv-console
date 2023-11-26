#include "control.h"

int pthread_mutex_init(mutex_t *mutex) {
  *mutex = 0;
  return 0;
}

int pthread_mutex_destroy(mutex_t *mutex) {
  return 0;
}
/*
__attribute__((always_inline)) inline int pthread_mutex_lock(mutex_t *mutex){
  int result;
  asm volatile ("li a1, 1\n\tamoor.w %0, a1, (%1)" : "=r"(result) : "r"(mutex));
  return result;
}
*/

__attribute__((always_inline)) inline void csr_enable_timer_interrupts(void){
  asm volatile ("li t0, 0x888\n\tcsrs mie, t0");
}

__attribute__((always_inline)) inline void csr_disable_timer_interrupts(void){
  asm volatile ("li t0, 0x888\n\tcsrc mie, t0");
}


int pthread_mutex_lock(mutex_t *mutex){
  csr_disable_timer_interrupts();
  int result = *mutex ^ 1;
  *mutex = 1;
  return result;
}

int pthread_mutex_unlock(mutex_t *mutex) {
  *mutex = 0;
  csr_enable_timer_interrupts();

  return 0;
}
