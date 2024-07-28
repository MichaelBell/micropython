# The nanoV port

This port is based on the minimal MicroPython port, targeting nanoV as on Tiny Tapeout 04.

## Building for TT04 nanoV

The Makefile expects the [nanoV-sdk](https://github.com/MichaelBell/nanoV-sdk) to be a sibling
directory of the micropython directory.

Additionally, it requires an RV32E compatible Risc-V toolchain in /opt/nanoV

To build:

    $ make

Building will produce the build/firmware.bin file suitable for loading into a 512kB FRAM.

## Running nanoV

Copy the Micropython files in the `tt` directory to your TT04 board.

Then you can load and run the firmware with:

    mpremote connect /dev/ttyACM0 + mount . + exec "import os; os.chdir('/'); import run_nanov_fram; run_nanov_fram.execute('/remote/build/firmware.bin')"

changing /dev/ttyACM0 to the appropriate serial port for your demo board.

This will give you a MicroPython REPL on the nanoV UART at 115200 baud.

## Building without the built-in MicroPython compiler

This minimal port can be built with the built-in MicroPython compiler
disabled.  This will reduce the firmware by about 20k on a Thumb2 machine,
and by about 40k on 32-bit x86.  Without the compiler the REPL will be
disabled, but pre-compiled scripts can still be executed.

To test out this feature, change the `MICROPY_ENABLE_COMPILER` config
option to "0" in the mpconfigport.h file in this directory.  Then
recompile and run the firmware and it will execute the frozentest.py
file.
