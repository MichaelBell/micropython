#include "py/runtime.h"
#include "py/stream.h"
#include "py/mphal.h"

// TODO: Fix
uint64_t mp_hal_time_ns(void) {
    return ((uint64_t)read_time()) * 1000;
}
