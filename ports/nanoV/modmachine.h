#ifndef MICROPY_INCLUDED_NANOV_MODMACHINE_H
#define MICROPY_INCLUDED_NANOV_MODMACHINE_H

#include "py/obj.h"

enum {
    MACHINE_PIN_MODE_IN = 0,
    MACHINE_PIN_MODE_OUT = 1,
};

typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    uint8_t id                  : 4;
    uint8_t is_output           : 1;
    uint8_t last_output_value   : 1;
} machine_pin_obj_t;

const machine_pin_obj_t nanov_pin_obj[16];

#endif // MICROPY_INCLUDED_NANOV_MODMACHINE_H
