#ifndef PRINT_SPEED_H
#define PRINT_SPEED_H

#include <stddef.h>
#include <stdint.h>
#include "cpucycles.h"

#ifndef ARM_CORTEX_A9
void print_results(const char *s, uint64_t *t, size_t tlen);
#else
void print_results(const char *s, uint32_t *t, size_t tlen);
#endif

#endif
