#ifndef CPUCYCLES_H
#define CPUCYCLES_H

#include <stdint.h>
#define ARM_CORTEX_A9

#if defined(ARM_CORTEX_A9)  /* Needs echo 2 > /sys/devices/cpu/rdpmc */

static inline uint32_t cpucycles(void) {
  uint32_t result;

  __asm__ volatile ("mrc p15, 0, %0, c9, c13, 0" : "=r" (result));

  return result;
}

#elif defined(USE_RDPMC)  /* Needs echo 2 > /sys/devices/cpu/rdpmc */

static inline uint64_t cpucycles(void) {
  const uint32_t ecx = (1U << 30) + 1;
  uint64_t result;

  __asm__ volatile ("rdpmc; shlq $32,%%rdx; orq %%rdx,%%rax"
    : "=a" (result) : "c" (ecx) : "rdx");

  return result;
}

#else

static inline uint64_t cpucycles(void) {
  uint64_t result;

  __asm__ volatile ("rdtsc; shlq $32,%%rdx; orq %%rdx,%%rax"
    : "=a" (result) : : "%rdx");

  return result;
}

#endif

#ifndef ARM_CORTEX_A9
uint64_t cpucycles_overhead(void);
#else
uint32_t cpucycles_overhead(void);
#endif
#endif
