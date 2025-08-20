#include <stdint.h>

// options to control how MicroPython is built

// Select the base configuration.
#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_BASIC_FEATURES)

// You can disable the built-in MicroPython compiler by setting the following
// config option to 0.  If you do this then you won't get a REPL prompt, but you
// will still be able to execute pre-compiled scripts, compiled with mpy-cross.
#define MICROPY_ENABLE_COMPILER     (1)

#define MICROPY_QSTR_EXTRA_POOL           mp_qstr_frozen_const_pool
#define MICROPY_ENABLE_GC                 (1)
#define MICROPY_HELPER_REPL               (1)
#define MICROPY_ENABLE_EXTERNAL_IMPORT    (1)
#define MICROPY_LONGINT_IMPL (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL (MICROPY_FLOAT_IMPL_FLOAT)

#define MICROPY_ALLOC_PATH_MAX            (256)

// type definitions for the specific machine

#define MP_SSIZE_MAX (0x7fffffff)
typedef intptr_t mp_int_t; // must be pointer size
typedef uintptr_t mp_uint_t; // must be pointer size
typedef long mp_off_t;

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

#define MICROPY_HW_BOARD_NAME "ttsky25a"
#define MICROPY_HW_MCU_NAME "tinyQV"

#define MICROPY_HEAP_SIZE      (1 * 1024 * 1024)    // heap size 1MB

#define MP_STATE_PORT MP_STATE_VM

// Module enables
#define MICROPY_PY_OS         (1)
#define MICROPY_VFS           (1)
#define MICROPY_VFS_FAT       (1)
#define MICROPY_VFS_LFS2      (1)
#define MICROPY_ENABLE_FINALISER (1)
#define MICROPY_GC_ALLOC_THRESHOLD (1)
#define MICROPY_READER_VFS          (MICROPY_VFS)
#define MICROPY_PY_BUILTINS_MEMORYVIEW (1)
#define MICROPY_PY_BUILTINS_FROZENSET               (1)
#define MICROPY_PY_BUILTINS_EXECFILE                (1)
#define MICROPY_PY_BUILTINS_HELP (1)
#define MICROPY_PY_FSTRINGS   (1)
#define MICROPY_PY_FRAMEBUF   (1)
#define MICROPY_PY_MICROPYTHON_MEM_INFO (1)
#define MICROPY_PY_SYS_MAXSIZE (1)
#define MICROPY_PY_MACHINE    (1)
#define MICROPY_PY_MACHINE_SPI (1)
#define MICROPY_PY_MACHINE_INCLUDEFILE "ports/tinyQV/modmachine.c"

#define MICROPY_KBD_EXCEPTION (1)

#define MICROPY_FATFS_ENABLE_LFN                (1)
#define MICROPY_FATFS_LFN_CODE_PAGE             437 /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */
#define MICROPY_FATFS_RPATH                     (2)

#define MICROPY_INTERNAL_EVENT_HOOK \
    do { \
        extern volatile uint8_t uart_rx_interrupt_seen; \
        if (uart_rx_interrupt_seen) { \
            mp_sched_keyboard_interrupt(); \
            uart_rx_interrupt_seen = 0; \
        } \
    } while (0)
