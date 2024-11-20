# The tinyQV port

This port is based on the minimal MicroPython port, targeting tinyQV as on Tiny Tapeout 06.

## Building for TT06 tinyQV

The Makefile expects the [tinyQV-sdk](https://github.com/MichaelBell/tinyQV-sdk) to be a sibling
directory of the micropython directory.

Additionally, it requires an RV32E compatible [Risc-V toolchain](https://github.com/MichaelBell/riscv-gnu-toolchain) in /opt/tinyQV

To build:

    $ make

Building will produce the build/firmware.bin file suitable for loading onto the [QSPI Pmod](https://github.com/mole99/qspi-pmod).

## Running tinyQV

Copy the Micropython files in the `tt` directory to your TT06 board (TODO: No boards to verify this on yet!)

Then you can load and run the firmware with:

    mpremote connect /dev/ttyACM0 + mount . + exec "import os; os.chdir('/'); import run_tinyqv; run_tinyqv.execute('/remote/build/firmware.bin')"

changing /dev/ttyACM0 to the appropriate serial port for your demo board.

This will give you a MicroPython REPL on the tinyQV UART at 115200 baud.  However, note that TinyQV UART has a bug which is worked around by sending all characters twice (link to customised pico-uart-bridge that does this, or possibly we can expose a UART that does this from the demoboard Micropython?)

## Using tinyQV Micropython

The Micropython build is currently minimal, with only gc, sys and machine modules.  It should be possible to expand this significantly.

The machine module provides a basic Pin object for accessing the inputs and outputs.  Outputs are pins 0-7 and inputs are pins 8-15.

For example

    for i in range(256):
      for j in range(2,8):
        Pin(j).value((i >> j) & 1)
    
will cycle the top 6 outputs (visible on the 7 segment display on the TT04 demo board).  Note out0 and out1 are used for UART, so if you configure them as output pins then the UART will stop working.

## TODO

* Get UART RX workaround working on the TT06 hardware (fingers crossed this is possible!)
* Add support for TinyQV's SPI
* Ability to set output pins back to non-GPIO mode
* Full interface support not just REPL - get Thonny working
* Make build less minimal
* Maybe can pick up better native Risc-V support if we move to MicroPython 1.24?
