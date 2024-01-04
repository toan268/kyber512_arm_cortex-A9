#include <stdint.h>
#include "cpucycles.h"

#ifndef ARM_CORTEX_A9
uint64_t cpucycles_overhead(void) {
  uint64_t t0, t1, overhead = -1LL;
#else
uint32_t cpucycles_overhead(void) {
  uint32_t t0, t1, overhead = -1L;
#endif
  unsigned int i;

  for(i=0;i<100000;i++) {
    t0 = cpucycles();
    __asm__ volatile ("");
    t1 = cpucycles();
    if(t1 - t0 < overhead)
      overhead = t1 - t0;
  }

  return overhead;
}
