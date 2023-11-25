#include <stdlib.h>

__attribute__((always_inline)) inline void csr_enable_interrupts(void){
  asm volatile ("csrsi mstatus, 0x8");
}

__attribute__((always_inline)) inline void csr_disable_interrupts(void){
  asm volatile ("csrci mstatus, 0x8");
}

void* _sbrk(size_t requested_size) {
    extern char _heap[];
    extern char _stack[];
    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == 0) {
      heap_end = _heap;
    }
   prev_heap_end = heap_end;
   heap_end += requested_size;
   return prev_heap_end;
 }

void* t_malloc(size_t size) {
  csr_disable_interrupts();
  void *requested_memory = malloc(size);
  csr_enable_interrupts();
  return requested_memory;
}

void* t_realloc(void *ptr, size_t size) {
  csr_disable_interrupts();
  void *requested_memory = realloc(ptr, size);
  csr_enable_interrupts();
  return requested_memory;
}

void t_free(void *ptr) {
  csr_disable_interrupts();
  free(ptr);
  csr_enable_interrupts();
}
