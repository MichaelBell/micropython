#include <stdint.h>

// options to control how MicroPython is built

// Use the minimal starting configuration (disables all optional features).
#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_MINIMUM)

// You can disable the built-in MicroPython compiler by setting the following
// config option to 0.  If you do this then you won't get a REPL prompt, but you
// will still be able to execute pre-compiled scripts, compiled with mpy-cross.
#define MICROPY_ENABLE_COMPILER     (1)

#define MICROPY_QSTR_EXTRA_POOL           mp_qstr_frozen_const_pool
#define MICROPY_ENABLE_GC                 (1)
#define MICROPY_HELPER_REPL               (1)
#define MICROPY_MODULE_FROZEN_MPY         (1)
#define MICROPY_ENABLE_EXTERNAL_IMPORT    (0)

#define MICROPY_ALLOC_PATH_MAX            (256)

// Use the minimum headroom in the chunk allocator for parse nodes.
#define MICROPY_ALLOC_PARSE_CHUNK_INIT    (16)

// type definitions for the specific machine

typedef intptr_t mp_int_t; // must be pointer size
typedef uintptr_t mp_uint_t; // must be pointer size
typedef long mp_off_t;

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

#define MICROPY_HW_BOARD_NAME "TT04"
#define MICROPY_HW_MCU_NAME "nanoV"

#define MICROPY_HEAP_SIZE      (24 * 1024)    // heap size 36 kilobytes
#define MICROPY_GC_STACK_ENTRY_TYPE uint16_t  // 16-bit type is sufficient to index the GC heap

#define MP_STATE_PORT MP_STATE_VM

// Module enables
#define MICROPY_PY_GC         (1)
#define MICROPY_PY_SYS        (1)
#define MICROPY_PY_MACHINE    (1)
#define MICROPY_PY_MACHINE_INCLUDEFILE "ports/nanoV/modmachine.c"
