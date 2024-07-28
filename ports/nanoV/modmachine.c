/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2023 Damien P. George
 * Copyright (c) 2016 Paul Sokolovsky
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// This file is never compiled standalone, it's included directly from
// extmod/modmachine.c via MICROPY_PY_MACHINE_INCLUDEFILE.

#include <gpio.h>
#include "modmachine.h"

// Required, but not implemented by nanoV
static void mp_machine_idle(void) {
    mp_event_handle_nowait(); // handle any events after possibly a long wait (eg feed WDT)
}

// Everything else relates to pins

#define MICROPY_PY_MACHINE_EXTRA_GLOBALS \
    { MP_ROM_QSTR(MP_QSTR_Pin), MP_ROM_PTR(&machine_pin_type) },

const machine_pin_obj_t nanov_pin_obj[16] = {
    {{&machine_pin_type}, 0, 1, 0},
    {{&machine_pin_type}, 1, 1, 0},
    {{&machine_pin_type}, 2, 1, 0},
    {{&machine_pin_type}, 3, 1, 0},
    {{&machine_pin_type}, 4, 1, 0},
    {{&machine_pin_type}, 5, 1, 0},
    {{&machine_pin_type}, 6, 1, 0},
    {{&machine_pin_type}, 7, 1, 0},
    {{&machine_pin_type}, 0, 0, 0},
    {{&machine_pin_type}, 1, 0, 0},
    {{&machine_pin_type}, 2, 0, 0},
    {{&machine_pin_type}, 3, 0, 0},
    {{&machine_pin_type}, 4, 0, 0},
    {{&machine_pin_type}, 5, 0, 0},
    {{&machine_pin_type}, 6, 0, 0},
    {{&machine_pin_type}, 7, 0, 0},
};

const machine_pin_obj_t *machine_pin_find(mp_obj_t pin) {
    // Is already a object of the proper type
    if (mp_obj_is_type(pin, &machine_pin_type)) {
        return MP_OBJ_TO_PTR(pin);
    }
    if (mp_obj_is_int(pin)) {
        // get the wanted pin object
        int wanted_pin = mp_obj_get_int(pin);
        if (0 <= wanted_pin && wanted_pin < MP_ARRAY_SIZE(nanov_pin_obj)) {
            return &nanov_pin_obj[wanted_pin];
        }
    }
    mp_raise_ValueError(MP_ERROR_TEXT("Invalid pin"));
}

// constructor(id, ...)
mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    const machine_pin_obj_t *self = machine_pin_find(args[0]);

    return MP_OBJ_FROM_PTR(self);
}

// fast method for getting/setting pin value
static mp_obj_t machine_pin_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    machine_pin_obj_t *self = self_in;
    if (n_args == 0) {
        // get pin
        if (self->is_output) return MP_OBJ_NEW_SMALL_INT(self->last_output_value);
        return MP_OBJ_NEW_SMALL_INT((get_inputs() & (1 << self->id)) ? 1 : 0);
    } else {
        // set pin
        if (!self->is_output) mp_raise_ValueError(MP_ERROR_TEXT("Not an output"));
        bool value = mp_obj_is_true(args[0]);
        self->last_output_value = value ? 1 : 0;
        uint32_t cur_outputs = get_outputs();
        const uint32_t mask = 1 << self->id;
        if (value) cur_outputs |= mask;
        else cur_outputs &= ~mask;
        set_outputs(cur_outputs);
    }
    return mp_const_none;
}

// pin.value([value])
static mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    return machine_pin_call(args[0], n_args - 1, 0, args + 1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

// pin.low()
static mp_obj_t machine_pin_low(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->is_output) mp_raise_ValueError(MP_ERROR_TEXT("Not an output"));
    self->last_output_value = 0;
    uint32_t cur_outputs = get_outputs();
    const uint32_t mask = 1 << self->id;
    cur_outputs &= ~mask;
    set_outputs(cur_outputs);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_low_obj, machine_pin_low);

// pin.high()
static mp_obj_t machine_pin_high(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->is_output) mp_raise_ValueError(MP_ERROR_TEXT("Not an output"));
    self->last_output_value = 1;
    uint32_t cur_outputs = get_outputs();
    const uint32_t mask = 1 << self->id;
    cur_outputs |= mask;
    set_outputs(cur_outputs);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_high_obj, machine_pin_high);

static const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_low), MP_ROM_PTR(&machine_pin_low_obj) },
    { MP_ROM_QSTR(MP_QSTR_high), MP_ROM_PTR(&machine_pin_high_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&machine_pin_low_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&machine_pin_high_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_IN), MP_ROM_INT(MACHINE_PIN_MODE_IN) },
    { MP_ROM_QSTR(MP_QSTR_OUT), MP_ROM_INT(MACHINE_PIN_MODE_OUT) },
};
static MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_type,
    MP_QSTR_Pin,
    MP_TYPE_FLAG_NONE,
    make_new, mp_pin_make_new,
    call, machine_pin_call,
    locals_dict, &machine_pin_locals_dict
    );
