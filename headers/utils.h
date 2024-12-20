#pragma once

#include <stdbool.h>
#include <stdint.h>

#define THROW(msg) { perror(msg); exit(EXIT_FAILURE); }

typedef struct OptionalUint32 {
    uint32_t value;
    bool hasValue;
} OptionalUint32;

uint32_t clamp(uint32_t value, uint32_t min, uint32_t max);
