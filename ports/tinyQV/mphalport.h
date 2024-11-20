#include <csr.h>

static inline mp_uint_t mp_hal_ticks_ms(void) {
    return read_time() / 1000;
}
static inline void mp_hal_set_interrupt_char(char c) {
}
