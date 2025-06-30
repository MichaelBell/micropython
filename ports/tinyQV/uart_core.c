#include <unistd.h>
#include "py/mpconfig.h"
#include <uart.h>

/*
 * Core UART functions to implement for a port
 */

// Receive single character, blocking
int mp_hal_stdin_rx_chr(void) {
    int c;
    do {
        c = uart_getc();
    } while (c == -1);
    return c;
}

// Send string of given length
mp_uint_t mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    mp_uint_t ret = len;
    while (len--) {
        uart_putc(*str++);
    }
    return ret;
}
