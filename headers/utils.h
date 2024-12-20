#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct OptionalUint32 {
    uint32_t value;
    bool hasValue;
} OptionalUint32;
