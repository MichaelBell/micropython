# The nanoV port

This port is based on the minimal MicroPython port, targeting nanoV as on Tiny Tapeout 04.

## Building for TT04 nanoV

The Makefile expects the [nanoV-sdk](https://github.com/MichaelBell/nanoV-sdk) to be a sibling
directory of the micropython directory.

Additionally, it requires an RV32E compatible Risc-V toolchain in /opt/nanoV

To build:

    $ make

Building will produce the build/firmware.bin file suitable for loading into a [512kB FRAM](https://www.adafruit.com/product/4719)).

## Running nanoV

Copy the Micropython files in the `tt` directory to your TT04 board.

Then you can load and run the firmware with:

    mpremote connect /dev/ttyACM0 + mount . + exec "import os; os.chdir('/'); import run_nanov_fram; run_nanov_fram.execute('/remote/build/firmware.bin')"

changing /dev/ttyACM0 to the appropriate serial port for your demo board.

This will give you a MicroPython REPL on the nanoV UART at 115200 baud.

## Using nanoV Micropython

512kB is barely enough space for code, leaving very little room for the Micropython GC heap (currently it is set to 18kB).  The Micropython build is minimal, with only gc, sys and machine modules.

The machine module provides a basic Pin object for accessing the inputs and outputs.  Outputs are pins 0-7 and inputs are pins 8-15.

For example

    for i in range(256):
      for j in range(8):
        Pin(j).value((i >> j) & 1)
    
will cycle all the outputs (visible on the 7 segment display on the TT04 demo board).

## Building without the built-in MicroPython compiler

This minimal port can be built with the built-in MicroPython compiler
disabled.  This will reduce the firmware by about 20k on a Thumb2 machine,
and by about 40k on 32-bit x86.  Without the compiler the REPL will be
disabled, but pre-compiled scripts can still be executed.

To test out this feature, change the `MICROPY_ENABLE_COMPILER` config
option to "0" in the mpconfigport.h file in this directory.  Then
recompile and run the firmware and it will execute the frozentest.py
file.
