#include <csr.h>
#include <uart.h>
#include <py/runtime.h>

static inline void mp_hal_delay_us(mp_uint_t us) {
    mp_event_handle_nowait();
    delay_us(us);
}

static inline void mp_hal_delay_ms(mp_uint_t ms) {
    uint32_t endtime = read_time() + ms * 1000;
    int32_t timediff;
    do {
        mp_event_handle_nowait();
        delay_us(100);
        timediff = endtime - read_time();
    } while (timediff > 0);
}

static inline mp_uint_t mp_hal_ticks_us(void) {
    return read_time();
}

static inline mp_uint_t mp_hal_ticks_ms(void) {
    //return read_time() / 1000;

    // Faster divide by constant
    uint64_t t = read_time();
    t *= 274877907;
    return (mp_uint_t)(t >> 38);
}

static inline mp_uint_t mp_hal_ticks_cpu(void) {
    return read_cycle();
}

static inline void mp_hal_set_interrupt_char(int c) {
    uart_rx_interrupt_char = c;
}
