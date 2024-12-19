#include <stdbool.h>
#include <stdint.h>

typedef struct OptionalUint32 {
    bool hasValue;
    uint32_t value;
} OptionalUint32;
