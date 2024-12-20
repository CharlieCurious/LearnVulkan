#pragma once

#include <stdbool.h>
#include <stdint.h>

#define THROW(msg) { perror(msg); exit(EXIT_FAILURE); }

typedef struct OptionalUint32 {
    uint32_t value;
    bool hasValue;
} OptionalUint32;
