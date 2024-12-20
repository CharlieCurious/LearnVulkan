#include <utils.h>

uint32_t clamp(uint32_t value, uint32_t min, uint32_t max) {
    if (value < min)
        return min;
    else if (value > max)
        return max;
    return value;
}
