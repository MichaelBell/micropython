#include <csr.h>
#include <timer.h>
#include <uart.h>
#include <py/runtime.h>

static inline void mp_hal_delay_us(mp_uint_t us) {
    mp_event_handle_nowait();
    delay_us(us);
}

static inline void mp_hal_delay_ms(mp_uint_t ms) {
    // Use mtime rather than read_time as mtime might be adjusted to the actual clock speed.
    uint32_t endtime = get_mtime() + ms * 1000;
    int32_t timediff;
    do {
        mp_event_handle_nowait();
        delay_us(100);
        timediff = endtime - get_mtime();
    } while (timediff > 0);
}

static inline mp_uint_t mp_hal_ticks_us(void) {
    return get_mtime();
}

static inline mp_uint_t mp_hal_ticks_ms(void) {
    //return get_mtime() / 1000;

    // Faster divide by constant
    uint64_t t = get_mtime();
    t *= 274877907;
    return (mp_uint_t)(t >> 38);
}

static inline mp_uint_t mp_hal_ticks_cpu(void) {
    return read_cycle();
}

static inline void mp_hal_set_interrupt_char(int c) {
    uart_rx_interrupt_char = c;
}
