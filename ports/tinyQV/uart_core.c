#include <unistd.h>
#include "py/mpconfig.h"
#include <uart.h>

/*
 * Core UART functions to implement for a port
 */

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    unsigned char c = 0;
    if (uart_is_char_available()) {
        c = uart_getc();
    }
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
