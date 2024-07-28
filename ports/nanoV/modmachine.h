#ifndef MICROPY_INCLUDED_NANOV_MODMACHINE_H
#define MICROPY_INCLUDED_NANOV_MODMACHINE_H

#include "py/obj.h"

int pin_get(uint pin);
void pin_set(uint pin, int value);

#endif // MICROPY_INCLUDED_NANOV_MODMACHINE_H
