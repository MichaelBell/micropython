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
#include <spi.h>
#include "modmachine.h"

// Required, but not implemented by tinyQV
static void mp_machine_idle(void) {
    mp_event_handle_nowait(); // handle any events after possibly a long wait (eg feed WDT)
}

// Everything else relates to pins

#define MICROPY_PY_MACHINE_EXTRA_GLOBALS \
    { MP_ROM_QSTR(MP_QSTR_Pin), MP_ROM_PTR(&machine_pin_type) },

machine_pin_obj_t tinyqv_pin_obj[16] = {
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
        if (0 <= wanted_pin && wanted_pin < MP_ARRAY_SIZE(tinyqv_pin_obj)) {
            return &tinyqv_pin_obj[wanted_pin];
        }
    }
    mp_raise_ValueError(MP_ERROR_TEXT("Invalid pin"));
}

// constructor(id, ...)
mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    const machine_pin_obj_t *self = machine_pin_find(args[0]);

    if (self->is_output) {
        uint32_t output_sel = get_gpio_sel();
        output_sel |= 1 << self->id;
        set_gpio_sel(output_sel);
    }

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
static mp_obj_t machine_pin_init(size_t n_args, const mp_obj_t* pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_mode, ARG_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_mode,  MP_ARG_INT, {.u_int = MACHINE_PIN_MODE_IN} },
        { MP_QSTR_value, MP_ARG_INT, {.u_int = 0} },
    };

    // Parse the arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_mode].u_int == MACHINE_PIN_MODE_OUT) {
        // TODO, this is probably not the best way to do this
        mp_obj_t value_obj = MP_OBJ_NEW_SMALL_INT(args[ARG_value].u_int);
        machine_pin_call(args[ARG_self].u_obj, 1, 0, &value_obj);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_init_obj, 1, machine_pin_init);

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

///// SPI /////

typedef struct machine_spi_obj {
    mp_obj_base_t base;
    bool use_dc;
} machine_spi_obj_t;
static machine_spi_obj_t machine_spi_obj = {{&machine_spi_type}, .use_dc=false};

mp_obj_t machine_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_divisor, ARG_read_latency, ARG_use_cs, ARG_use_dc };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_divisor,  MP_ARG_INT, {.u_int = 4} },
        { MP_QSTR_read_latency, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_use_cs,   MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_use_dc,   MP_ARG_BOOL, {.u_bool = false} },
    };

    // Parse the arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get static peripheral object.
    machine_spi_obj_t *self = (machine_spi_obj_t *)&machine_spi_obj;

    // Initialise the SPI peripheral
    int spi_config = (args[ARG_divisor].u_int >> 2) - 1;
    if (spi_config < 0) spi_config = 0;
    if (spi_config > 3) spi_config = 3;
    if (args[ARG_read_latency].u_int != 0) spi_config |= 4;

    // Determine which pins must be selected away from GPIO use
    int spi_pins = 0x28;
    if (args[ARG_use_cs].u_bool) spi_pins |= 0x10;
    if (args[ARG_use_dc].u_bool) spi_pins |= 0x04;
    self->use_dc = args[ARG_use_dc].u_bool;

    int sel = get_gpio_sel();
    sel &= ~spi_pins;
    set_gpio_sel(sel);

    return MP_OBJ_FROM_PTR(self);
}

static void machine_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SPI(use_dc=%s)",
        self->use_dc ? "True" : "False");
}

static void machine_spi_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_divisor, ARG_read_latency, ARG_use_cs, ARG_use_dc };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_divisor,  MP_ARG_INT, {.u_int = 4} },
        { MP_QSTR_read_latency, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_use_cs,   MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_use_dc,   MP_ARG_BOOL, {.u_bool = false} },
    };

    // Parse the arguments.
    machine_spi_obj_t *self = (machine_spi_obj_t *)self_in;
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Initialise the SPI peripheral
    int spi_config = (args[ARG_divisor].u_int >> 2) - 1;
    if (spi_config < 0) spi_config = 0;
    if (spi_config > 3) spi_config = 3;
    if (args[ARG_read_latency].u_int != 0) spi_config |= 4;

    // Determine which pins must be selected away from GPIO use
    int spi_pins = 0x28;
    if (args[ARG_use_cs].u_bool) spi_pins |= 0x10;
    if (args[ARG_use_dc].u_bool) spi_pins |= 0x04;
    self->use_dc = args[ARG_use_dc].u_bool;

    int sel = get_gpio_sel();
    sel &= ~spi_pins;
    set_gpio_sel(sel);
}

static void machine_spi_transfer(mp_obj_base_t *self_in, size_t len, const uint8_t *src, uint8_t *dest) {
    // note src is guaranteed to be non-NULL
    bool write_only = dest == NULL;

    if (write_only) {
        spi_send_bytes(src, len, true, machine_spi_obj.use_dc);
    } else {
        spi_send_recv_bytes(src, dest, len, true, machine_spi_obj.use_dc);
    }
}

static const mp_machine_spi_p_t machine_spi_p = {
    .init = machine_spi_init,
    .transfer = machine_spi_transfer,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_spi_type,
    MP_QSTR_SPI,
    MP_TYPE_FLAG_NONE,
    make_new, machine_spi_make_new,
    print, machine_spi_print,
    protocol, &machine_spi_p,
    locals_dict, &mp_machine_spi_locals_dict
    );

mp_obj_base_t *mp_hal_get_spi_obj(mp_obj_t o) {
    if (mp_obj_is_type(o, &machine_spi_type)) {
        return MP_OBJ_TO_PTR(o);
    }
    #if MICROPY_PY_MACHINE_SOFTSPI
    else if (mp_obj_is_type(o, &mp_machine_soft_spi_type)) {
        return MP_OBJ_TO_PTR(o);
    }
    #endif
    else {
        mp_raise_TypeError(MP_ERROR_TEXT("expecting an SPI object"));
    }
}



static const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_low), MP_ROM_PTR(&machine_pin_low_obj) },
    { MP_ROM_QSTR(MP_QSTR_high), MP_ROM_PTR(&machine_pin_high_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&machine_pin_low_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&machine_pin_high_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_pin_init_obj) },

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
